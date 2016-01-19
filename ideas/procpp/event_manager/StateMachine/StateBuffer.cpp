template <typename State>
bool StateBuffer<State>::need_buf(int id, string& message, bool cond)
{
	if (cond) {
		_buf[id] = message;
		return true;
	}

	return false;
}

template <typename State>
bool StateBuffer<State>::append_buf(int id, string& message)
{
	if (_buf.find(id) != _buf.end()) {
		message = _buf[id] + message;
		_buf.erase(id);

		return true;
	}

	return false;
}

template <typename State>
size_t StateBuffer<State>::count_buf()
{
	return _buf.size();
}

template <typename State>
void StateBuffer<State>::set_state(int id, State state)
{
	_state[id] = state;
}

template <typename State>
bool StateBuffer<State>::has_state(int id)
{
	return _state.find(id) != _state.end();
}

template <typename State>
State StateBuffer<State>::get_state(int id, bool* exists)
{
	if (_state.find(id) != _state.end()) {
		if (exists) *exists = true;

		return _state[id];
	} else {
		if (exists) *exists = false;

		return State();
	}
}

template <typename State>
size_t StateBuffer<State>::count_state()
{
	return _state.size();
}

template <typename State>
void StateBuffer<State>::erase_state_buffer(int id)
{
	_buf.erase(id);
	_state.erase(id);
}

template <typename State>
void StateBuffer<State>::clear_state_buffer()
{
	_buf.clear();
	_state.clear();
}
