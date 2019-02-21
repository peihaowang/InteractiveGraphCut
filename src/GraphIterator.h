
#ifndef _GRAPH_ITERATOR_H_
#define _GRAPH_ITERATOR_H_

#include <queue>
#include <map>
#include "Graph.h"

class GraphBFSIterator
{

protected:

    typedef Graph::Vertex Vertex;
    typedef Graph::Edge Edge;
    typedef Graph::Weight Weight;
    typedef Graph::Adjacence Adjacence;

public:

    struct Connection{
    private:
        Vertex                      m_from;
        Adjacence*                  m_adjacence;
        friend class GraphBFSIterator;
    public:
        Connection() : m_from(-1), m_adjacence(nullptr) { return; }
        bool isValid() const { return m_from >= Vertex(0) && m_adjacence != nullptr; }
        Vertex from() const { return m_from; }
        Vertex to() const { return m_adjacence->m_vertex; }
        Weight weight() const { return m_adjacence->m_weight; }
        Connection& operator=(const Connection& rhs) { m_from = rhs.m_from; m_adjacence = rhs.m_adjacence; return (*this); }
    };

protected:

    const Graph*                    m_graph;

    int                             m_current;
    std::vector<Adjacence*>         m_queue;
    std::map<Vertex, int>           m_parents;

protected:

    inline bool isVisited(Vertex v) const { return m_parents.find(v) != m_parents.end(); }

public:

    GraphBFSIterator(const Graph* graph, Vertex start);

    bool atBegin() const { return m_current == 0; }
    bool atEnd() const { return m_current == m_queue.size(); }

    Vertex vertex() const;
    Vertex parent() const;

    bool next();
    bool back();
    bool promote();

    Connection connection() const;

};

#endif // _GRAPH_ITERATOR_H_
