#include <string>
using namespace std;

class IStringStream
{
public:
	IStringStream(string&& str) :_str(move(str)) {}
	void restore(string& backup) { backup = move(_str); }

	template <typename plain_type>
	bool read(plain_type& value, char endian)
	{
		if (sizeof(plain_type) > _str.length() - _offset) return false;

		memcpy(&value, _str.data() + _offset, sizeof(plain_type));
		_offset += sizeof(plain_type);

		switch(endian) {
			case 'N':
				Utils::hton(value);
				break;
			case 'L':
				Utils::htol(value);
				break;
		}

		return true;
	}

	void consume(string str)
	{
	
	}

protected:
	string _str;
	size_t _offset;
};

class StringStream : public IStringStream
{
public:
};
