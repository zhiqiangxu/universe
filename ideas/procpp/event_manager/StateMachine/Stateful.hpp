

template <typename State>
void Stateful<State>::set_state(int id, State state)
{
	_state[id] = state;
}

template <typename State>
bool Stateful<State>::has_state(int id)
{
	return _state.find(id) != _state.end();
}

template <typename State>
State Stateful<State>::get_state(int id, bool* exists)
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
size_t Stateful<State>::count_state()
{
	return _state.size();
}

template <typename State>
void Stateful<State>::erase_state(int id)
{
	_state.erase(id);
}

template <typename State>
void Stateful<State>::clear_state()
{
	_state.clear();
}
