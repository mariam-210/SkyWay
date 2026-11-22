#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
using namespace std;

// ============================================================
// [SHARED] Util
// ============================================================
// Util.cpp  (shared)


// Returns a lower-case copy of a string, so we can compare ignoring upper/lower case.
string lower(string text) {
    string result = text;
    int length = result.length();
    for (int i = 0; i < length; i++) {
        result[i] = tolower(result[i]);
    }
    return result;
}

// ============================================================
// [SHARED] Passenger
// ============================================================
// Passenger.cpp  (shared)


// One passenger record.
struct Passenger {
    string pnr;        // booking reference, e.g. PNR1001
    string name;
    string passport;
    int flightNo;
    string seat;       // e.g. 3B
    int tier;          // 0 = Economy, 1 = Business, 2 = First, 3 = VIP
    bool checkedIn;
    bool boarded;

    Passenger() {
        pnr = "";
        name = "";
        passport = "";
        flightNo = 101;
        seat = "-";
        tier = 0;
        checkedIn = false;
        boarded = false;
    }
};

// Converts the tier number into a readable name.
string tierName(int tier) {
    if (tier == 0) {
        return "Economy";
    }
    if (tier == 1) {
        return "Business";
    }
    if (tier == 2) {
        return "First";
    }
    if (tier == 3) {
        return "VIP";
    }
    return "Unknown";
}

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


class AVLIndex {
private:
    struct Node {
        string key;
        string pnr;
        int height;
        Node* left;
        Node* right;

        Node(string k, string p) {
            key = k;
            pnr = p;
            height = 1;
            left = NULL;
            right = NULL;
        }
    };

    Node* root;
    bool insertedOk;     // set by insertNode
    bool removedOk;      // set by removeNode

    int heightOf(Node* node) {
        if (node == NULL) {
            return 0;
        }
        return node->height;
    }

    void updateHeight(Node* node) {
        int leftHeight = heightOf(node->left);
        int rightHeight = heightOf(node->right);
        if (leftHeight > rightHeight) {
            node->height = leftHeight + 1;
        } else {
            node->height = rightHeight + 1;
        }
    }

    // Right rotation (fixes a left-heavy tree).
    Node* rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Left rotation (fixes a right-heavy tree).
    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // Checks the balance factor of a node and rotates if it is out of balance.
    Node* balanceNode(Node* node) {
        updateHeight(node);
        int balance = heightOf(node->left) - heightOf(node->right);

        if (balance > 1) {
            // left-right case: first rotate the left child to the left
            if (heightOf(node->left->left) < heightOf(node->left->right)) {
                node->left = rotateLeft(node->left);
            }
            return rotateRight(node);
        }
        if (balance < -1) {
            // right-left case: first rotate the right child to the right
            if (heightOf(node->right->right) < heightOf(node->right->left)) {
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node);
        }
        return node;
    }

    // Recursive insert. Returns the new root of this subtree.
    Node* insertNode(Node* node, string key, string pnr) {
        if (node == NULL) {
            insertedOk = true;
            return new Node(key, pnr);
        }
        if (key < node->key) {
            node->left = insertNode(node->left, key, pnr);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, pnr);
        } else {
            insertedOk = false;      // duplicate key
            return node;
        }
        return balanceNode(node);
    }

    // Recursive delete. Returns the new root of this subtree.
    Node* removeNode(Node* node, string key) {
        if (node == NULL) {
            return NULL;
        }
        if (key < node->key) {
            node->left = removeNode(node->left, key);
        } else if (key > node->key) {
            node->right = removeNode(node->right, key);
        } else {
            removedOk = true;
            // zero or one child: replace the node by its child
            if (node->left == NULL || node->right == NULL) {
                Node* child = node->left;
                if (child == NULL) {
                    child = node->right;
                }
                delete node;
                return child;
            }
            // two children: copy the smallest node of the right side, then delete that one
            Node* smallest = node->right;
            while (smallest->left != NULL) {
                smallest = smallest->left;
            }
            node->key = smallest->key;
            node->pnr = smallest->pnr;
            string smallestKey = smallest->key;
            node->right = removeNode(node->right, smallestKey);
        }
        return balanceNode(node);
    }

    void destroy(Node* node) {
        if (node == NULL) {
            return;
        }
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    // In-order traversal: left, node, right (gives sorted order).
    void printNode(Node* node) {
        if (node == NULL) {
            return;
        }
        printNode(node->left);
        cout << "  " << node->key << " -> " << node->pnr << "\n";
        printNode(node->right);
    }

public:
    AVLIndex() {
        root = NULL;
        insertedOk = false;
        removedOk = false;
    }

    ~AVLIndex() {
        destroy(root);
    }

    bool insert(string key, string pnr) {
        insertedOk = false;
        root = insertNode(root, key, pnr);
        return insertedOk;
    }

    bool remove(string key) {
        removedOk = false;
        root = removeNode(root, key);
        return removedOk;
    }

    // Looks for a passport. If found, puts the PNR in pnrOut and returns true.
    bool search(string key, string& pnrOut) {
        Node* current = root;
        while (current != NULL) {
            if (key < current->key) {
                current = current->left;
            } else if (key > current->key) {
                current = current->right;
            } else {
                pnrOut = current->pnr;
                return true;
            }
        }
        return false;
    }

    bool contains(string key) {
        string unused;
        return search(key, unused);
    }

    int height() {
        return heightOf(root);
    }

    void printInOrder() {
        printNode(root);
    }
};


// Builds the LPS array: lps[i] = length of the longest proper prefix of the
// pattern that is also a suffix of pattern[0..i].
void buildLPS(string pattern, int lps[]) {
    int m = pattern.length();
    int len = 0;
    lps[0] = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len = len + 1;
            lps[i] = len;
            i = i + 1;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i = i + 1;
            }
        }
    }
}

// Returns true if 'pattern' appears anywhere inside 'text'.
bool kmpContains(string text, string pattern) {
    int n = text.length();
    int m = pattern.length();
    if (m == 0) {
        return false;
    }

    int* lps = new int[m];
    buildLPS(pattern, lps);

    bool found = false;
    int i = 0;      // position in text
    int j = 0;      // position in pattern

    while (i < n) {
        if (text[i] == pattern[j]) {
            i = i + 1;
            j = j + 1;
            if (j == m) {
                found = true;
                break;
            }
        } else {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i = i + 1;
            }
        }
    }

    delete[] lps;
    return found;
}

// A growing list of suspicious names or passport fragments.
class Watchlist {
private:
    string* items;
    int count;
    int capacity;

public:
    Watchlist() {
        capacity = 8;
        count = 0;
        items = new string[capacity];
    }

    ~Watchlist() {
        delete[] items;
    }

    void add(string entry) {
        if (count == capacity) {
            string* bigger = new string[capacity * 2];
            for (int i = 0; i < count; i++) {
                bigger[i] = items[i];
            }
            delete[] items;
            items = bigger;
            capacity = capacity * 2;
        }
        items[count] = entry;
        count = count + 1;
    }

    int size() {
        return count;
    }

    string get(int index) {
        return items[index];
    }

    // Checks the text against every entry (ignoring case).
    // Returns the position of the first matching entry, or -1 if none match.
    int findMatch(string text) {
        string lowerText = lower(text);
        for (int i = 0; i < count; i++) {
            if (kmpContains(lowerText, lower(items[i]))) {
                return i;
            }
        }
        return -1;
    }
};



class Cabin {
private:
    int rows;
    int* rowLength;        // rowLength[r] = number of seats in row r
    string** owner;        // owner[r][c] = PNR of the passenger, "" means free
    int firstEnd;          // rows 0 .. firstEnd-1            = First class
    int businessEnd;       // rows firstEnd .. businessEnd-1  = Business class
                           // remaining rows                  = Economy class

    // Finds which rows belong to a passenger's class.
    void getRowRange(int tier, int& firstRow, int& lastRow) {
        if (tier >= 2) {                 // First and VIP
            firstRow = 0;
            lastRow = firstEnd;
        } else if (tier == 1) {          // Business
            firstRow = firstEnd;
            lastRow = businessEnd;
        } else {                         // Economy
            firstRow = businessEnd;
            lastRow = rows;
        }
    }

    // Builds a seat name such as "3B" from row and column numbers.
    string seatName(int row, int col) {
        string name = to_string(row + 1);
        name = name + (char)('A' + col);
        return name;
    }

public:
    Cabin() {
        rows = 10;
        firstEnd = 2;
        businessEnd = 5;

        rowLength = new int[rows];
        rowLength[0] = 4;
        rowLength[1] = 4;
        rowLength[2] = 6;
        rowLength[3] = 6;
        rowLength[4] = 6;
        rowLength[5] = 9;
        rowLength[6] = 9;
        rowLength[7] = 9;
        rowLength[8] = 9;
        rowLength[9] = 9;

        // allocate each row with its own length (this is what makes it jagged)
        owner = new string*[rows];
        for (int r = 0; r < rows; r++) {
            owner[r] = new string[rowLength[r]];
        }
    }

    ~Cabin() {
        for (int r = 0; r < rows; r++) {
            delete[] owner[r];
        }
        delete[] owner;
        delete[] rowLength;
    }

    // Gives the passenger the first free seat in their class.
    // Returns false if that class is full.
    bool assign(int tier, string pnr, string& seatOut) {
        int firstRow;
        int lastRow;
        getRowRange(tier, firstRow, lastRow);

        for (int r = firstRow; r < lastRow; r++) {
            for (int c = 0; c < rowLength[r]; c++) {
                if (owner[r][c] == "") {
                    owner[r][c] = pnr;
                    seatOut = seatName(r, c);
                    return true;
                }
            }
        }
        return false;
    }

    // Frees the seat held by this PNR.
    bool releaseByPnr(string pnr) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < rowLength[r]; c++) {
                if (owner[r][c] == pnr) {
                    owner[r][c] = "";
                    return true;
                }
            }
        }
        return false;
    }

    int freeSeats(int tier) {
        int firstRow;
        int lastRow;
        getRowRange(tier, firstRow, lastRow);

        int freeCount = 0;
        for (int r = firstRow; r < lastRow; r++) {
            for (int c = 0; c < rowLength[r]; c++) {
                if (owner[r][c] == "") {
                    freeCount = freeCount + 1;
                }
            }
        }
        return freeCount;
    }

    void display() {
        for (int r = 0; r < rows; r++) {
            if (r == 0) {
                cout << "--- First Class ---\n";
            }
            if (r == firstEnd) {
                cout << "--- Business Class ---\n";
            }
            if (r == businessEnd) {
                cout << "--- Economy Class ---\n";
            }

            if (r + 1 < 10) {
                cout << " ";
            }
            cout << (r + 1) << " ";

            for (int c = 0; c < rowLength[r]; c++) {
                cout << "[" << (char)('A' + c);
                if (owner[r][c] == "") {
                    cout << " ";
                } else {
                    cout << "x";
                }
                cout << "]";
            }
            cout << "\n";
        }
        cout << "(x = occupied)  Free seats: First " << freeSeats(2)
             << ", Business " << freeSeats(1)
             << ", Economy " << freeSeats(0) << "\n";
    }
};


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


const int INF = 1000000000;         // "infinity" for distances
const int MAX_VERTICES = 20;

class Graph {
private:
    // One edge in a vertex's adjacency list.
    struct Edge {
        int to;           // the vertex this edge leads to
        int weight;
        Edge* next;

        Edge(int t, int w, Edge* n) {
            to = t;
            weight = w;
            next = n;
        }
    };

    string names[MAX_VERTICES];
    Edge* adjacency[MAX_VERTICES];      // adjacency[v] = list of edges leaving v
    bool blocked[MAX_VERTICES];         // closed vertex (weather / closed area)
    int vertexCount;
    bool isDirected;
    bool isWeighted;
    int dfsStep;

    // Is there already an edge a -> b ?
    bool hasEdge(int a, int b) {
        Edge* e = adjacency[a];
        while (e != NULL) {
            if (e->to == b) {
                return true;
            }
            e = e->next;
        }
        return false;
    }

    // Recursive depth-first visit.
    void dfsVisit(int u, bool visited[]) {
        visited[u] = true;
        dfsStep = dfsStep + 1;
        cout << "  " << dfsStep << ". " << names[u] << "\n";

        Edge* e = adjacency[u];
        while (e != NULL) {
            if (visited[e->to] == false && blocked[e->to] == false) {
                dfsVisit(e->to, visited);
            }
            e = e->next;
        }
    }

    // Directed cycle check. color: 0 = not visited, 1 = on current path, 2 = finished.
    bool cycleDirected(int u, int color[]) {
        color[u] = 1;
        Edge* e = adjacency[u];
        while (e != NULL) {
            if (color[e->to] == 1) {
                return true;                      // we came back to the current path
            }
            if (color[e->to] == 0) {
                if (cycleDirected(e->to, color)) {
                    return true;
                }
            }
            e = e->next;
        }
        color[u] = 2;
        return false;
    }

    // Undirected cycle check: meeting a visited vertex that is not our parent.
    bool cycleUndirected(int u, int parent, bool visited[]) {
        visited[u] = true;
        Edge* e = adjacency[u];
        while (e != NULL) {
            if (visited[e->to] == false) {
                if (cycleUndirected(e->to, u, visited)) {
                    return true;
                }
            } else if (e->to != parent) {
                return true;
            }
            e = e->next;
        }
        return false;
    }

public:
    Graph() {
        vertexCount = 0;
        isDirected = false;
        isWeighted = false;
        dfsStep = 0;
        for (int i = 0; i < MAX_VERTICES; i++) {
            adjacency[i] = NULL;
            blocked[i] = false;
        }
    }

    ~Graph() {
        for (int i = 0; i < vertexCount; i++) {
            Edge* e = adjacency[i];
            while (e != NULL) {
                Edge* temp = e;
                e = e->next;
                delete temp;
            }
        }
    }

    // Chooses the type of graph. Call this before adding edges.
    void setType(bool directed, bool weighted) {
        isDirected = directed;
        isWeighted = weighted;
    }

    // ---------- building the graph ----------

    // Adds a vertex and returns its number (or -1 if the graph is full).
    int addVertex(string name) {
        if (vertexCount >= MAX_VERTICES) {
            return -1;
        }
        names[vertexCount] = name;
        vertexCount = vertexCount + 1;
        return vertexCount - 1;
    }

    // Finds a vertex by name (ignoring case). Returns -1 if not found.
    int indexOf(string name) {
        for (int i = 0; i < vertexCount; i++) {
            if (lower(names[i]) == lower(name)) {
                return i;
            }
        }
        return -1;
    }

    // Adds an edge from a to b. Undirected graphs also get the edge b to a.
    bool addEdge(int a, int b, int weight) {
        if (a < 0 || b < 0 || a >= vertexCount || b >= vertexCount) {
            return false;
        }
        if (a == b) {
            return false;                  // no self-loops
        }
        if (hasEdge(a, b)) {
            return false;                  // no duplicate edges
        }

        int usedWeight = 1;
        if (isWeighted) {
            usedWeight = weight;
        }

        adjacency[a] = new Edge(b, usedWeight, adjacency[a]);
        if (isDirected == false) {
            adjacency[b] = new Edge(a, usedWeight, adjacency[b]);
        }
        return true;
    }

    void setBlocked(int vertex, bool value) {
        blocked[vertex] = value;
    }

    bool isBlocked(int vertex) {
        return blocked[vertex];
    }

    // Prints every vertex and its neighbours.
    void show() {
        if (isDirected) {
            cout << "  [directed, ";
        } else {
            cout << "  [undirected, ";
        }
        if (isWeighted) {
            cout << "weighted";
        } else {
            cout << "unweighted";
        }
        cout << " graph: " << vertexCount << " vertices]\n";

        for (int i = 0; i < vertexCount; i++) {
            cout << "  " << names[i];
            if (blocked[i]) {
                cout << " [CLOSED]";
            }
            if (isDirected) {
                cout << " => ";
            } else {
                cout << " -- ";
            }

            Edge* e = adjacency[i];
            while (e != NULL) {
                cout << names[e->to];
                if (isWeighted) {
                    cout << "(" << e->weight << ")";
                }
                cout << "  ";
                e = e->next;
            }
            cout << "\n";
        }
    }

    // ---------- traversals ----------

    // Breadth-first search: visits vertices level by level using a queue.
    void bfs(int start) {
        if (blocked[start]) {
            cout << "  That vertex is closed.\n";
            return;
        }
        bool visited[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            visited[i] = false;
        }

        IntQueue queue;
        visited[start] = true;
        queue.enqueue(start);

        int u;
        int step = 0;
        while (queue.dequeue(u)) {
            step = step + 1;
            cout << "  " << step << ". " << names[u] << "\n";

            Edge* e = adjacency[u];
            while (e != NULL) {
                if (visited[e->to] == false && blocked[e->to] == false) {
                    visited[e->to] = true;
                    queue.enqueue(e->to);
                }
                e = e->next;
            }
        }
    }

    // Depth-first search: goes as deep as possible first (uses recursion).
    void dfs(int start) {
        if (blocked[start]) {
            cout << "  That vertex is closed.\n";
            return;
        }
        bool visited[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            visited[i] = false;
        }

        dfsStep = 0;
        dfsVisit(start, visited);

        bool printedHeading = false;
        for (int i = 0; i < vertexCount; i++) {
            if (visited[i] == false && blocked[i] == false) {
                if (printedHeading == false) {
                    cout << "  Cannot be reached from here:\n";
                    printedHeading = true;
                }
                cout << "    - " << names[i] << "\n";
            }
        }
    }

    // Route with the fewest steps (BFS on an unweighted graph).
    void fewestSteps(int start, int target) {
        if (blocked[start] || blocked[target]) {
            cout << "  Start or destination is closed.\n";
            return;
        }
        int previous[MAX_VERTICES];
        bool visited[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            previous[i] = -1;
            visited[i] = false;
        }

        IntQueue queue;
        visited[start] = true;
        queue.enqueue(start);

        int u;
        while (queue.dequeue(u)) {
            if (u == target) {
                break;
            }
            Edge* e = adjacency[u];
            while (e != NULL) {
                if (visited[e->to] == false && blocked[e->to] == false) {
                    visited[e->to] = true;
                    previous[e->to] = u;
                    queue.enqueue(e->to);
                }
                e = e->next;
            }
        }

        if (visited[target] == false) {
            cout << "  No route available.\n";
            return;
        }

        // follow 'previous' backwards from the target to build the path
        int path[MAX_VERTICES];
        int length = 0;
        int v = target;
        while (v != -1) {
            path[length] = v;
            length = length + 1;
            v = previous[v];
        }

        cout << "  ";
        for (int i = length - 1; i >= 0; i--) {
            cout << names[path[i]];
            if (i > 0) {
                cout << " -> ";
            }
        }
        cout << "  (" << (length - 1) << " steps)\n";
    }

    // ---------- cycle detection ----------
    bool hasCycle() {
        if (isDirected) {
            int color[MAX_VERTICES];
            for (int i = 0; i < vertexCount; i++) {
                color[i] = 0;
            }
            for (int i = 0; i < vertexCount; i++) {
                if (color[i] == 0) {
                    if (cycleDirected(i, color)) {
                        return true;
                    }
                }
            }
            return false;
        }

        bool visited[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            visited[i] = false;
        }
        for (int i = 0; i < vertexCount; i++) {
            if (visited[i] == false) {
                if (cycleUndirected(i, -1, visited)) {
                    return true;
                }
            }
        }
        return false;
    }

    // ---------- DIRECTED graph: topological order (Kahn's algorithm) ----------
    void topologicalSort() {
        if (isDirected == false) {
            cout << "  Topological order needs a directed graph.\n";
            return;
        }

        // in-degree = number of edges coming INTO each vertex
        int inDegree[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            inDegree[i] = 0;
        }
        for (int i = 0; i < vertexCount; i++) {
            Edge* e = adjacency[i];
            while (e != NULL) {
                inDegree[e->to] = inDegree[e->to] + 1;
                e = e->next;
            }
        }

        // start with every vertex that has no prerequisites
        IntQueue queue;
        for (int i = 0; i < vertexCount; i++) {
            if (inDegree[i] == 0) {
                queue.enqueue(i);
            }
        }

        int printed = 0;
        int u;
        while (queue.dequeue(u)) {
            printed = printed + 1;
            cout << "  " << printed << ". " << names[u] << "\n";

            Edge* e = adjacency[u];
            while (e != NULL) {
                inDegree[e->to] = inDegree[e->to] - 1;
                if (inDegree[e->to] == 0) {
                    queue.enqueue(e->to);
                }
                e = e->next;
            }
        }

        if (printed != vertexCount) {
            cout << "  !! Circular dependency detected - no valid order exists.\n";
        }
    }

    // ---------- WEIGHTED graph: Dijkstra's shortest path ----------
    void shortestPath(int start, int target) {
        if (isWeighted == false) {
            cout << "  Shortest path needs a weighted graph.\n";
            return;
        }
        if (blocked[start] || blocked[target]) {
            cout << "  Source or destination airport is closed.\n";
            return;
        }

        int dist[MAX_VERTICES];
        int previous[MAX_VERTICES];
        bool done[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            dist[i] = INF;
            previous[i] = -1;
            done[i] = false;
        }
        dist[start] = 0;

        for (int round = 0; round < vertexCount; round++) {
            // pick the unfinished, open vertex with the smallest distance
            int u = -1;
            for (int i = 0; i < vertexCount; i++) {
                if (done[i] == false && blocked[i] == false && dist[i] < INF) {
                    if (u == -1) {
                        u = i;
                    } else if (dist[i] < dist[u]) {
                        u = i;
                    }
                }
            }
            if (u == -1) {
                break;
            }
            done[u] = true;

            // try to improve the distance of each neighbour
            Edge* e = adjacency[u];
            while (e != NULL) {
                if (blocked[e->to] == false) {
                    int newDistance = dist[u] + e->weight;
                    if (newDistance < dist[e->to]) {
                        dist[e->to] = newDistance;
                        previous[e->to] = u;
                    }
                }
                e = e->next;
            }
        }

        if (dist[target] == INF) {
            cout << "  No available route.\n";
            return;
        }

        int path[MAX_VERTICES];
        int length = 0;
        int v = target;
        while (v != -1) {
            path[length] = v;
            length = length + 1;
            v = previous[v];
        }

        cout << "  ";
        for (int i = length - 1; i >= 0; i--) {
            cout << names[path[i]];
            if (i > 0) {
                cout << " -> ";
            }
        }
        cout << "  (" << dist[target] << " km)\n";
    }

    // ---------- UNDIRECTED + WEIGHTED: Prim's minimum spanning tree ----------
    void minimumSpanningTree() {
        if (isDirected || isWeighted == false) {
            cout << "  MST needs an undirected, weighted graph.\n";
            return;
        }
        if (vertexCount == 0) {
            return;
        }

        int key[MAX_VERTICES];          // cheapest edge found so far to join each vertex
        int parent[MAX_VERTICES];
        bool inTree[MAX_VERTICES];
        for (int i = 0; i < vertexCount; i++) {
            key[i] = INF;
            parent[i] = -1;
            inTree[i] = false;
        }
        key[0] = 0;

        long total = 0;
        for (int round = 0; round < vertexCount; round++) {
            int u = -1;
            for (int i = 0; i < vertexCount; i++) {
                if (inTree[i] == false) {
                    if (u == -1) {
                        u = i;
                    } else if (key[i] < key[u]) {
                        u = i;
                    }
                }
            }
            if (key[u] == INF) {
                cout << "  (graph is disconnected)\n";
                break;
            }
            inTree[u] = true;
            total = total + key[u];

            if (parent[u] != -1) {
                cout << "  " << names[parent[u]] << " -- " << names[u]
                     << " (" << key[u] << " km)\n";
            }

            Edge* e = adjacency[u];
            while (e != NULL) {
                if (inTree[e->to] == false && e->weight < key[e->to]) {
                    key[e->to] = e->weight;
                    parent[e->to] = u;
                }
                e = e->next;
            }
        }
        cout << "  Total network length: " << total << " km\n";
    }
};


const int MAX_PASSENGERS = 100;     // the cabin only has 71 seats, so 100 is enough

// ======================= input helpers =======================

// Shows a prompt and reads one line of text.
string readLine(string prompt) {
    cout << prompt;
    string text;
    if (!getline(cin, text)) {      // getline fails when the input has ended
        cout << "\n";
        exit(0);
    }
    return text;
}

// Returns true if the text contains only digits (and is not empty).
bool isNumber(string text) {
    int length = text.length();
    if (length == 0) {
        return false;
    }
    for (int i = 0; i < length; i++) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }
    return true;
}

// Keeps asking until the user types a valid whole number.
int readInt(string prompt) {
    while (true) {
        string text = readLine(prompt);
        if (isNumber(text)) {
            return atoi(text.c_str());
        }
        cout << "  Please enter a valid number.\n";
    }
}

// ======================= the main program class =======================
class Skyway {
private:
    // ----- passenger side -----
    PassengerHashTable table;       // PNR -> passenger
    AVLIndex passportIndex;         // passport -> PNR (sorted)
    StringQueue checkinQueue;       // check-in counter
    PriorityHeap boardingHeap;      // boarding order
    PriorityHeap emergencyHeap;     // emergency flights
    GateManager gates;              // gate changes with undo/redo
    Watchlist watchlist;            // security watchlist
    int nextPnr;

    // ----- aircraft and airport side -----
    Cabin cabin;
    BaggageList bags;
    Carousel carousel;

    // ----- the three kinds of graph -----
    Graph flights;                  // undirected + weighted : airports and distances
    Graph terminal;                 // undirected            : terminal areas and walkways
    Graph maintenance;              // directed              : task A must finish before task B

    // ======================= helper functions =======================

    // Creates a booking. Returns true if it worked.
    bool registerPassenger(string name, string passport, int tier, bool showMessages) {
        if (tier < 0 || tier > 3) {
            cout << "  Invalid class.\n";
            return false;
        }
        if (passportIndex.contains(passport)) {
            if (showMessages) {
                cout << "  A passenger with that passport already exists.\n";
            }
            return false;
        }

        Passenger p;
        p.name = name;
        p.passport = passport;
        p.tier = tier;
        p.pnr = "PNR" + to_string(nextPnr);

        if (cabin.assign(tier, p.pnr, p.seat) == false) {
            if (showMessages) {
                cout << "  No free seats in that class.\n";
            }
            return false;
        }

        nextPnr = nextPnr + 1;
        table.insert(p);
        passportIndex.insert(passport, p.pnr);

        if (showMessages) {
            cout << "  Booked! PNR: " << p.pnr << " | " << tierName(tier)
                 << " | Seat " << p.seat << "\n";
        }
        return true;
    }

    // Asks for a PNR and returns that passenger (or NULL if not found).
    Passenger* askPassenger() {
        string pnr = readLine("  Enter PNR: ");
        Passenger* p = table.find(pnr);
        if (p == NULL) {
            cout << "  PNR not found.\n";
        }
        return p;
    }

    void printPassenger(Passenger* p) {
        cout << "  " << p->pnr << " | " << p->name << " | Passport " << p->passport
             << " | " << tierName(p->tier) << " | Seat " << p->seat
             << " | Flight SK" << p->flightNo;
        if (p->checkedIn) {
            cout << " | checked-in";
        }
        if (p->boarded) {
            cout << " | boarded";
        }
        cout << "\n";
    }

    // Asks for a vertex name and returns its number (or -1 if unknown).
    int askVertex(Graph& graph, string prompt) {
        string name = readLine(prompt);
        int vertex = graph.indexOf(name);
        if (vertex < 0) {
            cout << "  Unknown name (use the names shown in the map).\n";
        }
        return vertex;
    }

    // Closes a vertex if it is open, or reopens it if it is closed.
    void toggleClosed(Graph& graph, int vertex) {
        if (graph.isBlocked(vertex)) {
            graph.setBlocked(vertex, false);
            cout << "  Reopened.\n";
        } else {
            graph.setBlocked(vertex, true);
            cout << "  Closed.\n";
        }
    }

    // Adds an edge between two vertices given by NAME.
    void connect(Graph& graph, string from, string to, int weight) {
        int a = graph.indexOf(from);
        int b = graph.indexOf(to);
        graph.addEdge(a, b, weight);
    }

    // ======================= 1. Passenger & Booking =======================
    void actionRegister() {
        string name = readLine("  Name: ");
        string passport = readLine("  Passport no: ");
        int tier = readInt("  Class (0 Economy, 1 Business, 2 First, 3 VIP): ");
        registerPassenger(name, passport, tier, true);
    }

    void actionFind() {
        Passenger* p = askPassenger();
        if (p != NULL) {
            printPassenger(p);
        }
    }

    void actionCancel() {
        Passenger* p = askPassenger();
        if (p == NULL) {
            return;
        }
        string pnr = p->pnr;
        string passport = p->passport;

        cabin.releaseByPnr(pnr);
        passportIndex.remove(passport);
        table.remove(pnr);                 // 'p' must not be used after this line
        cout << "  Booking " << pnr << " cancelled and seat released.\n";
    }

    void actionListAll() {
        Passenger* all[MAX_PASSENGERS];
        int total = table.collectAll(all, MAX_PASSENGERS);
        cout << "  Total passengers: " << total << "\n";
        for (int i = 0; i < total; i++) {
            printPassenger(all[i]);
        }
    }

    void bookingMenu() {
        while (true) {
            cout << "\n-- Passenger & Booking --\n";
            cout << "1 Register passenger\n";
            cout << "2 Find by PNR\n";
            cout << "3 Cancel booking\n";
            cout << "4 List all\n";
            cout << "0 Back\n";
            int choice = readInt("> ");

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                actionRegister();
            } else if (choice == 2) {
                actionFind();
            } else if (choice == 3) {
                actionCancel();
            } else if (choice == 4) {
                actionListAll();
            }
        }
    }

    // ======================= 2. Check-in Counter =======================
    void actionJoinQueue() {
        Passenger* p = askPassenger();
        if (p == NULL) {
            return;
        }
        if (p->checkedIn) {
            cout << "  Already checked in.\n";
            return;
        }
        checkinQueue.enqueue(p->pnr);
        cout << "  " << p->name << " joined the queue (position "
             << checkinQueue.size() << ").\n";
    }

    void actionServeNext() {
        string pnr;
        if (checkinQueue.dequeue(pnr) == false) {
            cout << "  Queue is empty.\n";
            return;
        }
        Passenger* p = table.find(pnr);
        if (p == NULL) {
            cout << "  " << pnr << " no longer holds a booking - skipped.\n";
            return;
        }
        p->checkedIn = true;
        cout << "  Checked in: " << p->name << ", seat " << p->seat << "\n";
    }

    void checkinMenu() {
        while (true) {
            cout << "\n-- Check-in Counter --\n";
            cout << "1 Join queue\n";
            cout << "2 Serve next passenger\n";
            cout << "3 Queue length\n";
            cout << "0 Back\n";
            int choice = readInt("> ");

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                actionJoinQueue();
            } else if (choice == 2) {
                actionServeNext();
            } else if (choice == 3) {
                cout << "  Waiting: " << checkinQueue.size() << "\n";
            }
        }
    }

    // ======================= 3. Boarding & Emergency =======================
    void actionPrepareBoarding() {
        Passenger* all[MAX_PASSENGERS];
        int total = table.collectAll(all, MAX_PASSENGERS);

        boardingHeap.clear();
        for (int i = 0; i < total; i++) {
            if (all[i]->checkedIn == true && all[i]->boarded == false) {
                boardingHeap.push(all[i]->tier, all[i]->pnr);
            }
        }
        cout << "  " << boardingHeap.size() << " checked-in passengers ready for boarding.\n";
    }

    void actionBoardNext() {
        HeapItem item;
        if (boardingHeap.pop(item) == false) {
            cout << "  Nobody waiting to board.\n";
            return;
        }
        Passenger* p = table.find(item.label);
        if (p == NULL) {
            cout << "  (booking cancelled - skipped)\n";
            return;
        }
        p->boarded = true;
        cout << "  Boarding: " << p->name << " [" << tierName(p->tier)
             << "], seat " << p->seat << "\n";
    }

    void actionDeclareEmergency() {
        string flight = readLine("  Flight code: ");
        int severity = readInt("  Severity (1 Technical, 2 Medical, 3 Fuel/Critical): ");
        emergencyHeap.push(severity, flight);
        cout << "  Emergency logged for " << flight << ".\n";
    }

    void actionClearEmergency() {
        HeapItem item;
        if (emergencyHeap.pop(item) == false) {
            cout << "  No emergency flights.\n";
            return;
        }
        cout << "  RUNWAY CLEARED for " << item.label
             << " (severity " << item.priority << ")\n";
    }

    void boardingMenu() {
        while (true) {
            cout << "\n-- Boarding & Emergency --\n";
            cout << "1 Prepare boarding list\n";
            cout << "2 Board next passenger\n";
            cout << "3 Declare flight emergency\n";
            cout << "4 Clear next emergency flight\n";
            cout << "0 Back\n";
            int choice = readInt("> ");

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                actionPrepareBoarding();
            } else if (choice == 2) {
                actionBoardNext();
            } else if (choice == 3) {
                actionDeclareEmergency();
            } else if (choice == 4) {
                actionClearEmergency();
            }
        }
    }

    // ======================= 4. Gate Changes =======================
    void actionChangeGate() {
        string flight = readLine("  Flight: ");
        string newGate = readLine("  New gate: ");
        if (gates.changeGate(flight, newGate)) {
            cout << "  Gate updated.\n";
        } else {
            cout << "  Unknown flight.\n";
        }
    }

    void gateMenu() {
        while (true) {
            cout << "\n-- Gate Changes --\n";
            cout << "1 Show gates\n";
            cout << "2 Change gate\n";
            cout << "3 Undo\n";
            cout << "4 Redo\n";
            cout << "0 Back\n";
            int choice = readInt("> ");

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                gates.show();
            } else if (choice == 2) {
                actionChangeGate();
            } else if (choice == 3) {
                if (gates.undo()) {
                    cout << "  Undone.\n";
                } else {
                    cout << "  Nothing to undo.\n";
                }
            } else if (choice == 4) {
                if (gates.redo()) {
                    cout << "  Redone.\n";
                } else {
                    cout << "  Nothing to redo.\n";
                }
            }
        }
    }

    // ======================= 6. Baggage =======================
    void actionAddBag() {
        Passenger* p = askPassenger();
        if (p == NULL) {
            return;
        }
        int weight = readInt("  Weight (kg): ");
        int tag = bags.add(p->pnr, weight);
        cout << "  Tag issued: " << tag << "\n";
    }

    void actionAdvanceBag() {
        int tag = readInt("  Tag: ");
        Bag* b = bags.find(tag);
        if (b == NULL) {
            cout << "  Bag not found.\n";
            return;
        }
        if (b->stage >= STAGE_UNLOADED) {
            cout << "  Use option 3 to put it on the carousel.\n";
            return;
        }
        b->stage = b->stage + 1;
        cout << "  Tag " << b->tag << " -> " << stageNames[b->stage] << "\n";
    }

    void actionToCarousel() {
        int tag = readInt("  Tag: ");
        Bag* b = bags.find(tag);
        if (b == NULL) {
            cout << "  Bag not found.\n";
            return;
        }
        if (b->stage != STAGE_UNLOADED) {
            cout << "  Bag has not been unloaded yet.\n";
            return;
        }
        b->stage = STAGE_CAROUSEL;
        carousel.add(b->tag, b->pnr);
        cout << "  Bag placed on the carousel.\n";
    }

    void actionClaimBag() {
        int tag = readInt("  Tag: ");
        if (carousel.claim(tag)) {
            bags.remove(tag);
            cout << "  Bag " << tag << " claimed.\n";
        } else {
            cout << "  That bag is not on the carousel.\n";
        }
    }

    void baggageMenu() {
        while (true) {
            cout << "\n-- Baggage --\n";
            cout << "1 Check in bag\n";
            cout << "2 Advance bag to next stage\n";
            cout << "3 Move unloaded bag to carousel\n";
            cout << "4 Rotate carousel\n";
            cout << "5 Passenger claims bag\n";
            cout << "6 Show all bags (forward)\n";
            cout << "7 Show all bags (backward)\n";
            cout << "8 Show carousel\n";
            cout << "0 Back\n";
            int choice = readInt("> ");

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                actionAddBag();
            } else if (choice == 2) {
                actionAdvanceBag();
            } else if (choice == 3) {
                actionToCarousel();
            } else if (choice == 4) {
                carousel.rotate();
            } else if (choice == 5) {
                actionClaimBag();
            } else if (choice == 6) {
                bags.showForward();
            } else if (choice == 7) {
                bags.showBackward();
            } else if (choice == 8) {
                carousel.show();
            }
        }
    }

    // ======================= 7. Graphs =======================
    void actionShortestFlight() {
        int from = askVertex(flights, "  From: ");
        if (from < 0) {
            return;
        }
        int to = askVertex(flights, "  To: ");
        if (to < 0) {
            return;
        }
        flights.shortestPath(from, to);
    }

    void actionWalkingRoute() {
        int from = askVertex(terminal, "  From: ");
        if (from < 0) {
            return;
        }
        int to = askVertex(terminal, "  To: ");
        if (to < 0) {
            return;
        }
        terminal.fewestSteps(from, to);
    }

    void actionAddDependency() {
        int first = askVertex(maintenance, "  Task that must come FIRST: ");
        if (first < 0) {
            return;
        }
        int second = askVertex(maintenance, "  Task that depends on it: ");
        if (second < 0) {
            return;
        }
        if (maintenance.addEdge(first, second, 1) == false) {
            cout << "  Invalid or duplicate dependency.\n";
            return;
        }
        cout << "  Dependency added.\n";
        if (maintenance.hasCycle()) {
            cout << "  WARNING: this creates a circular dependency!\n";
        }
    }

    void graphMenu() {
        while (true) {
            cout << "\n-- Graphs: Flights, Terminal & Maintenance --\n";
            cout << " WEIGHTED / UNDIRECTED : flight network\n";
            cout << "  1 Show flight network\n";
            cout << "  2 Shortest flight path (Dijkstra)\n";
            cout << "  3 Close/reopen airport (weather)\n";
            cout << "  4 Minimum spanning network (Prim)\n";
            cout << " UNDIRECTED : terminal map\n";
            cout << "  5 Show terminal map\n";
            cout << "  6 Fewest-steps walking route (BFS)\n";
            cout << "  7 Explore terminal from a point (DFS)\n";
            cout << "  8 Close/reopen terminal area\n";
            cout << " DIRECTED : maintenance tasks\n";
            cout << "  9 Show task dependencies\n";
            cout << " 10 Maintenance order (topological sort)\n";
            cout << " 11 Add dependency (with cycle check)\n";
            cout << "  0 Back\n";
            int choice = readInt("> ");

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                flights.show();
            } else if (choice == 2) {
                actionShortestFlight();
            } else if (choice == 3) {
                int airport = askVertex(flights, "  Airport: ");
                if (airport >= 0) {
                    toggleClosed(flights, airport);
                }
            } else if (choice == 4) {
                flights.minimumSpanningTree();
            } else if (choice == 5) {
                terminal.show();
            } else if (choice == 6) {
                actionWalkingRoute();
            } else if (choice == 7) {
                int start = askVertex(terminal, "  Start at: ");
                if (start >= 0) {
                    terminal.dfs(start);
                }
            } else if (choice == 8) {
                int area = askVertex(terminal, "  Area: ");
                if (area >= 0) {
                    toggleClosed(terminal, area);
                }
            } else if (choice == 9) {
                maintenance.show();
            } else if (choice == 10) {
                maintenance.topologicalSort();
            } else if (choice == 11) {
                actionAddDependency();
            }
        }
    }

    // ======================= 8. Security & Records =======================
    void actionScanManifest() {
        Passenger* all[MAX_PASSENGERS];
        int total = table.collectAll(all, MAX_PASSENGERS);

        int alerts = 0;
        for (int i = 0; i < total; i++) {
            string text = all[i]->name + " " + all[i]->passport;
            int match = watchlist.findMatch(text);
            if (match != -1) {
                alerts = alerts + 1;
                cout << "  ALERT: " << all[i]->name << " (" << all[i]->pnr
                     << ") matches '" << watchlist.get(match) << "'\n";
            }
        }
        cout << "  Scan complete, " << alerts << " alert(s).\n";
    }

    void actionSearchPassport() {
        string passport = readLine("  Passport: ");
        string pnr;
        if (passportIndex.search(passport, pnr) == false) {
            cout << "  Not found.\n";
            return;
        }
        Passenger* p = table.find(pnr);
        if (p != NULL) {
            printPassenger(p);
        }
    }

    void securityMenu() {
        while (true) {
            cout << "\n-- Security & Records --\n";
            cout << "1 Add watchlist entry\n";
            cout << "2 Show watchlist\n";
            cout << "3 Scan passenger manifest\n";
            cout << "4 Search by passport (AVL)\n";
            cout << "5 Passport index in sorted order\n";
            cout << "0 Back\n";
            int choice = readInt("> ");

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                watchlist.add(readLine("  Name / passport fragment: "));
                cout << "  Added.\n";
            } else if (choice == 2) {
                for (int i = 0; i < watchlist.size(); i++) {
                    cout << "  - " << watchlist.get(i) << "\n";
                }
            } else if (choice == 3) {
                actionScanManifest();
            } else if (choice == 4) {
                actionSearchPassport();
            } else if (choice == 5) {
                passportIndex.printInOrder();
                cout << "  (tree height " << passportIndex.height() << ")\n";
            }
        }
    }

    // ======================= sample data =======================
    void seedData() {
        // passengers
        registerPassenger("Ali Raza", "PK1001", 0, false);
        registerPassenger("Sara Khan", "PK1002", 1, false);
        registerPassenger("Ahmed Malik", "PK1003", 2, false);
        registerPassenger("Hina Siddiqui", "PK1004", 3, false);
        registerPassenger("Bilal Ahmed", "PK1005", 0, false);

        // watchlist
        watchlist.add("Z999999");
        watchlist.add("john doe");

        // gates
        gates.addFlight("SK101", "A1");
        gates.addFlight("SK202", "B4");
        gates.addFlight("SK303", "C2");

        // flight network (undirected + weighted, distances are approximate)
        flights.addVertex("Karachi");
        flights.addVertex("Lahore");
        flights.addVertex("Islamabad");
        flights.addVertex("Dubai");
        flights.addVertex("Doha");
        flights.addVertex("Jeddah");
        flights.addVertex("Istanbul");
        flights.addVertex("London");

        connect(flights, "Karachi", "Lahore", 1040);
        connect(flights, "Karachi", "Islamabad", 1200);
        connect(flights, "Lahore", "Islamabad", 270);
        connect(flights, "Karachi", "Dubai", 1200);
        connect(flights, "Lahore", "Dubai", 1600);
        connect(flights, "Islamabad", "Dubai", 2000);
        connect(flights, "Karachi", "Doha", 1400);
        connect(flights, "Dubai", "Doha", 380);
        connect(flights, "Dubai", "Jeddah", 1900);
        connect(flights, "Doha", "Jeddah", 1500);
        connect(flights, "Doha", "Istanbul", 3000);
        connect(flights, "Jeddah", "Istanbul", 2400);
        connect(flights, "Istanbul", "London", 2500);
        connect(flights, "Dubai", "London", 5500);
        connect(flights, "Doha", "London", 5200);
        connect(flights, "Islamabad", "Istanbul", 4100);

        // terminal map (undirected, unweighted: walkways work both ways)
        terminal.addVertex("Check-in Hall");
        terminal.addVertex("Security");
        terminal.addVertex("Immigration");
        terminal.addVertex("Duty Free");
        terminal.addVertex("Food Court");
        terminal.addVertex("Lounge");
        terminal.addVertex("Gate A1");
        terminal.addVertex("Gate B4");
        terminal.addVertex("Gate C2");
        terminal.addVertex("Prayer Area");

        connect(terminal, "Check-in Hall", "Security", 1);
        connect(terminal, "Security", "Immigration", 1);
        connect(terminal, "Immigration", "Duty Free", 1);
        connect(terminal, "Immigration", "Food Court", 1);
        connect(terminal, "Duty Free", "Food Court", 1);
        connect(terminal, "Duty Free", "Gate A1", 1);
        connect(terminal, "Food Court", "Gate B4", 1);
        connect(terminal, "Food Court", "Lounge", 1);
        connect(terminal, "Lounge", "Gate C2", 1);
        connect(terminal, "Gate B4", "Gate C2", 1);
        connect(terminal, "Food Court", "Prayer Area", 1);

        // maintenance tasks (directed: first task must finish before the second)
        maintenance.addVertex("Visual Inspection");
        maintenance.addVertex("Engine Check");
        maintenance.addVertex("Hydraulics Test");
        maintenance.addVertex("Fuel Loading");
        maintenance.addVertex("Cabin Cleaning");
        maintenance.addVertex("Safety Equipment Check");
        maintenance.addVertex("Final Clearance");

        connect(maintenance, "Visual Inspection", "Engine Check", 1);
        connect(maintenance, "Visual Inspection", "Hydraulics Test", 1);
        connect(maintenance, "Engine Check", "Fuel Loading", 1);
        connect(maintenance, "Hydraulics Test", "Fuel Loading", 1);
        connect(maintenance, "Fuel Loading", "Final Clearance", 1);
        connect(maintenance, "Cabin Cleaning", "Safety Equipment Check", 1);
        connect(maintenance, "Safety Equipment Check", "Final Clearance", 1);
    }

public:
    Skyway() {
        nextPnr = 1001;
        flights.setType(false, true);        // undirected, weighted
        terminal.setType(false, false);      // undirected, unweighted
        maintenance.setType(true, false);    // directed, unweighted
        seedData();
    }

    void run() {
        cout << "==========================================\n";
        cout << " SKYWAY - Airline & Airport Operations\n";
        cout << "==========================================\n";

        while (true) {
            cout << "\n1 Passenger & Booking\n";
            cout << "2 Check-in Counter\n";
            cout << "3 Boarding & Emergency\n";
            cout << "4 Gate Changes\n";
            cout << "5 Cabin Seating Chart\n";
            cout << "6 Baggage\n";
            cout << "7 Graphs: Flights, Terminal, Maintenance\n";
            cout << "8 Security & Records\n";
            cout << "0 Exit\n";
            int choice = readInt("> ");

            if (choice == 1) {
                bookingMenu();
            } else if (choice == 2) {
                checkinMenu();
            } else if (choice == 3) {
                boardingMenu();
            } else if (choice == 4) {
                gateMenu();
            } else if (choice == 5) {
                cabin.display();
            } else if (choice == 6) {
                baggageMenu();
            } else if (choice == 7) {
                graphMenu();
            } else if (choice == 8) {
                securityMenu();
            } else if (choice == 0) {
                cout << "Goodbye.\n";
                return;
            } else {
                cout << "  Invalid option.\n";
            }
        }
    }
};

int main() {
    Skyway app;
    app.run();
    return 0;
}