#include <iostream>
using namespace std;

class Abs
{
public:
	virtual void test()
	{
	}
	void nonvirtual()
	{
		cout << "nonvirtual" << endl;
	}
};

class Ch1 : public virtual Abs
{
public:
	void test()
	{
		cout << "Ch1" << endl;
	}

/** override只对virtual而言
	void nonvirtual() override
	{
	}
**/
};

class Ch2 : public virtual Abs
{
public:
	void test()
	{
		cout << "Ch2" << endl;
	}
};

/* 允许钻形继承 */
class Ch3 : public Ch1, public Ch2
{
public:
	void test()
	{
		cout << "Ch3" << endl;
	}
};

int main()
{
	Ch3 c;
	c.test();
	// 利用virtual继承，解决歧义问题
	c.nonvirtual();
	return 0;
}

