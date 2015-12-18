#include <iostream>
using namespace std;


class A
{
public:
	virtual void test() = 0;
};

class B : public A
{
public:
	void test() override
	{
		cout << "B" << endl;
	}
	void test(char c)
	{
		cout << "B char" << endl;
	}
};

class C : public B
{
public:

	/* 下面的test方法导致父类的同名方法被隐藏，需要显式继承 */
	using B::test;
	void test(int i)
	{
		cout << "C" << endl;
	}
};

int main()
{
	C c;
	c.test(1);
	c.test();
	c.test('a');
	return 0;
}
