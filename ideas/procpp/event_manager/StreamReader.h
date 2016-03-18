#include <string>
#include <map>
using namespace std;

class IStreamReader
{
public:
	IStreamReader(string&& str) :_str(move(str)) {}
	void restore(string& backup) { backup = move(_str); }

	template <char endian, typename plain_type>
	bool read_plain(plain_type& value)
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

	template <typename plain_type>
	bool read_plain(plain_type& value)
	{

		return true;
	}

	void read_string(string str)
	{
	
	}

	void read_regex(string r)
	{
	}

protected:
	string _str;
	size_t _offset;

	static map<string, regex> _r_cache;
};

class StreamReader : public IStreamReader
{
public:
};
