#pragma once

class ISharedMemory
{
public:
	virtual void* allocate(size_t size) = 0;
	virtual void deallocate(void* pointer, size_t size) = 0;
};

class SharedMemory : public ISharedMemory
{
public:
	virtual void* allocate(size_t size) override;
	virtual void deallocate(void* pointer, size_t size) override;
};
