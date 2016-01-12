#include "Memory/SharedMemory.h"
#include <sys/mman.h>
#include <stdio.h>//perror
#include <stdlib.h>//exit
#include <iostream>//cout
using namespace std;

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

static SharedMemory* instance = nullptr;
SharedMemory* SharedMemory::get_instance()
{
	if (instance == nullptr) {
		instance = new SharedMemory;
	}

	return instance;
}

void* SharedMemory::allocate(size_t size)
{
	auto mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	cout << "allocate\t" << size << "\tmem " << mem << endl;

	if (mem == MAP_FAILED) error_exit("mmap");


	return mem;
}

void SharedMemory::deallocate(void* pointer, size_t size)
{
	cout << "deallocate\t" << size << "\tmem " << pointer << endl;

	if (munmap(pointer, size) == -1) error_exit("munmap");
}
