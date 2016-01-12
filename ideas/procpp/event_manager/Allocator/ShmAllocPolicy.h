#pragma once
#include "Memory/SharedMemory.h"
#include <limits>
#include <memory>

template<typename T>
class ShmAllocPolicy {
public : 
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public : 
    //    convert an ShmAllocPolicy<T> to ShmAllocPolicy<U>
    template<typename U>
    struct rebind {
        typedef ShmAllocPolicy<U> other;
    };

public : 
    inline explicit ShmAllocPolicy() {}
    inline ~ShmAllocPolicy() {}
    inline explicit ShmAllocPolicy(ShmAllocPolicy const&) {}
    template <typename U>
    inline explicit ShmAllocPolicy(ShmAllocPolicy<U> const&) {}
    
    //    memory allocation
    inline pointer allocate(size_type cnt, 
      typename std::allocator<void>::const_pointer = 0) {
        return reinterpret_cast<pointer>(SharedMemory::get_instance()->allocate(cnt * sizeof (T)));
    }
    inline void deallocate(pointer p, size_type size)
                            { SharedMemory::get_instance()->deallocate(p, size); }

    //    size
    inline size_type max_size() const {
        return std::numeric_limits<size_type>::max();
    }
};    //    end of class ShmAllocPolicy

// determines if memory from another
// allocator can be deallocated from this one
template<typename T, typename T2>
inline bool operator==(ShmAllocPolicy<T> const&,
                        ShmAllocPolicy<T2> const&) {
    return true;
}
template<typename T, typename OtherAllocator>
inline bool operator==(ShmAllocPolicy<T> const&,
                                     OtherAllocator const&) {
    return false;
}
