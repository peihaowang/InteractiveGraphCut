
#include <iostream>
#include <algorithm>
#include "GraphIterator.h"

GraphBFSIterator::GraphBFSIterator(const Graph* graph, Vertex start)
    : m_graph(graph)
    , m_current(-1)
{
    m_parents[start] = -1;

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
    Vertex v = Vertex(-1);
    std::map<Vertex, int>::const_iterator it = m_parents.find(vertex());
    if(it != m_parents.end()){
        int idx = it->second;
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
        int n = 0;
        while(adjacence){
            if(!isVisited(adjacence->m_vertex)){
                m_queue.push_back(adjacence);
                m_parents[adjacence->m_vertex] = m_current;
            }
            adjacence = adjacence->m_next;
            n++;
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
    std::map<Vertex, int>::const_iterator it = m_parents.find(vertex());
    if(it != m_parents.end()){
        m_current = it->second;
        succ = true;
    }
    return succ;
}

GraphBFSIterator::Connection GraphBFSIterator::connection() const
{
    Connection conn;
    if(m_current >= 0 && m_current < m_queue.size()){
        Adjacence* adjacence = m_queue[m_current];
        conn.m_from = parent();
        conn.m_adjacence = adjacence;
    }
    return conn;
}
