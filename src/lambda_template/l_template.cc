#include <iostream>

template<typename T>
T get_unitialized() {
  struct alignas(T) {
    char bytes[sizeof(T)] {};
  } buffer {};

  return reinterpret_cast<T const&>(buffer);
}

template<typename Func>
struct foo {
    static Func fun;
};

template<typename Func>
Func foo<Func>::fun = get_unitialized<Func>();

int main(void) {

  auto fun = [] (int x) {
    std::cout << "x is: " << x << std::endl;
  };

  foo<decltype(fun)> f;
  f.fun(10);

  return 0;
}
