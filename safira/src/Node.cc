#include <config.h>
//#include <cstdio>
#include "Node.h"
#include "map"
#include <iostream>

int Node::maxId = 0;
extern int currentIdPos;

extern map<string, int> label2id;
extern map<int, string> id2label;

/// constructor
Node::Node(Formula *_formula, int _id) : id(_id), formula(_formula) {
	if (maxId < id){
		maxId = id;
	}
	assert(id <= maxId);
	outEdges = new list<Node*>[label2id.size()];

}

Node::Node(Formula *_formula) : formula(_formula){
	assert(formula);
	id = ++maxId;
	outEdges = new list<Node*>[label2id.size()];
}


/// destructor
Node::~Node() {
	delete formula;
	delete[] outEdges;
}

/// add an outpoing edge to the node
void Node::addEdge(int label, Node *target) {
	assert(target);
	//assert(label <= label2id.size()-1);
	assert(id2label.find(label) != id2label.end());
	outEdges[label].push_back(target);
}

/// reset the maxId
void Node::init(){
	maxId = -1;
}


/// constructor
Nodepair::Nodepair(Formula* _f, Node* _n1, Node* _n2) : n1(_n1), n2(_n2) {

	assert(_f);
	assert(n1);
	assert(n2);

	node = new Node(_f);
	//id = maxId++;
	//outEdges = new list<Nodepair*>[label2id.size()];

}


/// destructor
Nodepair::~Nodepair() {
	//delete[] outEdges;
	delete node;
}

/// add an outpoing edge to the node
//void Nodepair::addEdge(int label, Nodepair *target) {
//	assert(target);
//	//assert(label <= label2id.size()-1);
//	assert(id2label.find(label) != id2label.end());
//	outEdges[label].push_back(target->node);
//}

