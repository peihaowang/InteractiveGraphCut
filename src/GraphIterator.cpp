
#include <algorithm>
#include "GraphIterator.h"

GraphBFSIterator::GraphBFSIterator(const Graph* graph, Vertex start)
    : m_graph(graph)
    , m_current(-1)
{
    m_parents[start] = nullptr;

    m_queue.reverse(m_graph.numOfVertices());
    m_queue.push_back(&m_graph.m_vertices[start]);
    next(); // Move to the start node
}

bool GraphBFSIterator::next()
{
    bool succ = false;
    if(m_current != m_queue.size()) m_current++;
    if(m_current < m_queue.size())
        Graph::Adjacence* adjacence = m_queue[m_current];
        while(adjacence){
            if(!isVisited(adjacence->m_vertex)){
                m_queue.push(adjacence->m_vertex);
                m_parents[adjacence->m_vertex] = m_current;
            }
            adjacence = adjacence->m_next;
        }
        succ = true;
    }
    return succ;
}

GraphBFSIterator::Vertex GraphBFSIterator::parent(Vertex v) const
{
    std::map<Vertex, Vertex>::const_iterator it = m_parents.find(v);
    return it != m_parents.end() ? it->second : Vertex(-1);
}
