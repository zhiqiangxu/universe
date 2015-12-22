#include <iostream>
#include <string>
#include <regex>
using namespace std;

auto match()
{
	smatch m;
	string str = "hello world";
	regex r("[a-z ]*");
	cout << (regex_match(str, m, r) ? m[0] : string("empty")) << endl;
}

auto search()
{
	smatch m;
	string str = "hello world";
	regex r("[a-z]*");
	cout << (regex_search(str, m, r) ? m[0] : string("empty")) << endl;
}

auto replace()
{
	const string str("<body><h1>Header</h1><p>Some text</p></body>");
	regex r("<h1>(.*)</h1><p>(.*)</p>");
	const string format("H1=$1 and P=$2");
	string result = regex_replace(str, r, format);
	cout << "Original string: '" << str << "'" << endl;
	cout << "New string : '" << result << "'" << endl;
}

int main()
{
	match();
	search();
	replace();
	return 0;
}
