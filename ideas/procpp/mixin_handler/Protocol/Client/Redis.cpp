#include "ReactHandler.h"
#include <string.h>//strlen
#include <stdlib.h>//atol



namespace P { namespace Client {
    //should include it in cpp when **fully** specialize:
    //http://stackoverflow.com/a/4445772
    #include "Protocol/Client/Redis.hpp"

    template<>
    const char* Utils::enum_strings<NXXX>::data[] = {"NX", "XX"};


    void Redis::on_message(int client, string message)
    {
        append_buf(client, message);

        StreamReader s(message);

        size_t offset = 0;

        auto itcb = _callbacks.find(client);

        try {
            do {

                auto r = parse_response(s);
                offset = s.offset();

                if (itcb == _callbacks.end() || itcb->second.empty()) {
                    L.error_log("callbacks empty for redis response");
                    break;
                }

                auto cb = itcb->second.front();
                itcb->second.pop();
                cb.second(r);

                //notify wait
                _scheduler.fire<::Client::ON_PACKET_OK, GUID&>(cb.first);

            } while (!s.end());

        } catch (ReaderException e) {
            //TODO error handle
            switch (e) {
                case ReaderException::AG:
                {
                    need_buf(client, offset > 0 ? message.substr(offset) : message, true);
                    return;
                }
                case ReaderException::NG:
                {
                    _scheduler.close(client);
                    return;
                }
            }

        }

    }

    RedisReply Redis::parse_response(StreamReader& s)
    {
        RedisReply r;

        char next_byte;
        s.read_plain(next_byte);
        r.type = next_byte;

        switch(next_byte) {
            //http://redis.io/topics/protocol
            case '+'://For Simple Strings the first byte of the reply is "+"
            case '-'://For Errors the first byte of the reply is "-"
            case ':'://For Integers the first byte of the reply is ":"
            {
                s.read_until("\r", r.reply, true);
                s.read_plain(next_byte);
                s.fail_if(next_byte != '\n');
                break;
            }
            case '$'://For Bulk Strings the first byte of the reply is "$"
            {
                string length_string;
                s.read_until("\r", length_string, true);
                s.read_plain(next_byte);
                s.fail_if(next_byte != '\n');
                auto length = atol(length_string.c_str());

                if (length >= 0) {
                    if (length > 0) {
                        r.reply.resize(length);
                        s.read_size(length, &r.reply[0]);
                    }

                    char crlf[2];
                    s.read_size(sizeof(crlf), crlf);
                    s.fail_if(strncmp(crlf, "\r\n", 2) != 0);
                } else {
                    r.type = ' ';
                }
                break;
            }
            case '*'://For Arrays the first byte of the reply is "*"
            {
                string length_string;
                s.read_until("\r", length_string, true);
                s.read_plain(next_byte);
                s.fail_if(next_byte != '\n');
                auto length = atol(length_string.c_str());

                if (length > 0) {
                    for (long i = 0; i < length; i++) {
                        r.elements.push_back(parse_response(s));
                    }
                }
                break;
            }
        }

        return r;
    }

    void Redis::on_close(int client)
    {
        erase_buf(client);
        _callbacks.erase(client);
    }

    void Redis::add_callback(GUID& request_id, int fd, RedisCB callback) {
        auto itcb = _callbacks.find(fd);
        if (itcb == _callbacks.end()) {
            _callbacks[fd] = queue<pair<GUID, RedisCB>>( {pair<GUID, RedisCB>({request_id, callback})} );
        } else {
            itcb->second.push(pair<GUID, RedisCB>(request_id, callback));
        }
    }

    // static methods of IRedis
    string IRedis::resp_encode(string* p_bulk_string)
    {
        if (p_bulk_string) {
            auto& bulk_string = *p_bulk_string;

            return resp_encode(bulk_string);
        } else return "$-1\r\n";//Null Bulk String
    }

    string IRedis::resp_encode(char* p_bulk_string)
    {
        auto len = strlen(p_bulk_string);
        return "$" + to_string(len) + "\r\n" + string(p_bulk_string, len) + "\r\n";
    }

    string IRedis::resp_encode(const string& bulk_string)
    {
        return "$" + to_string(bulk_string.length()) + "\r\n" + bulk_string + "\r\n";
    }

    string IRedis::resp_encode(uint64_t n)
    {
        return ":" + to_string(n) + "\r\n";
    }

}}
