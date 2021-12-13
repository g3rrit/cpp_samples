#include "defer.hh"
#include <cstdio>

int main() {

    defer {
        std::printf("defer1\n");
    };

    defer {
        std::printf("defer2\n");
    };

    do {
        defer {
            std::printf("defer3\n");
        };

    } while(0);

    defer {
        std::printf("defer4\n");
    };

    return 0;
}
