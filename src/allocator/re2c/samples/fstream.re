/*
 * compile with: 
 * re2c --input custom -o fstream.cc fstream.re
 * g++ -o fstream fstream.cc
 */

#include <cstdio>
#include <fstream>

enum Token {
    INTEGER,
    STRING,
    ID,
    OTHER,
    END
};

void print_token(Token token) {
    switch(token) {
        case Token::ID:
            printf("toke id\n");
        default:
            printf("other\n");
    }
}

static Token lex(std::ifstream &in)
{
    std::streampos mar;
#   define YYCTYPE     char
#   define YYPEEK()    in.peek()
#   define YYSKIP()    do { in.ignore(); if (in.eof()) return Token::END; } while(0)
#   define YYBACKUP()  mar = in.tellg()
#   define YYRESTORE() in.seekg(mar)

    /*!re2c
        re2c:yyfill:enable = 0;

        *                       { return Token::OTHER; }
        [a-zA-Z_][a-zA-Z_0-9]*  { return Token::INTEGER; }
    */
}

int main(int argc, char **argv)
{
    //std::ifstream in(stdin, std::ios::binary);
    //if (in.fail()) return 2;

    Token token = Token::OTHER;
    do {
        token = lex(stdin);
        print_token(token);
    } while(token != Token::END);

    return 0;
}
