
#include <thread>

#include <iostream>

#include "thread_map/thread_map.hpp"
#include <map>
#include <string>
#include <chrono>

int main()
{
    thread_map_t<std::string> map;

    std::thread([&] () -> void {
        int count = 0;
        for(int i = 0; i< 10; i++)
        {
            using namespace std::chrono_literals;
            std::string key = "key" + std::to_string(count);
            std::string *value = new std::string("value" + std::to_string(count));
            map.add(key, *value);
            count++;
            std::this_thread::sleep_for(1s);
        }

    }).detach();

    for(int i = 0; i < 10; i++)
    {
        std::cout << "for each: " << i << std::endl;

        using namespace std::chrono_literals;
        map.for_each([&] (std::string& key, std::string& value) -> bool {
            std::cout << "key: " << key << " value: " << value << std::endl;
            return false;
        });
        std::this_thread::sleep_for(1s);
    }
   
    return 0;
}
