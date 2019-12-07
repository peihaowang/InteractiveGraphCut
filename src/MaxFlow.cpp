
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
    m_levels = new int[numOfVertices];
}

MaxFlow::~MaxFlow()
{
    delete[] m_levels;
    m_levels = nullptr;
}

bool MaxFlow::breathFirstSearch()
{
    std::queue<Vertex> queue;
    queue.push(m_source);

    // Clear levels
    for(int i = 0; i < m_numOfVertices; i++) m_levels[i] = -1;
    m_levels[m_source] = 0;     // Initialize levels

    // Build up a level tree, which contains auxiliary level information for DFS
    while(!queue.empty()){
        Vertex u = queue.front(); queue.pop();
        Adjacence* adjacence = m_vertices[u].m_next;
        while(adjacence){
            Vertex v = adjacence->m_vertex;
            if(m_levels[v] == -1){
                m_levels[v] = m_levels[u] + 1;
                queue.push(v);
            }
            adjacence = adjacence->m_next;
        }
    }
    return m_levels[m_sink] != -1;
}

bool MaxFlow::depthFirstSearch(Vertex u, std::vector<Edge>& path)
{
    if(u == m_sink) return true;

    bool found = false;
    Adjacence* adjacence = m_vertices[u].m_next;
    while(adjacence){
        Vertex v = adjacence->m_vertex;
        if(m_levels[v] == m_levels[u] + 1){
            found = depthFirstSearch(v, path);
            if(found){
                path.push_back(Graph::edgeOf(u, v));
                break;
            }else{
                m_levels[v] = -1;
            }
        }
        adjacence = adjacence->m_next;
    }
    return found;
}

MaxFlow::Weight MaxFlow::maxFlow()
{
    double tBFS = 0.0, tDFS = 0.0, tFindMin = 0.0, tSetEdge = 0.0;
    StopWatch sw;
    int n = 0;
    sw.tick();

    while(breathFirstSearch()){
        tBFS = sw.escapeTime(); sw.tick();

        std::vector<Edge> path;
        while(depthFirstSearch(m_source, path)){
            tDFS += sw.escapeTime(); sw.tick();

            std::reverse(path.begin(), path.end());

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
                tSetEdge += sw.escapeTime(); sw.tick();
            }

            if(n % 1000 == 0) {
                std::cout << "Path:" << path[0].m_from << " - ";
                for(Edge e : path){
                    std::cout << e.m_weight << " -> " << e.m_to << " - ";
                }
                std::cout << std::endl;

                std::cout << "Flow:" << flow << " #OfEdges:" << m_numOfEdges << " TotalFlow:" << m_totalFlow << std::endl;
                std::cout << "BFS:" << tBFS << " DFS:" << tDFS << " FindMin:" << tFindMin << " SetEdge:" << tSetEdge << std::endl;
                tDFS = tFindMin = tSetEdge = 0.0;
            }
            n++;

            // Clear path right now to avoid accumulate edges
            path.clear();
        }
    }
    return m_totalFlow;
}

#if 0
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
#endif

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
