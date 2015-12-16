#include <iostream>

using namespace std;

int main()
{
	string s = "1234l"s;
	int __i = stoi(s);
	s = R"abc( multiple line
			statement example)abc";

	cout << __i << endl << s << endl;
	return 0;
}
