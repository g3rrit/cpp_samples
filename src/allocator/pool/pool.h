#include <cstddef>
#include <new>
#include <cstdint>
#include <cstdio>

template<typename T>
class object_traits {

public:

    typedef T type;
     
    template<typename U>
    struct rebind {
        typedef object_traits<U> other;
    };

    //constructor 
    object_traits(void) {} 

    //copy construtor
    template<typename U>
        object_traits(object_traits<U> const& other) {}

    //address oj object
    type* address(type& obj) const { return &obj; }
    type const* address(type const& obj) const { return &obj; }

    //construct object
    void construct(type* ptr, type const& ref) const {
        //in place copy constructor
        new(ptr) type(ref);
    }

    //destroy object
    void destroy(type* ptr) const {
        //call destructor
        ptr->~type();
    }
};

#define ALLOCATOR_TRAITS(T)                         \
    typedef T                   type;               \
    typedef type                value_type;         \
    typedef value_type*         pointer;            \
    typedef value_type const*   const_pointer;      \
    typedef value_type&         reference;          \
    typedef value_type const&   const_reference;     \
    typedef std::size_t         size_type;          \
    typedef std::ptrdiff_t      difference_type;

/*
template<typename T>
struct max_allocations {
    enum { value = static_cast<std::size>(-1) / sizeof(T) };
};
*/

#define POOL_BUFFER_SIZE 1024
uint8_t pool_buffer[POOL_BUFFER_SIZE];
size_t pool_buffer_pos = 0;


template<typename T, uint8_t* pb, size_t pb_size>
class pool {
private:
    size_t pb_pos = 0;
public:

    ALLOCATOR_TRAITS(T)

    template<typename U>        
    struct rebind {
        typedef pool<U, pb, pb_size> other;
    };

    //default constructor
    pool(void) {}

    //copy constructor
    template<typename U>
    pool(pool<U, pb, pb_size> const& other) {}

    //allocate memory
    pointer allocate(size_type count, const_pointer /* hint */ = 0) {
        size_t size = count * sizeof(type);
        size_t last_pos = pb_pos;
        if(last_pos + size >= pb_size) {
            throw std::bad_alloc();
        }
        pb_pos = last_pos + size;
        printf("allocating mem: %p\n", pb + last_pos);
        return reinterpret_cast<pointer>(pb + last_pos);
    }

    //deallocate memory
    void deallocate(pointer ptr, size_type /* count */) {
        printf("deallocating mem: %p\n", ptr);
        //pool_buffer_pos = static_cast<size_t>(ptr - pool_buffer);
    }

    //max number of objects that can be allocated in one call
    size_type max_size(void) const {
        return static_cast<size_t>(POOL_BUFFER_SIZE / sizeof(T));
    }
};


#define FORWARD_ALLOCATOR_TRAITS(C)                         \
    typedef typename C::value_type      value_type;         \
    typedef typename C::pointer         pointer;            \
    typedef typename C::const_pointer   const_pointer;      \
    typedef typename C::reference       reference;          \
    typedef typename C::const_reference const_reference;    \
    typedef typename C::size_type       size_type;          \
    typedef typename C::difference_type difference_type;


template <typename T,
        typename PolicyT,
        typename TraitsT = object_traits<T> >
class allocator : public PolicyT, public TraitsT {

public:

    //template parameters
    typedef PolicyT Policy;
    typedef TraitsT Traits;

    FORWARD_ALLOCATOR_TRAITS(PolicyT)
        
    template<typename U>
    struct rebind {
        typedef allocator<U,
                typename Policy::template rebind<U>::other,
                typename Traits::template rebind<U>::other
                    > other;
    };

    //constructor
    allocator(void) {}

    //copy constructor
    template<typename U,
        typename PolicyU,
        typename TraitsU>
    allocator(allocator<U,
            PolicyU, TraitsU> const& other) : 
        Policy(other),
        Traits(other) {}

};


/*
//Two allocators are not equal unless a specialization says so
template<typename T, typename PolicyT, typename TraitsT,
    typename U, typename PolicyU, typename TraitsU>
bool operator==(allocator<T, PolicyT, TraitsT> const& left,
        allocator<U, PolicyU, TraitsU> const& right)
{
    return false;
}

//Also implement inequality
template<typename T, typename PolicyT, typename TraitsT,
    typename U, typename PolicyU, typename TraitsU>
bool operator!=(allocator<T, PolicyT, TraitsT> const& left,
        allocator<U, PolicyU, TraitsU> const& right)
{
    return !(left == right);
}

//Comparing an allocator to anything else should not show equality
template<typename T, typename PolicyT, typename TraitsT,
    typename OtherAllocator>
bool operator==(allocator<T, PolicyT, TraitsT> const& left,
        OtherAllocator const& right)
{
    return false;
}

//Also implement inequality
template<typename T, typename PolicyT, typename TraitsT,
    typename OtherAllocator>
bool operator!=(allocator<T, PolicyT, TraitsT> const& left,
        OtherAllocator const& right)
{
    return !(left == right);
}

//Specialize for the pool policy
template<typename T, typename TraitsT,
    typename U, typename TraitsU>
bool operator==(allocator<T, pool<T>, TraitsT> const& left,
        allocator<U, pool<U>, TraitsU> const& right)
{
    return true;
}

//Also implement inequality
template<typename T, typename TraitsT,
    typename U, typename TraitsU>
bool operator!=(allocator<T, pool<T>, TraitsT> const& left,
        allocator<U, pool<U>, TraitsU> const& right)
{
    return !(left == right);
} 
*/
