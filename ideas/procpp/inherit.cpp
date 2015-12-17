#include <iostream>

class Abs
{
public:
	virtual void test()
	{
	}
	void nonvirtual()
	{
	}
};

class Ch1 : public Abs
{
public:
	void test()
	{
	}

/** override只对virtual而言
	void nonvirtual() override
	{
	}
**/
};

class Ch2 : public Abs
{
public:
	void test()
	{
	}
};

/* 允许钻形继承 */
class Ch3 : public Ch1, public Ch2
{
public:
	void test()
	{
	}
};

int main()
{
	Ch3 c;
	return 0;
}

