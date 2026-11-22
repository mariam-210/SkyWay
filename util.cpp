#ifndef UTIL_CPP
#define UTIL_CPP

#include <iostream>
#include <string>
#include <cctype>
using namespace std;

// Returns a lower-case copy of a string, so we can compare ignoring upper/lower case.
string lower(string text) {
    string result = text;
    int length = result.length();
    for (int i = 0; i < length; i++) {
        result[i] = tolower(result[i]);
    }
    return result;
}

#endif