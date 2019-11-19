#include <iostream>

const long int n = 10;

template<long int b>
struct bc {
  const static long int value = (b & 1) + bc<(b >> 1)>::value;
};

template<>
struct bc<0> {
  const static long int value = 0;
};

int main() {

  std::cout << "Bitcount: " << n << " = " << bc<n>::value << std::endl;

  return 0;
}
