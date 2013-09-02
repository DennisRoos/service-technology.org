/*****************************************************************************\
 BSD -- generating BSD automata

 Copyright (c) 2013 Simon Heiden

 BSD is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 BSD is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with BSD.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#pragma once

#include <pnapi/pnapi.h>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include "types.h"


/*!
 \brief XES log

 Contains all functions needed to generate logs in the XES format. (See @url{http://www.xes-standard.org/}.)
 The logs consist of traces which themselves consist of events. The number of traces in a log can be
 specified by the user as well as the number of events in a trace which also can be randomized.
 An event contains the label of a fired transition. A trace is a sequence of transitions which can be
 fired in the net or the service automaton from the initial state.
 The user may choose if a trace has to end in a final state or not.

*/
class BSD {

    public:

		static void initialize();

        static void computeBSD();

        static BSDNode* computeClosure(BSDNode &node, Label_ID label);

//        static MarkingList* computeClosure(MarkingList &list);

        static bool computeClosure(InnerMarking_ID id);//, bool &boundbroken);

        static void mergeWithoutDuplicates(MarkingList &result, MarkingList &temp);

        static BSDNode* checkInsertIntoGraph(BSDNode &node);

        static bool checkEquality(MarkingList &list1, MarkingList &list2);

        static void assignLambdas(BSDNodeList *graph);

        static std::list<MarkingList> SCCsearch(BSDNode * node);

        static std::list<MarkingList> tarjan(InnerMarking_ID markingID, BSDNode * node, MarkingList & U);

        static void printBSD(BSDNodeList *graph);

        static void printlist(MarkingList *list);

        static void finalize();

        static bool checkBiSimAndLambda(BSDgraph & graph1, BSDgraph & graph2);

        static bool computeBiSim(BSDNode * node_g1, BSDNode * node_g2, std::map<Label_ID, Label_ID> * mapping, Label_ID events);

        static std::map<Label_ID, Label_ID>* computeMapping(BSDgraph & graph1, BSDgraph & graph2);

        static void computeUBSD(BSDgraph & graph);

        static BSDNodeList* graph;

        static BSDNode* U;
        static BSDNode* emptyset;

    private:

        /// constructor
        BSD();

        static std::list<std::pair<BSDNode*, BSDNode*> >* bisimtemp;

        static MarkingList* templist;

        // helpers for tarjan algorithm (lambda value computation)
        static std::map<InnerMarking_ID, int>* dfs;
        static std::map<InnerMarking_ID, int>* lowlink;
        static int maxdfs;
        static std::stack<InnerMarking_ID>* S;
        static std::map<InnerMarking_ID, bool>* inStack;

};

#define TAU 0
#define BOUND 1
