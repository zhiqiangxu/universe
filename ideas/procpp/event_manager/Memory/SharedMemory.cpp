#include "Memory/SharedMemory.h"
#include <sys/mman.h>

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

void* SharedMemory::allocate(size_t size)
{
	size += sizeof(size)
	auto mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED) error_exit("mmap");

	*reinterpret_cast<size_t*>(mem) = size;

	return mem + sizeof(size)
}

void SharedMemory::deallocate(void* pointer, size_t size)
{
	auto mem = pointer - sizeof(size);
	auto size = *reinterpret_cast<size_t*>(mem);
	if (munmap(mem, size) == -1) error_exit("munmap");
}
