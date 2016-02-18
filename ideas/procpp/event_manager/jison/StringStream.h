#include <string>
using namespace std;

class IStringStream
{
public:
	IStringStream(string&& str) :_str(move(str)) {}
	void restore(string& backup) { backup = move(_str); }

	template <typename type>
	void read(type& value)
	{
		memcpy(&value, _str.data() + _offset, sizeof(type));
		_offset += sizeof(type);
	}

	//for optimal performance, size check is the responsibility of caller side
	bool can_read(size_t size)
	{
		return size <= _str.length() - _offset;
	}

protected:
	string _str;
	size_t _offset;
};

class StringStream : public IStringStream
{
public:
};
