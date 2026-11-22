// MaxHeap.cpp  (Mariam Aqeel)
// Binary max-heap stored in an array. The item with the highest priority is
// always at index 0. Used for priority boarding and emergency clearance.
#ifndef MAXHEAP_CPP
#define MAXHEAP_CPP

#include <iostream>
#include <string>
using namespace std;

struct HeapItem {
    int priority;      // bigger number = more important
    long seq;          // arrival number, used to break ties (earlier goes first)
    string label;
};

// Swaps two heap items.
void swapItems(HeapItem& a, HeapItem& b) {
    HeapItem temp = a;
    a = b;
    b = temp;
}

class PriorityHeap {
private:
    HeapItem* items;
    int capacity;
    int count;
    long nextSeq;

    // Returns true if item a should come out before item b.
    bool isBefore(HeapItem a, HeapItem b) {
        if (a.priority > b.priority) {
            return true;
        }
        if (a.priority < b.priority) {
            return false;
        }
        // same priority: the one that arrived earlier goes first
        if (a.seq < b.seq) {
            return true;
        }
        return false;
    }

    // Moves a new item up until the heap order is correct.
    void moveUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (isBefore(items[index], items[parent])) {
                swapItems(items[index], items[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // Moves the root item down until the heap order is correct.
    void moveDown(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int best = index;

            if (left < count) {
                if (isBefore(items[left], items[best])) {
                    best = left;
                }
            }
            if (right < count) {
                if (isBefore(items[right], items[best])) {
                    best = right;
                }
            }
            if (best == index) {
                break;
            }
            swapItems(items[index], items[best]);
            index = best;
        }
    }

public:
    PriorityHeap() {
        capacity = 16;
        count = 0;
        nextSeq = 0;
        items = new HeapItem[capacity];
    }

    ~PriorityHeap() {
        delete[] items;
    }

    void push(int priority, string label) {
        // if the array is full, make it twice as big
        if (count == capacity) {
            HeapItem* bigger = new HeapItem[capacity * 2];
            for (int i = 0; i < count; i++) {
                bigger[i] = items[i];
            }
            delete[] items;
            items = bigger;
            capacity = capacity * 2;
        }

        items[count].priority = priority;
        items[count].seq = nextSeq;
        items[count].label = label;
        nextSeq = nextSeq + 1;

        moveUp(count);
        count = count + 1;
    }

    // Removes the most important item into 'result'. Returns false if empty.
    bool pop(HeapItem& result) {
        if (count == 0) {
            return false;
        }
        result = items[0];
        count = count - 1;
        items[0] = items[count];
        moveDown(0);
        return true;
    }

    bool isEmpty() {
        return count == 0;
    }

    int size() {
        return count;
    }

    void clear() {
        count = 0;
    }
};

#endif
