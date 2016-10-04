#include "SSLConnection.h"
#include "ReactHandler.h"

SSL_CTX* SSLConnection::S_sslContext = nullptr;

bool SSLConnection::initialize_ssl()
{

    if (!S_sslContext) {

        SSL_load_error_strings();
        SSL_library_init();

        S_sslContext = SSL_CTX_new(SSLv23_client_method());

        if (!S_sslContext) {


            print_error("SSL_CTX_new failed");

            return false;

        }
    }

    return true;

}

bool SSLConnection::initialize()
{
    _sslHandle = SSL_new(S_sslContext);
    if (!_sslHandle) {
        print_error("SSL_new failed");
        return false;
    }

    if (!SSL_set_fd(_sslHandle, _sock)) {
        print_error("SSL_set_fd failed");
        return false;
    }

    if (_client) SSL_set_connect_state(_sslHandle);
    else SSL_set_accept_state(_sslHandle);

    return true;
}

bool SSLConnection::handshake()
{
    if (_handshake_done) return true;

    auto r = SSL_do_handshake(_sslHandle);
    if (r == 1) {
        _handshake_done = true;
        if (_buf.length()) write(_buf);
    }
    else print_error("SSL_do_handshake fail");

    L.debug_log("handshake result = " + to_string(r));

    return _handshake_done;
}

string SSLConnection::read()
{

    string message;
    char buf[1024];

    while (true) {
        auto r = SSL_read(_sslHandle, buf, sizeof(buf));
        if (r < 0) {
            auto err = SSL_get_error(_sslHandle, r);
            if (err == SSL_ERROR_WANT_READ) {
                return message;
            } else {
                //error handle
                print_error("SSL_read fail");
            }
        } else if (r == 0) {
            //eof
            break;
        }

        // TODO 避免拷贝
        message.append(buf, r);
    }

    return message;

}

int SSLConnection::read(void* buf, int num)
{
    return SSL_read(_sslHandle, buf, num);
}

int SSLConnection::write(const string& message)
{
    return write(message.data(), message.length());
}

// TODO write buf
int SSLConnection::write(const void* buf, int num)
{
    if (!_handshake_done) {
        _buf.append((const char*)buf, num);
        return num;
    }

    int size = 0;
    do {
        auto len = SSL_write(_sslHandle, buf, num);
        if (len > 0) size += len;
        else return -1;
    } while (size < num);


    return size;
}

void SSLConnection::print_error(const string& message)
{
    L.debug_log(message);
    ERR_print_errors_fp(stderr);
}
