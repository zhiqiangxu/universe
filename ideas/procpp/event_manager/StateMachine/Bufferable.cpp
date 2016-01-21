#include "StateMachine/Bufferable.h"


bool Bufferable::need_buf(int id, string& message, bool cond)
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


void Bufferable::erase_buf(int id)
{
	_buf.erase(id);
}

void Bufferable::clear_buf()
{
	_buf.clear();
}
