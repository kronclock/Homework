#include<iostream>
using namespace std;

template<class T>
void InsertionSort(T* a, const int n) {
    for (int j = 1; j < n; j++) {
        T temp = a[j];
        int i = j - 1;
        while (i >= 0 && a[i] > temp) {
            a[i + 1] = a[i];
            i--;
        }
        a[i + 1] = temp;
    }
}
