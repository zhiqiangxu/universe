#pragma once
#include <map>
using namespace std;

template <typename State>
class IStateBuffer
{
public:
	virtual bool need_buf(int id, string& message, bool cond) = 0;
	virtual bool append_buf(int id, string& message) = 0;
	virtual size_t count_buf() = 0;

	virtual void set_state(int id, State state) = 0;
	virtual bool has_state(int id) = 0;
	virtual State get_state(int id, bool* exists) = 0;
	virtual size_t count_state() = 0;

	virtual void erase_state_buffer(int id) = 0;
	virtual void clear_state_buffer() = 0;
};

template <typename State>
class StateBuffer : public IStateBuffer<State>
{
public:

	virtual bool need_buf(int id, string& message, bool cond) override;
	virtual bool append_buf(int id, string& message) override;
	virtual size_t count_buf() override;

	virtual void set_state(int id, State state) override;
	virtual bool has_state(int id) override;
	virtual State get_state(int id, bool* exists = nullptr) override;
	virtual size_t count_state() override;

	virtual void erase_state_buffer(int id) override;
	virtual void clear_state_buffer() override;

private:
	map<int, string> _buf;
	map<int, State> _state;
};

#include "StateMachine/StateBuffer.cpp"
