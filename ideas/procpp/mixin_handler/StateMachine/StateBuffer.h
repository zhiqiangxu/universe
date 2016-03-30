#pragma once
#include "StateMachine/Bufferable.h"
#include "StateMachine/Stateful.h"

class IStateBuffer
{
public:

	virtual void erase_state_buffer(int fd) = 0;
};

template <typename State>
class StateBuffer : public IStateBuffer, public Bufferable, public Stateful<State>
{
public:
	virtual void erase_state_buffer(int fd) override;
};

#include "StateMachine/StateBuffer.hpp"
