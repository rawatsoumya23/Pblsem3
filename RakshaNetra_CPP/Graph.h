#pragma once
#include <climits>
#include <stdexcept>
#include <string>
#include "ds/BinaryHeap.h"
#include "ds/HashMap.h"
#include "ds/Queue.h"
#include "ds/Stack.h"
#include "ds/Vector.h"

// Graph: the construction site as a map.
//   - Each NODE is a zone (Main Gate, Tunnel Section, ...).
//   - Each EDGE is a path between two zones, with a length in metres.
//   - Edges are undirected: if you can walk A -> B, you can walk B -> A.
//
// Stored as an "adjacency list": for each zone, a Vector of its neighbours.
//
//   0 Main Gate     -> (1 Admin Office, 200m), (6 Medical Camp, 250m)
//   1 Admin Office  -> (0 Main Gate, 200m), (7 Fire Post, 150m), ...
//
// It reuses the team's own data structures:
//   Vector      - node names and adjacency lists
//   HashMap     - zone name -> node number, O(1)
//   BinaryHeap  - min-heap inside Dijkstra (closest zone first)
//   Stack       - turning the route around into the correct order
//   Queue       - BFS, to check every zone can be reached
//
// Time complexity:
//   addNode, addEdge          O(1) (amortised)
//   shortestPath (Dijkstra)   O((V + E) log E)   V = zones, E = edges
//   countReachable (BFS)      O(V + E)

// One path from a zone to a neighbour.
struct Edge {
    int to = 0;          // neighbour's node number
    int metres = 0;      // length of the path
};

// The answer from shortestPath().
struct Route {
    bool found = false;     // false if the destination cannot be reached
    int metres = 0;         // total length of the route
    Vector<int> path;       // node numbers from start to destination, in order
};

class Graph {
public:
    static constexpr int INF = INT_MAX;   // "not reachable (yet)"

    // Add a zone. Returns its node number (0, 1, 2, ...).
    // Throws if a zone with the same name already exists.
    int addNode(const std::string& name) {
        int id = static_cast<int>(names_.size());
        if (!index_.insert(name, id)) {
            throw std::invalid_argument("Zone already exists: " + name);
        }
        names_.push_back(name);
        adj_.push_back(Vector<Edge>());
        return id;
    }

    // Add a path between two zones (both directions).
    // Throws for an unknown node number or a length that is not positive,
    // because Dijkstra gives WRONG answers with negative lengths.
    void addEdge(int a, int b, int metres) {
        checkNode(a);
        checkNode(b);
        if (metres <= 0) throw std::invalid_argument("Edge length must be positive");
        adj_[a].push_back({b, metres});
        adj_[b].push_back({a, metres});
    }

    // Node number for a zone name, or -1 if there is no such zone.
    int indexOf(const std::string& name) const {
        const int* id = index_.find(name);
        return id ? *id : -1;
    }

    const std::string& nameOf(int node) const {
        checkNode(node);
        return names_[node];
    }

    int nodeCount() const { return static_cast<int>(names_.size()); }

    // Dijkstra: shortest distance from 'start' to EVERY zone.
    // dist[v] = metres from start to v (INF if unreachable)
    // prev[v] = the zone just before v on the shortest route (-1 if none)
    void dijkstra(int start, Vector<int>& dist, Vector<int>& prev) const {
        checkNode(start);
        dist.clear();
        prev.clear();
        for (int i = 0; i < nodeCount(); ++i) {
            dist.push_back(INF);
            prev.push_back(-1);
        }
        dist[start] = 0;

        // Min-heap of (distance, node): always process the closest unfinished zone next.
        BinaryHeap<DistNode, CloserFirst> heap;
        heap.push({0, start});

        while (!heap.empty()) {
            DistNode cur = heap.pop();
            int u = cur.node;
            if (cur.dist > dist[u]) continue;   // old, out-of-date entry: skip ("lazy deletion")

            for (const Edge& e : adj_[u]) {
                int newDist = dist[u] + e.metres;   // safe: dist[u] is never INF here
                if (newDist < dist[e.to]) {          // found a shorter way to e.to
                    dist[e.to] = newDist;
                    prev[e.to] = u;
                    heap.push({newDist, e.to});
                }
            }
        }
    }

    // Shortest route between two zones, with the full path.
    Route shortestPath(int start, int goal) const {
        checkNode(goal);
        Vector<int> dist, prev;
        dijkstra(start, dist, prev);

        Route route;
        if (dist[goal] == INF) return route;   // found = false: no route exists
        route.found = true;
        route.metres = dist[goal];

        // prev[] leads BACKWARDS from goal to start. A Stack reverses it.
        Stack<int> backwards;
        for (int v = goal; v != -1; v = prev[v]) backwards.push(v);
        while (!backwards.empty()) route.path.push_back(backwards.pop());
        return route;
    }

    // BFS: how many zones can be reached from 'start' (including start itself).
    int countReachable(int start) const {
        checkNode(start);
        Vector<bool> seen;
        for (int i = 0; i < nodeCount(); ++i) seen.push_back(false);

        Queue<int> toVisit;
        toVisit.enqueue(start);
        seen[start] = true;
        int count = 0;
        while (!toVisit.empty()) {
            int u = toVisit.dequeue();
            ++count;
            for (const Edge& e : adj_[u]) {
                if (!seen[e.to]) {
                    seen[e.to] = true;
                    toVisit.enqueue(e.to);
                }
            }
        }
        return count;
    }

    // True if every zone can reach every other zone (used to check the loaded site map).
    bool isConnected() const {
        return nodeCount() == 0 || countReachable(0) == nodeCount();
    }

private:
    struct DistNode {
        int dist = 0;
        int node = 0;
    };
    // Comparator for a MIN-heap: a has lower priority if it is FARTHER away.
    struct CloserFirst {
        bool operator()(const DistNode& a, const DistNode& b) const { return a.dist > b.dist; }
    };

    Vector<std::string> names_;     // node number -> zone name
    Vector<Vector<Edge>> adj_;      // node number -> list of paths from it
    HashMap<std::string, int> index_;   // zone name -> node number

    void checkNode(int n) const {
        if (n < 0 || n >= nodeCount()) throw std::out_of_range("Unknown node " + std::to_string(n));
    }
};
