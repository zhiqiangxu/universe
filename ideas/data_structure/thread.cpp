#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

static std::mutex barrier;


//This function will be called from a thread

void func(int tid) {
    std::cout << "Launched by thread " << tid << std::endl;
}

void hello() {
    std::vector<std::thread> th;

    int nr_threads = 10;

    //Launch a group of threads
    for (int i = 0; i < nr_threads; ++i) {
        th.push_back(std::thread(func,i));
    }

    //Join the threads with the main thread
    for(auto &t : th){
        t.join();
    }
}

void dot_product(const std::vector<int> &v1, const std::vector<int> &v2, int &result, int L, int R){
	int sum = 0;
    for(int i = L; i < R; ++i){
        sum += v1[i] * v2[i];
    }

	std::lock_guard<std::mutex> block_threads_until_finish_this_job(barrier);
	result += sum;
}

std::vector<int> bounds(int parts, int mem) {
    std::vector<int>bnd;
    int delta = mem / parts;
    int reminder = mem % parts;
    int N1 = 0, N2 = 0;
    bnd.push_back(N1);
    for (int i = 0; i < parts; ++i) {
        N2 = N1 + delta;
        if (i == parts - 1)
            N2 += reminder;
        bnd.push_back(N2);
        N1 = N2;
    }
    return bnd;
}

void race() {
    int nr_elements = 100000;
    int nr_threads = 2;
    int result = 0;
    std::vector<std::thread> threads;

    //Fill two vectors with some constant values for a quick verification
    // v1={1,1,1,1,...,1}
    // v2={2,2,2,2,...,2}
    // The result of the dot_product should be 200000 for this particular case
    std::vector<int> v1(nr_elements,1), v2(nr_elements,2);

    //Split nr_elements into nr_threads parts
    std::vector<int> limits = bounds(nr_threads, nr_elements);

    //Launch nr_threads threads:
    for (int i = 0; i < nr_threads; ++i) {
        threads.push_back(std::thread(dot_product, std::ref(v1), std::ref(v2), std::ref(result), limits[i], limits[i+1]));
    }


    //Join the threads with the main thread
    for(auto &t : threads){
        t.join();
    }

    //Print the result
    std::cout<<result<<std::endl;
}

void dot_product_atomic(const std::vector<int> &v1, const std::vector<int> &v2, std::atomic<int> &result, int L, int R){
	int sum = 0;
    for(int i = L; i < R; ++i){
        sum += v1[i] * v2[i];
    }

	result += sum;
}
void race_atomic() {
    int nr_elements = 100000;
    int nr_threads = 2;
    std::atomic<int> result(0);

    std::vector<std::thread> threads;

    //Fill two vectors with some constant values for a quick verification
    // v1={1,1,1,1,...,1}
    // v2={2,2,2,2,...,2}
    // The result of the dot_product should be 200000 for this particular case
    std::vector<int> v1(nr_elements,1), v2(nr_elements,2);

    //Split nr_elements into nr_threads parts
    std::vector<int> limits = bounds(nr_threads, nr_elements);

    //Launch nr_threads threads:
    for (int i = 0; i < nr_threads; ++i) {
        threads.push_back(std::thread(dot_product_atomic, std::ref(v1), std::ref(v2), std::ref(result), limits[i], limits[i+1]));
    }


    //Join the threads with the main thread
    for(auto &t : threads){
        t.join();
    }

    //Print the result
    std::cout<<result<<std::endl;
}

int main() {
	//hello();
	//race();
	race_atomic();
}
