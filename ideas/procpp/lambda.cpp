#include <iostream>
#include <vector>
#include <functional>

using namespace std;

auto hello()
{
	auto basicLambda = []{ cout << "Hello from Lambda" << endl; };
	basicLambda();
}

auto multiplyBy2Lambda(int x)
{
	return [x]{ return 2 * x; };
}

auto high_function()
{
	auto fn = multiplyBy2Lambda(5);
	cout << fn() << endl;
}

void func(int num, const string& str)
{
	cout << "func(" << num << ", " << str << ")" << endl;
}

auto bind_function()
{
	string myString = "abc";
	auto f1 = bind(func, placeholders::_1, myString);
	f1(16);
}

auto remove_empty_string(vector<string>& strings)
{
//需要升级g++才有remove_if
/*
	auto it = remove_if(begin(strings), end(strings),
		[](const string& str){ return str.empty(); });
	// Erase the removed elements.
	strings.erase(it, end(strings));
*/
}

auto remove_erase_idiom()
{
	vector<string> myVector = {"", "one", "", "two", "three", "four"};
	for (auto& str : myVector) { cout << "\"" << str << "\" "; }
	cout << endl;
	remove_empty_string(myVector);
	for (auto& str : myVector) { cout << "\"" << str << "\" "; }
	cout << endl;
}

int main()
{
	hello();
	high_function();
	bind_function();
	remove_erase_idiom();
	return 0;
}
