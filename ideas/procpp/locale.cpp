#include <iostream>
#include <locale>
#include <chrono>
using namespace std;

#define LAN "en_US.UTF8"
int main()
{
	locale loc(LAN);
	cout << loc.name() << endl;
	wcout.imbue(locale(LAN));
	wstring pounds = use_facet<moneypunct<wchar_t>>(loc).curr_symbol();
	wcout << 32767 << pounds << endl;


	return 0;
}
