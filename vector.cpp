#include <vector>
#include <iostream>

namespace tt {
template <typename T>
class Allocator : public std::allocator<T>{
public:
    [[nodiscard]] T* allocate(size_t count) {
        std::cout << "Allocated " << count * sizeof(T) << " bytes\n";
        return static_cast<T*>(::operator new(count * sizeof(T)));
    }

    void deallocate(T *memory, size_t n) {
        std::cout << "Deallocated " << n << " objects\n";
        ::operator delete(memory);
    }

    template< typename U, typename...  Args>
    void construct(U *memory, Args&& ... args) {
        new(memory) T(args...);
    }


    void destroy(T* object) {
        object->~T();
    }
};

template <typename T, typename Alloc = tt::Allocator<T>>
class Vector {
    size_t m_size {0};
    size_t m_capacity {0};
    T* memory {nullptr};
    Alloc allocator;

    void checkSize() {
        if(m_size == m_capacity) {
            reserve(m_capacity == 0 ? 2 : m_capacity * 2);
        }
    }

public:

    ~Vector() noexcept {
        for(int i = 0; i < m_size; ++i) {
            allocator.destroy(memory+i);
        }
        allocator.deallocate(memory, m_size);
    }
    Vector() : allocator(Alloc()) {
    }

    Vector(size_t count, const T& init_value = T(), const tt::Allocator<T>& alloc = Alloc()) :
        allocator(alloc) {
        reserve(count, init_value);
    }


    template<typename... Args>
    void reserve(size_t count, const Args&... args) {

        if(count <= 0) {
            return;
        }

        T* mem;
        int constructed;
        try {
            mem = allocator.allocate(count);

            for (constructed = 0; constructed < count; ++constructed) {
                allocator.construct(mem+constructed, args...);
            }

            if(m_size > 0) {
                for (int i = 0; i < m_size; ++i) {
                    allocator.destroy(memory + i);
                }


                allocator.deallocate(memory, m_size);
            }

            m_size = m_capacity = count;
            memory = mem;

        }  catch (...) {

            for(int i = 0; i < constructed; ++i) {
                allocator.destroy(memory+i);
            }

            allocator.deallocate(mem, count);
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
        checkSize();

        try {
            allocator.construct(memory + m_size++, value);
        }  catch (...) {
            allocator.destroy(memory + m_size);
            throw;
        }
    }

    template<typename... Args>
    void emplace_back(const Args& ... args) {
        checkSize();

        try {
            allocator.construct(memory + m_size++, args...);
        }  catch (...) {
            allocator.destroy(memory + m_size);
            throw;
        }
    }
};
}


class Entity {
public:
    ~Entity() noexcept {

    }
    Entity() {

    }
    explicit Entity(int n) noexcept : m(n) {

    }
    Entity(const Entity& rhs) noexcept{
        m = rhs.m;
    }
    Entity(Entity&& rhs) noexcept {
        m = rhs.m;
    }
    Entity& operator=(const Entity& rhs) noexcept{
        m = rhs.m;
        return *this;
    }
    Entity& operator=(Entity&& rhs) noexcept {
        m = rhs.m;
        return *this;
    }
    int m;

};


int main(){

    tt::Vector<int> entities;
    entities.reserve(5, 5);

    for(int i = 0; i < entities.size(); ++i) {
        std::cout << entities[i] << '\t';
    }
    std::cout << '\n';

//    for(int i = 0; i < 10; i++) {
//        entities.push_back(5);
//    }

    for(int i = 0; i < entities.size(); ++i) {
        std::cout << entities[i] << '\t';
    }
    std::cout << '\n';


    return 0;
}
