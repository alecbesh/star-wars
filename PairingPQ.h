

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <algorithm>
#include <cassert> 
#include <cstddef>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// A specialized version of the 'priority queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{nullptr}, num_nodes{0} {
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{nullptr}, num_nodes{0} {
        for (InputIterator i = start; i != end; ++i) {
            push(*i);
        }
    } // PairingPQ()

 
    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, root{nullptr}, num_nodes{0} {
        std::deque<Node*> dq;
        dq.push_front(other.root);
        Node* current;
        while (!dq.empty()) {
            current = dq.front();
            dq.pop_front();
            if (current->child) dq.push_back(current->child);
            if (current->sibling) dq.push_back(current->sibling);
            push(current->elt);
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);
        std::swap(root, temp.root);
        std::swap(num_nodes, temp.num_nodes);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        std::deque<Node*> dq;
        if (root != nullptr) {
            dq.push_front(root);
            Node* current;
            while (!dq.empty()) {
                current = dq.front();
                dq.pop_front();
                if (current->child != nullptr) dq.push_back(current->child);
                if (current->sibling != nullptr) dq.push_back(current->sibling);
                delete current;
            }
        }
    } // ~PairingPQ()

 
    // Description: Assumes that all elements inside the pairing heap are out of order and
    //              'rebuilds' the pairing heap by fixing the pairing heap invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node*> dq;
        dq.push_front(root);
        Node* current;
        root = nullptr;
        while (!dq.empty()) {
            current = dq.front();
            dq.pop_front();
            if (current->child != nullptr) dq.push_back(current->child);
            if (current->sibling != nullptr) dq.push_back(current->sibling);
            // Do something specific:
            if (root != nullptr) {
                root->parent = nullptr;
                root->sibling = nullptr;
            }
            current->sibling = nullptr;
            current->parent = nullptr;
            current->child = nullptr;
            root = meld(root, current);
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already done.
    //              You should implement push functionality entirely in the addNode()
    //              function, and this function calls addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the pairing heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the pairing heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        std::deque<Node*> dq;
        Node *p;
        if (num_nodes == 1) {
            delete root;
            root = nullptr;
            num_nodes--;
            return;
        }
        
        if (num_nodes == 2) {
            p = root->child;
            delete root;
            root = p;
            --num_nodes;
            p->parent = nullptr;
        }

        else {
            p = root->child;
            delete root;
            num_nodes--;
            while (p != nullptr) {
                dq.push_back(p);
                p = p->sibling;
            }
            while (dq.size() > 1) {
                dq[0]->parent = nullptr;
                dq[0]->sibling = nullptr;
                dq[1]->parent = nullptr;
                dq[1]->sibling = nullptr;
                dq.push_back(meld(dq[0], dq[1]));
                dq.pop_front();
                dq.pop_front();
            }
            root = dq[0];
        }
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->getElt();
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return num_nodes;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        if (num_nodes == 0) return true;
        else return false;
    } // empty()


    // Description: Updates the priority of an element already in the pairing heap by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        node->elt = new_value;
        if (num_nodes == 1) {
            return;
        }
        if (num_nodes > 1) {
            if (node == root) {
                return;
            }
            // If I am the leftmost node:
            else if (node->parent->child == node) {
                if (node->sibling != nullptr) node->parent->child = node->sibling;
                else node->parent->child = nullptr;
                node->sibling = nullptr;
                node->parent = nullptr;
                root = meld(root, node);
            }
            // If not:
            else {
                if (node->sibling != nullptr) node->parent->child->sibling = node->sibling;
                else node->parent->child->sibling = nullptr;
                node->sibling = nullptr;
                node->parent = nullptr;
                root = meld(root, node);
            }
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE &val) {
        Node *newNode = new Node(val);
        if (this->empty()) {
            root = newNode;
            num_nodes++;
            return newNode;
        }
        if (root->parent == nullptr && newNode->parent == nullptr && root->sibling == nullptr && newNode->sibling == nullptr) {
            root = meld(root, newNode);
        }
        num_nodes++;
        return newNode;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    Node* meld(Node *pa, Node *pb) {
        if (pa == nullptr) return pb;
        if (pb == nullptr) return pa;

        if (this->compare(pa->getElt(), pb->getElt())) {
            if (pb->child) pa->sibling = pb->child;
            pa->parent = pb;
            pb->child = pa;
            return pb;
        }
        else {
            if (pa->child) pb->sibling = pa->child;
            pb->parent = pa;
            pa->child = pb;
            return pa;
        }
    }

    // NOTE: For member variables, you are only allowed to add a "root pointer"
    //       and a "count" of the number of nodes.  Anything else (such as a deque)
    //       should be declared inside of member functions as needed.
    Node *root;
    uint32_t num_nodes;
};


#endif // PAIRINGPQ_H
