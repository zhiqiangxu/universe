#pragma once
#include "ReactHandler.h"

class IProcessWorker
{
};


class ProcessWorker : public IProcessWorker
{
public:
	static const int NUMBER_CORES = -1;

	ProcessWorker(ClientServer& server, int n = NUMBER_CORES);


};
