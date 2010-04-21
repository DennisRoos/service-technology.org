#ifndef STATEEQUATION_H_
#define STATEEQUATION_H_

#include "config.h"
#include "helpers.h"
#include "eventTerm.h"

/// A data structure for the extended state equation of an open net.
class ExtendedStateEquation {
public:
	static BinaryTree<pnapi::Place*,std::pair<int, std::pair<int*,REAL*> > >* lines; //!< For the analyzed open net, those are the place<->transition lines in the equation system.
	static std::pair<int*,REAL*>* eventLines; //!< For the analyzed open net, those are the event lines in the equation system.
	static BinaryTree<pnapi::Transition*, unsigned int>* transitionID; //!< Quick transition referencing.

	bool storeHistory;
	lprec* lp; //!< The lp system for this state equation
	pnapi::PetriNet* net; //!< The open net that is to be analyzed
	BinaryTree<pnapi::Place*,std::pair<int,int> >* finalMarking; //!< The partial final marking to be assumed

    /// Basic constructor, assigning/initializing fields.
	ExtendedStateEquation(pnapi::PetriNet* aNet, BinaryTree<pnapi::Place*,std::pair<int,int> >* aFinalMarking, bool history=true) : storeHistory(history), net(aNet), finalMarking(aFinalMarking), lp(NULL), isFeasible(false), isConstructed(false) {}

    /// Constructs the lp system from ExtendedStateEquation::lines and the final marking, returns true if no error occured.
	bool constructLP();

	bool isFeasible; //!< the lp system is feasible.
	bool isConstructed; //!< the lp system was constructed.

    /// Outputs the lp system.
	void output() {
		print_lp(lp);
	}

    /// Evaluates a given event term and returns the bounds.
	EventTermBound* evaluate(EventTerm*);
    /// Evaluates a given event term and returns the bounds.
	EventTermBound* evaluate(int*);

    /// Deletes the underlying lp system.
	~ExtendedStateEquation() {
		delete_lp(lp);
	}


	std::vector<int*> calculatedEventTerms; //!< Contains all evaluated event terms.
	std::vector<EventTermBound*> calculatedBounds; //!< Contains all computed bounds.

};

#endif /* STATEEQUATION_H_ */
