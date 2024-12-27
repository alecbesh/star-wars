

#ifndef UNORDEREDFASTPQ_H
#define UNORDEREDFASTPQ_H

#include "Eecs281PQ.h"

#include <limits>  // needed for UNKNOWN

static const size_t UNKNOWN = std::numeric_limits<size_t>::max();

template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class UnorderedFastPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit UnorderedFastPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, extreme{ UNKNOWN } {
    } // UnorderedFastPQ()


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    UnorderedFastPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, data{ start, end }, extreme{ UNKNOWN } {
    } // UnorderedFastPQ()


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automaticslly.
    virtual ~UnorderedFastPQ() {
    } // ~UnorderedFastPQ()


    // Description: The only thing needed is to mark that we no longer know the
    //              most extreme element.
    // Runtime: O(1)
    virtual void updatePriorities() {
        extreme = UNKNOWN;
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: Amortized O(1)
    virtual void push(const TYPE &val) {
        data.push_back(val);

        // Since a new element has been added, we no longer know where to find
        // the most extreme element.
        extreme = UNKNOWN;
    } // push()


    virtual void pop() {
        // If we don't already know the index of the most extreme element, find it.
        if (extreme == UNKNOWN)
            findExtreme();

        // Replace the most extreme element with the element at the back, then
        // pop_back().  This is much faster than erasing from the middle of a
        // vector.
        data[extreme] = data.back();
        data.pop_back();

        // Since the most extreme element has been removed, we no longer know
        // where to find it.
        extreme = UNKNOWN;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the vector.  This should be a reference for speed.  It MUST
    //              be const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(n)
    virtual const TYPE &top() const {
        // If we don't already know the index of the most extreme element, find it.
        if (extreme == UNKNOWN)
            findExtreme();

        // Return the most extreme element by const reference.
        return data[extreme];
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return data.size();
    } // size()

    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return data.empty();
    } // empty()


private:
    // Note: This vector *must* be used for your heap implementation.
    std::vector<TYPE> data;

private:
    // A member variable that can be changed by a const member function;
    // stores the index of the most extreme element, or UNKNOWN.
    mutable size_t extreme;

    // Description: Find the 'most extreme' element of the data vector, using
    //              this->compare() to check if one element is 'less than'
    //              another.
    // Runtime: O(n)
    void findExtreme() const {
        size_t index = 0;

        for (size_t i = 1; i < data.size(); ++i)
            if (this->compare(data[index], data[i]))
                index = i;

        extreme = index;
    } // findExtreme()
}; // UnorderedFastPQ

#endif // UNORDEREDFASTPQ_H
