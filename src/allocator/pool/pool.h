#include <cstddef>
#include <new>

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
std::byte pool_buffer[POOL_BUFFER_SIZE];
size_t pool_buffer_pos = 0;


template<typename T>
class pool {
public:

    ALLOCATOR_TRAITS(T)

    template<typename U>        
    struct rebind {
        typedef pool<U> other;
    };

    //default constructor
    pool(void) {}

    //copy constructor
    template<typename U>
    pool(pool<U> const& other) {}

    //allocate memory
    pointer allocate(size_type count, const_pointer /* hint */ = 0) {
        size_t size = count * sizeof(type);
        size_t last_pos = pool_buffer_pos;
        if(last_pos + size >= POOL_BUFFER_SIZE) {
            throw std::bad_alloc();
        }
        pool_buffer_pos = last_pos + size;
        return static_cast<pointer>(pool_buffer + last_pos);
    }

    //deallocate memory
    void deallocate(pointer ptr, size_type /* count */) {
        pool_buffer_pos = static_cast<size_t>(ptr - pool_buffer);
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
        typename PolicyT = pool<T>,
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

