#pragma once
// MinPQ.h
template <class T>
class MinPQ {
public:
    virtual ~MinPQ() {}                           // �����Ѻc�l
    virtual bool IsEmpty() const = 0;             // �^�Ǧ�C�O�_����
    virtual const T& Top() const = 0;             // �^�ǳ̤p�Ȫ��Ѧ�
    virtual void Push(const T& x) = 0;            // �N�����[�J��C
    virtual void Pop() = 0;                       // �����̤p�Ȥ���
};
// MinHeap.h
#include <vector>
#include <stdexcept>

template <class T>
class MinHeap : public MinPQ<T> {
private:
    std::vector<T> heap;  // �ϥ� vector �x�s heap

    void HeapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index] < heap[parent]) {
                std::swap(heap[index], heap[parent]);
                index = parent;
            }
            else break;
        }
    }

    void HeapifyDown(int index) {
        int size = heap.size();
        while (index * 2 + 1 < size) {
            int left = index * 2 + 1;
            int right = index * 2 + 2;
            int smallest = index;

            if (left < size && heap[left] < heap[smallest])
                smallest = left;
            if (right < size && heap[right] < heap[smallest])
                smallest = right;

            if (smallest != index) {
                std::swap(heap[index], heap[smallest]);
                index = smallest;
            }
            else break;
        }
    }

public:
    bool IsEmpty() const override {
        return heap.empty();
    }

    const T& Top() const override {
        if (IsEmpty())
            throw std::runtime_error("Heap is empty");
        return heap[0];
    }

    void Push(const T& x) override {
        heap.push_back(x);
        HeapifyUp(heap.size() - 1);
    }

    void Pop() override {
        if (IsEmpty())
            throw std::runtime_error("Heap is empty");
        heap[0] = heap.back();
        heap.pop_back();
        if (!IsEmpty())
            HeapifyDown(0);
    }
};