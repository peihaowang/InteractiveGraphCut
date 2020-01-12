
#ifndef _MAX_FLOW_H_
#define _MAX_FLOW_H_

#include <set>
#include <vector>
#include "Graph.h"

class MaxFlow : public Graph
{

protected:

    Vertex                      m_source;
    Vertex                      m_sink;
    Weight                      m_totalFlow;

    std::vector<int>            m_levels;

protected:

    bool breathFirstSearch();
    bool depthFirstSearch(Vertex u, std::vector<Edge>& path);

public:

    MaxFlow(int numOfVertices, Vertex source = Vertex(-1), Vertex sink = Vertex(-1));

    Vertex source() const { return m_source; }
    Vertex sink() const { return m_sink; }
    void setTerminals(Vertex source, Vertex sink) { m_source = source, m_sink = sink; }

    bool isTerminal(Vertex v) const { return v == m_source || v == m_sink; }

    Weight maxFlow();
    void minCut(std::set<Vertex>& s, std::set<Vertex>& t);

};

#endif // _MAX_FLOW_H_
