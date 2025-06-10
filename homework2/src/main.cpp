//Main 
#include <iostream>
#include "header.h"
int main() {
    MinHeap<int> h;

    int a[11] = { 10, 2, 16, 30, 8, 28, 4, 12, 20, 6, 18 };

    for (int i = 0; i < 11; i++)
        h.Push(a[i]);

    while (!h.IsEmpty()) {
        std::cout << h.Top() << " ";
        h.Pop();
    }

    return 0;
}
