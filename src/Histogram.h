#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include <iostream>
#include <math.h>

template<typename T, int Dim>
class Histogram{

public:

    typedef unsigned int Frequency;
    typedef size_t Size;
    typedef size_t Index;

protected:

    Frequency*          m_data;
    Frequency           m_total;

    T*                  m_lowerBounds;
    T*                  m_upperBounds;
    T*                  m_steps;
    Size*               m_sizes;

protected:

    void init(T* lowerBounds, T* upperBounds, T* steps)
    {
        // Copy and initiate data
        m_lowerBounds = new T[Dim]; for(int i = 0; i < Dim; i++) m_lowerBounds[i] = lowerBounds[i];
        m_upperBounds = new T[Dim]; for(int i = 0; i < Dim; i++) m_upperBounds[i] = upperBounds[i];
        m_steps = new T[Dim]; for(int i = 0; i < Dim; i++) m_steps[i] = steps[i];

        // Calculate shape
        m_sizes = new Size[Dim]; Size totalSize = 1;
        for(int i = 0; i < Dim; i++){
            m_sizes[i] = ceil((double)(m_upperBounds[i] - m_lowerBounds[i]) / (double)m_steps[i]);
            totalSize *= m_sizes[i];
        }

        // Allocate memory and initiate data
        m_data = new Frequency[totalSize]; for(int i = 0; i < totalSize; i++) m_data[i] = 0;
        m_total = Frequency(0);
    }

    Index rawIdx(Index* index) const
    {
        Index idx = 0;
        Size* subshapes = new Size[Dim];
        Size accumulator = 1;
        for(int i = 0; i < Dim; i++){
            subshapes[i] = accumulator;
            accumulator *= m_sizes[i];
        }
        for(int i = 0; i < Dim; i++){
            idx += index[Dim - i - 1] * subshapes[i];
        }
        delete[] subshapes;
        return idx;
    }

    Size rawSize() const
    {
        Size totalSize = 1;
        for(int i = 0; i < Dim; i++){
            totalSize *= m_sizes[i];
        }
        return totalSize;
    }

    void indexOf(T* value, Index* index) const
    {
        for(int i = 0; i < Dim; i++){
            Index idx = floor((double)(value[i] - m_lowerBounds[i]) / (double)m_steps[i]);
            if(idx == m_sizes[i]) idx--; // Consider the marginal case
            index[i] = idx;
        }
    }

public:

    Histogram(T* lowerBounds, T* upperBounds, T* steps) { init(lowerBounds, upperBounds, steps); }

    Histogram(T* lowerBounds, T* upperBounds, T step)
    {
        T* steps = new T[Dim]; for(int i = 0; i < Dim; i++) steps[i] = step;
        init(lowerBounds, upperBounds, steps);
        delete[] steps;
    }

    Histogram(T lowerBound, T upperBound, T step)
    {
        T* lowerBounds = new T[Dim]; for(int i = 0; i < Dim; i++) lowerBounds[i] = lowerBound;
        T* upperBounds = new T[Dim]; for(int i = 0; i < Dim; i++) upperBounds[i] = upperBound;
        T* steps = new T[Dim]; for(int i = 0; i < Dim; i++) steps[i] = step;

        init(lowerBounds, upperBounds, steps);
    }

    virtual ~Histogram()
    {
        delete[] m_data;
        delete[] m_lowerBounds;
        delete[] m_upperBounds;
        delete[] m_steps;
        delete[] m_sizes;
    }

    T lowerBound(int dim) const { return m_lowerBounds[dim]; }
    T upperBound(int dim) const { return m_upperBounds[dim]; }
    Size size(int dim) const { return m_sizes[dim]; }

    void accumulate(T* value, Frequency increment = 1)
    {
        Index* index = new Index[Dim]; indexOf(value, index);
        Index i = rawIdx(index);
        m_data[i] += increment;
        m_total += increment;
        delete[] index;
    }

    Frequency frequency(T* value) const
    {
        Index* index = new Index[Dim]; indexOf(value, index);
        Index i = rawIdx(index);
        return m_data[i];
    }

    Frequency total() const { return m_total; }
    double mean() const { return (double)total() / (double)rawSize(); }

};

typedef Histogram<int, 1>           Histogram1i;
typedef Histogram<int, 2>           Histogram2i;
typedef Histogram<int, 3>           Histogram3i;

typedef Histogram<double, 1>        Histogram1f;
typedef Histogram<double, 2>        Histogram2f;
typedef Histogram<double, 3>        Histogram3f;

typedef Histogram<unsigned char, 1> Histogram1c;
typedef Histogram<unsigned char, 2> Histogram2c;
typedef Histogram<unsigned char, 3> Histogram3c;

#endif // _HISTOGRAM_H_
