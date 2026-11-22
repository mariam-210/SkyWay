#ifndef PASSENGER_CPP
#define PASSENGER_CPP

#include <iostream>
#include <string>
#include "util.cpp"
using namespace std;

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

#endif