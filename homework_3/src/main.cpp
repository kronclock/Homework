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

// 隨機產生初始 sorted runs，模擬預排序過的區塊
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

struct Record {
    int key;
    int run_id;
    bool operator>(const Record& other) const { return key > other.key; }
};

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

int main() {
    vector<int> n_values = { 100, 200, 500, 1000, 2000, 5000, 10000 };

    for (int n : n_values) {
        runExperiment(n);
    }

    analyze_input_time_vs_k();

    return 0;
}

