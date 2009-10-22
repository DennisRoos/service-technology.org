#ifndef Node_H_
#define Node_H_

#include "settings.h"
#include "Formula.h"
#include "Event.h"



class Node {

    public:

    	/*-----------.
		| attributes |
		`-----------*/

        /// successor nodes together with their corresponding events
        map< Node*, list<Event*> > successors;

        /// this node's formula
        Formula* formula;

        /// states if this is a final node or not
        bool final;

        /// a boolean flag used to speed up some algorithms
        bool flag;

        /*--------.
		| methods |
		`--------*/

        /// basic constructor
        explicit Node(unsigned int _id) : id(_id), final(false), flag(false) {};

        /// basic destructor
        virtual ~Node() {
            formula->clear();
            successors.clear();
        };

        /// getter and setter for id
        unsigned int getID() const { return id; }
        void setID(unsigned int newID) { id = newID; }

        /// set flag for this node and all successor nodes
        void setFlagRecursively(bool);

        /// cut connections to inefficient successor nodes
        unsigned int computeEfficientSuccessors();

        /// compute a list of cost minimal assignments for this node
        unsigned int getCostMinimalAssignments(
        		list< pair< pair< Node*, Event*>, unsigned int> >,
        		list< FormulaAssignment >& );

        /// helper function for getCostMinimalAssignments
        void getCostMinimalAssignmentsRecursively(
        		list< pair< pair<Node*, Event*>, unsigned int> >,
        		unsigned int,
        		FormulaAssignment,
        		unsigned int&,
        		list< FormulaAssignment >&);

        /// print debug information about this node
        void outputDebug(std::ostream&);
        void outputDebugRecursively(std::ostream&, map<Node*, bool>&);

        /// print standard information about this node
        void output(std::ostream&, map<Node*, bool>&, bool);


    private:

        /// the node's ID
        unsigned int id;
};

#endif /* Node_H_ */
