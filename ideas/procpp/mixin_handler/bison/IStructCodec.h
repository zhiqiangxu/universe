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

    void on_message(
                     int client, string message,
                     function<int client, void(ResultClass &r)> on_ok,
                     function<void(int client)> on_error,
                     function<void(int client)> on_again = []{}
                    );
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

    void on_message(
                     int client, string message,
                     function<int client, void(ResultClass &r)> on_ok,
                     function<void(int client)> on_error,
                     function<void(int client)> on_again = []{}
                    );
    */

    virtual void on_close(int client) = 0;

protected:
    size_t _parsed_length = 0;
};

