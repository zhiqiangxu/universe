#include <string>
using namespace std;

class IStringStream
{
public:
	IStringStream(string&& str) :_str(move(str)) {}
	void restore(string& backup) { backup = move(_str); }

	template <typename type>
	bool read(type& value)
	{
		if (_str.length() - _offset < sizeof(type) ) return false;

		memcpy(&value, _str.data() + _offset, sizeof(type));
		_offset += sizeof(type);

		return true;
	}

protected:
	string _str;
	size_t _offset;
};

class StringStream : public IStringStream
{
public:
};
