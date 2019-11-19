#ifndef MABY_H
#define MABY_H

template <typename T>
struct Maybe {
    bool nothing;
    T value;

    Maybe(T const& v) : value(v), nothing(false) {}
    Maybe() : nothing(true) {}
};

#endif
