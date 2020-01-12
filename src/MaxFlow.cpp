
#include <iostream>
#include <queue>
#include "MaxFlow.h"

MaxFlow::MaxFlow(int numOfVertices, Vertex source, Vertex sink)
    : Graph(numOfVertices)
    , m_source(source)
    , m_sink(sink)
    , m_totalFlow(0.0f)
{
    m_levels.resize(numOfVertices);
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
        // The parent connection is built on BFS
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
    while(breathFirstSearch()){
        std::vector<Edge> path;
        while(depthFirstSearch(m_source, path)){
            std::reverse(path.begin(), path.end());

            // Select the minimum capacity along the path as the flow
            Weight flow = Weight(std::numeric_limits<float>::infinity());
            for(Edge e : path) {
                if(flow > e.m_weight) flow = e.m_weight;
            }

            // Add up flow
            m_totalFlow += flow;

            // Update edges
            for(Edge e : path){
                setEdge(e.m_from, e.m_to, e.m_weight - flow);
                setEdge(e.m_to, e.m_from, e.m_weight + flow);
            }

            // Clear path right now to avoid accumulate edges
            path.clear();
        }
    }
    return m_totalFlow;
}

void MaxFlow::minCut(std::set<Vertex>& s, std::set<Vertex>& t)
{
    // Run Ford-Fulkerson algorithm first
    maxFlow();

    // Clear the s,t sets
    s.clear(); t.clear();

    // Fill s set by BFS
    std::queue<Vertex> queue;
    queue.push(m_source);
    while (!queue.empty()){
        Vertex u = queue.front();
        queue.pop();

        // Add vertex u
        s.insert(u);

        // Add all successive nodes
        Adjacence* adjacence = m_vertices[u].m_next;
        while (adjacence){
            Vertex v = adjacence->m_vertex;
            std::set<Vertex>::const_iterator it = s.find(v);
            if (it == s.end()){
                queue.push(v);
            }
            adjacence = adjacence->m_next;
        }
    }

    // Fill t set. If v not in s, it must be in t
    for(Vertex v = Vertex(0); v < Vertex(m_numOfVertices); v++){
        if(s.find(v) == s.end()){
            t.insert(v);
        }
    }
}
