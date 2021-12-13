#ifndef _DEFER_H_
#define _DEFER_H_

#include<functional>

#define CAT(a,b) CAT2(a,b)
#define CAT2(a,b) a##b
#define UNIQUE_ID CAT(_uid_,__LINE__)

#define defer \
    auto UNIQUE_ID = _Defer {}; UNIQUE_ID.fn = [&]()

struct _Defer {
    std::function<void()> fn = nullptr;

    ~_Defer() {
        if (fn != nullptr) {
            fn();
        }
    }
};

#endif /* _DEFER_H_ */
