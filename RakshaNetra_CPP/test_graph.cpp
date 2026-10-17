#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include "ds/Graph.h"

// Turn a route into text like "Fire Post -> Admin Office -> ..." for easy checking.
std::string routeText(const Graph& g, const Route& r) {
    std::string text;
    for (std::size_t i = 0; i < r.path.size(); ++i) {
        if (i > 0) text += " -> ";
        text += g.nameOf(r.path[i]);
    }
    return text;
}

// Build the same site map as the zones and zone_edges tables in MySQL.
void buildSite(Graph& g) {
    int gate   = g.addNode("Main Gate");
    int admin  = g.addNode("Admin Office");
    int yard   = g.addNode("Material Yard");
    int excav  = g.addNode("Excavation Zone");
    int tunnel = g.addNode("Tunnel Section");
    int bridge = g.addNode("Bridge Pier A");
    int camp   = g.addNode("Medical Camp");
    int fire   = g.addNode("Fire Post");

    g.addEdge(gate,   admin,  200);
    g.addEdge(admin,  fire,   150);
    g.addEdge(admin,  camp,   150);
    g.addEdge(camp,   gate,   250);
    g.addEdge(admin,  yard,   300);
    g.addEdge(yard,   excav,  250);
    g.addEdge(excav,  tunnel, 300);
    g.addEdge(yard,   bridge, 400);
    g.addEdge(bridge, excav,  350);
}

int main() {
    Graph site;
    buildSite(site);
    assert(site.nodeCount() == 8);
    assert(site.isConnected());

    // Test 1: Fire team (Fire Post) to the tunnel fire - the Step 4 demo route
    Route r = site.shortestPath(site.indexOf("Fire Post"), site.indexOf("Tunnel Section"));
    assert(r.found);
    assert(r.metres == 1000);
    assert(routeText(site, r) ==
           "Fire Post -> Admin Office -> Material Yard -> Excavation Zone -> Tunnel Section");

    // Test 2: Dijkstra picks the SHORTER of two possible routes.
    // Medical Camp -> Bridge Pier A:
    //   via Material Yard:                150 + 300 + 400       = 850  <- shortest
    //   via Material Yard + Excavation:   150 + 300 + 250 + 350 = 1050
    r = site.shortestPath(site.indexOf("Medical Camp"), site.indexOf("Bridge Pier A"));
    assert(r.metres == 850);
    assert(routeText(site, r) == "Medical Camp -> Admin Office -> Material Yard -> Bridge Pier A");

    // Test 3: a direct path wins over a longer detour
    // Medical Camp -> Main Gate: direct 250 vs via Admin Office 150 + 200 = 350
    r = site.shortestPath(site.indexOf("Medical Camp"), site.indexOf("Main Gate"));
    assert(r.metres == 250);
    assert(routeText(site, r) == "Medical Camp -> Main Gate");

    // Test 4: route from a zone to itself
    r = site.shortestPath(site.indexOf("Tunnel Section"), site.indexOf("Tunnel Section"));
    assert(r.found && r.metres == 0 && r.path.size() == 1);

    // Test 5: distances to ALL zones from one Dijkstra run
    // (the dispatcher uses this to compare every team's base at once)
    Vector<int> dist, prev;
    site.dijkstra(site.indexOf("Tunnel Section"), dist, prev);
    assert(dist[site.indexOf("Excavation Zone")] == 300);
    assert(dist[site.indexOf("Fire Post")] == 1000);
    assert(dist[site.indexOf("Medical Camp")] == 1000);   // 300 + 250 + 300 + 150
    assert(dist[site.indexOf("Main Gate")] == 1050);      // 300 + 250 + 300 + 200

    // Test 6: unknown zone name
    assert(site.indexOf("Parking") == -1);

    // Test 7: a zone with no paths cannot be reached
    int store = site.addNode("Store Room");
    assert(!site.isConnected());
    assert(site.countReachable(site.indexOf("Main Gate")) == 8);   // 8 of 9 zones
    r = site.shortestPath(site.indexOf("Main Gate"), store);
    assert(!r.found && r.path.empty());

    // Test 8: invalid input is rejected
    bool threw = false;
    try { site.addEdge(0, 1, -50); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);                                   // negative length
    threw = false;
    try { site.addEdge(0, 99, 100); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);                                   // node does not exist
    threw = false;
    try { site.addNode("Main Gate"); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);                                   // duplicate zone name

    // Test 9: big test - a straight line of 10000 zones, each 1 m apart
    Graph line;
    for (int i = 0; i < 10000; ++i) line.addNode("Z" + std::to_string(i));
    for (int i = 0; i + 1 < 10000; ++i) line.addEdge(i, i + 1, 1);
    r = line.shortestPath(0, 9999);
    assert(r.found && r.metres == 9999 && r.path.size() == 10000);
    assert(line.isConnected());

    std::cout << "All Graph tests passed\n";
    return 0;
}
