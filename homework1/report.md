# 0430
#### 組員:  郭思翰(41243233), 潘彥愷(41243251)

## 作業一

## 解題說明

本作業需要比較多種排序法的效能，並分析其時間與空間複雜度。需要實作了五種排序演算法：插入排序（Insertion Sort）、快速排序（Quick Sort）、合併排序（Merge Sort）、堆積排序（Heap Sort），以及複合排序（Composite Sort）。
測試不同的資料（n=500, 1000, 2000, 3000, 4000, 5000），並將結果以圖表方式呈現，進一步分析各排序法的時間複雜度，並與理論進行比對。
寫一個結合多種排序法的函式 (Composite Sort)，可以對應不同輸入資料，在最短的時間內完成排序
此外，也需要討論各排序法在最壞情況（worst-case）和平均情況（average-case）下的表現，並說明了測試資料的產生方式與計時方法的精度。最後，報告中附上所有程式碼並分段說明，讓讀者可以清楚了解每個部分的實作細節。

## 解題策略

1. 將問題拆解為排序演算法實作、效能測試、資料產生、複雜度分析、圖表與報告撰寫等幾個主要部分。
2. 撰寫隨機資料產生器，確保所有排序法測試時用的是同一組資料，並重複多次取平均，避免單一資料造成誤差。
3. 針對不同 n（如 500, 1000, ... 5000）反覆測試每種排序法，記錄平均與最大執行時間、記憶體用量。
4. 將測試數據整理成表格與折線圖，直觀比較各排序法在不同 n 下的效能。
   
## 程式實作

插入排序：

```cpp
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

```
快速排序:

```cpp
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

```
合併排序:
```cpp
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
        if (arr[mid] <= arr[mid + 1]) return; 
        merge(arr, aux, left, mid, right);
    }

    template<typename T>
    void MergeSort(T* arr, int n) {
        std::vector<T> aux(n);
        mergeSortUtil(arr, aux.data(), 0, n - 1);
    }

```
堆積排序:
```cpp
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
```
複合排序:
```cpp
void CompositeSort(const int n, ofstream& memoryFile) {
    SortResult t;
    if (n <= 80)
        t = testSort(InsertionSort<int>, "InsertionSort", n, memoryFile);
    else if (80 < n && n < 240)
        t = testSort(QuickSortWrapper<int>, "QuickSort", n, memoryFile);
    else if (80 < n && n < 4400)
        t = testSort(MergeSort<int>, "MergeSort", n, memoryFile);
    else
        t = testSort(QuickSortWrapper<int>, "QuickSort", n, memoryFile);
    cout << n << "\t" << t.avg_time << "\t\t" << t.max_time << endl;
    
}
```
main(用來呼叫計算時間、空間、複合排序，並將結果寫入檔案):
```cpp
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <chrono>
#include <fstream> 
#include <algorithm>
#include<Windows.h>
#include <Psapi.h>
#include"QuickSort.cpp"
#include"InsertionSort.cpp"
#include"MergeSort.cpp"
#include"HeapSort.cpp"
using namespace std;
vector<int> vec;
struct SortResult {	//用來存取平均和最壞的情況
    double avg_time;
    long long max_time;
};
int main() {
    srand(time(0));
    ofstream memoryFile("MemoryUsage.txt");
    ofstream memoryFile2("MemoryUsage2.txt");
    ofstream outFile("output.txt");
    if (!outFile || !memoryFile || !memoryFile2) {
        cerr << "無法開啟輸出檔案！" << endl;
        return 1;
    }

    vector<int> test_sizes = { 500, 1000, 2000, 3000, 4000, 5000 };		//要測試的n

    outFile << "Size\tInsertionSort(us)\tQuickSort(us)\tMergeSort(us)\tHeapSort(us) ";
    outFile<< "\tInsertionSort(us)\tQuickSort(us)\tMergeSort(us)\tHeapSort(us)" << endl;
    
    for (int size : test_sizes) {
        SortResult t_insert = testSort(InsertionSort<int>, "InsertionSort", size, memoryFile);
        SortResult t_quick = testSort(QuickSortWrapper<int>, "QuickSort", size, memoryFile);
        SortResult t_merge = testSort(MergeSort<int>, "MergeSort", size, memoryFile);
        SortResult t_heap = testSort(HeapSort<int>, "HeapSort", size, memoryFile);

        outFile << size << "\t"  << t_insert.avg_time << "       \t\t" << t_quick.avg_time << "\t\t" << t_merge.avg_time << "\t\t" << t_heap.avg_time << "\t\t";
        outFile  << t_insert.max_time << "       \t\t" << t_quick.max_time << "\t\t" << t_merge.max_time << "\t\t" << t_heap.max_time << endl;

        CompositeSort(size, memoryFile2);
    }
 
 
    outFile.close();
    memoryFile.close();
    memoryFile2.close();
    return 0;
}
```
資料隨機排列:
```cpp
void permute(vector<int>& arr, int n) {
    for (int i = n - 1; i >= 1; --i) {
        int j = rand() % i + 1;
        swap(arr[i], arr[j]);
    }
}
```
記憶體用量輸出:
```cpp
void printMemoryUsage(ofstream& outFile, const string& tag = "") {
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    outFile << "====== Memory Usage " << (tag.empty() ? "" : ("[" + tag + "]")) << " ======" << endl;
    outFile << "Working Set Size     : " << memInfo.WorkingSetSize / 1024 << " KB" << endl;
    outFile << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB" << endl;
    outFile << "Pagefile Usage       : " << memInfo.PagefileUsage / 1024 << " KB" << endl;
    outFile << "======================================" << endl << endl;
}
```
排序測試主函式:
```cpp
SortResult testSort(void (*sortFunc)(int*, int), const string& name, int Test_Data, ofstream& memoryFile) {
    vector<int> arr(Test_Data);
    for (int i = 0; i < Test_Data; ++i) arr[i] = i;

    long long max_time = 0;
    long long total_time = 0;
    SIZE_T worst_mem = 0;
    SIZE_T mem_sum = 0;
    int repeat = 1000;
    printMemoryUsage(memoryFile,"Before " + name);

    for (int i = 0; i < repeat; ++i) {
        permute(arr, Test_Data);
        vector<int> tmp = arr;

        PROCESS_MEMORY_COUNTERS memInfoBefore;
        GetProcessMemoryInfo(GetCurrentProcess(), &memInfoBefore, sizeof(memInfoBefore));
        SIZE_T memBefore = memInfoBefore.WorkingSetSize;

        auto start = chrono::high_resolution_clock::now();
        sortFunc(&tmp[0], Test_Data);
        auto end = chrono::high_resolution_clock::now();

        PROCESS_MEMORY_COUNTERS memInfoAfter;
        GetProcessMemoryInfo(GetCurrentProcess(), &memInfoAfter, sizeof(memInfoAfter));
        SIZE_T memAfter = memInfoAfter.WorkingSetSize;

        SIZE_T memMax = max(memBefore, memAfter);
        mem_sum += memMax;
        if (memMax > worst_mem) worst_mem = memMax;

        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        total_time += duration.count();
        if (duration.count() > max_time) max_time = duration.count();
    }
    printMemoryUsage(memoryFile, "After " + name);
    double avg_mem = static_cast<double>(mem_sum) / repeat / 1024.0;
    memoryFile << name << " average-case memory: " << avg_mem << " KB   \t";
    double worst_mem_kb = static_cast<double>(worst_mem) / 1024.0;
    memoryFile << name << " worst-case memory: " << worst_mem_kb << " KB" << endl << endl;
    double avg_time = static_cast<double>(total_time) / repeat;
    return { avg_time,max_time };
}
```

## 效能分析

1. 時間複雜度：
   
	插入排序:
	| 情況   | 時間複雜度 |
	|----------|--------------|
	| 最佳情況（已排序） | $O(n)$         |
	| 最差情況（倒序）   | $O(n^2)$       |
	| 平均情況         | $O(n^2)$       |
        插入排序每次插入新元素時，最壞情況下要與已排序區段的每個元素比較並移動。

	快速排序:
	| 情況   | 時間複雜度 |
	|----------|--------------|
	| 最佳情況（已排序） | $O(n log n)$         |
	| 最差情況（倒序）   | $O(n^2)$       |
	| 平均情況         | $O(n log n)$       |
    	測試資料顯示 Quick Sort 執行時間隨 n 增長而緩慢增加，遠低於 Insertion Sort，符合 O(n log n)。

	合併排序:
	| 情況   | 時間複雜度 |
	|----------|--------------|
	| 最佳情況（已排序） | $O(n log n)$         |
	| 最差情況（倒序）   | $O(n log n)$       |
	| 平均情況         | $O(n log n)$       |
        測試結果顯示 Merge Sort 執行時間隨 n 增長呈現對數級數增加，符合 O(n log n)。
 
	堆積排序:
	| 情況   | 時間複雜度 |
	|----------|--------------|
	| 最佳情況（已排序） | $O(n log n)$         |
	| 最差情況（倒序）   | $O(n log n)$       |
	| 平均情況         | $O(n log n)$       |
        Heap Sort 先建堆（O(n)），再不斷將最大（或最小）元素移除並重建堆（O(log n)），總體 O(n log n)。


2. 空間複雜度：
   
	插入排序:O(1)

	快速排序:平均情況O(log n)、最壞情況O(n)

	合併排序:O(n)

	堆積排序:O(1)
   
## 測試與驗證

#### 測試案例((最糟)

| Size | InsertionSort (us) | QuickSort (us) | MergeSort (us) | HeapSort (us) |
|------|---------------------|----------------|----------------|---------------|
| 50   | 30                  | 11             | 13             | 33            |
| 100  | 18                  | 27             | 45             | 25            |
| 500  | 863                 | 217            | 65             | 284           |
| 1000 | 4220                | 1127           | 162            | 409           |
| 2000 | 11786               | 1012           | 597            | 577           |
| 3000 | 15061               | 1576           | 1035           | 1552          |
| 4000 | 26793               | 4082           | 2262           | 3468          |
| 5000 | 33516               | 2100           | 2893           | 4032          |

![bbb](https://github.com/kronclock/Homework/blob/main/homework1/pictures/worst.webp)

#### 測試案例((平均)

| Size | InsertionSort (us) | QuickSort (us) | MergeSort (us) | HeapSort (us) |
|------|---------------------|----------------|----------------|---------------|
| 50   | 2.664               | 3.498          | 4.496          | 4.418         |
| 100  | 7.957               | 7.453          | 7.307          | 10.057        |
| 500  | 183.213             | 53.513         | 45.432         | 69.690        |
| 1000 | 762.932             | 120.932        | 105.386        | 153.991       |
| 2000 | 2712.880            | 248.770        | 235.650        | 340.884       |
| 3000 | 7381.350            | 489.786        | 498.069        | 682.277       |
| 4000 | 13983.300           | 691.761        | 673.281        | 936.611       |
| 5000 | 21320.600           | 856.615        | 897.296        | 1179.840      |

![aaa](https://github.com/kronclock/Homework/blob/main/homework1/pictures/average.webp)

## 編譯與執行指令

輸出的內容為複合排序的結果，資料量、平均、最差
```shell

$ g++ -std=c++17 -o main.exe main.cpp QuickSort.cpp InsertionSort.cpp MergeSort.cpp HeapSort.cpp
$ .\main.exe
500     53.068          249
1000    105.087         256
2000    230.386         937
3000    394.499         1660
4000    536.979         1691
5000    675.587         1994
```

## 結論

1. 透過對各排序法的時間複雜度驗證，讓我們更能清楚地感覺到各個排序的複雜度確實與理論是一致的，理解到時間複雜度的重要性。
2. 透過排序法效能比較，更能了解到各排序法的優缺點。
3. 由於理論與實測一致，驗證了演算法分析的可靠性。	

## 申論及開發報告

#### 1.資料隨機排列函式

確保每次排序測試時資料都是隨機的。這樣可以有效模擬 average-case 的排序效能，避免只測到特殊情境（如已排序、逆序）。

用途： 產生公平且具代表性的測試資料。

#### 2.系統化效能測試流程

為了讓所有排序法的效能測試流程一致，使用了 testSort 函式，統一隨機資料產生、排序、計時與記憶體量測。

用途：這樣能減少偶發誤差，讓數據更具代表性。

#### 3.複合排序函式
根據資料大小自動選擇最適合的排序法，並呼叫測試流程。

用途： 展示如何將多種排序法結合，提升整體效能。

#### 4.記憶體與時間量測的輔助函式

為了分析空間複雜度，使用 printMemoryUsage 函式，利用系統 API 量測排序前後的記憶體消耗。

用途：這有助於驗證演算法的理論空間複雜度與實際表現是否一致。
