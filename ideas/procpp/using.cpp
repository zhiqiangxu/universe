#include <iostream>

using std::cout;
using std::endl;

// typedef int MyInt
//type alias sometimes is easier than typedef
using MyInt = int;
MyInt main()
{
	cout << "hello world!" << endl;
	return 0;
}
