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
struct SortResult {
    double avg_time;
    long long max_time;
};
void permute(vector<int>& arr, int n) {
	for (int i = n - 1; i >= 1; --i) {
		int j = rand() % i + 1;
		swap(arr[i], arr[j]);
	}
}
void printMemoryUsage(ofstream& outFile, const string& tag = "") {
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    outFile << "====== Memory Usage " << (tag.empty() ? "" : ("[" + tag + "]")) << " ======" << endl;
    outFile << "Working Set Size     : " << memInfo.WorkingSetSize / 1024 << " KB" << endl;
    outFile << "Peak Working Set Size: " << memInfo.PeakWorkingSetSize / 1024 << " KB" << endl;
    outFile << "Pagefile Usage       : " << memInfo.PagefileUsage / 1024 << " KB" << endl;
    outFile << "======================================" << endl << endl;
}
SortResult testSort(void (*sortFunc)(int*, int), const string& name, int Test_Data, ofstream& memoryFile) {
    vector<int> arr(Test_Data);
    for (int i = 0; i < Test_Data; ++i) arr[i] = i;

    long long max_time = 0;
    long long total_time = 0; // 用來累加總執行時間
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
    printMemoryUsage(memoryFile, "After " + name);  // 執行後記憶體
    double avg_mem = static_cast<double>(mem_sum) / repeat / 1024.0; // 平均 KB
    memoryFile << name << " average-case memory: " << avg_mem << " KB   \t";
    double worst_mem_kb = static_cast<double>(worst_mem) / 1024.0;   // KB
    memoryFile << name << " worst-case memory: " << worst_mem_kb << " KB" << endl << endl;
    double avg_time = static_cast<double>(total_time) / repeat;
    return { avg_time,max_time };
}

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
int main() {
    srand(time(0));
    ofstream memoryFile("MemoryUsage.txt");
    ofstream memoryFile2("MemoryUsage2.txt");
    ofstream outFile("output.txt");
    if (!outFile || !memoryFile || !memoryFile2) {
        cerr << "無法開啟輸出檔案！" << endl;
        return 1;
    }

    vector<int> test_sizes = { 500, 1000, 2000, 3000, 4000, 5000 };

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
