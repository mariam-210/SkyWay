// BaggageList : DOUBLY linked list that tracks every bag.
// Carousel    : CIRCULAR linked list (the belt that keeps going round).
#ifndef BAGGAGE_CPP
#define BAGGAGE_CPP

#include <iostream>
#include <string>
using namespace std;

// The stages a bag moves through.
string stageNames[7] = {
    "Checked-in",
    "Security Screening",
    "Sorted",
    "Loaded on Aircraft",
    "Unloaded at Destination",
    "On Carousel",
    "Claimed"
};
const int STAGE_UNLOADED = 4;
const int STAGE_CAROUSEL = 5;

struct Bag {
    int tag;
    string pnr;
    double kg;
    int stage;
    Bag* prev;      // previous bag in the list
    Bag* next;      // next bag in the list

    Bag(int t, string p, double w) {
        tag = t;
        pnr = p;
        kg = w;
        stage = 0;
        prev = NULL;
        next = NULL;
    }
};

class BaggageList {
private:
    Bag* head;
    Bag* tail;
    int nextTag;

public:
    BaggageList() {
        head = NULL;
        tail = NULL;
        nextTag = 5001;
    }

    ~BaggageList() {
        while (head != NULL) {
            Bag* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Adds a bag at the end of the list and returns its tag number.
    int add(string pnr, double kg) {
        Bag* newBag = new Bag(nextTag, pnr, kg);
        nextTag = nextTag + 1;

        if (tail == NULL) {
            head = newBag;
            tail = newBag;
        } else {
            newBag->prev = tail;
            tail->next = newBag;
            tail = newBag;
        }
        return newBag->tag;
    }

    Bag* find(int tag) {
        Bag* current = head;
        while (current != NULL) {
            if (current->tag == tag) {
                return current;
            }
            current = current->next;
        }
        return NULL;
    }

    // Removes a bag from anywhere in the list.
    bool remove(int tag) {
        Bag* target = find(tag);
        if (target == NULL) {
            return false;
        }

        if (target->prev == NULL) {
            head = target->next;
        } else {
            target->prev->next = target->next;
        }

        if (target->next == NULL) {
            tail = target->prev;
        } else {
            target->next->prev = target->prev;
        }

        delete target;
        return true;
    }

    void showForward() {
        if (head == NULL) {
            cout << "  (no bags in system)\n";
            return;
        }
        Bag* current = head;
        while (current != NULL) {
            cout << "  Tag " << current->tag << " | " << current->pnr << " | "
                 << current->kg << " kg | " << stageNames[current->stage] << "\n";
            current = current->next;
        }
    }

    void showBackward() {
        if (tail == NULL) {
            cout << "  (no bags in system)\n";
            return;
        }
        Bag* current = tail;
        while (current != NULL) {
            cout << "  Tag " << current->tag << " | " << current->pnr << " | "
                 << current->kg << " kg | " << stageNames[current->stage] << "\n";
            current = current->prev;
        }
    }
};

class Carousel {
private:
    struct Node {
        int tag;
        string pnr;
        Node* next;

        Node(int t, string p) {
            tag = t;
            pnr = p;
            next = NULL;
        }
    };

    Node* current;      // the bag now in front of the passengers
    int count;

public:
    Carousel() {
        current = NULL;
        count = 0;
    }

    ~Carousel() {
        if (current == NULL) {
            return;
        }
        // delete every node except 'current', then delete 'current' itself
        Node* node = current->next;
        for (int i = 1; i < count; i++) {
            Node* temp = node;
            node = node->next;
            delete temp;
        }
        delete current;
    }

    void add(int tag, string pnr) {
        Node* newNode = new Node(tag, pnr);

        if (current == NULL) {
            newNode->next = newNode;          // a loop of one bag
            current = newNode;
        } else {
            // find the last node (the one that points back to 'current')
            Node* last = current;
            while (last->next != current) {
                last = last->next;
            }
            last->next = newNode;
            newNode->next = current;
        }
        count = count + 1;
    }

    // Moves the belt forward by one bag.
    void rotate() {
        if (current == NULL) {
            cout << "  Carousel is empty.\n";
            return;
        }
        current = current->next;
        cout << "  Belt moves... now in front: Tag " << current->tag
             << " (" << current->pnr << ")\n";
    }

    // Passenger takes their bag off the belt.
    bool claim(int tag) {
        if (current == NULL) {
            return false;
        }

        // walk around the loop looking for the node BEFORE the bag we want
        Node* before = current;
        bool found = false;
        for (int i = 0; i < count; i++) {
            if (before->next->tag == tag) {
                found = true;
                break;
            }
            before = before->next;
        }
        if (found == false) {
            return false;
        }

        Node* target = before->next;
        if (target == before) {
            current = NULL;                   // it was the only bag
        } else {
            before->next = target->next;
            if (current == target) {
                current = target->next;
            }
        }
        delete target;
        count = count - 1;
        return true;
    }

    void show() {
        if (current == NULL) {
            cout << "  Carousel is empty.\n";
            return;
        }
        cout << "  Carousel loop: ";
        Node* node = current;
        for (int i = 0; i < count; i++) {
            cout << "[" << node->tag << "] -> ";
            node = node->next;
        }
        cout << "(back to start)\n";
    }
};

#endif