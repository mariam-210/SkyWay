#include <iostream>
#include <string>
#include <cstdlib>
#include "Passenger.cpp"
#include "HashTable.cpp"
#include "Queue.cpp"
#include "Stack.cpp"
#include "MaxHeap.cpp"
#include "AVLTree.cpp"
#include "StringMatch.cpp"
#include "Seating.cpp"
#include "Baggage.cpp"
#include "Graph.cpp"
using namespace std;

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