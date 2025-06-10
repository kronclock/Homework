#include<iostream>
#include<algorithm>
using namespace std;

template<typename T>
void heapify(T* arr, int n, int i) {
    T temp = arr[i];
    int child;
    for (; 2 * i + 1 < n; i = child) {
        child = 2 * i + 1;
        if (child + 1 < n && arr[child] < arr[child + 1])
            ++child;
        if (temp < arr[child])
            arr[i] = arr[child];
        else
            break;
    }
    arr[i] = temp;
}

template<typename T>
void HeapSort(T* arr, int n) {
    // 建立最大堆
    for (int i = n / 2 - 1; i >= 0; --i)
        heapify(arr, n, i);
    // 取出元素
    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}
