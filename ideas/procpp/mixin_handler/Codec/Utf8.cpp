#include "Codec/Utf8.h"

// https://en.wikipedia.org/wiki/UTF-8
size_t Utf8::length(const string& s)
{
	if (s.length() < 1) return 0;

	auto length = s.length();
	size_t offset = 0;
	size_t n = 0;

	while (offset < length) {
		auto char_length = decode_char_length(s, offset);
		if (char_length == 0) return n;

		offset += char_length;
		n ++;
	}

	return n;
}

size_t Utf8::decode_char_length(const string& s, size_t offset, uint32_t* p_codepoint)
{
	if (s.length() <= offset) return 0;

	auto first_byte = (uint8_t)s[offset];

	auto msb = first_byte & 0x80;
	if (msb == 0x00) {

		if (p_codepoint) *p_codepoint = first_byte;

		//1 byte
		return 1;

	} else {
		size_t length;

		auto next2bits = first_byte & 0x60;
		if (next2bits == 0x40) length = 2;
		else {
			auto next3bits = first_byte & 0x70;
			if (next3bits == 0x60) length = 3;
			else {
				auto next4bits = first_byte & 0x78;
				if (next4bits == 0x70) length = 4;
				else return 0;//invalid utf8
			}
		}

		if (s.length() < offset + length) return 0;
		if (p_codepoint) {
			switch (length) {
				case 2:
					*p_codepoint = ( ( s[offset] & 0b11111 ) << 6 ) + ( s[offset+1] & 0b111111 );
					break;
				case 3:
					*p_codepoint = ( ( s[offset] & 0b1111 ) << 12 ) + ( ( s[offset+1] & 0b111111 ) << 6 ) + ( s[offset+2] & 0b111111 );
					break;
				case 4:
					*p_codepoint = ( ( s[offset] & 0b111 ) << 18 ) + ( ( s[offset+1] & 0b111111 ) << 12 ) + ( ( s[offset+2] & 0b111111 ) << 6 ) + ( s[offset+3] & 0b111111 );
					break;
			}
		}

		return length;
	}

}

bool Utf8::decode_char(const string& s, string& ch, size_t& offset)
{
	auto length = decode_char_length(s, offset);

	if (length == 0) return false;

	ch.assign(&s[offset], length);
	offset += length;

	return true;
}

string Utf8::encode(uint32_t unicode)
{
	if (unicode <= 0x7F) {
		return string(1, (char)unicode);
	}

	if (unicode >= 0x80 && unicode <= 0x7FF) {
		//need 11 bits
		char first_byte = 0b11000000 | ((unicode & (0b11111 << 6)) >> 6);
		char second_byte = 0b10000000 | (unicode & 0b111111);

		return string({first_byte, second_byte});
	}

	if (unicode >= 0x800 && unicode <= 0xFFFF) {
		//need 16 bits
		char first_byte = 0b11100000 | ((unicode & (0b1111 << 12)) >> 12);
		char second_byte = 0b10000000 | ((unicode & (0b111111 << 6)) >> 6);
		char third_byte = 0b10000000 | (unicode & 0b111111);

		return string({first_byte, second_byte, third_byte});
	}

	if (unicode >= 0x10000) {
		//need 21 bits
		char first_byte = 0b11110000 | ((unicode & (0b111 << 18)) >> 18);
		char second_byte = 0b10000000 | ((unicode & (0b111111 << 12)) >> 12);
		char third_byte = 0b10000000 | ((unicode & (0b111111 << 6)) >> 6);
		char fourth_byte = 0b10000000 | (unicode & 0b111111);

		return string({first_byte, second_byte, third_byte, fourth_byte});
	}

	return "";
}
