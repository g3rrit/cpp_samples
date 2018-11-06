#include "pool.h"
#include <set>

struct Foo {
    int value;

    Foo(int v) :
        value(v) {}

};

int main(void) {
    
    {
        std::set<Foo, std::less<Foo>, allocator<Foo, pool<Foo>>> foo;

        foo.insert(Foo(2));
        foo.insert(Foo(3));
        foo.insert(Foo(4));
        foo.insert(Foo(8));
    }

    return 0;
}
