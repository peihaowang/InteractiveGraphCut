
#ifndef _MAX_FLOW_H_
#define _MAX_FLOW_H_

#include <set>
#include "Graph.h"
#include "GraphIterator.h"

class MaxFlow : public Graph
{

protected:

    std::vector<Adjacence**>    m_adjacenceIndex;

    Vertex                      m_source;
    Vertex                      m_sink;
    Weight                      m_totalFlow;

protected:

    ;

public:

    MaxFlow(int numOfVertices, Vertex source = Vertex(-1), Vertex sink = Vertex(-1));

    Vertex source() const { return m_source; }
    Vertex sink() const { return m_sink; }
    void setTerminals(Vertex source, Vertex sink) { m_source = source, m_sink = sink; }

    bool breathFirstSearchPath(std::vector<Edge>& path) const;

    Weight maxFlow();
    void minCut(std::set<Vertex>& s, std::set<Vertex>& t);

};

#endif // _MAX_FLOW_H_
