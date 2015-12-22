#include <iostream>
#include <chrono>
using namespace std;


int main()
{
	// Get current time as a time_point
	auto tpoint = chrono::system_clock::now();
	// Convert to a time_t
	time_t tt = chrono::system_clock::to_time_t(tpoint);
	// Convert to local time
	tm* t = localtime(&tt);
	// Convert to readable format
	char buffer[80] = {0};
	strftime(buffer, sizeof(buffer), "%H:%M:%S", t);
	// Write the time to the console
	cout << buffer << endl;

	return 0;
}
