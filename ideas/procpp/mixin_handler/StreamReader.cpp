#include "ReactHandler.h"


void StreamReader::read_size(size_t size, void* p)
{
	if (size > _str.length() - _offset) throw ReaderException::AG;

	memcpy(p, _str.data() + _offset, size);
	_offset += size;
}

void StreamReader::pread_size(size_t size, void* p)
{
	if (size > _str.length() - _offset) throw ReaderException::AG;

	memcpy(p, _str.data() + _offset, size);
}

//TODO refactor in a more generic way
bool StreamReader::read_character(Encoding e, uint32_t& codepoint)
{
	switch (e) {
		case Encoding::Utf8:
		{
			return read_character<Encoding::Utf8>(codepoint);
			break;
		}
		case Encoding::Utf16LE:
			return read_character<Encoding::Utf16LE>(codepoint);
			break;
		case Encoding::Utf16BE:
			return read_character<Encoding::Utf16BE>(codepoint);
			break;
		case Encoding::Utf32LE:
			return read_character<Encoding::Utf32LE>(codepoint);
			break;
		case Encoding::Utf32BE:
			return read_character<Encoding::Utf32BE>(codepoint);
			break;
	}

	// you got a bug if arrive here
	return false;
}

bool StreamReader::read_characters(Encoding e, uint32_t* p_codepoint, int n)
{
	uint32_t codepoint;
	for (int i = 0; i < n; i++) {
		if ( !read_character(e, codepoint) ) return false;

		*(p_codepoint + i) = codepoint;
	}

	return true;
}

void StreamReader::read_up(const string& s, bool nullable, string* result)
{
	auto init_offset = _offset;

	auto data = _str.data();

	while (_offset < _str.length()) {
		auto ch = data[_offset];

		if (s.find(ch) != string::npos) _offset++;
		else {
			if ( (_offset == init_offset) && (!nullable) ) throw ReaderException::NG;

			if (result) *result = _str.substr(init_offset, _offset - init_offset);

			return;
		}
	}

	throw ReaderException::AG;
}

void StreamReader::read_up(Encoding e, const string& s, bool nullable, string* result)
{
	//TODO more generic way of writing this
	switch (e) {
		case Encoding::Utf8:
			return read_up<Encoding::Utf8>(s, nullable, result);
			break;
		case Encoding::Utf16LE:
			return read_up<Encoding::Utf16LE>(s, nullable, result);
			break;
		case Encoding::Utf16BE:
			return read_up<Encoding::Utf16BE>(s, nullable, result);
			break;
		case Encoding::Utf32LE:
			return read_up<Encoding::Utf32LE>(s, nullable, result);
			break;
		case Encoding::Utf32BE:
			return read_up<Encoding::Utf32BE>(s, nullable, result);
			break;
	}
}

void StreamReader::read_until(const string& s, string& result, bool eatup_final)
{
	auto init_offset = _offset;

	auto data = _str.data();

	while (_offset < _str.length()) {
		auto ch = data[_offset];

		if (s.find(ch) == string::npos) _offset++;
		else {
			if (_offset != init_offset) result = _str.substr(init_offset, _offset - init_offset);

			if (eatup_final) _offset++;

			return;
		}
	}

	throw ReaderException::AG;
}

