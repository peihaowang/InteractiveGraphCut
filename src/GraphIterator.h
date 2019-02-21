
#ifndef _GRAPH_ITERATOR_H_
#define _GRAPH_ITERATOR_H_

#include <queue>
#include <map>
#include "Graph.h"

class GraphBFSIterator
{

protected:

    const Graph*                    m_graph;

    typedef Graph::Vertex Vertex;
    typedef Graph::Adjacence Adjacence;

    std::vector<Adjacence*>::size_t m_current;
    std::vector<Adjacence*>         m_queue;
    std::map<Vertex, Adjacence*>    m_parents;

protected:

    inline bool isVisited(Vertex v) const { return m_parents.find(v) != m_parents.end(); }

public:

    GraphBFSIterator(const Graph* graph, Vertex start);

    bool atBegin() const { return parent() == Vertex(-1) && !atEnd(); }
    bool atEnd() const { return m_current == Vertex(-1); }

    Vertex vertex() const { return m_current; }
    Vertex parent() const { return parent(m_current); }
    Vertex parent(Vertex v) const;

    bool next();

};

#endif // _GRAPH_ITERATOR_H_
