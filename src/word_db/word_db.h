#pragma once

#include <cstdlib>
#include <initializer_list>

struct Sentence {
    size_t len;
    size_t size;
    char *words;

    Sentence(char *_words, size_t _len);

    Sentence(std::initializer_list<char*> list);

    ~Sentence();

    char *get(size_t pos) const;
};

class Word_Db {
private:
    Sentence *container;

public:

   
};
