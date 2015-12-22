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
};

int main()
{
	int myArray[getArraySize()];
	Test t;
	const Test t2;
	t.hi();
	t2.hi();
	return 0;
}
