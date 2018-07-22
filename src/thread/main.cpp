
#include <thread>

#include <iostream>

#include "thread_map/thread_map.hpp"
#include <map>

int main()
{
    Thread_map<std::string> mmap;

    std::string *key = new std::string("Sdf");

    //mmap.insert(std::pair<std::string, std::string>(std::string("sdfs"), std::string("sdfsdf")));

    mmap[*key] = "sdfs";
    mmap["key2"] = "value2";
    mmap["key3"] = "value3";
    mmap["key4"] = "value4";
    mmap["key5"] = "value5";


    std::map<std::string, std::string> map;
    map["mkey"] = "value";

    for(int i = 0; i < 10; i++)
    {
        std::thread ([&] () -> void {
            std::cout << "hello world" << mmap["Sdf"] << std::endl;
        }).detach();
    }

    int count = 0;

    mmap.for_each([&] (Thread_map<std::string>& map, std::string& key, std::string& value) -> bool {
        std::cout << "key: " << key << " value: " << value << std::endl;
        count++;
        if(count >= 2)
            return true;
        else 
            return false;
    });
    
    return 0;
}
