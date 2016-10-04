#pragma once
#include <stdlib.h>

class ISharedMemory
{
public:
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* pointer, size_t size) = 0;
};

class SharedMemory : public ISharedMemory
{
public:
    static SharedMemory* get_instance();

    virtual void* allocate(size_t size) override;
    virtual void deallocate(void* pointer, size_t size) override;
};
