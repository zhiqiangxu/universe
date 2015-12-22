#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
using namespace std;

int main()
{
	vector<int> myVector(10);
	// iota从1累加
	iota(begin(myVector), end(myVector), 1); // Fill vector with 1,2,3...10
	// Print the contents of the vector.
	copy(begin(myVector), end(myVector), ostream_iterator<int>(cout, " "));


	cout << "Enter numbers separated by white space." << endl;
	cout << "Press Ctrl+Z followed by Enter to stop." << endl;
	istream_iterator<int> numbersIter(cin);
	istream_iterator<int> endIter;
	int sum = accumulate(numbersIter, endIter, 0);
	cout << "Sum: " << sum << endl;
	return 0;
}
