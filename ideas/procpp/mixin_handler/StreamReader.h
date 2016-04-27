#pragma once
#include <string>
#include "Utils.h"
using namespace std;

enum class ReaderException
{
	NG,
	AG,
};


class IStreamReader
{
public:
	//读取定长

	IStreamReader(const string& str) : _str(str), _offset(0) {}

	template <bool network_byte_order, typename plain_type>
	void read_plain(plain_type& value)
	{
		if (sizeof(plain_type) > _str.length() - _offset) throw ReaderException::AG;

		memcpy(&value, _str.data() + _offset, sizeof(plain_type));
		_offset += sizeof(plain_type);

		if (network_byte_order) Utils::hton(value);
		else Utils::htol(value);
	}

	template <bool network_byte_order, typename plain_type>
	void pread_plain(plain_type& value)
	{
		if (sizeof(plain_type) > _str.length() - _offset) throw ReaderException::AG;

		memcpy(&value, _str.data() + _offset, sizeof(plain_type));

		if (network_byte_order) Utils::hton(value);
		else Utils::htol(value);
	}

	//caller is responsible for endian handling
	template <typename plain_type>
	void read_plain(plain_type& value)
	{
		if (sizeof(plain_type) > _str.length() - _offset) throw ReaderException::AG;

		memcpy(&value, _str.data() + _offset, sizeof(plain_type));
		_offset += sizeof(plain_type);
	}

	template <typename plain_type>
	void pread_plain(plain_type& value)
	{
		if (sizeof(plain_type) > _str.length() - _offset) throw ReaderException::AG;

		memcpy(&value, _str.data() + _offset, sizeof(plain_type));
	}

	virtual void read_size(size_t size, void* p) = 0;
	virtual void pread_size(size_t size, void* p) = 0;

	//读取一个**字**
	virtual bool read_character(Encoding e, uint32_t& codepoint) = 0;
	//读取N个字
	virtual bool read_characters(Encoding e, uint32_t* p_codepoint, int n) = 0;
	template <Encoding e>
	bool pread_character(uint32_t& codepoint, size_t& length);
	template <Encoding e>
	bool read_character(uint32_t& codepoint);


	//吃尽s
	virtual void read_up(const string& s, bool nullable, string* result) = 0;
	//吃尽s
	template <Encoding e>
	void read_up(const string& s, bool nullable, string* result);
	virtual void read_up(Encoding e, const string& s, bool nullable, string* result) = 0;

	//吃到s
	virtual void read_until(const string& s, string& result, bool eatup_final) = 0;

	virtual void fail_if(bool exp) = 0;
	virtual void fail_unless(bool exp) = 0;

    //到头没?
    virtual bool end() = 0;

    virtual size_t offset() = 0;
protected:
	const string& _str;
	size_t _offset;

};

class StreamReader : public IStreamReader
{
public:
	using IStreamReader::IStreamReader;
	using IStreamReader::read_character;
	using IStreamReader::read_up;

	virtual void read_size(size_t size, void* p) override;
	virtual void pread_size(size_t size, void* p) override;

	virtual bool read_character(Encoding e, uint32_t& codepoint) override;
	virtual bool read_characters(Encoding e, uint32_t* p_codepoint, int n) override;

	virtual void read_up(const string& s, bool nullable = true, string* result = nullptr) override;
	virtual void read_up(Encoding e, const string& s, bool nullable = true, string* result = nullptr) override;

	virtual void read_until(const string& s, string& result, bool eatup_final = false) override;

	virtual void fail_if(bool exp) override { if (exp) throw ReaderException::NG; }
	virtual void fail_unless(bool exp) override { if (!exp) throw ReaderException::NG; }

    virtual bool end() override { return _offset >= _str.length(); }

    virtual size_t offset() override { return _offset; };
};

#include "StreamReader.hpp"
