
#include <iostream>
#include <time.h>
#include "StopWatch.h"
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
    do{ if(it.peek(m_sink)) break; }while(it.next());

    if(!it.atEnd()){
        Vertex v = it.vertex();

        // Add the edge between pixel and sink
        path.push_back(edgeOf(v, m_sink));

        while(v != Vertex(-1)){
            it.promote(); Vertex u = it.vertex();
            if(u != Vertex(-1)) path.push_back(edgeOf(u, v));
            v = u;
        }

        std::reverse(path.begin(), path.end());
    }
    return !path.empty();
}

MaxFlow::Weight MaxFlow::maxFlow()
{
    std::vector<Edge> path;
    double tBFS = 0.0, tFindMin = 0.0, tSetEdge = 0.0;
    StopWatch sw;
    int n = 0;
    sw.tick();
    while(breathFirstSearchPath(path)){
        tBFS += sw.escapeTime(); sw.tick();
        // Select the minimum capacity along the path as the flow
        Weight flow = Weight(std::numeric_limits<float>::infinity());
        for(Edge e : path){ if(flow > e.m_weight) flow = e.m_weight; }
        tFindMin += sw.escapeTime(); sw.tick();

        // Add up flow
        m_totalFlow += flow;

        // Update edges
        for(Edge e : path){
            setEdge(e.m_from, e.m_to, e.m_weight - flow);
            setEdge(e.m_to, e.m_from, e.m_weight + flow);
            tFindMin += sw.escapeTime(); sw.tick();
        }
        tSetEdge += sw.escapeTime(); sw.tick();

        if(n % 1000 == 0) {
            std::cout << "Path:" << path[0].m_from << " - ";
            for(Edge e : path){
                std::cout << e.m_weight << " -> " << e.m_to << " - ";
            }
            std::cout << std::endl;

            std::cout << "Flow:" << flow << " #OfEdges:" << m_numOfEdges << " TotalFlow:" << m_totalFlow << std::endl;
            std::cout << "BFS:" << tBFS << " FindMin:" << tFindMin << " SetEdge:" << tSetEdge << std::endl;
            tBFS = tFindMin = tSetEdge = 0.0;
        }
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
