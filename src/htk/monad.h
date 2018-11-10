#ifndef MONAD_H
#define MONAD_H

#include "maybe.h"

template <template <typename> class m>
struct Monad {
    template <typename a>
    static m<a> mreturn(const a&);

    template <typename a, typename b>
    static m<b> mbind(const m<a>&, m<b>(*)(const a&));
};

template <>
struct Monad<Maybe> {
    template <typename a>
    static Maybe<a> mreturn(const a& v) {
        Maybe<a> x;
        x.nothing = false;
        x.value = v;
        return x;
    }

    template <typename a, typename b>
    static Maybe<b> mbind(const Maybe<a>& action, Maybe<b>(*function)(const a&)) {
        if (action.nothing)
            return action;
        else
            return function(action.value);
    }
};

#endif
