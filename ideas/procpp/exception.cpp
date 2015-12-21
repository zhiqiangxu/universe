#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;


/***
Throw lists don’t prevent functions from throwing unlisted exception
types, but they prevent the exception from leaving the function, resulting in
a run-time error.
****/
void readIntegerFile(const string& fileName, vector<int>& dest)
throw(invalid_argument, runtime_error)
{
	throw 5;
}

int main()
{
	vector<int> myInts;
	const string fileName = "IntegerFile.txt";
	try {
	readIntegerFile(fileName, myInts);
	} catch (int x) {
	cerr << "Caught int " << x << endl;
	}
	return 0;
}
