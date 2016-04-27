#include <vector>
#include "ReactHandler.h"
using namespace std;


static uint8_t calculate_decimal(uint8_t h)
{
	auto diff = h - '0';

	if (diff >= 49) {
		//小写字母
		return h - 'a' + 10;
	}
	else if (diff >= 17) {
		//大写字母
		return h - 'A' + 10;
	}
	else {
		//数字
		return h - '0';
	}
}

static uint32_t calculate_hex(const vector<uint8_t>& u_hex)
{
	uint32_t codepoint = 0;
	auto size = u_hex.size();
	for (size_t i = 0; i < size; i++) {
		auto decimal_value = calculate_decimal(u_hex[i]);
		codepoint += decimal_value << ((size - 1 - i) * 4);
	}

	return codepoint;
}

Encoding Json::detect_encoding(const string& json_string)
{
	if (json_string.length() < 2) return Encoding::Utf8;

	//基于第一个和最后一个**字**必定是ascii

	auto first_char = json_string[0];
	auto second_char = json_string[1];

	if (first_char == 0 && second_char == 0) return Encoding::Utf32BE;

	auto length = json_string.length();
	auto last_char = json_string[length-1];
	auto last_but1_char = json_string[length-2];

	if (last_char == 0 && last_but1_char == 0) return Encoding::Utf32LE;

	if (first_char != 0 && last_char != 0) return Encoding::Utf8;

	if (last_char != 0) return Encoding::Utf16BE;

	return Encoding::Utf16LE;
}

Json::Value Json::decode(const string& json_string)
{
	Value v;
	v.type = ValueType::Null;

	auto encoding = detect_encoding(json_string);

	if (encoding == Encoding::Utf32LE || encoding == Encoding::Utf32BE) {
		L.error_log("utf32 not supported yet");
		return v;
	}

	StreamReader s(json_string);

	uint32_t codepoint;

	try {
		s.read_up(encoding, " \t\r\n");//ws

		if (s.read_character(encoding, codepoint)) return v;

		switch (codepoint) {
			case '{'://object
				break;
			case '['://array
				break;
			case '-'://number
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				break;
			case '"'://string
			{
				bool in_escape = false;
				bool in_u = false;
				bool in_pair = false;
				uint32_t surrogate1 = 0;
				vector<uint8_t> u_hex;
				vector<uint8_t> pair_parts;
				do {
					s.fail_unless( s.read_character(encoding, codepoint) );
					if (in_escape) {
						switch (codepoint) {
							case 0x22/*"*/:
							{
								v.str += '"';//TODO consider encoding
								break;
							}
							case 0x5C/*\*/:
							{
								v.str += '\\';
								break;
							}
							case 0x2F/*"/"*/:
							{
								v.str += '/';
								break;
							}
							case 0x62/*b*/:
							{
								v.str += '\b';
								break;
							}
							case 0x66/*f*/:
							{
								v.str += '\f';
								break;
							}
							case 0x6E/*n*/:
							{
								v.str += '\n';
								break;
							}
							case 0x72/*r*/:
							{
								v.str += '\r';
								break;
							}
							case 0x74/*t*/:
							{
								v.str += '\t';
								break;
							}
							case 0x75/*u*/:
							{
								in_escape = false;
								in_u = true;
								break;
							}
						}
					} else if (in_u) {
						u_hex.push_back(codepoint);
						if (u_hex.size() == 4) {
							auto hex_codepoint = calculate_hex(u_hex);
							if ( Utf16::has_surrogate_pair(hex_codepoint) ) {
								surrogate1 = hex_codepoint;
								in_pair = true;
							}
							in_u = false;
							u_hex.clear();
						}
					} else if (in_pair) {
						pair_parts.push_back(codepoint);
						if (pair_parts.size() == 6) {
							if (pair_parts[0] == '\\' && pair_parts[1] == 'u') {
								pair_parts.erase(pair_parts.begin(), pair_parts.begin() + 2);//TODO optimize performance
								auto hex_codepoint = calculate_hex(pair_parts);
								uint32_t sp_codepoint;
								if ( Utf16::to_codepoint(surrogate1, hex_codepoint, sp_codepoint) ) {
								}
							}
							pair_parts.clear();
						}
					} else {
						if (codepoint == '\\') {
							in_escape = true;
						} else {
						}
					}
				} while (!s.end());
				break;
			}
			case 't'://bool
			{
				uint32_t rue[3];
				s.fail_unless( s.read_characters(encoding, rue, 3) );
				s.fail_unless( rue[0] == 'r' && rue[1] == 'u' && rue[2] == 'e' );

				v.type = ValueType::Bool;
				v.number = 0;
				break;
			}
			case 'f':
			{
				uint32_t alse[4];
				s.fail_unless( s.read_characters(encoding, alse, 4) );
				s.fail_unless( alse[0] == 'a' && alse[1] == 'l' && alse[2] == 's' && alse[3] == 'e' );

				v.type = ValueType::Bool;
				v.number = 1;
				break;
			}
			case 'n'://null
			{
				v.type = ValueType::Null;
				break;
			}
			default:
				s.fail_if(true);
		}

	} catch (ReaderException e) {
		L.debug_log( "invalid character at offset " + to_string(s.offset()) + "\r\n hex string : " + Utils::string2hex(json_string) );
	}

	return v;
}

string Json::encode(const Value& json_value)
{
	return "";
}

