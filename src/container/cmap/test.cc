#include "cmap.h"

#include <iostream>

int main(void) {


  con::cmap<std::string, 32> m;

  char *key1 = "key1";
  std::string data1 = "data1";
  m.insert(key1, data1);


  char *key2 = "key2";
  std::string data2 = "data2";
  m.insert(key2, data2);

  char *key3 = "key3";
  std::string data3 = "data3";
  m.insert(key3, data3);


  for(auto data : m) {
    std::cout << "DATA: " << std::endl;
    std::cout << data << std::endl;
  }


  return 0;
}
