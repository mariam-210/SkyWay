// StringMatch.cpp  (Mariam Aqeel)
// KMP (Knuth-Morris-Pratt) string matching and the security watchlist.
#ifndef STRINGMATCH_CPP
#define STRINGMATCH_CPP

#include <iostream>
#include <string>
#include "Util.cpp"
using namespace std;

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

#endif
