#include "EventHook.hpp"
#include <iostream>
using namespace std;


int main()
{

	using HookGroup = EventHook<int, int, int>;

	auto id1 = HookGroup::get_instance(1).attach([](int a, int b) {
		cout << "1 a = " << a << " b = " << b << endl;
	});
	HookGroup::get_instance(1).attach([](int a, int b) {
		cout << "2 a = " << a << " b = " << b << endl;
	});

	HookGroup::get_instance(1).detach(id1);
	HookGroup::get_instance(1).fire(1, 2);

	cout << "-----------------" << endl;

	using Hook = EventHookGlobal<int, int, int>;

	id1 = Hook::get_instance().attach([](int a, int b) {
		cout << "3 a = " << a << " b = " << b << endl;
	});
	Hook::get_instance().attach([](int a, int b) {
		cout << "4 a = " << a << " b = " << b << endl;
	});

	Hook::get_instance().detach(id1);
	Hook::get_instance().fire(1, 2);

	return 0;
}
