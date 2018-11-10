#include "monad.h"
#include "maybe.h"
#include <cstdio>

int main(void) {

    Monad<Maybe<>> m(10);

    Monad<Maybe<int>>::mbind(m, [](Maybe<int> r) {
        printf("value: %i\n", r.value); 
    });

    return 0;
}


