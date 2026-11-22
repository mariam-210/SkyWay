// HashTable.cpp  (Mariam Aqeel)
// Hash table with separate chaining. Key = PNR, value = Passenger.
#ifndef HASHTABLE_CPP
#define HASHTABLE_CPP

#include <iostream>
#include <string>
#include "Passenger.cpp"
using namespace std;

class PassengerHashTable {
private:
    // One node in a bucket's linked list.
    struct Node {
        Passenger data;
        Node* next;

        Node(Passenger p) {
            data = p;
            next = NULL;
        }
    };

    Node** table;     // array of bucket heads
    int capacity;     // number of buckets
    int count;        // number of passengers stored

    // djb2 hash function: turns a PNR into a bucket number.
    int hashFunction(string key) {
        unsigned long h = 5381;
        int length = key.length();
        for (int i = 0; i < length; i++) {
            h = h * 33 + (unsigned char)key[i];
        }
        return (int)(h % capacity);
    }

    // Makes the table bigger and re-inserts every node (done when it gets too full).
    void rehash() {
        int oldCapacity = capacity;
        Node** oldTable = table;

        capacity = capacity * 2 + 1;
        table = new Node*[capacity];
        for (int i = 0; i < capacity; i++) {
            table[i] = NULL;
        }

        for (int i = 0; i < oldCapacity; i++) {
            Node* current = oldTable[i];
            while (current != NULL) {
                Node* nextNode = current->next;
                int newIndex = hashFunction(current->data.pnr);
                current->next = table[newIndex];
                table[newIndex] = current;
                current = nextNode;
            }
        }
        delete[] oldTable;
    }

public:
    PassengerHashTable() {
        capacity = 31;
        count = 0;
        table = new Node*[capacity];
        for (int i = 0; i < capacity; i++) {
            table[i] = NULL;
        }
    }

    ~PassengerHashTable() {
        for (int i = 0; i < capacity; i++) {
            Node* current = table[i];
            while (current != NULL) {
                Node* nextNode = current->next;
                delete current;
                current = nextNode;
            }
        }
        delete[] table;
    }

    // Adds a passenger. Returns false if the PNR already exists.
    bool insert(Passenger p) {
        int index = hashFunction(p.pnr);

        Node* current = table[index];
        while (current != NULL) {
            if (current->data.pnr == p.pnr) {
                return false;
            }
            current = current->next;
        }

        Node* newNode = new Node(p);
        newNode->next = table[index];
        table[index] = newNode;
        count = count + 1;

        if (count > capacity * 2) {
            rehash();
        }
        return true;
    }

    // Returns a pointer to the passenger, or NULL if not found.
    Passenger* find(string pnr) {
        int index = hashFunction(pnr);
        Node* current = table[index];
        while (current != NULL) {
            if (current->data.pnr == pnr) {
                return &(current->data);
            }
            current = current->next;
        }
        return NULL;
    }

    // Deletes a passenger. Returns false if the PNR was not found.
    bool remove(string pnr) {
        int index = hashFunction(pnr);
        Node* previous = NULL;
        Node* current = table[index];

        while (current != NULL) {
            if (current->data.pnr == pnr) {
                if (previous == NULL) {
                    table[index] = current->next;
                } else {
                    previous->next = current->next;
                }
                delete current;
                count = count - 1;
                return true;
            }
            previous = current;
            current = current->next;
        }
        return false;
    }

    int size() {
        return count;
    }

    // Fills the list[] array with a pointer to every passenger.
    // Returns how many passengers were stored (at most maxCount).
    int collectAll(Passenger* list[], int maxCount) {
        int stored = 0;
        for (int i = 0; i < capacity; i++) {
            Node* current = table[i];
            while (current != NULL) {
                if (stored < maxCount) {
                    list[stored] = &(current->data);
                    stored = stored + 1;
                }
                current = current->next;
            }
        }
        return stored;
    }
};

#endif
