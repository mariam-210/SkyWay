// AVLTree.cpp  (Mariam Aqeel)
// Self-balancing binary search tree. Key = passport number, value = PNR.
// After every insert or delete the tree rebalances itself with rotations,
// so the height stays small and search is always O(log n).
#ifndef AVLTREE_CPP
#define AVLTREE_CPP

#include <iostream>
#include <string>
using namespace std;

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

#endif
