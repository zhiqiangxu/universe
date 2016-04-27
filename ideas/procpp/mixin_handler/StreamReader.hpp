#include "Codec/Utf8.h"

template <Encoding e>
bool IStreamReader::pread_character(uint32_t& codepoint, size_t& length)
{
	switch (e) {
		case Encoding::Utf8:
		{
			length = Utf8::decode_char_length(_str, _offset, &codepoint);
			break;
		}
/*TODO
		case Encoding::Utf16LE:
		{
			//TODO
			length = Utf16::decode_char_length<Endian::Little>(_str, _offset, &codepoint);
			break;
		}
		case Encoding::Utf16BE:
		{
			//TODO
			length = Utf16::decode_char_length<Endian::Big>(_str, _offset, &codepoint);
			break;
		}
*/
		default:
			L.error_exit("unsupported encoding" + Utils::enum_string(e));
	}

	if (length == 0) return false;

	return true;
}

template <Encoding e>
bool IStreamReader::read_character(uint32_t& codepoint)
{
	size_t length;
	auto ok = pread_character<e>(codepoint, length);

	if (ok) _offset += length;

	return ok;
}

template <Encoding e>
void IStreamReader::read_up(const string& s, bool nullable, string* result)
{
	auto init_offset = _offset;

	uint32_t codepoint;
	size_t length;

	while (pread_character<e>(codepoint, length)) {
		if (codepoint > 255) return;//TODO 若需要read_up ascii，需要重新设计接口

		if (s.find((char)codepoint) != string::npos) _offset += length;
		else {
			if ( (_offset == init_offset) && (!nullable) ) throw ReaderException::NG;

			if (result) *result = _str.substr(init_offset, _offset - init_offset);

			return;
		}
	}
}


