#include <initializer_list>
#include <iostream>

using namespace std;

auto test(initializer_list<int> args)
{
	cout << "size of args: " << args.size() << endl;
	for (auto value : args) {
		cout << value << endl;
	}
}


class Foo
{
	int i = 10;//in-class memory initializer since c++11
public:
	Foo(int i)
	{
		cout << "Foo ctor:" << i << endl;
		// ctor中的virtual调用指向本身
		print();
	}
	Foo()
	{
		cout << "Foo ctor empty" << endl;
		// ctor中的virtual指向本身
		print();
	}
	~Foo()
	{
		cout << "Foo dtor" << endl;
		// dtor中的virtual指向本身
		print();
	}

	virtual void print()
	{
		cout << "i=" << i << endl;
	}
};

class Bar : public Foo
{
	int j = 10;
public:
	Bar() : Foo(15)
	{
		
	}

	void print() override
	{
		cout << "j=" << j << endl;
	}
};

int main()
{
	test({10,20,30});

	Foo f;

	Bar b;
	b.print();

	return 0;
}
