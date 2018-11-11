#include "word_db.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

Sentence::Sentence(char *_words, size_t _len) : 
    len(_len) {
    size = 0;

    //calculate size of words array
    for(;; size++) {
        if(_len < 0) {
            break;
        }

        if(_words[size] == 0) {
            _len--;
        }
    }

    words = (char*) malloc(size);
    if(!words) {
        printf("allocation failed\n");
        exit(-1);
    }

    memcpy(words, _words, size);
}


Sentence::Sentence(std::initializer_list<char*> list) {
    size = 0;
    len = 0;

    for(char *word : list) {
        size += strlen(word) + 1;
        len++;
    }

    words = (char*) malloc(size);
    memset(words, 0, size);

    size_t pos = 0;
    size_t wlen = 0;
    for(char *word : list) {
        wlen = strlen(word);
        memcpy(words + pos, word, wlen); 
        pos += wlen + 1;
    }
}


Sentence::~Sentence() {
    free(words);
}

char *Sentence::get(size_t pos) const {
    if(pos >= len) {
        return 0;
    }

    char *res = words;
    while(pos) {
        while(*res) {
            res++;
        }
        res++;
        pos--;
    }
    return res;
}

