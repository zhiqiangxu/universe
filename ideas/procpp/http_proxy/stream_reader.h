#pragma once
#include <string>
#include <string.h>//memcpy
#include "utils.h"


enum class ReaderException
{
    NG,
    AG,
};


using std::string;

class StreamReader
{
  public:
    StreamReader(const string& str, size_t offset = 0) : _str(str), _offset(offset) {}

    virtual void read_size(size_t size, void* p = nullptr/*result ignored, not used*/);
    virtual void read_size(size_t size, string& result);
    //not used
    virtual void read_size_offset(size_t size, void* p, size_t offset);
    virtual void pread_size(size_t size, void* p);

    virtual void read_up(const string& s, bool nullable = true, string* result = nullptr);

    virtual void read_until(const string& s, string& result, bool eatup_final = false);

    virtual void fail_if(bool exp) { if (exp) throw ReaderException::NG; }
    virtual void fail_unless(bool exp) { if (!exp) throw ReaderException::NG; }

    virtual bool end() { return _offset >= _str.length(); }

    virtual size_t offset() { return _offset; };

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

  protected:
    const string& _str;
    size_t _offset;
};

