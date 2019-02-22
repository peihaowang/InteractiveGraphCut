
#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <math.h>

class Graph
{

public:

    typedef int                     Vertex;
    typedef float                   Weight;
    struct Edge{
        Vertex      m_from;
        Vertex      m_to;
        Weight      m_weight;
        Edge(Vertex from = Vertex(-1), Vertex to = Vertex(-1), Weight weight = Weight(0.0)) : m_from(from), m_to(to), m_weight(weight) { return; }
        Edge& operator=(const Edge& rhs) { m_from = rhs.m_from; m_to = rhs.m_to; m_weight = rhs.m_weight; return (*this); }
    };

protected:

    friend class GraphBFSIterator;

    struct Adjacence
    {
        Vertex              m_vertex;
        Weight              m_weight;
        Adjacence*          m_prev;
        Adjacence*          m_next;

        Adjacence(Vertex vertex = Vertex(-1), Weight weight = Weight(0.0f))
            : m_vertex(vertex)
            , m_weight(weight)
            , m_prev(nullptr)
            , m_next(nullptr)
        {
            return;
        }

        ~Adjacence()
        {
            if(m_next){
                delete m_next;
                m_next = nullptr;
            }
        }
    };

    inline bool isVertValid(Vertex v) const
    {
        return Vertex(0) <= v && v < Vertex(m_numOfVertices);
    }

    inline bool isZero(float f) const
    {
        return abs(f) < 10e-6;
    }

    virtual Adjacence* adjacenceOf(Vertex u, Vertex v) const;
    virtual Adjacence* addEdge(Vertex u, Vertex v, Weight w);
    virtual bool removeEdge(Vertex u, Vertex v);

protected:

    Adjacence*              m_vertices;
    int                     m_numOfVertices;
    int                     m_numOfEdges;

public:

    Graph(int numOfVertices);
    virtual ~Graph();

    int numberOfVertices() const { return m_numOfVertices; }
    int numberOfEdges() const { return m_numOfEdges; }

    void setEdge(Vertex u, Vertex v, Weight w);
    Edge edgeOf(Vertex u, Vertex v) const;

};

#endif // _GRAPH_H_
