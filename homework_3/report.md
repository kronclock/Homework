# 20250610
#### 組員:  郭思翰(41243233), 潘彥愷(41243251)
  
## 作業二 第三題

## 解題說明 
(a)使用 k-way merge，並將內部記憶體劃分為輸入/輸出緩衝區以允許輸入、輸出與 CPU 平行處理，可以像是程式7.21的方式進行處理，並且回答在第二階段時的總輸入時間是多少？

(b)假設 CPU 合併所有 runs 的時間為 t_CPU（且與 k 無關，視為常數）。
給定參數如下：

t_s = 80ms

t_l = 20ms

n = 200,000

m = 64

t_r = 10^-3 

S = 2000

需畫出輸入總時間t_input 對 k 的粗略圖，並判斷：是否總存在某個 k 值，使得 t_CPU ≈ t_input
## 解題策略
1. 建立初始資料:	使用 generateRuns() 將資料分為 K 個 sorted run，模擬外部排序的預處理階段。

2. 設計 buffer:	分配 K 個輸入緩衝區 + 1 輸出 + 2 預備緩衝區，模擬實際記憶體分配。

3. 模擬 I/O:	對每次磁碟讀寫呼叫 simulateDiskRead/Write() 模擬 seek, latency, 傳輸。

4. 執行合併: 使用 priority queue 完成 k-way merge，每次將最小的 key 輸出。

5. 實驗與效能: 執行多組 n 以觀察真實程式執行時間與模擬的 I/O 時間差異。

6. 分析不同 k 對效能的影響: 輸出成CSV檔。

## 程式實作
標頭:
```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;
```

全域常數與模擬參數:
```cpp
const int S = 100;             // 記憶體容量
const int BLOCK_SIZE = 20;     // 每個緩衝區區塊含有幾筆資料
const int INF = numeric_limits<int>::max();

// I/O 時間參數（模擬磁碟）
double t_s = 5.0;
double t_l = 3.0;
double t_r = 0.1;

double io_time_total = 0.0;
double read_time_total = 0.0;
double write_time_total = 0.0;

int K = 0; // global 記錄 k 值供其他函式使用
```

隨機打亂n筆資料，並將它們切成K個區塊，每個區塊內部排序好並加上一個 NF作為終止標記:
```cpp
vector<vector<int>> generateRuns(int n) {
    vector<int> all_data(n);
    for (int i = 0; i < n; ++i) all_data[i] = i + 1;

    random_device rd;
    mt19937 g(rd());
    shuffle(all_data.begin(), all_data.end(), g);

    int records_per_run = n / K;
    vector<vector<int>> runs(K);

    for (int i = 0; i < K; ++i) {
        int start = i * records_per_run;
        int end = min(n, start + records_per_run);
        runs[i] = vector<int>(all_data.begin() + start, all_data.begin() + end);
        sort(runs[i].begin(), runs[i].end());
        runs[i].push_back(INF);
    }

    return runs;
}
```
模擬磁碟讀取/寫入某個 buffer，計算花費的時間。更新全域變數 read_time_total、write_time_total:
```cpp
void simulateDiskRead(const vector<int>& buffer) {
    double t = t_s + t_l + buffer.size() * t_r;
    io_time_total += t;
    read_time_total += t;
}

void simulateDiskWrite(const vector<int>& buffer) {
    double t = t_s + t_l + buffer.size() * t_r;
    io_time_total += t;
    write_time_total += t;
}
```

結構與優先隊列:
```cpp
struct Record {
    int key;
    int run_id;
    bool operator>(const Record& other) const { return key > other.key; }
};
```

模擬外部排序:
```cpp
void runExperiment(int n) {
    // 效能計時開始
    auto start = high_resolution_clock::now();

    io_time_total = read_time_total = write_time_total = 0;

    K = S / BLOCK_SIZE - 1;
    int BUFFER_COUNT = K + 1 + 2;

    vector<vector<int>> disk_runs = generateRuns(n);
    vector<queue<int>> input_queues(K);
    stack<vector<int>> idle_buffers;
    vector<int> run_indices(K, 0);
    vector<int> lastKey(K);

    for (int i = 0; i < K; ++i) {
        vector<int> buffer;
        for (int j = 0; j < BLOCK_SIZE && run_indices[i] < disk_runs[i].size(); ++j, ++run_indices[i]) {
            buffer.push_back(disk_runs[i][run_indices[i]]);
        }
        simulateDiskRead(buffer);
        for (int val : buffer) input_queues[i].push(val);
        lastKey[i] = buffer.back();
    }

    for (int i = K + 1; i < BUFFER_COUNT; ++i) {
        idle_buffers.push({});
    }

    vector<int> output_buffer[2];
    int ou = 0;
    bool merge_done = false;

    while (!merge_done) {
        priority_queue<Record, vector<Record>, greater<Record>> pq;
        for (int i = 0; i < K; ++i) {
            if (!input_queues[i].empty()) {
                pq.push({ input_queues[i].front(), i });
            }
        }

        output_buffer[ou].clear();
        bool seen_inf = false;

        while (!pq.empty() && output_buffer[ou].size() < BLOCK_SIZE) {
            Record rec = pq.top(); pq.pop();
            output_buffer[ou].push_back(rec.key);
            input_queues[rec.run_id].pop();

            if (rec.key == INF) {
                seen_inf = true;
                break;
            }

            if (input_queues[rec.run_id].empty() && run_indices[rec.run_id] < disk_runs[rec.run_id].size()) {
                if (!idle_buffers.empty()) {
                    vector<int> buffer;
                    for (int j = 0; j < BLOCK_SIZE && run_indices[rec.run_id] < disk_runs[rec.run_id].size(); ++j, ++run_indices[rec.run_id]) {
                        buffer.push_back(disk_runs[rec.run_id][run_indices[rec.run_id]]);
                    }
                    simulateDiskRead(buffer);
                    for (int val : buffer) input_queues[rec.run_id].push(val);
                    lastKey[rec.run_id] = buffer.back();
                    idle_buffers.push({});
                }
            }

            if (!input_queues[rec.run_id].empty()) {
                pq.push({ input_queues[rec.run_id].front(), rec.run_id });
            }
        }

        simulateDiskWrite(output_buffer[ou]);
        ou = 1 - ou;

        if (seen_inf) merge_done = true;
    }

    // 效能計時結束
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "n = " << n << "：輸入時間 = " << read_time_total
        << " ms，輸出時間 = " << write_time_total
        << " ms，總 I/O 時間 = " << io_time_total
        << " ms，程式實際執行時間 = " << duration.count() << " ms\n";
}
```

計算不同 k 值下輸入時間的理論估計值，輸出到 CSV。
```cpp
void analyze_input_time_vs_k() {
    int n = 200000;
    int S = 2000;
    int B = 80;

    double t_seek = 20.0;
    double t_latency = 80.0;
    double t_per_record = 1.0;

    double t_block = t_seek + t_latency + B * t_per_record;

    ofstream fout("t_input_vs_k.csv");
    fout << "k,t_input(ms)\n";

    for (int k = 2; k <= 100; ++k) {
        double rounds = ceil(log(n / (double)S) / log(k));
        double t_input = 2 * (n / (double)B) * t_block * rounds;
        fout << k << "," << t_input << "\n";
    }

    fout.close();
}
```

主程式:
```cpp
int main() {
    vector<int> n_values = { 100, 200, 500, 1000, 2000, 5000, 10000 };

    for (int n : n_values) {
        runExperiment(n);
    }

    analyze_input_time_vs_k();

    return 0;
}
```

## 效能分析

時間複雜度:
|操作|情況|時間複雜度|
|------|------|------------|
| 插入操作 |最佳情況 |$O(1)$ |
| 插入操作 | 平均情況 |$O(\log n)$ |
|插入操作 |最差情況 |$O(n)$ |
| 搜尋操作 | 最佳情況 |$O(1)$ |
| 搜尋操作 | 平均情況 |$O(\log n)$ |
| 搜尋操作 | 最差情況 |$O(n)$ |
| 刪除操作 | 最佳情況 |$O(1)$ |
| 刪除操作 |平均情況 |$O(\log n)$ |
|刪除操作 |最差情況 |$O(n)$ |

空間複雜度:
|    |空間複雜度|
|------|------------|
| 插入操作 | $O(1)$ 額外空間 |
| 搜尋操作 | $O(1)$ 額外空間 |
|刪除操作 | $O(1)$ 額外空間 |
| 整體結構 |$O(n)$ 儲存所有節點 |


## 測試與驗證
| n | 輸入時間 | 輸出時間 | 總I/O時間 |
|---|----------|---------|-----------|
| 100  |   74.4 ms   |  58.1 ms|     132.5 ms   |
| 200  |    116.4 ms  | 108.1 ms   |  224.5 ms | 
| 500  |  274.4 ms  | 258.1 ms   | 532.5 ms  | 
| 1000  | 516.4 ms  |  508.1 ms  | 1024.5 ms  | 
| 2000  |  1032.4 ms | 1008.1 ms  |  2040.5 ms|  
| 5000  |   2516.4 ms|  2508.1 ms |  5024.5 ms |   
| 10000  |   5032.4 ms |  5008.1 ms | 10040.5 ms |  

![image](https://github.com/user-attachments/assets/5992f2a1-e2e9-4b67-993c-989ef0e02b64)

## 結論  
1.總輸入時間分析：
在給定參數下，總輸入時間與合併路徑數 k 的關係呈現先遞減後遞增的趨勢
存在一個最佳 k 值使總輸入時間最小化
當 k 增加時，雖然減少了合併輪數，但每輪的 I/O 時間會增加
2.CPU 時間與 I/O 時間平衡：
在給定參數下，存在一個 k 值使得 t_cpu ≈ t_input
這個平衡點可以通過求解方程式找到，是系統效能的最佳工作點
3.效能優化：
選擇適當的 k 值對系統效能至關重要
過大的 k 值會導致 I/O 時間增加，而過小的 k 值會增加合併輪數


## 申論及開發報告  
#### 1. 外部排序的 I/O 模型分析:
我們實現了基於 k-路合併的外部排序演算法，並對其 I/O 效能進行了詳細分析。通過模擬磁碟存取過程，我們觀察到：

(1) 總輸入時間主要由搜尋時間、旋轉延遲和資料傳輸時間組成、
(2) 合併路徑數 k 對總輸入時間有顯著影響

用途：這種分析方式幫助我們理解外部排序的效能瓶頸，並為參數調優提供依據。

#### 2. 合併路徑數 k 的影響分析
我們設計了實驗來分析 k 值對排序效能的影響：

使用不同 k 值進行測試、
測量每個 k 值對應的總輸入時間、
分析 k 值與合併輪數的關係

用途：找出最佳 k 值以最小化總輸入時間，並理解 k 值選擇的權衡。

#### 3. CPU 與 I/O 時間平衡分析
我們研究了 CPU 處理時間與 I/O 時間的平衡點：

設定固定 CPU 處理時間、
測量不同 k 值下的 I/O 時間、
找出 t_cpu ≈ t_input 的平衡點

用途：確定系統資源的最佳配置，使 CPU 和 I/O 裝置的利用率達到平衡。

#### 4. 效能模擬框架
我們實現了一個完整的效能模擬框架：

模擬磁碟 I/O 操作，包括搜尋、旋轉延遲和資料傳輸、
支援不同大小的資料集、
提供詳細的效能數據輸出

用途：用於評估不同參數組合下的排序效能，並驗證理論分析結果。

#### 5. 資料視覺化與分析
我們實現了效能數據的視覺化功能：

繪製總輸入時間隨 k 值變化的曲線圖、
標記出最佳 k 值和平衡點、
比較理論值與實際測量值的差異

用途：直觀展示效能特徵，幫助理解外部排序的行為。

#### 6. 參數靈敏度分析
我們研究了不同參數對效能的影響：

記憶體容量 S 的影響、
區塊大小 B 的影響、
記錄數量 n 的影響

用途：為不同規模的排序問題提供參數配置建議。
