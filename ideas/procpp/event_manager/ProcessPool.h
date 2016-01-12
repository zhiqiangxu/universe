#pragma once



class IProcessPool
{
public:
	using Task = function<void(void)>;
	virtual void execute(Task t) = 0;
};

class ProcessPool : public IProcessPool
{
public:
	ThreadPool(int n = 0);

	virtual void execute(Task t) override;

};

