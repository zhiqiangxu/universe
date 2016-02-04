#include <string>
using namespace std;

enum class ParseResult
{
	AGAIN,
	OK,
	NG
};

class IStructCodec : public Bufferable
{
public:
	/* template can't be virtual, so commented...

	virtual ParseResult on_message(int client, string message, ResultClass &r);
	*/

	virtual void on_close(int client) = 0;

protected:
	size_t _parsed_length = 0;
};

template <typename T>
class IStateStructCodec : public Bufferable
{
public:
	/* template can't be virtual, so commented...

	virtual ParseResult on_message(int client, string message, T state, ResultClass &r);
	*/

	virtual void on_close(int client) = 0;

protected:
	size_t _parsed_length = 0;
};

