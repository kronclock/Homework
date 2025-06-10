# 20250610

#### 組員:  41243233郭思翰, 41243251潘彥愷

## 解題說明

本題作業，需寫二元搜尋樹，需使用一個均勻分布的亂數產生器取得要加入的數值。測量二元搜尋樹的高度並除以log₂(n)。使用n = 100,500,1000,2000,~,10000實作，並且繪製出圖形。另外需寫一個函式，可從二元搜尋樹中移除K值，並計算他的時間複雜度。

## 解題策略

1.	先初始化一棵空的二元搜尋樹（BST）。
2.	對每個指定的 n 值（如 100, 500, ..., 10000）：
              。執行均勻分布的隨機數產生器
              。測量此 BST 的高度 h。
              。計算 log₂(n) 並求出高度比值 h / log₂(n)。
              。將結果輸出到表格檔案及螢幕。
3.	使用python透過csv檔，去繪製出圖形
4.	寫移除K值的函式，需處理三種情況，節點無子節點、節點有一個子節點、節點有兩個子節點。
5.	分析函式的時間複雜度

## 程式實作

標頭
```cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <stack>
#include <queue>
#include <chrono>
#include <algorithm>
using namespace std::chrono;
using namespace std;
```

TreeNode 定義:
```cpp
template <class K, class E>
struct TreeNode {
    pair<K, E> data;
    TreeNode<K, E>* left;
    TreeNode<K, E>* right;
    int leftSize;
    TreeNode(const pair<K, E>& thePair)
         : data(thePair), left(nullptr), right(nullptr), leftSize(1) {}
};
```

BST 類別定義:
```cpp
// 二元搜尋樹類別
template <class K, class E>
class BST {
private:
    TreeNode<K, E>* root;
    int size;  // 樹的大小

    // 使用迭代方式清空樹
    void clearHelper(TreeNode<K, E>* node) {
        if (!node) return;

        // 使用堆疊來模擬遞迴
        vector<TreeNode<K, E>*> nodes;
        nodes.push_back(node);

        while (!nodes.empty()) {
            TreeNode<K, E>* current = nodes.back();
            nodes.pop_back();

            if (current->left) {
                nodes.push_back(current->left);
            }
            if (current->right) {
                nodes.push_back(current->right);
            }

            delete current;
        }
    }

    // 使用 BFS 計算樹高
    int getHeightHelper(TreeNode<K, E>* node) const {
        if (!node) return -1;

        queue<TreeNode<K, E>*> q;
        q.push(node);
        int height = -1;

        while (!q.empty()) {
            int levelSize = q.size();
            height++;

            for (int i = 0; i < levelSize; i++) {
                TreeNode<K, E>* current = q.front();
                q.pop();

                if (current->left) {
                    q.push(current->left);
                }
                if (current->right) {
                    q.push(current->right);
                }
            }
        }

        return height;
    }

public:
    BST() : root(nullptr), size(0) {}
    ~BST() { clear(); }

    // 基本操作
    bool IsEmpty() const { return root == nullptr; }

    // 查找元素
    pair<K, E>* Get(const K& k) const {
        TreeNode<K, E>* current = root;
        while (current) {
            if (k < current->data.first)
                current = current->left;
            else if (k > current->data.first)
                current = current->right;
            else
                return &(current->data);
        }
        return nullptr;
    }

    // 插入元素
    void Insert(const pair<K, E>& thePair) {
        TreeNode<K, E>** current = &root;
        TreeNode<K, E>* parent = nullptr;

        // 尋找插入位置
        while (*current) {
            parent = *current;
            if (thePair.first < (*current)->data.first) {
                (*current)->leftSize++;
                current = &((*current)->left);
            }
            else if (thePair.first > (*current)->data.first) {
                current = &((*current)->right);
            }
            else {
                // 鍵值已存在，更新值
                (*current)->data.second = thePair.second;
                return;
            }
        }

        // 創建新節點
        *current = new TreeNode<K, E>(thePair);
        size++;
    }

    // 刪除元素 - 題目 (b) 要求
    void Delete(const K& k) {
        auto start = high_resolution_clock::now();  // 開始計時

        TreeNode<K, E>** current = &root;
        TreeNode<K, E>* toDelete = nullptr;

        // 尋找要刪除的節點
        while (*current) {
            if (k < (*current)->data.first) {
                if ((*current)->left) {
                    (*current)->leftSize--;
                    current = &((*current)->left);
                }
                else {
                    break;
                }
            }
            else if (k > (*current)->data.first) {
                current = &((*current)->right);
            }
            else {
                toDelete = *current;
                break;
            }
        }

        if (!toDelete) {
            cout << "找不到要刪除的節點 " << k << "！" << endl;
            return;
        }

        // 有兩個子節點
        if (toDelete->left && toDelete->right) {
            TreeNode<K, E>* parent = toDelete;
            TreeNode<K, E>* minNode = toDelete->right;

            while (minNode->left) {
                parent = minNode;
                minNode = minNode->left;
            }

            toDelete->data = minNode->data;

            if (parent == toDelete) {
                parent->right = minNode->right;
            }
            else {
                parent->left = minNode->right;
            }

            toDelete = minNode;
        }
        else {
            *current = (toDelete->left) ? toDelete->left : toDelete->right;
        }

        delete toDelete;
        size--;

        auto stop = high_resolution_clock::now();  // 結束計時
        auto duration = duration_cast<nanoseconds>(stop - start);

        // 輸出時間與複雜度
        cout << "[Delete 函式分析]" << endl;
        cout << "- 操作時間: " << duration.count() << " 納秒" << endl;
        int h = getHeight();
        cout << "- 樹高 h = " << h << endl;
        cout << "- 理論複雜度 = O(h) 約= O(" << h << ")" << endl;
        //cout << "- 最差情況: O(n), 平均: O(log n)" << endl;
        cout << endl;
    }

    // 清空樹
    void clear() {
        clearHelper(root);
        root = nullptr;
        size = 0;
    }

    // 獲取樹高
    int getHeight() const {
        return getHeightHelper(root);
    }

    // 獲取樹的大小
    int getSize() const {
        return size;
    }

    // 新增：獲取根節點（用於打印樹結構）
    TreeNode<K, E>* getRoot() const {
        return root;
    }
};
```

生成隨機數:
```cpp
int getRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}
```

中序遍歷:
```cpp
template <class K, class E>
void inOrderHelper(TreeNode<K, E>* node, vector<pair<K, E>>& result) {
    if (!node) return;
    inOrderHelper(node->left, result);
    result.push_back(node->data);
    inOrderHelper(node->right, result);
}
```

打印樹的結構:
```cpp
template <class K, class E>
void printTree(const BST<K, E>& tree) {
    auto root = tree.getRoot();  // 使用公開方法獲取根節點
    if (!root) {
        cout << "空樹" << endl;
        return;
    }

    // 使用 BFS 層序遍歷
    queue<TreeNode<K, E>*> q;
    q.push(root);
    int level = 0;

    while (!q.empty()) {
        int levelSize = q.size();
        cout << "第 " << level << " 層: ";

        for (int i = 0; i < levelSize; ++i) {
            TreeNode<K, E>* current = q.front();
            q.pop();

            // 打印當前節點
            cout << current->data.first << "(" << current->leftSize << ")";

            // 如果有子節點，加入隊列
            if (current->left || current->right) {
                cout << " -> [";
                if (current->left) {
                    cout << current->left->data.first;
                    q.push(current->left);
                }
                else {
                    cout << "null";
                }
                cout << ", ";
                if (current->right) {
                    cout << current->right->data.first;
                    q.push(current->right);
                }
                else {
                    cout << "null";
                }
                cout << "]";
            }

            cout << "   ";
        }

        cout << endl;
        level++;
    }

    // 打印中序遍歷結果
    vector<pair<K, E>> inOrderList;
    inOrderHelper(root, inOrderList);
    cout << "中序遍歷: ";
    for (const auto& p : inOrderList) {
        cout << p.first << " ";
    }
    cout << endl << endl;
}
```

分析時間複雜度:
```cpp
void analyzeDeleteComplexity() {
    cout << "\n===== 分析 Delete 操作的時間複雜度 =====" << endl;

    // 測試不同的樹大小
    vector<int> sizes = { 1000, 2000, 4000, 8000, 16000 };

    cout << "大小\t最差情況(ms)\t平均情況(ms)\t理論複雜度" << endl;
    cout << "------------------------------------------" << endl;

    for (int n : sizes) {
        // 測試最差情況（退化的樹）
        {
            BST<int, int> tree;
            for (int i = 0; i < n; i++) {
                tree.Insert(make_pair(i, i));  // 最差情況：退化成鍊錶
            }

            // 測量刪除根節點的時間
            auto start = high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                // 創建新樹並刪除節點
                BST<int, int> temp;
                for (int j = 0; j < n; j++) {
                    temp.Insert(make_pair(j, j));
                }
                temp.Delete(n / 2);
            }
            auto stop = high_resolution_clock::now();
            auto worst_case = duration_cast<milliseconds>(stop - start).count() / 100.0;

            cout << n << "\t" << worst_case << "\t\t";
        }  // temp 在這裡被銷毀

        // 測試平均情況（平衡的樹）
        {
            BST<int, int> tree;
            vector<int> nums;
            for (int i = 0; i < n; i++) nums.push_back(i);
            random_shuffle(nums.begin(), nums.end());
            for (int num : nums) {
                tree.Insert(make_pair(num, num));
            }

            // 測量刪除隨機節點的時間
            auto start = high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                // 創建新樹並刪除節點
                BST<int, int> temp;
                for (int num : nums) {
                    temp.Insert(make_pair(num, num));
                }
                temp.Delete(nums[rand() % n]);
            }
            auto stop = high_resolution_clock::now();
            auto avg_case = duration_cast<milliseconds>(stop - start).count() / 100.0;

            cout << avg_case << "\t\tO(n) / O(log n)" << endl;
        }  // temp 在這裡被銷毀
    }
}
```

主函式:
```cpp
int main() {
    // 設置隨機數種子
    srand(static_cast<unsigned int>(time(0)));

    // 測試不同的n值
    vector<int> n_values = { 100, 500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000 };

    // 輸出結果到檔案
    ofstream outFile("bst_results.csv");
    outFile << "n,height,log2(n),height/log2(n)" << endl;

    cout << "n\theight\tlog2(n)\theight/log2(n)" << endl;
    cout << "------------------------------------" << endl;

    for (int n : n_values) {
        BST<int, int> tree;

        // 插入n個隨機數
        for (int i = 0; i < n; i++) {
            int num = getRandomNumber(1, n * 10);  // 範圍設大一些以減少重複
            tree.Insert(make_pair(num, num * 10));
        }

        // 計算高度和比值
        int height = tree.getHeight();
        double log2n = log2(n);
        double ratio = height / log2n;

        // 輸出結果
        cout << n << "\t" << height << "\t" << fixed << setprecision(2)
            << log2n << "\t" << ratio << endl;
        outFile << n << "," << height << "," << fixed << setprecision(2)
            << log2n << "," << ratio << endl;
    }

    outFile.close();
    cout << "\n測試完成！結果已儲存到 bst_results.csv" << endl;

    // 題目 (b) 部分
    // 題目 (b) 部分 - 使用固定數據測試
    cout << "\n===== 題目 (b) 測試 =====\n";
    BST<int, int> testTree;
    vector<int> testData = { 50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45, 55, 65, 75, 90 };

    // 插入測試數據
    cout << "插入測試數據: ";
    for (int num : testData) {
        cout << num << " ";
        testTree.Insert(make_pair(num, num * 10));
    }
    cout << endl;

    // 打印初始樹
    cout << "\n初始樹結構：" << endl;
    printTree(testTree);
    cout << "樹高: " << testTree.getHeight() << endl;
    cout << "節點數: " << testTree.getSize() << endl;

    // 測試刪除所有節點
    vector<int> nodesToDelete = testData;  // 複製一份來刪除
    random_shuffle(nodesToDelete.begin(), nodesToDelete.end());  // 隨機打亂刪除順序

    cout << "\n===== 開始刪除節點 =====\n";
    for (int num : nodesToDelete) {
        cout << "\n====================================" << endl;
        cout << "刪除節點: " << num << endl;

        // 測量刪除操作的時間
        auto start = high_resolution_clock::now();
        testTree.Delete(num);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(stop - start);

        // 輸出刪除後的樹結構
        cout << "刪除後的樹結構：" << endl;
        printTree(testTree);
        cout << "當前樹高: " << testTree.getHeight() << endl;
        cout << "剩餘節點數: " << testTree.getSize() << endl;
        cout << "刪除操作耗時: " << duration.count() << " 納秒" << endl;
    }
    return 0;
}
```

## 效能分析

1. 時間複雜度：
   
   插入操作 :
  	| 情況   | 時間複雜度 |
	|----------|--------------|
	| 最佳情況 | $O(log n)$         |
	| 最差情況   | $O(n)$       |
	| 平均情況         | $O(log n)$       |

  	插入操作的效率取決於樹的高度。若 BST 結構接近平衡（如隨機插入），插入只需 O(log n)；若退化為鏈狀結構（如遞增/遞減資料），插入需 O(n)。

  	 刪除操作 :   
   	| 情況     | 時間複雜度    |
	|-----------|-------------|
	| 最佳情況  | $O(log n)$   |
	| 最差情況  | $O(n)$       |
	| 平均情況  | $O(log n)$   |

  	刪除操作的成本與樹高成正比。刪除一個節點需先定位該節點，再根據其子節點情況進行調整（如找後繼節點）。若樹為平衡，刪除效率高；否則會退化。

3. 空間複雜度:

   插入操作 : O(log n)
   
   刪除操作 : O(log n)

## 測試與驗證

#### 測試題目(a)
|n       |height  |log2(n) |height/log2(n)|
|--------|--------|--------|--------------|
|100     |10      |6.64    |1.51          |
|500     |16      |8.97    |1.78          |
|1000    |19      |9.97    |1.91          |
|2000    |22      |10.97   |2.01          |
|3000    |24      |11.55   |2.08          |
|4000    |25      |11.97   |2.09          |
|5000    |27      |12.29   |2.20          |
|6000    |26      |12.55   |2.07          |
|7000    |28      |12.77   |2.19          |
|8000    |28      |12.97   |2.16          |
|9000    |31      |13.14   |2.36          |
|10000   |28      |13.29   |2.11          |

![image](https://github.com/user-attachments/assets/5a3d26ec-8245-4ea3-baff-4dff144a5898)

|統計分析|       |
|-------|-------|
|平均比值| 2.0392|
|中位數  | 2.0850|
|最小值  | 1.5100|
|最大值  | 2.3600|
|標準差  | 0.2218|

#### 測試題目(b)
插入測試數據: 50 30 70 20 40 60 80 10 25 35 45 55 65 75 90

刪除節點: 50

|Delete 函式分析|     |
|--------------|------|
|操作時間:| 6400 納秒|
| 樹高 |h = 3|
| 理論複雜度 | O(h) 約= O(3)|

## 編譯與執行指令
輸出的內容上半部為(a)，下半為(b)
```shell
$ g++ -std=c++17 -o main.exe
$ .\main.exe
n       height  log2(n) height/log2(n)
------------------------------------
100     11      6.64    1.66
500     17      8.97    1.90
1000    20      9.97    2.01
2000    21      10.97   1.92
3000    24      11.55   2.08
4000    27      11.97   2.26
5000    27      12.29   2.20
6000    30      12.55   2.39
7000    30      12.77   2.35
8000    28      12.97   2.16
9000    30      13.14   2.28
10000   28      13.29   2.11

===== 題目 (b) 測試 =====
插入測試數據: 50 30 70 20 40 60 80 10 25 35 45 55 65 75 90

初始樹結構：
第 0 層: 50(8) -> [30, 70]
第 1 層: 30(4) -> [20, 40]   70(4) -> [60, 80]
第 2 層: 20(2) -> [10, 25]   40(2) -> [35, 45]   60(2) -> [55, 65]   80(2) -> [75, 90]
第 3 層: 10(1)   25(1)   35(1)   45(1)   55(1)   65(1)   75(1)   90(1)
中序遍歷: 10 20 25 30 35 40 45 50 55 60 65 70 75 80 90

樹高: 3
節點數: 15

===== 開始刪除節點 =====

====================================
刪除節點: 50
[Delete 函式分析]
- 操作時間: 1400 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 55(8) -> [30, 70]
第 1 層: 30(4) -> [20, 40]   70(4) -> [60, 80]
第 2 層: 20(2) -> [10, 25]   40(2) -> [35, 45]   60(2) -> [null, 65]   80(2) -> [75, 90]
第 3 層: 10(1)   25(1)   35(1)   45(1)   65(1)   75(1)   90(1)
中序遍歷: 10 20 25 30 35 40 45 55 60 65 70 75 80 90

當前樹高: 3
剩餘節點數: 14
刪除操作耗時: 623800 納秒

====================================
刪除節點: 35
[Delete 函式分析]
- 操作時間: 1900 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 55(7) -> [30, 70]
第 1 層: 30(4) -> [20, 40]   70(4) -> [60, 80]
第 2 層: 20(2) -> [10, 25]   40(1) -> [null, 45]   60(2) -> [null, 65]   80(2) -> [75, 90]
第 3 層: 10(1)   25(1)   45(1)   65(1)   75(1)   90(1)
中序遍歷: 10 20 25 30 40 45 55 60 65 70 75 80 90

當前樹高: 3
剩餘節點數: 13
刪除操作耗時: 648900 納秒

====================================
刪除節點: 30
[Delete 函式分析]
- 操作時間: 1900 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 55(6) -> [40, 70]
第 1 層: 40(4) -> [20, 45]   70(4) -> [60, 80]
第 2 層: 20(2) -> [10, 25]   45(1)   60(2) -> [null, 65]   80(2) -> [75, 90]
第 3 層: 10(1)   25(1)   65(1)   75(1)   90(1)
中序遍歷: 10 20 25 40 45 55 60 65 70 75 80 90

當前樹高: 3
剩餘節點數: 12
刪除操作耗時: 427100 納秒

====================================
刪除節點: 40
[Delete 函式分析]
- 操作時間: 1400 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 55(5) -> [45, 70]
第 1 層: 45(4) -> [20, null]   70(4) -> [60, 80]
第 2 層: 20(2) -> [10, 25]   60(2) -> [null, 65]   80(2) -> [75, 90]
第 3 層: 10(1)   25(1)   65(1)   75(1)   90(1)
中序遍歷: 10 20 25 45 55 60 65 70 75 80 90

當前樹高: 3
剩餘節點數: 11
刪除操作耗時: 444800 納秒

====================================
刪除節點: 65
[Delete 函式分析]
- 操作時間: 800 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 55(5) -> [45, 70]
第 1 層: 45(4) -> [20, null]   70(3) -> [60, 80]
第 2 層: 20(2) -> [10, 25]   60(2)   80(2) -> [75, 90]
第 3 層: 10(1)   25(1)   75(1)   90(1)
中序遍歷: 10 20 25 45 55 60 70 75 80 90

當前樹高: 3
剩餘節點數: 10
刪除操作耗時: 465900 納秒

====================================
刪除節點: 55
[Delete 函式分析]
- 操作時間: 1000 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 60(5) -> [45, 70]
第 1 層: 45(4) -> [20, null]   70(3) -> [null, 80]
第 2 層: 20(2) -> [10, 25]   80(2) -> [75, 90]
第 3 層: 10(1)   25(1)   75(1)   90(1)
中序遍歷: 10 20 25 45 60 70 75 80 90

當前樹高: 3
剩餘節點數: 9
刪除操作耗時: 479700 納秒

====================================
刪除節點: 20
[Delete 函式分析]
- 操作時間: 1000 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 60(4) -> [45, 70]
第 1 層: 45(3) -> [25, null]   70(3) -> [null, 80]
第 2 層: 25(2) -> [10, null]   80(2) -> [75, 90]
第 3 層: 10(1)   75(1)   90(1)
中序遍歷: 10 25 45 60 70 75 80 90

當前樹高: 3
剩餘節點數: 8
刪除操作耗時: 434100 納秒

====================================
刪除節點: 70
[Delete 函式分析]
- 操作時間: 1100 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 60(4) -> [45, 80]
第 1 層: 45(3) -> [25, null]   80(2) -> [75, 90]
第 2 層: 25(2) -> [10, null]   75(1)   90(1)
第 3 層: 10(1)
中序遍歷: 10 25 45 60 75 80 90

當前樹高: 3
剩餘節點數: 7
刪除操作耗時: 472700 納秒

====================================
刪除節點: 60
[Delete 函式分析]
- 操作時間: 2200 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 75(4) -> [45, 80]
第 1 層: 45(3) -> [25, null]   80(2) -> [null, 90]
第 2 層: 25(2) -> [10, null]   90(1)
第 3 層: 10(1)
中序遍歷: 10 25 45 75 80 90

當前樹高: 3
剩餘節點數: 6
刪除操作耗時: 614600 納秒

====================================
刪除節點: 80
[Delete 函式分析]
- 操作時間: 1000 納秒
- 樹高 h = 3
- 理論複雜度 = O(h) 約= O(3)

刪除後的樹結構：
第 0 層: 75(4) -> [45, 90]
第 1 層: 45(3) -> [25, null]   90(1)
第 2 層: 25(2) -> [10, null]
第 3 層: 10(1)
中序遍歷: 10 25 45 75 90

當前樹高: 3
剩餘節點數: 5
刪除操作耗時: 428000 納秒

====================================
刪除節點: 25
[Delete 函式分析]
- 操作時間: 1800 納秒
- 樹高 h = 2
- 理論複雜度 = O(h) 約= O(2)

刪除後的樹結構：
第 0 層: 75(3) -> [45, 90]
第 1 層: 45(2) -> [10, null]   90(1)
第 2 層: 10(1)
中序遍歷: 10 45 75 90

當前樹高: 2
剩餘節點數: 4
刪除操作耗時: 503400 納秒

====================================
刪除節點: 90
[Delete 函式分析]
- 操作時間: 900 納秒
- 樹高 h = 2
- 理論複雜度 = O(h) 約= O(2)

刪除後的樹結構：
第 0 層: 75(3) -> [45, null]
第 1 層: 45(2) -> [10, null]
第 2 層: 10(1)
中序遍歷: 10 45 75

當前樹高: 2
剩餘節點數: 3
刪除操作耗時: 436600 納秒

====================================
刪除節點: 45
[Delete 函式分析]
- 操作時間: 1600 納秒
- 樹高 h = 1
- 理論複雜度 = O(h) 約= O(1)

刪除後的樹結構：
第 0 層: 75(2) -> [10, null]
第 1 層: 10(1)
中序遍歷: 10 75

當前樹高: 1
剩餘節點數: 2
刪除操作耗時: 668300 納秒

====================================
刪除節點: 75
[Delete 函式分析]
- 操作時間: 800 納秒
- 樹高 h = 0
- 理論複雜度 = O(h) 約= O(0)

刪除後的樹結構：
第 0 層: 10(1)
中序遍歷: 10

當前樹高: 0
剩餘節點數: 1
刪除操作耗時: 541800 納秒

====================================
刪除節點: 10
[Delete 函式分析]
- 操作時間: 1800 納秒
- 樹高 h = -1
- 理論複雜度 = O(h) 約= O(-1)

刪除後的樹結構：
空樹
當前樹高: -1
剩餘節點數: 0
刪除操作耗時: 467500 納秒
```

## 結論

## 申論及開發報告

#### 1. 二元搜尋樹的建立與高度分析
我們使用隨機生成的整數序列來建立 BST，以模擬平均情況下的樹結構。通過測量不同資料量下的樹高 h，並計算 h/log₂n 比值，我們觀察到：

隨機資料建立的 BST 高度接近理論最小值
實驗結果驗證了 BST 平均高度為 O(log n) 的理論分析
用途：這種方法確保了測試結果的客觀性，避免了特定資料分佈可能導致的偏差。

#### 2. 刪除操作的系統化測試
我們設計了完整的測試流程來驗證刪除操作的正確性和效能：

使用固定測試資料集 {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45, 55, 65, 75, 90}
分別測試刪除葉節點、單子節點和雙子節點的情況
測量並比較不同情況下的執行時間
用途：確保刪除操作在所有情況下都能正確執行，並驗證其時間複雜度。

#### 3. 時間複雜度分析框架
   
我們實現了自動化測試框架來分析刪除操作的時間複雜度：

對不同大小的樹進行測試
分別測量最差情況（退化樹）和平均情況（隨機樹）的執行時間
比較實際執行時間與理論複雜度的關係
用途：驗證刪除操作的時間複雜度是否與理論分析一致，並觀察資料量對效能的影響。

#### 4. 樹結構視覺化工具

我們實現了樹的層序遍歷顯示功能，可以直觀地觀察：

樹的整體結構
每個節點的左子樹大小
刪除操作前後的樹結構變化
用途：幫助理解 BST 的結構特性和刪除操作的影響，便於除錯和教學演示。

#### 5. 效能量測與分析
使用高精度計時器 (high_resolution_clock) 測量微秒級執行時間
重複測試取平均值以減少測量誤差
分析樹高與節點數的關係，驗證理論複雜度
用途：提供客觀的效能數據，支持對不同操作的時間複雜度進行量化分析。
