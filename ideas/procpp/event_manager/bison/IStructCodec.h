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
	virtual ParseResult on_message(int client, string message) = 0;


	virtual void on_close(int client) = 0;
};

template <typename T>
class IStateStructCodec : public Bufferable
{
public:
	virtual ParseResult on_message(int client, string message, T state) = 0;

	virtual void on_close(int client) = 0;
};

