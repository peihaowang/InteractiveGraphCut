
#include <iostream>
#include <algorithm>
#include "GraphIterator.h"

GraphBFSIterator::GraphBFSIterator(const Graph* graph, Vertex start)
    : m_graph(graph)
    , m_current(-1)
{
    m_parents.resize(m_graph->numberOfVertices());
    for(int& p : m_parents) p = Parent::Invalid;
    m_parents[start] = Parent::None;

    m_queue.reserve(m_graph->numberOfVertices());
    m_queue.push_back(&m_graph->m_vertices[start]);
    next(); // Move to the start node
}

Graph::Vertex GraphBFSIterator::vertex() const
{
    return m_current >= 0 && m_current < m_queue.size() ? m_queue[m_current]->m_vertex : Vertex(-1);
}

Graph::Vertex GraphBFSIterator::parent() const
{
    Vertex v = vertex();
    if(v != Vertex(-1)){
        int idx = m_parents[vertex()];
        if(idx >= 0) v = m_queue[idx]->m_vertex;
    }
    return v;
}

bool GraphBFSIterator::next()
{
    bool succ = false;
    if(!atEnd()) m_current++;
    if(m_current >= 0 && m_current < m_queue.size()){
        Adjacence* adjacence = m_graph->m_vertices[vertex()].m_next;
        while(adjacence){
            if(!isVisited(adjacence->m_vertex)){
                m_queue.push_back(adjacence);
                m_parents[adjacence->m_vertex] = m_current;
            }
            adjacence = adjacence->m_next;
        }
        succ = true;
    }
    return succ;
}

bool GraphBFSIterator::back()
{
    bool succ = false;
    if(!atBegin()){
        m_current--;
        succ = true;
    }
    return succ;
}

bool GraphBFSIterator::promote()
{
    bool succ = false;
    Vertex v = vertex();
    if(v != Vertex(-1)){
        m_current = m_parents[v];
        succ = true;
    }
    return succ;
}

// GraphBFSIterator::Connection GraphBFSIterator::connection() const
// {
//     Connection conn;
//     if(m_current >= 0 && m_current < m_queue.size()){
//         Adjacence* adjacence = m_queue[m_current];
//         conn.m_from = parent();
//         conn.m_adjacence = adjacence;
//     }
//     return conn;
// }
