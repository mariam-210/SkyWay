// One adjacency-list Graph class. Call setType() to choose the kind of graph:
//   directed = false -> UNDIRECTED : every edge is stored in both directions
//   directed = true  -> DIRECTED   : an edge goes one way only (a -> b)
//   weighted = true  -> WEIGHTED   : every edge has a number (for example km)
//   weighted = false -> UNWEIGHTED : every edge simply counts as 1
#ifndef GRAPH_CPP
#define GRAPH_CPP

#include <iostream>
#include <string>
#include "Util.cpp"
#include "Queue.cpp"
using namespace std;

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

#endif