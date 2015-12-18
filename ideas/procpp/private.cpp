#include <iostream>

using namespace std;

class Base
{
public:
	Base()
	{
	}
	void hi()
	{
		test();
	}
private:
	virtual void test()
	{
		cout << "base" << endl;
	}
};

//私有virtual也能重载
class Child : public Base
{
private:
	void test() override
	{
		cout << "child" << endl;
	}
};

int main()
{
	Child c;
	c.hi();
}
