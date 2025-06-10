#include<iostream>
#include <algorithm>
#include <vector>
using namespace std;

template<typename T>
void insertionSort(T* arr, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
         T key = arr[i];
         int j = i - 1;
         while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
         }
         arr[j + 1] = key;
    }
}

    template<typename T>
    void merge(T* arr, T* aux, int left, int mid, int right) {
        int i = left, j = mid + 1, k = left;
        for (int l = left; l <= right; ++l) aux[l] = arr[l];
        while (i <= mid && j <= right) {
            if (aux[i] <= aux[j]) arr[k++] = aux[i++];
            else arr[k++] = aux[j++];
        }
        while (i <= mid) arr[k++] = aux[i++];
        // 右半剩餘已在原位，不需額外複製
    }

    template<typename T>
    void mergeSortUtil(T* arr, T* aux, int left, int right) {
        if (right - left <= 16) { // 小區間用插入排序
            insertionSort(arr, left, right);
            return;
        }
        int mid = left + (right - left) / 2;
        mergeSortUtil(arr, aux, left, mid);
        mergeSortUtil(arr, aux, mid + 1, right);
        if (arr[mid] <= arr[mid + 1]) return; // 已經有序，不用 merge
        merge(arr, aux, left, mid, right);
    }

    template<typename T>
    void MergeSort(T* arr, int n) {
        std::vector<T> aux(n);
        mergeSortUtil(arr, aux.data(), 0, n - 1);
    }
