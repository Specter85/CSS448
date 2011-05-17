// File: bintree.cpp
// Function definitions for BinTree class.

#include "bintree.h"
#include "procfunc.h"
#include "pointertype.h"
#include "recordtype.h"
#include "arraytype.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// Constructor
// Function for building a new BinTree object.
BinTree::BinTree() {
	root = NULL;
}

//------------------------------------------------------------------------------
// Destructor
// Function for destroying a BinTree object.
BinTree::~BinTree() {
	clear(root);
}

//------------------------------------------------------------------------------
// clear
// Recursive function which clears the current object.
void BinTree::clear(Node *rhs) {
	if(rhs != NULL) {
		clear(rhs->left);
		clear(rhs->right);
		ProcFunc *temp = dynamic_cast<ProcFunc*>(rhs->data);
		if(temp == NULL) {
			delete rhs->data;
		}
		delete rhs;
	}
}

//------------------------------------------------------------------------------
// insert
// Function which inserts an object of type NodeData into the current object.
// Returns true if successful false otherwise.
bool BinTree::insert(Symbol *rhs) {
	// Make sure rhs is valid.
	if(rhs == NULL) {
		return false;
	}

	// Create the new node.
	Node *temp = new Node;
	temp->data = rhs;
	temp->left = NULL;
	temp->right = NULL;

	// If the current object is empty insert at the root.
	if(root == NULL) {
		root = temp;
	}
	// If the current object is not empty determine the corrent spot.
	else {
		Node *current = root;

		// Walk through the tree until the correct position is found.
		for(;;) {
			// If data is duplicate don't insert it and return false.
			if(*temp->data == *current->data) {
				cout << "***ERROR: Symbol: " << temp->data->name 
					<< " already in table" << endl;
				delete temp->data;
				delete temp;
				return false;
			}
			// If the new node is less than the current node.
			else if(*temp->data < *current->data) {
				// If the correct position is found insert the node.
				if(current->left == NULL) {
					current->left = temp;
					break;
				}
				// If the correct position has not been found step down the
				// tree.
				else {
					current = current->left;
				}
			}
			// If the new node is greater than the current node.
			else {
				// If the currect position is found insert the node.
				if(current->right == NULL) {
					current->right = temp;
					break;
				}
				// If the correct position has not been found step down the
				// tree.
				else {
					current = current->right;
				}
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// retrieve
// Function which returns a pointer to an element of the current object
// specified by rhs. If the element specified by rhs is not found this function
// returns NULL.
Symbol *BinTree::retrieve(const Symbol &rhs) {
	Node *current = root;
	// Loop till the specified node is found.
	for(;;) {
		// If the specified node is found not to exist return NULL.
		if(current == NULL) {
			return NULL;
		}

		// If the specified node is found return a pointer to its data.
		if(*current->data == rhs) {
			return current->data;
		}
		// If rhs is less than the current node step left.
		else if(rhs < *current->data) {
			current = current->left;
		}
		// If rhs is greater than the current node step right.
		else {
			current = current->right;
		}
	}
}

//------------------------------------------------------------------------------
// isEmpty
// Function for determining if the current object is empty. Returns true if
// empty false otherwise.
bool BinTree::isEmpty()const {
	return root == NULL;
}

//------------------------------------------------------------------------------
// makeEmpty
// Function which empties the current object.
void BinTree::makeEmpty() {
	clear(root);
	root = NULL;
}

//------------------------------------------------------------------------------
// ==
// Operator which returns true if the current object has the same content and
// structure as rhs false otherwise.
bool BinTree::operator==(const BinTree &rhs)const {
	return compare(root, rhs.root);
}

//------------------------------------------------------------------------------
// compare
// Recursive function which returns true if two subtrees are the same false
// otherwise.
bool BinTree::compare(Node *currentT, Node *currentO)const {
	// If both pointers are NULL the subtrees are equal so return true
	if((currentT == NULL) && (currentO == NULL)) {
		return true;
	}
	// If one of the pointers is NULL but not the other the trees are equal
	// so far so return false.
	if((currentT == NULL) || (currentO == NULL)) {
		return false;
	}
	// If the data of currentT and currentO is not equal then the subtrees are
	// not equal so return false.
	if(*currentT->data != *currentO->data) {
		return false;
	}

	// Call compare for both the left and right pointers of currentT and
	// currentO return true only if the results of both are true.
	return compare(currentT->left, currentO->left) &&
		compare(currentT->right, currentO->right);
}

//------------------------------------------------------------------------------
// cullBadPointers
void BinTree::cullBadPointers() {
	pCullHelper(root);
}

void BinTree::pCullHelper(Node *rhs) {
	if(rhs != NULL) {
		PointerType *temp = dynamic_cast<PointerType*>(rhs->data);

		if(temp != NULL && temp->valid) {
			Type *pType = temp->typeTo;
			for(;;) {
				if(!pType->valid) {
					temp->valid = false;
					break;
				}
				else {
					if(dynamic_cast<PointerType*>(pType) != NULL) {
						pType = dynamic_cast<PointerType*>(pType)->typeTo;
					}
					else {
						break;
					}
				}
			}
		}

		pCullHelper(rhs->left);
		pCullHelper(rhs->right);
	}
}

//------------------------------------------------------------------------------
// cullBadTypes
void BinTree::cullBadTypes(bool &clean) {
	tCullHelper(root, clean);
}

void BinTree::tCullHelper(Node *rhs, bool &clean) {
	if(rhs != NULL) {
		
		// See if rhs is an array type if so make sure it is valid.
		ArrayType *aTemp = dynamic_cast<ArrayType*>(rhs->data);
		if(aTemp != NULL && aTemp->valid) {
			Type *pType = aTemp->type;
			if(!pType->valid) {
				aTemp->valid = false;
				clean = false;
			}
		}

		// See if rhs is an record type if so make sure it is valid.
		RecordType *rTemp = dynamic_cast<RecordType*>(rhs->data);
		if(rTemp != NULL && rTemp->valid) {
			for(vector<Variable*>::iterator i = rTemp->vars.begin();
				i != rTemp->vars.end(); i++) {
					if((*i)->type == NULL || !(*i)->type->valid) {
						rTemp->valid = false;
						clean = false;
						break;
					}
			}
		}

		tCullHelper(rhs->left, clean);
		tCullHelper(rhs->right, clean);
	}
}

//------------------------------------------------------------------------------
// ==
// Operator which returns true if the current object does not have same 
// content and structure as rhs false otherwise.
bool BinTree::operator!=(const BinTree &rhs)const {
	return !(*this == rhs);
}

//------------------------------------------------------------------------------
// <<
// Operator for outputing the contents of rhs in order.
ostream &operator<<(ostream &out, const BinTree &rhs) {
	// Output the tree inorder with an line ending at the end.
	rhs.output(out, rhs.root, 0);
	cout << endl;
	return out;
}

//------------------------------------------------------------------------------
// output
// Recursive helper function for the insertion operator, which outputs the
// contents of the current object in order.
ostream &BinTree::output(ostream &out, Node *current, int indent)const {
	// Make sure current is not NULL.
	if(current != NULL) {
		// Output the left subtree.
		output(out, current->left, indent);
		ProcFunc *temp = dynamic_cast<ProcFunc*>(current->data);
		if(temp == NULL && current->data->valid) {
			// Output the current node with a space after it.
			current->data->display(indent);
		}
		// Output the right subtree.
		output(out, current->right, indent);
	}

	return out;
}

//------------------------------------------------------------------------------
// display
// Function for outputing the tree.
void BinTree::display(int indent)const {
	// Output the tree inorder with an line ending at the end.
	output(std::cout, root, indent);
	std::cout << endl;
}