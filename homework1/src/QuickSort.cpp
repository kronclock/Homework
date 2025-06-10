#include<iostream>
using namespace std;

template<class T>
void QuickSort(T* a, const int left, const int right) {
	if (left < right) {
		int i = left;
		int j = right + 1;
		int pivot = a[left];
		do {
			do i++;while (a[i] < pivot);
			do j--;while (a[j] > pivot);
			if (i < j) swap(a[i], a[j]);
		} while (i < j);
		swap(a[left], a[j]);

		QuickSort(a, left, j - 1);
		QuickSort(a, j + 1, right);
	}
}
template<class T>
void QuickSortWrapper(T* a, int n) {
	QuickSort(a, 0, n - 1);
}
