// Stack.cpp  (Mariam Aqeel)
// A linked-list stack (Last In, First Out) and the gate manager that uses
// two stacks to provide undo and redo.
#ifndef STACK_CPP
#define STACK_CPP

#include <iostream>
#include <string>
#include "Util.cpp"
using namespace std;

// One gate change: which flight, the old gate and the new gate.
struct GateChange {
    string flight;
    string fromGate;
    string toGate;
};

class GateStack {
private:
    struct Node {
        GateChange change;
        Node* next;

        Node(GateChange c) {
            change = c;
            next = NULL;
        }
    };

    Node* top;

public:
    GateStack() {
        top = NULL;
    }

    ~GateStack() {
        clear();
    }

    void push(GateChange change) {
        Node* newNode = new Node(change);
        newNode->next = top;
        top = newNode;
    }

    // Removes the top item into 'change'. Returns false if the stack is empty.
    bool pop(GateChange& change) {
        if (top == NULL) {
            return false;
        }
        Node* oldTop = top;
        change = oldTop->change;
        top = top->next;
        delete oldTop;
        return true;
    }

    bool isEmpty() {
        return top == NULL;
    }

    void clear() {
        GateChange temp;
        while (pop(temp)) {
            // keep removing until the stack is empty
        }
    }
};

class GateManager {
private:
    string flights[10];
    string gates[10];
    int flightCount;
    GateStack undoStack;
    GateStack redoStack;

    // Returns the position of a flight in the arrays, or -1 if not found.
    int findFlight(string flight) {
        for (int i = 0; i < flightCount; i++) {
            if (lower(flights[i]) == lower(flight)) {
                return i;
            }
        }
        return -1;
    }

public:
    GateManager() {
        flightCount = 0;
    }

    bool addFlight(string flight, string gate) {
        if (flightCount >= 10) {
            return false;
        }
        if (findFlight(flight) != -1) {
            return false;
        }
        flights[flightCount] = flight;
        gates[flightCount] = gate;
        flightCount = flightCount + 1;
        return true;
    }

    // Changes a flight's gate and remembers it so it can be undone.
    bool changeGate(string flight, string newGate) {
        int i = findFlight(flight);
        if (i == -1) {
            return false;
        }
        GateChange change;
        change.flight = flights[i];
        change.fromGate = gates[i];
        change.toGate = newGate;

        undoStack.push(change);
        redoStack.clear();       // a new change makes the old redo history invalid
        gates[i] = newGate;
        return true;
    }

    bool undo() {
        GateChange change;
        if (undoStack.pop(change) == false) {
            return false;
        }
        int i = findFlight(change.flight);
        gates[i] = change.fromGate;
        redoStack.push(change);
        return true;
    }

    bool redo() {
        GateChange change;
        if (redoStack.pop(change) == false) {
            return false;
        }
        int i = findFlight(change.flight);
        gates[i] = change.toGate;
        undoStack.push(change);
        return true;
    }

    void show() {
        for (int i = 0; i < flightCount; i++) {
            cout << "  " << flights[i] << " -> Gate " << gates[i] << "\n";
        }
    }
};

#endif
