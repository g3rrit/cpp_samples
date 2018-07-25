#ifndef THREAD_MAP_HPP
#define THREAD_MAP_HPP

#include <mutex>
#include <functional>
#include <string>

#define map_for_each for(thread_map_node_t *entry = this->head; entry->next; entry = entry->next)

template<class T> class thread_map_t
{
private:
    struct thread_map_node_t
    {
        std::string key;
        thread_map_node_t *next;
        T& value;

        thread_map_node_t(std::string& key, T& value, thread_map_node_t* next) :
            key(key), 
            value(value),
            next(next)
        {
        }
    };

    std::mutex map_mutex;
    thread_map_node_t *head = nullptr;
    size_t size = 0;

public:
    thread_map_t() {}

    ~thread_map_t() 
    {
        clear();
    }

    void clear()
    {
        thread_map_node_t *f_entry = nullptr;

        this->map_mutex.lock();
        map_for_each
        {
            if(f_entry != nullptr)
                delete f_entry;

            f_entry = entry;
        }

        if(f_entry != nullptr)
            delete f_entry;

        this->size = 0;
        this->head = nullptr;
        this->map_mutex.unlock();
    }

    void add(std::string& key, T& value)
    {
        this->map_mutex.lock();
        this->head = new thread_map_node_t(key, value, this->head);
        this->size++;
        this->map_mutex.unlock();
    }

    T* remove(std::string& key)
    {
        thread_map_node_t *p_entry = nullptr;

        this->map_mutext.lock();
        map_for_each
        {
            if(key.compare(entry->key) == 0)
            {
                T* res = &entry->value;
                if(p_entry != nullptr)
                    p_entry->next = entry->next;
                else 
                    this->head = entry->next;

    	        this->size--;
                this->map_mutex.unlock();
                return res;
            }

            p_entry = entry;
        }
        this->map_mutex.unlock();

        return nullptr;
    }

    T* get(std::string& key)
    {
        this->map_mutex.lock();
        map_for_each
        {
            if(key.compare(entry->key))
            {
                T* res = &entry->value;
                this->map_mutex.unlock();
                return res;
            }
        }
        this->map_mutex.unlock();

        return nullptr;
    }

    bool for_each(std::function<bool (std::string& key, T& value)> fun)
    {
        this->map_mutex.lock();
        map_for_each
        {
            if(fun(entry->key, entry->value))
            {
                this->map_mutex.unlock();
                return true;
            }
        }
        this->map_mutex.unlock();

        return false;
    }

};


#endif