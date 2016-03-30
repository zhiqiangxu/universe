#include "StateMachine/StateBuffer.h"



template <typename State>
void StateBuffer<State>::erase_state_buffer(int fd)
{
	this->erase_state(fd);
	erase_buf(fd);
}
