#pragma once

#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <algorithm>

namespace con {

  size_t hash_position(char *name, size_t max_size) {
    size_t s_t_s = sizeof(size_t);
    uint8_t res_hash[s_t_s];
    memset(res_hash, 0, s_t_s);
    int hash_pos = 0;
    int name_len = strlen(name);

    for(int i = 0; i < name_len; i++) {
      res_hash[hash_pos] ^= (uint8_t)name[i];
      hash_pos++;
      hash_pos %= s_t_s - 1;
    }

    uint32_t res = 0;
    memcpy(&res, res_hash, s_t_s);

    return res % max_size;
  }


  /*
   * iterator not done but this is genericly how
   * one might be implemented
   */

  template<typename T, size_t m_size = 32>
  class cmap {
  private:

    struct node {
      char *key;
      T data;
      node *next;

      node() : key(0), data(0), next(0) {}

      node(char *_key, T& _data) : key(_key), data(std::move(_data)), next(0) {}

      ~node() {}
    };

    std::vector<node> arr[m_size];

    size_t p_size;

  public:

    class iterator : public std::iterator<std::input_iterator_tag, T> {
      size_t pos = 0;
      size_t map_pos = 0;
      typename std::vector<node>::iterator it;
      cmap &map;

      void inc() {
        if(map_pos >= m_size) {
          return;
        }
        it++;
        pos++;
        if(it == map.arr[map_pos].end()) {
          map_pos++;
          if(map_pos < m_size) {
            it = map.arr[map_pos].begin();
          }
        }
      }

    public:
      iterator(cmap& _map)
        : map(_map), it(_map.arr[0].begin()) {}

      iterator(cmap& _map, bool end)
        : map(_map), it(_map.arr[m_size].end()), map_pos(m_size) {
      }

      iterator& operator++() {
        inc();
        return *this;
      }

      iterator operator++(int x) {
        inc();
        return *this;
      }

      bool operator==(iterator other) const {
        return pos == other.pos;
      }

      bool operator!=(iterator other) const {
        return !(*this == other);
      }

      T& operator*() const {
        return it->data;
      }
    };

    iterator begin() {
      return iterator(*this);
    }

    iterator end() {
      return iterator(*this, true);
    }

    cmap() { }

    /* ITERATORS */


    /* CAPACITY */

    inline bool empty() const {
      return this->p_size;
    }

    inline size_t size() const {
      return this->p_size;
    }

    inline size_t max_size() const {
      return m_size;
    }

    /* ELEMENT ACCESS */

    T* at(char *key) const {
      for(auto const& entry : arr[hash_position(key, m_size)]) {
        if(!std::strcmp(entry->key, key)) {
          return &entry->data;
        }
      }

      return nullptr;
    }

    /* MODIFIERS */

    T& insert(char *key, T& data) {
      std::vector<node>& e_vec = arr[hash_position(key, m_size)];

      for(auto& entry : e_vec) {
        if(!std::strcmp(entry.key, key)) {
          return entry.data;
        }
      }

      e_vec.emplace_back(key, data);

      this->p_size++;

      return e_vec.back().data;
    }

    bool erase(char *key) {
      std::vector<node>& e_vec = arr[hash_position(key, m_size)];

      auto it = e_vec.begin();

      while(it != e_vec.end()) {
        if(!std::strcmp(it->key, key)) {
          e_vec.erase(it);
          this->p_size--;
          return true;
        }

        it++;
      }

      return false;
    }

  };

}
