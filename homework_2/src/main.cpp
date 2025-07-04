#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <stack>
#include <queue>
#include <chrono>  // 添加時間庫
#include <algorithm>
#include <cmath>
using namespace std::chrono;  // 添加這一行
using namespace std;

// 定義二元樹節點
template <class K, class E>
struct TreeNode {
    pair<K, E> data;
    TreeNode<K, E>* left;
    TreeNode<K, E>* right;
    int leftSize;  // 左子樹的節點數，用於排名操作

    TreeNode(const pair<K, E>& thePair)
        : data(thePair), left(nullptr), right(nullptr), leftSize(1) {}
};

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

        // 情況1: 有兩個子節點
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

        // 打印時間與複雜度估計
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

// 生成隨機數的輔助函數
int getRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}

// 中序遍歷輔助函數
template <class K, class E>
void inOrderHelper(TreeNode<K, E>* node, vector<pair<K, E>>& result) {
    if (!node) return;
    inOrderHelper(node->left, result);
    result.push_back(node->data);
    inOrderHelper(node->right, result);
}

// 打印樹的結構
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

// 分析時間複雜度的函數
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
