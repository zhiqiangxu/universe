#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

auto hello_atomic()
{
	atomic<int> counter(0);
	++counter;//线程安全

	cout << "counter is " << counter << endl;
}

auto func(atomic<int>& counter)
{
	for (int i = 0; i < 100; ++i) {
		++counter;
		this_thread::sleep_for(chrono::milliseconds(1));
	}
}
auto thread_atomic()
{
	atomic<int> counter(0);
	vector<thread> threads;
	for (int i = 0; i < 10; ++i) {
		threads.push_back(thread{ func, ref(counter) });
	}
	for (auto& t : threads) {
		t.join();
	}
	cout << "Result = " << counter << endl;
}

int main()
{
	hello_atomic();
	thread_atomic();
	return 0;
}
