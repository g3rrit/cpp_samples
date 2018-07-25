#ifndef THREAD_MAP_H
#define THREAD_MAP_H

#include <map>
#include <functional>
#include <mutex>

template <class T>class Thread_map
{
private:
    std::map<std::string, T> map;
    std::mutex map_mutex;

public:

    ~Thread_map();

    T& at(size_t pos);
    T& operator[](const std::string &key);

    size_t size() const;
    bool empty() const;

    void clear();
    void insert(std::pair<std::string, T> pair);
    T& erase(const std::string &key);

    bool for_each(std::function<bool (Thread_map<T>&, std::string&, T&)> fun);
};

template<class T> Thread_map<T>::~Thread_map()
{
    this->map.clear();
}

template<class T> T& Thread_map<T>::at(size_t pos)
{
    T *value;
    this->map_mutex.lock();
    value = this->map.at(pos);
    this->map_mutex.unlock();
    return value;
}

template<class T> T& Thread_map<T>::operator[](const std::string &key)
{
    this->map_mutex.lock();
    T &value = this->map[key];
    this->map_mutex.unlock();
    return value;
}

template<class T> size_t Thread_map<T>::size() const
{
    size_t size;
    this->map_mutex.lock();
    size = this->map.size();
    this->map_mutex.unlock();
    return size;
}

template<class T> bool Thread_map<T>::empty() const
{
    bool empty; 
    this->map_mutex.lock();
    empty = this->map.empty();
    this->map_mutex.unlock();
    return empty;
}

template<class T> void Thread_map<T>::clear()
{
    this->map_mutex.lock();
    this->map.clear();
    this->map_mutex.unlock();
}

template<class T> void Thread_map<T>::insert(std::pair<std::string, T> pair)
{
    this->map_mutex.lock();
    this->map.insert(pair);
    this->map_mutex.unlock();
}

template<class T> T& Thread_map<T>::erase(const std::string &key)
{
    T *value;
    this->map_mutex.lock();
    value = this->map.erase(key);
    this->map_mutex.unlock();
    return value;
}

template<class T> bool Thread_map<T>::for_each(std::function<bool (Thread_map<T>&, std::string&, T&)> fun)
{
    this->map_mutex.lock();
    for(std::pair<std::string, T> element : this->map)
    {
        if(fun(*this, element.first, element.second))
        {
            this->map_mutex.unlock();
            return true;
        }
    }
    this->map_mutex.unlock();
    return false;
}


#endif
