#include "vector.h"


struct A {

};

A& getA() {
    A *a = new A();
    return *a;
}

int main()
{


    return 0;
}
