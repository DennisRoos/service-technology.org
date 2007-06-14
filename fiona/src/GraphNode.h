/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    GraphNode.h
 *
 * \brief   functions for handling of nodes of IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef GraphNode_H_
#define GraphNode_H_

#include "mynew.h"
#include "enums.h"
#include "commGraphFormula.h"

class State;
class GraphEdge;
class successorNodeList;
class literal;
class CNF_formula;

typedef std::set<State*> StateSet;


class GraphNode {
private:
    /**
     * Number of this GraphNode in the graph.
     */
    unsigned int number;

    /**
     * Color of this GraphNode.
     */
    GraphNodeColor color;

    /**
     * Annotation of this node (a CNF) as a formula.
     */
    CNF_formula* annotation;

    /**
     * List of all the nodes succeeding this one including the edge between
     * them
     */
    successorNodeList * successorNodes;

public:
    bool hasFinalStateInStateSet;

	// GraphNode management
	GraphNode(int);
	~GraphNode ();

	int * eventsUsed;
	int eventsToBeSeen;
	
    unsigned int getNumber() const;    
    void setNumber(unsigned int);
    
	// states in GraphNode
    bool addState(State *);
    int getNumberOfDeadlocks();
    
    StateSet reachGraphStateSet;		// this set contains only a reduced number of states in case the state reduced graph is to be build
//    StateSet setOfStatesTemp;	// this set contains all states

	// traversing successors/predecessors
    void addSuccessorNode(GraphEdge *);

    GraphEdge * getNextSuccEdge();

    void resetIteratingSuccNodes();

	// annotation
    CNF_formula* getCNF_formula();
    std::string getCNFString();
    
    CommGraphFormulaAssignment* getAssignment();
    
    void addClause(CommGraphFormulaMultiaryOr*);
   
//	int numberOfElementsInAnnotation();

	// analysis   
	bool finalAnalysisDone;

    GraphNodeColor analyseNodeByFormula();

    GraphNodeColor getColor() const;
    void setColor(GraphNodeColor c);

    bool isToShow(const GraphNode* rootOfGraph) const;

	void removeLiteralFromFormula(unsigned int, GraphEdgeType);
	
//	friend bool operator == (GraphNode const&, GraphNode const& );		// could be changed and replaced by hash-value
	friend bool operator < (GraphNode const&, GraphNode const& );
//	friend ostream& operator << (std::ostream& os, const GraphNode& v);

// Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
    NEW_OPERATOR(GraphNode)
#define new NEW_NEW
    
};

#endif /*GraphNode_H_*/
