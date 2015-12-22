#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <vector>
using namespace std;

once_flag gOnceFlag;

void initializeSharedResources()
{
	// ... Initialize shared resources that will be used by multiple threads.
	cout << "Shared resources initialized." << endl;
	// While this call_once() call is in progress,
	// other threads block until initializeSharedResources() returns:
}

auto processingFunction()
{
	// Make sure the shared resources are initialized.
	call_once(gOnceFlag, initializeSharedResources);
	// ... Do some work, including using the shared resources
	cout << "Processing" << endl;
}

auto once()
{
	// Launch 3 threads.
	vector<thread> threads(3);
	for (auto& t : threads) {
		t = thread{ processingFunction };
	}
	// Join on all threads
	for (auto& t : threads) {
		t.join();
	}
}

auto calculate()
{
	return "123string";
}
auto future_async()
{
	auto fut = async(calculate);
	//auto fut = async(launch::async, calculate);//新线程
	//auto fut = async(launch::deferred, calculate);//当前线程

	auto res = fut.get();
	cout << "res is " << res << endl;
}

int main()
{
	once();
	future_async();
}
