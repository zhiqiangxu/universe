#include "StreamReader.h"


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

