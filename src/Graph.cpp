
#include <iostream>
#include <vector>
#include <utility>
#include <math.h>

#include "Graph.h"

Graph::Graph(int numOfVertices)
    : m_numOfVertices(numOfVertices)
    , m_numOfEdges(0)
{
    m_vertices = new Adjacence[m_numOfVertices];
    for(Vertex v = Vertex(0); v < m_numOfVertices; v++) m_vertices[v].m_vertex = v;
}

Graph::~Graph()
{
    delete[] m_vertices;
    m_vertices = nullptr;
}

Graph::Adjacence*& Graph::adjacenceOf(Vertex u, Vertex v) const
{
    Adjacence*& adjacence = m_vertices[u].m_next;
    while(adjacence){
        if(adjacence->m_vertex == v) break;
        adjacence = adjacence->m_next;
    }
    return adjacence;
}

// Graph::Adjacence* Graph::adjacenceOf(Vertex u, Vertex v) const
// {
//     Adjacence*& adjacence = adjacenceOf(u, v);
//     return (*adjacence);
// }

Graph::Adjacence* Graph::addEdge(Vertex u, Vertex v, Weight w)
{
    Adjacence* newConn = adjacenceOf(u, v);
    if(!newConn && !isZero(w)){
        newConn = new Adjacence(v, w);
        if(m_vertices[u].m_next) m_vertices[u].m_next->m_prev = newConn;
        newConn->m_next = m_vertices[u].m_next;
        newConn->m_prev = &m_vertices[u];
        m_vertices[u].m_next = newConn;
        m_numOfEdges++;
    }
    return newConn;
}

bool Graph::removeEdge(Vertex u, Vertex v)
{
    bool succ = false;
    Adjacence* adjacence = adjacenceOf(u, v);
    if(adjacence){
        Adjacence* prev = adjacence->m_prev;
        if(adjacence->m_next) adjacence->m_next->m_prev = prev;
        prev->m_next = adjacence->m_next;
        adjacence->m_next = nullptr; adjacence->m_prev = nullptr;
        delete adjacence; adjacence = nullptr;
        m_numOfEdges--;
        succ = true;
    }
    return succ;
}

void Graph::setEdge(Vertex u, Vertex v, Weight w)
{
    if(!isVertValid(u) || !isVertValid(v)) return;

    // Retrieve the already existing adjacent vertex
    Adjacence* adjacence = adjacenceOf(u, v);

    // If the capacity is set to zero, the edge will be removed(not added)
    if(adjacence){
        if(!isZero(w)){
            adjacence->m_weight = w;
        }else{
            // std::cout << "Remove Edge " << u << " " << v << std::endl;
            removeEdge(u, v);
        }
    }else{
        // std::cout << "Add Edge " << u << " " << v << " " << w << std::endl;
        addEdge(u, v, w);
    }
}

Graph::Edge Graph::edgeOf(Vertex u, Vertex v) const
{
    if(!isVertValid(u) || !isVertValid(v)) return Edge();

    // Retrieve the already existing adjacent vertex
    Adjacence* adjacence = adjacenceOf(u, v);
    Weight w = adjacence ? adjacence->m_weight : Weight(0.0f);
    return Edge(u, v, w);
}
