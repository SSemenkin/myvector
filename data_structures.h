#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <initializer_list>
#include <iostream>

namespace data_structures {
template <typename type>
class iterator{


};

template <typename type>
class list{
protected:
    struct node {
      node *next;
      node *prev;
      type *m_value;

      node(const type &value) : m_value(new type(value)), next(nullptr), prev(nullptr) {

      }
      ~node() {
          delete m_value;
      }
    };

private:
    unsigned m_size;

    node *head {nullptr};
    node *tail {nullptr};

public:
    void append(const type& value) {
        if (!head) {
            head = new node(value);
            tail = head;
        } else {
            tail->next = new node(value);
            tail->next->prev = tail;
            tail = tail->next;
        }
    }



    explicit list (unsigned size = 0, const type &init_val = type()) : m_size(size), head(nullptr), tail(nullptr) {
         for (int i = 0; i < m_size; ++i) {
             append(init_val);
         }
    }

    explicit list (const std::initializer_list<type> &initializer_list) :
        m_size(initializer_list.size()), head(nullptr), tail(nullptr) {
          for (auto it = initializer_list.begin(); it != initializer_list.end(); ++it) {
              append(*it);
          }
    }

    iterator<type> begin() const {
        return iterator<type>();
    }

};
}

#endif // DATA_STRUCTURES_H
