#include <iostream>

const long int num = 10;

template<long int n>
struct fac {
  const static long int value = n * fac<n - 1>::value;
};

template<>
struct fac<0> {
  const static long int value = 1;
};


int main() {

  std::cout << "fac: " << num << " = " << fac<num>::value << std::endl;

  return 0;
}
