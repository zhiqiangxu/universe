#pragma once
#include <string>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
using namespace std;

//FYI https://github.com/yedf/openssl-example/blob/master/async-ssl-cli.cc

class ISSLConnection
{
public:
    virtual bool initialize() = 0;
    virtual bool handshake() = 0;
    virtual string read() = 0;
    virtual int read(void* buf, int num) = 0;
    virtual int write(const string& message) = 0;
    virtual int write(const void* buf, int num) = 0;
};

class SSLConnection : public ISSLConnection
{
private:
    SSL* _sslHandle;
    bool _handshake_done;
    int _sock;
    bool _client;
    string _buf;

    static SSL_CTX* S_sslContext;
    static void print_error(const string& message);

public:
    SSLConnection(int sock, bool client = true) : _sslHandle(nullptr), _handshake_done(false), _sock(sock), _client(client) {}

    virtual ~SSLConnection()
    {
        if (_sslHandle) {
            SSL_shutdown(_sslHandle);
            SSL_free(_sslHandle);
        }
    }


    static bool initialize_ssl();
    virtual bool initialize() override;
    virtual bool handshake() override;
    virtual string read() override;
    virtual int read(void* buf, int num) override;
    virtual int write(const string& message) override;
    virtual int write(const void* buf, int num) override;

};


