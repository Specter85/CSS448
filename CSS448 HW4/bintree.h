// File: bintree.h
// BinTree class header

#ifndef BIN_TREE_H
#define BIN_TREE_H
#include "symbol.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// BinTree class: A binary sort tree class.
//
// ADT Binary Sort Tree: A data structure whose data is stored in a tree, which
// if balanced, gives O(log(n)) time for insertion and retrieval. The data
// structure supports insertion, retrieval, clearing, and printing of data.
//
// Assumptions:
//		-- The computer will not run out of memory.
//		-- The function insert is always passed a pointer that is either valid
//		   or NULL.
//------------------------------------------------------------------------------
class BinTree {
private:

	// Struct for the nodes of the binary tree.
	struct Node {
		Symbol *data;
		Node *left;
		Node *right;
	};

	// Pointer to the root of the binary tree.
	Node *root;
	// Const for the size of arrays.
	static const int ARRAY_SIZE = 100;

	// Recursive function for emptying the tree.
	void clear(Node *);
	// Recursive function for comparing two binary trees.
	bool compare(Node *, Node *)const;
	// Recursive function for outputing the current object inorder.
	ostream &output(ostream &, Node *, int)const;
	// Recursive function for building a binary search tree from an
	// array.
	void arrayToBSTreeHelper(Node *&, int, int, Symbol *[]);

	// Helper function for cullBadPointers()
	void pCullHelper(Node *);

	// Helper function for cullBadTypes()
	void tCullHelper(Node *, bool &clean);

public:

	BinTree(); // Constructor
	//BinTree(const BinTree &); // Copy Constructor
	~BinTree(); // Destructor

	// Function for adding an element to the current object.
	bool insert(Symbol *);
	// Function for retrieving a element from the current object.
	Symbol *retrieve(const Symbol &);
	// Function for finding the depth of a node.
	int getDepth(const Symbol &)const;

	// Function for determining if the current object is empty.
	bool isEmpty()const;
	// Function for empting the current object.
	void makeEmpty();

	// Function for invalidating bad pointers.
	void cullBadPointers();

	// Function for invalidating bad types.
	void cullBadTypes(bool &clean);

	// Overloaded equality operator.
	bool operator==(const BinTree &)const;
	// Overloaded inequality operator.
	bool operator!=(const BinTree &)const;

	// Overloaded insertion operator.
	friend ostream &operator<<(ostream &, const BinTree &);

	// Function for outputing the tree.
	void display(int)const;

};

#endif