
#include <algorithm>
#include "GraphIterator.h"

GraphBFSIterator::GraphBFSIterator(const Graph* graph, Vertex start)
    : m_graph(graph)
{
    m_parents[start] = Vertex(-1);
    m_queue.push(start);
    next(); // Move to the start node
}


bool GraphBFSIterator::next()
{
    bool succ = false;
    if(!m_queue.empty()){
        m_current = m_queue.front(); m_queue.pop();
        Graph::Adjacence* adjacence = m_graph->m_vertices[m_current].m_next;
        while(adjacence){
            if(!isVisited(adjacence->m_vertex)){
                m_queue.push(adjacence->m_vertex);
                m_parents[adjacence->m_vertex] = m_current;
            }
            adjacence = adjacence->m_next;
        }
        succ = true;
    }else{
        m_current = Vertex(-1);
    }
    return succ;
}

GraphBFSIterator::Vertex GraphBFSIterator::parent(Vertex v) const
{
    std::map<Vertex, Vertex>::const_iterator it = m_parents.find(v);
    return it != m_parents.end() ? it->second : Vertex(-1);
}
