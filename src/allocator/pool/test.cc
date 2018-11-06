#include "pool.h"
#include <set>
#include <cstdint>

uint8_t buffer[1024];
uint8_t buffer2[1024];

struct Foo {
    int value;

    Foo(int v) :
        value(v) {}

    bool operator<(Foo const& other) const {
        return value < other.value;
    }
};

int main(void) {
    
    {
        std::set<Foo, std::less<Foo>, allocator<Foo, pool<Foo, buffer2, 1024>>> foo;

        foo.insert(Foo(2));
        foo.insert(Foo(3));
        foo.insert(Foo(4));
        foo.insert(Foo(8));
    }

    return 0;
}
