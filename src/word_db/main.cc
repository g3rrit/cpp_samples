#include "word_db.h"

#include <cstdlib>
#include <cstdio>

int main(void) {

    Sentence test = {"hello", "does", "this", "work"};

    for(size_t i = 0; i < 4; i++) {
        printf("word: %s\n", test.get(i));
    }

    return 0;
}
