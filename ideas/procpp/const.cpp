#include <iostream>

using namespace std;

constexpr int getArraySize() { return 32; }

class Test
{
public:
	void hi() const
	{
		cout << "hello world const" << endl;
	}
	// 如果没有非const，就都会调到const。
	void hi()
	{
		cout << "hello world nonconst" << endl;
	}

	//ilegal according to 
	//http://stackoverflow.com/a/34828179
	constexpr virtual int test() { return 0; };
};

class Derived : public Test
{
public:
	constexpr virtual int test() { return 10; }
};

int main()
{
	int myArray[getArraySize()];
	Test t;
	const Test t2;
	t.hi();
	t2.hi();


	Test* d = new Derived;
	cout << "t.test = " << d->test() << endl;
	return 0;
}
