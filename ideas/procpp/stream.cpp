#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>

using namespace std;

auto cout_manip()
{
	cout << __FUNCTION__ << endl;
	bool myBool = true;
	cout << "This is the default: " << myBool << endl;
	cout << "This should be true: " << boolalpha << myBool << endl;
	cout << "This should be 1: " << noboolalpha << myBool << endl;

	// Simulate "%6d" with streams
	int i = 123;
	printf("This should be ' 123': %6d\n", i);
	cout << "This should be ' 123': " << setw(6) << i << endl;
	// Simulate "%06d" with streams
	printf("This should be '000123': %06d\n", i);
	cout << "This should be '000123': " << setfill('0') << setw(6) << i << endl;

	// Fill with *
	cout << "This should be '***123': " << setfill('*') << setw(6) << i << endl;
	// Reset fill character
	cout << setfill(' ');

	// Floating point values
	double dbl = 1.452;
	double dbl2 = 5;
	cout << "This should be ' 5': " << setw(2) << noshowpoint << dbl2 << endl;
	cout << "This should be @@1.452: " << setw(7) << setfill('@') << dbl << endl;
	// Reset fill character
	cout << setfill(' ');

	// Instructs cout to start formatting numbers according to your location.
	// Chapter 18 explains the details of the imbue call and the locale object.
	cout.imbue(locale(""));
	// Format numbers according to your location
	cout << "This is 1234567 formatted according to your location: " << 1234567 <<
	endl;
	// Money amount
	cout << "This should be a money amount of 120000, "
		 << "formatted according to your location: "
		 << put_money("120000") << endl;
	// Date and time
/* put_time在gcc5才有
	time_t t_t = time(nullptr); // Get current system time
	tm* t = localtime(&t_t); // Convert to local time
	cout << "This should be the current date and time "
		 << "formatted according to your location: "
		 << std::put_time(t, "%c") << endl;
*/
	// C++14: Quoted string
	cout << "This should be: \"Quoted string with \\\"embedded quotes\\\".\": "
		 << quoted("Quoted string with \"embedded quotes\".") << endl;

}

auto tie_stream()
{
	cout << __FUNCTION__ << endl;
	ifstream inFile("/tmp/input.txt"); // Note: input.txt must exist.
	ofstream outFile("/tmp/output.txt");
	// Set up a link between inFile and outFile.
	// 当读取inFile时，触发outFile.flush
	inFile.tie(&outFile);
	// Output some text to outFile. Normally, this would
	// not flush because std::endl is not sent.
	outFile << "Hello there!";
	// outFile has NOT been flushed.
	// Read some text from inFile. This will trigger flush()
	// on outFile.
	string nextToken;
	inFile >> nextToken;
	// outFile HAS been flushed.

	cout << "nextToken:" << nextToken << endl;
}

int main()
{
	cout_manip();
	tie_stream();

	return 0;
}
