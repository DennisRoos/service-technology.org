/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include "CompositeMarking.h"
#include "LivelockOperatingGuideline.h"
#include "Label.h"


/********************
 * CompositeMarking *
 ********************/

/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
  constructor
  \todo check if this constructor is always called before CompositeMarkingsHandler::getMarking() -- if yes, combine them
*/
CompositeMarking::CompositeMarking(const StoredKnowledge * _storedKnowledge,
                                   const InnerMarking_ID _innerMarking_ID,
                                   InterfaceMarking* _interface) : dfs(0), lowlink(0), storedKnowledge(_storedKnowledge), interface(_interface), innerMarking_ID(_innerMarking_ID)

{

}


/*!
  destructor deletes the interface only, because before constructing a composite marking the interface has been created anew
*/
CompositeMarking::~CompositeMarking() {
    // an interface stored in a composite marking has always been created anew just to get stored in here
    delete interface;
}


/********************
 * MEMBER FUNCTIONS *
 ********************/

/*!
  constructs a string that contains the annotation of the composite marking with respect to the given set of knowledges
  \param knowledgeSet set of knowledges
  \todo std::set<Label_ID> durch einen Bool-Vektor der Länge Labels ersetzen
*/
std::set<Label_ID> CompositeMarking::getMyFormula(const std::set<StoredKnowledge *> & knowledgeSet) {

    // a set holding receiving and synchronous events
    std::set<Label_ID> disjunctionReceivingSynchronous;

    // receiving event
    for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
        // receiving event resolves deadlock
        if (interface->marked(l) and knowledgeSet.find(storedKnowledge->successors[l-1]) == knowledgeSet.end() and
            storedKnowledge->successors[l-1] != NULL and storedKnowledge->successors[l-1] != StoredKnowledge::empty and
            storedKnowledge->successors[l-1]->is_sane) {
            disjunctionReceivingSynchronous.insert(l);
        }
    }

    // synchronous communication
    for (Label_ID l = Label::first_sync; l <= Label::last_sync; ++l) {
        // synchronous communication resolves deadlock
        if (InnerMarking::synchs[l].find(innerMarking_ID) != InnerMarking::synchs[l].end() and
                knowledgeSet.find(storedKnowledge->successors[l-1]) == knowledgeSet.end() and
                storedKnowledge->successors[l-1] != NULL and storedKnowledge->successors[l-1] != StoredKnowledge::empty and
                storedKnowledge->successors[l-1]->is_sane) {

            disjunctionReceivingSynchronous.insert(l);
        }
    }

    // collect outgoing !-edges
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (knowledgeSet.find(storedKnowledge->successors[l-1]) == knowledgeSet.end() and
                storedKnowledge->successors[l-1] != NULL and
                storedKnowledge->successors[l-1]->is_sane) {

            disjunctionReceivingSynchronous.insert(l);
        }
    }

    // this marking is final
    if (interface->unmarked() and InnerMarking::inner_markings[innerMarking_ID]->is_final) {
        // override label SILENT
        disjunctionReceivingSynchronous.insert(0);
    }

    // return clause part
    return disjunctionReceivingSynchronous;
}


bool CompositeMarking::operator== (const CompositeMarking& other) const {

    if (storedKnowledge != other.storedKnowledge or innerMarking_ID != other.innerMarking_ID or *interface != *(other.interface)) {
        return false;
    }

    return true;
}


std::ostream& operator<< (std::ostream& o, const CompositeMarking& m) {
    o << "[";

    o << m.storedKnowledge << " m" << static_cast<unsigned int>(m.innerMarking_ID) << " " << *(m.interface) << " d: " << m.dfs << " l: " << m.lowlink;

    return o << "]";
}



/***************************
 * CompositeMarkingsHandler *
 ***************************/

/******************
 * STATIC MEMBERS *
 ******************/

CompositeMarking ** CompositeMarkingsHandler::visitedCompositeMarkings = NULL;
std::vector<CompositeMarking* > CompositeMarkingsHandler::tarjanStack;
unsigned int CompositeMarkingsHandler::numberElements = 0;
unsigned int CompositeMarkingsHandler::maxSize = 0;
unsigned int CompositeMarkingsHandler::minBookmark = 0;
std::vector<std::set<Label_ID> > CompositeMarkingsHandler::conjunctionOfDisjunctions;


/********************
 * STATIC FUNCTIONS *
 ********************/

/*!
  checks if the given marking has been visited already, if so return a pointer to the marking otherwise return NULL
  Note: in case the marking has been found, you may need to delete the given marking afterwards
  \param marking composite marking to be looked for
  
  \todo Niels thinks the for loop can be simplified
*/
CompositeMarking* CompositeMarkingsHandler::getMarking(const CompositeMarking * marking) {
    if (numberElements == 0) {
        return NULL;
    }

    CompositeMarking * storedMarking;

    for (unsigned int i = 0; i < numberElements; ++i) {
        storedMarking = visitedCompositeMarkings[i];

        assert(storedMarking != NULL);

        if (*marking == *storedMarking) {
            return storedMarking;
        }
    }

    return NULL;
}


/*!
  store the given marking in the set of visited markings and set the Tarjan values of the marking to the current number of elements stored
  \param marking composite marking to be stored
*/
bool CompositeMarkingsHandler::visitMarking(CompositeMarking * marking) {

    assert(visitedCompositeMarkings != NULL);
    assert(numberElements < maxSize);

    visitedCompositeMarkings[numberElements++] = marking;

    // set Tarjan values
    marking->dfs = marking->lowlink = numberElements;
}


/*!
  adds the given string clause to the conjunction of disjunctions associated with the current set of knowledges
  \param clause a string containing a disjunction of events
*/
void CompositeMarkingsHandler::addClause(std::set<Label_ID> & clause) {
    // add whole clause to the conjunction of clauses
    conjunctionOfDisjunctions.push_back(clause);

    LivelockOperatingGuideline::stats.numberAllElementsAnnotations += clause.size();
}

/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/


/*!
  initialize all values that are essential in the composite marking handler;
  using the given set of knowledges a disjunction of all possible sending events is constructed
  \note make sure function finalize() has been called before
  \param setOfStoredKnowledges the currently considered set of knowledges used for calculating the current set of sending events
*/
void CompositeMarkingsHandler::initialize(const std::set<StoredKnowledge* > & setOfStoredKnowledges) {

    // construct a valid disjunction of sending events
    for (std::set<StoredKnowledge* >::const_iterator iter = setOfStoredKnowledges.begin(); iter != setOfStoredKnowledges.end(); ++iter) {
        maxSize += (*iter)->sizeAllMarkings;
    }

    visitedCompositeMarkings = new CompositeMarking*[maxSize];
}


/*!
  clean up all values
*/
void CompositeMarkingsHandler::finalize() {

    // delete all stored composite markings one by one
    for (unsigned int i = 0; i < numberElements; ++i) {
        delete visitedCompositeMarkings[i];
    }

    delete [] visitedCompositeMarkings;

    numberElements = 0;

    tarjanStack.clear();
    maxSize = 0;
    minBookmark = 0;
    conjunctionOfDisjunctions.clear();
}