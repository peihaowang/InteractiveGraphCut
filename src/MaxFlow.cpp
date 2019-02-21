
#include <iostream>
#include "MaxFlow.h"

MaxFlow::MaxFlow(int numOfVertices, Vertex source, Vertex sink)
    : Graph(numOfVertices)
    , m_source(source)
    , m_sink(sink)
    , m_totalFlow(0.0f)
{
    return;
}

bool MaxFlow::breathFirstSearchPath(std::vector<Edge>& path) const
{
    // Clear the path first
    path.clear();

    GraphBFSIterator it(this, m_source);
    // Breath first search for the sink node
    do{ if(it.vertex() == m_sink) break; }while(it.next());

    if(!it.atEnd()){
        while(it.vertex() != Vertex(-1)){
            GraphBFSIterator::Connection conn = it.connection();
            if(conn.isValid()) path.push_back(Edge(conn.from(), conn.to(), conn.weight()));
            it.promote();
        }
        std::reverse(path.begin(), path.end());
    }
    return !path.empty();
}

MaxFlow::Weight MaxFlow::maxFlow()
{
    std::vector<Edge> path;
    int n = 0;
    while(breathFirstSearchPath(path)){
        // Select the minimum capacity along the path as the flow
        Weight flow = Weight(std::numeric_limits<float>::infinity());
        for(Edge e : path){ if(flow > e.m_weight) flow = e.m_weight; }

        if(n % 100 == 0) {
            std::cout << path[0].m_from << " -";
            for(Edge e : path){
                std::cout << e.m_weight << "-> " << e.m_to << " -";
            }
            std::cout << std::endl;
        }

        // Add up flow
        m_totalFlow += flow;

        // Update edges
        for(Edge e : path){
            setEdge(e.m_from, e.m_to, e.m_weight - flow);
            setEdge(e.m_to, e.m_from, e.m_weight + flow);
        }

        if(n % 100 == 0) std::cout << flow << " " << m_numOfEdges << " " << m_totalFlow << std::endl;
        n++;
    }
    return m_totalFlow;
}

void MaxFlow::minCut(std::set<Vertex>& s, std::set<Vertex>& t)
{
    // Run Ford-Fulkerson algorithm first
    maxFlow();

    // Clear the s,t sets
    s.clear(); t.clear();

    // Fill s set
    GraphBFSIterator it(this, m_source);
    do{
        s.insert(it.vertex());
    }while(it.next());

    // Fill t set
    for(Vertex v = Vertex(0); v < Vertex(m_numOfVertices); v++){
        if(s.find(v) == s.end()) t.insert(v);
    }
}
