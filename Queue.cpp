// Queue.cpp  (Mariam Aqeel)
// Two simple linked-list queues (First In, First Out).
//   StringQueue : used for the check-in counter (stores PNRs)
//   IntQueue    : used by BFS and topological sort (stores vertex numbers)
#ifndef QUEUE_CPP
#define QUEUE_CPP

#include <iostream>
#include <string>
using namespace std;

class StringQueue {
private:
    struct Node {
        string value;
        Node* next;

        Node(string v) {
            value = v;
            next = NULL;
        }
    };

    Node* head;     // front of the queue (removed first)
    Node* tail;     // back of the queue (added last)
    int count;

public:
    StringQueue() {
        head = NULL;
        tail = NULL;
        count = 0;
    }

    ~StringQueue() {
        string temp;
        while (dequeue(temp)) {
            // keep removing until the queue is empty
        }
    }

    // Adds a value at the back.
    void enqueue(string value) {
        Node* newNode = new Node(value);
        if (tail == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count = count + 1;
    }

    // Removes the front value into 'value'. Returns false if the queue is empty.
    bool dequeue(string& value) {
        if (head == NULL) {
            return false;
        }
        Node* oldHead = head;
        value = oldHead->value;
        head = head->next;
        if (head == NULL) {
            tail = NULL;
        }
        delete oldHead;
        count = count - 1;
        return true;
    }

    bool isEmpty() {
        return count == 0;
    }

    int size() {
        return count;
    }
};

class IntQueue {
private:
    struct Node {
        int value;
        Node* next;

        Node(int v) {
            value = v;
            next = NULL;
        }
    };

    Node* head;
    Node* tail;
    int count;

public:
    IntQueue() {
        head = NULL;
        tail = NULL;
        count = 0;
    }

    ~IntQueue() {
        int temp;
        while (dequeue(temp)) {
            // keep removing until the queue is empty
        }
    }

    void enqueue(int value) {
        Node* newNode = new Node(value);
        if (tail == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count = count + 1;
    }

    bool dequeue(int& value) {
        if (head == NULL) {
            return false;
        }
        Node* oldHead = head;
        value = oldHead->value;
        head = head->next;
        if (head == NULL) {
            tail = NULL;
        }
        delete oldHead;
        count = count - 1;
        return true;
    }

    bool isEmpty() {
        return count == 0;
    }

    int size() {
        return count;
    }
};

#endif
