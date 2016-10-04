#include "StateMachine/Bufferable.h"


bool Bufferable::need_buf(int id, const string& message, bool cond)
{
    if (cond) {
        _buf[id] = message;
        return true;
    }

    return false;
}

bool Bufferable::append_buf(int id, string& message)
{
    if (_buf.find(id) != _buf.end()) {
        message = _buf[id] + message;
        _buf.erase(id);

        return true;
    }

    return false;
}

size_t Bufferable::count_buf()
{
    return _buf.size();
}

bool Bufferable::has_buf(int id)
{
    return _buf.find(id) != _buf.end();
}

string Bufferable::get_buf(int id, bool clear)
{
    auto buf = _buf[id];
    if (clear) _buf.erase(id);

    return buf;
}

void Bufferable::erase_buf(int id)
{
    _buf.erase(id);
}

void Bufferable::clear_buf()
{
    _buf.clear();
}
