#include "pool.h"
#include <set>
#include <cstdint>
#include <cstdlib>
#include <thread>

uint8_t global_buffer[1024];

struct Foo {
    int value;

    Foo(int v) :
        value(v) {}

    bool operator<(Foo const& other) const {
        return value < other.value;
    }
};

int main(void) {

    for(int i = 0; i < 10; i++) {
        std::thread([] () {
            uint8_t *local_buffer = (uint8_t*)malloc(1024);
            /* not working
            std::set<Foo, std::less<Foo>, allocator<Foo, pool<Foo, local_buffer, 1024>>> foo;
            */

            allocator<Foo, pool<Foo>> a(local_buffer, 1024);
            std::less<Foo> l;

            std::set<Foo, std::less<Foo>, allocator<Foo, pool<Foo>>> foo(l, a);

            foo.insert(Foo(2));
            foo.insert(Foo(3));

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(2s);
        }).detach();
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(9s);


    return 0;
}
