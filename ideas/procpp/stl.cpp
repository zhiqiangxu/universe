#include <iostream>
#include <typeinfo>
using namespace std;

class A
{
public:
	template <typename T>
	void f(const T& t) { cout << "f in A : " << typeid(T).name() << endl; }
};

class B : public A
{
public:
	template <typename T>
	void f(const T& t);

	
};
template <>
void B::f<int*>(int* const & t) { cout << "f in B t =" << t << endl; }

int main()
{
	B b;
	int i;
	b.f(&i);
	return 0;
}
