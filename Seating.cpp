// Aircraft cabin stored as a JAGGED ARRAY: an array of rows where every row
// can have a different number of seats.
#ifndef SEATING_CPP
#define SEATING_CPP

#include <iostream>
#include <string>
using namespace std;

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

#endif