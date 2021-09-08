#include "vector.h"

int main()
{
    tt::Vector<int> v(5, 10);

    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << '\n';
    }
    return 0;
}
