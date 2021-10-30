#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
#include <iostream>

namespace bicycle {
template <typename T>
class Allocator : public std::allocator<T> {
public:
    explicit Allocator() = default;
    [[nodiscard]] T* allocate(size_t count) {
        return static_cast<T*>(::operator new(count * sizeof(T)));
    }

    void deallocate(T *memory, size_t /*n*/) {
        ::operator delete(memory);
    }

    template< typename U, typename...  Args>
    void construct(U *memory, Args&& ... args) {
        new(memory) T(std::forward<Args>(args)...);
    }


    void destroy(T* object) {
        object->~T();
    }
};

template <typename T, typename Alloc = bicycle::Allocator<T>>
class Vector {

    size_t m_size {0};
    size_t m_capacity {0};
    T* memory {nullptr};
    Alloc allocator;

    using AllocTraits = std::allocator_traits<Alloc>;
    using size_type = size_t;

    void checkSize() {
        if(m_size >= m_capacity) {
            reserve(m_capacity == 0 ? 2 : m_capacity * 2);
        }
    }

    void clear() {
        for(size_t i = 0; i < m_size; ++i) {
            AllocTraits::destroy(allocator, memory+i);
        }
        AllocTraits::deallocate(allocator, memory, m_size);
    }
public:

    struct iterator {
      size_t m_offset {0};
      T* m_data {nullptr};
      iterator (size_t offset, T* weak_data) : m_data(weak_data), m_offset(offset) {

      }

      T& operator *() const noexcept {
          return *(m_data + m_offset);
      }
      iterator& operator++ () noexcept {
          m_offset++;
          return *this;
      }
      bool operator <(const iterator& rhs) const {
          return m_offset < rhs.m_offset;
      }
      bool operator != (const iterator& rhs) const {
          return m_offset != rhs.m_offset;
      }
    };


    ~Vector() noexcept {
        clear();
    }
    Vector() : allocator(Alloc()) {
    }

    Vector(size_t count) :
        allocator(Alloc()) {
        reserve(count);
    }
    Vector(size_t count, const T& value, const Alloc &alloc = Alloc()) :
        allocator(alloc) {
        assign(count, value);
    }

    Vector(const Vector& rhs) {
        //TODO Copy constructor
    }
    Vector(Vector&& rhs) {
        //TODO Move constructor
    }

    void reserve(size_t count) {

        if(count <= 0 || count < m_capacity) {
            return;
        }

        T* mem {nullptr};
        size_t constructed {0};
        try {
            mem = AllocTraits::allocate(allocator, count);

            if(m_size) {
                for(constructed = 0; constructed < m_size; ++constructed) {
                    AllocTraits::construct(allocator, mem + constructed, std::move_if_noexcept(*(memory+constructed)));
                }
            }

            if(m_size > 0) {
                clear();
            }

            m_capacity = count;
            memory = mem;

        }  catch (...) {

            for(size_t i = 0; i < constructed; ++i) {
                AllocTraits::destroy(allocator, memory+i);
            }

            AllocTraits::deallocate(allocator, mem, count);
            throw;
        }
    }

    void assign(size_t count, const T& value) {
        if (m_size > 0) {
            clear();
        }

        reserve(count);
        size_t constructed;
        try {
            for(constructed = 0; constructed < count;++constructed) {
                AllocTraits::construct(allocator, memory + constructed, value);
            }

            m_size = count;
        }  catch (...) {
            for (size_t i = 0; i < constructed; ++i) {
                AllocTraits::destroy(allocator, memory);
            }
            throw;
        }
    }

    size_t size() const {
        return m_size;
    }

    size_t capacity() const {
        return m_capacity;
    }

    T& operator[](size_t index) const {
        return *(memory + index);
    }

    T& at(size_t index) const {
        if(index > m_size - 1) {
            throw std::out_of_range("index out of range.");
        }
        return *(memory + index);
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(std::move_if_noexcept(value));
    }

    template<typename... Args>
    void emplace_back(Args&& ... args) {
        checkSize();

        try {
            AllocTraits::construct(allocator, memory + m_size++, std::forward<Args>(args)...);
        }  catch (...) {
            AllocTraits::destroy(allocator, memory + m_size);
            throw;
        }
    }

    iterator begin() const {
        return iterator(0, memory);
    }

    iterator end() const {
        return iterator(m_size, memory);
    }
};


}
#endif


