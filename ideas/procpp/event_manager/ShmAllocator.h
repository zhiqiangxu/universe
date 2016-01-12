#pragma once

template<typename T, typename Policy = ShmAllocPolicy<T>, typename Traits = ObjectTraits<T> >
class Allocator : public Policy, public Traits {
private:
    typedef Policy AllocationPolicy;
    typedef Traits TTraits;

public:
    typedef typename AllocationPolicy::size_type size_type;
    typedef typename AllocationPolicy::difference_type difference_type;
    typedef typename AllocationPolicy::pointer pointer;
    typedef typename AllocationPolicy::const_pointer const_pointer;
    typedef typename AllocationPolicy::reference reference;
    typedef typename AllocationPolicy::const_reference const_reference;
    typedef typename AllocationPolicy::value_type value_type;

public:
    template<typename U>
    struct rebind {
        typedef Allocator<U, typename AllocationPolicy::rebind<U>::other, 
            typename TTraits::rebind<U>::other > other;
    };

public:
    inline explicit Allocator() {}
    inline ~Allocator() {}
    inline Allocator(Allocator const& rhs):Traits(rhs), Policy(rhs) {}
    template <typename U>
    inline Allocator(Allocator<U> const&) {}
    template <typename U, typename P, typename T2>
    inline Allocator(Allocator<U, P, 
       T2> const& rhs):Traits(rhs), Policy(rhs) {}
};    //    end of class Allocator

// determines if memory from another
// allocator can be deallocated from this one
template<typename T, typename P, typename Tr>
inline bool operator==(Allocator<T, P,
   Tr> const& lhs, Allocator<T,
   P, Tr> const& rhs) {
    return operator==(static_cast<P&>(lhs),
                       static_cast<P&>(rhs));
}
template<typename T, typename P, typename Tr,
        typename T2, typename P2, typename Tr2>
inline bool operator==(Allocator<T, P,
    Tr> const& lhs, Allocator<T2, P2, Tr2> const& rhs) {
      return operator==(static_cast<P&>(lhs),
                       static_cast<P2&>(rhs));
}
template<typename T, typename P, typename Tr, typename OtherAllocator>
inline bool operator==(Allocator<T, P, 
          Tr> const& lhs, OtherAllocator const& rhs) { 
    return operator==(static_cast<P&>(lhs), rhs); 
}
template<typename T, typename P, typename Tr>
inline bool operator!=(Allocator<T, P, Tr> const& lhs, 
                         Allocator<T, P, Tr> const& rhs) { 
    return !operator==(lhs, rhs); 
}
template<typename T, typename P, typename Tr, 
           typename T2, typename P2, typename Tr2>
inline bool operator!=(Allocator<T, P, Tr> const& lhs, 
                   Allocator<T2, P2, Tr2> const& rhs) { 
    return !operator==(lhs, rhs);
}
template<typename T, typename P, typename Tr, 
                              typename OtherAllocator>
inline bool operator!=(Allocator<T, P,
        Tr> const& lhs, OtherAllocator const& rhs) {
    return !operator==(lhs, rhs);
}
