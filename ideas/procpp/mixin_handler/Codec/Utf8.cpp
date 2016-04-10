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

size_t Utf8::decode_char_length(const string& s, size_t offset)
{
	if (s.length() < offset + 1) return 0;

	auto first_byte = (uint8_t)s[offset];

	auto msb = first_byte & 0x80;
	if (msb == 0x00) {

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
				auto next4bits = first_byte & 0x77;
				if (next4bits == 0x70) length = 4;
				else return 0;//invalid utf8
			}
		}

		if (s.length() < offset + length) return 0;

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
