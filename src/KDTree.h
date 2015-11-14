/**
 * File: KDTree.h
 * Author: Hyounggap An
 * ------------------------
 * An interface representing a kd-tree in some number of dimensions. The tree
 * can be constructed from a set of data and then queried for membership and
 * nearest neighbors.
 */

#ifndef KDTREE_INCLUDED
#define KDTREE_INCLUDED

#include "Point.h"
#include "BoundedPQueue.h"
#include <stdexcept>
#include <cmath>

// "using namespace" in a header file is conventionally frowned upon, but I'm
// including it here so that you may use things like size_t without having to
// type std::size_t every time.
using namespace std;

template <size_t N, typename ElemType>
class KDTree {
public:
    // Constructor: KDTree();
    // Usage: KDTree<3, int> myTree;
    // ----------------------------------------------------
    // Constructs an empty KDTree.
    KDTree();
    
    // Destructor: ~KDTree()
    // Usage: (implicit)
    // ----------------------------------------------------
    // Cleans up all resources used by the KDTree.
    ~KDTree();
    
    // KDTree(const KDTree& rhs);
    // KDTree& operator=(const KDTree& rhs);
    // Usage: KDTree<3, int> one = two;
    // Usage: one = two;
    // -----------------------------------------------------
    // Deep-copies the contents of another KDTree into this one.
    KDTree(const KDTree& rhs);
    KDTree& operator=(const KDTree& rhs);
    
    // size_t dimension() const;
    // Usage: size_t dim = kd.dimension();
    // ----------------------------------------------------
    // Returns the dimension of the points stored in this KDTree.
    size_t dimension() const;
    
    // size_t size() const;
    // bool empty() const;
    // Usage: if (kd.empty())
    // ----------------------------------------------------
    // Returns the number of elements in the kd-tree and whether the tree is
    // empty.
    size_t size() const;
    bool empty() const;
    
    // bool contains(const Point<N>& pt) const;
    // Usage: if (kd.contains(pt))
    // ----------------------------------------------------
    // Returns whether the specified point is contained in the KDTree.
    bool contains(const Point<N>& pt) const;
    
    // void insert(const Point<N>& pt, const ElemType& value);
    // Usage: kd.insert(v, "This value is associated with v.");
    // ----------------------------------------------------
    // Inserts the point pt into the KDTree, associating it with the specified
    // value. If the element already existed in the tree, the new value will
    // overwrite the existing one.
    void insert(const Point<N>& pt, const ElemType& value);
    
    // ElemType& operator[](const Point<N>& pt);
    // Usage: kd[v] = "Some Value";
    // ----------------------------------------------------
    // Returns a reference to the value associated with point pt in the KDTree.
    // If the point does not exist, then it is added to the KDTree using the
    // default value of ElemType as its key.
    ElemType& operator[](const Point<N>& pt);
    
    // ElemType& at(const Point<N>& pt);
    // const ElemType& at(const Point<N>& pt) const;
    // Usage: cout << kd.at(v) << endl;
    // ----------------------------------------------------
    // Returns a reference to the key associated with the point pt. If the point
    // is not in the tree, this function throws an out_of_range exception.
    ElemType& at(const Point<N>& pt);
    const ElemType& at(const Point<N>& pt) const;
    
    // ElemType kNNValue(const Point<N>& key, size_t k) const
    // Usage: cout << kd.kNNValue(v, 3) << endl;
    // ----------------------------------------------------
    // Given a point v and an integer k, finds the k points in the KDTree
    // nearest to v and returns the most common value associated with those
    // points. In the event of a tie, one of the most frequent value will be
    // chosen.
    ElemType kNNValue(const Point<N>& key, size_t k) const;

private:
    // TODO: Add implementation details here.
	size_t splittingDimension;
	size_t count;
	struct Node{
		Node(){ left=right=NULL;}
		Node(const Point<N>& _key, const ElemType& _value):key(_key), value(_value)//, splittingValue(0)
		{ left = right = NULL; }
		Point<N> key;
		ElemType value;
		/*double splittingValue;*/
		Node *left, *right;
	};
	Node* root;
	Node* findNode(const Point<N>& pt) const;
	void insert(Node& node);
};

/** KDTree class implementation details */

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() : splittingDimension(0), count(0) {
    // Create an empty KDTree
	root = NULL;
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {
    // Release all resources KDTree kept
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
    // return KDTree's dimension
    return N;
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const
{
	// return the number of nodes KDTree contains
	return count;
}
template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::empty() const
{
	// return zero if KDTree is empty
	return (count == 0);
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::contains(const Point<N>& pt) const
{
	return (findNode(pt) != NULL);
}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert(const Point<N>& pt, const ElemType& value)
{
	// insert a node in KDTree
	if(!root)
	{
		root = new Node(pt, value);
		++count;
		return;
	}
	Node* curNode = root;
	size_t dim=0;
	while(curNode)
	{
		if(curNode->key == pt)
		{
			curNode->value = value;
			return;
		}
		else
		{
			if(pt[dim] < curNode->key[dim])
			{
				// left
				if(curNode->left)
					curNode=curNode->left;
				else
				{
					Node* cNode = new Node(pt, value);
					curNode->left = cNode;
					++count;
					return;
				}	
			}
			else
			{
				// right
				if(curNode->right)
					curNode=curNode->right;
				else
				{
					Node* cNode = new Node(pt, value);
					curNode->right = cNode;
					++count;
					return;
				}
			}
		}
		dim = (dim+1)%N;
	}
}

template <size_t N, typename ElemType>
typename KDTree<N,ElemType>::Node* KDTree<N, ElemType>::findNode(const Point<N>& pt) const
{
	Node* curNode = root;
	size_t dim=0;
	while(curNode)
	{
		if(curNode->key == pt)
			return curNode;
		if(pt[dim] < curNode->key[dim])
			curNode = curNode->left;
		else
			curNode = curNode->right;
		dim = (dim+1)%N;
	}
	return NULL;
}

template <size_t N, typename ElemType>
void KDTree<N,ElemType>::insert(Node& node)
{
	// insert a node in KDTree
	if(!root)
	{
		root = &node;
		++count;
		return;
	}
	Node* curNode = root;
	size_t dim=0;
	while(curNode)
	{
		if(curNode->key == node.key)
		{
			curNode->value = node.value;
			return;
		}
		else
		{
			if(node.key[dim] < curNode->key[dim])
			{
				// left
				if(curNode->left)
					curNode=curNode->left;
				else
				{
					curNode->left = &node;
					++count;
					return;
				}	
			}
			else
			{
				// right
				if(curNode->right)
					curNode=curNode->right;
				else
				{
					curNode->right = &node;
					++count;
					return;
				}
			}
		}
		dim = (dim+1)%N;
	}
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::operator[](const Point<N>& pt)
{
	Node* target = const_cast<Node*>(findNode(pt));
	if(!target)
	{
		target = new Node();
		target->key = pt;
		insert(*target);
	}
	return target->value;
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::at(const Point<N>& pt)
{
	Node* target = findNode(pt);
	if(!target)
		throw out_of_range("The point does not exist in the tree");
	return target->value;
}

template <size_t N, typename ElemType>
const ElemType& KDTree<N, ElemType>::at(const Point<N>& pt) const
{
	Node* target = findNode(pt);
	if(!target)
		throw out_of_range("The point does not exist in the tree");
	return target->value;
}

template <size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::kNNValue(const Point<N>& key, size_t k) const
{
	ElemType foo;
	return foo;
}
#endif // KDTREE_INCLUDED