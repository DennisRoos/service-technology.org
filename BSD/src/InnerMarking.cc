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

#include <config.h>
#include <climits>
#include "openNet.h"
#include "InnerMarking.h"
#include "Label.h"

#include "verbose.h"


/******************
 * STATIC MEMBERS *
 ******************/

std::map<InnerMarking_ID, InnerMarking*> InnerMarking::markingMap;
std::map<InnerMarking_ID, bool> InnerMarking::finalMarkingReachableMap;
bool InnerMarking::is_acyclic = true;
InnerMarking** InnerMarking::inner_markings = NULL;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::receivers;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::senders;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::synchs;
InnerMarking::_stats InnerMarking::stats;

/******************
 * STATIC METHODS *
 ******************/

/*!
 Copy the markings from the mapping markingMap to C-style arrays.
 Additionally, a mapping is filled to quickly determine whether a marking can
 become transient if a message with a given label was sent to the net.

 \todo replace the mapping receivers and synchs by a single two-dimensional
       C-style array or do this check in the constructor
 */
void InnerMarking::initialize() {
    assert(stats.markings == markingMap.size());
    inner_markings = new InnerMarking*[stats.markings];

    // copy data from STL mapping (used during parsing) to a C array
    for (InnerMarking_ID i = 0; i < stats.markings; ++i) {
        assert(markingMap[i]);

        inner_markings[i] = markingMap[i];
        inner_markings[i]->is_bad = (inner_markings[i]->is_bad or
                                     (finalMarkingReachableMap.find(i) != finalMarkingReachableMap.end()
                                      and finalMarkingReachableMap[i] == false));

        // register markings that may become activated by sending a message
        // to them, by receiving a message or by synchronization
        for (uint8_t j = 0; j < inner_markings[i]->out_degree; ++j) {
        	status("node: %u, successor id: %u, label: %u, %s", i, inner_markings[i]->successors[j], inner_markings[i]->labels[j], Label::id2name[inner_markings[i]->labels[j]].c_str());
            if (SENDING(inner_markings[i]->labels[j])) {
                receivers[inner_markings[i]->labels[j]].insert(i);
//                status("%u, %s", inner_markings[i]->labels[j], Label::id2name[inner_markings[i]->labels[j]].c_str());
            }
            if (RECEIVING(inner_markings[i]->labels[j])) {
                senders[inner_markings[i]->labels[j]].insert(i);
            }
            if (SYNC(inner_markings[i]->labels[j])) {
                synchs[inner_markings[i]->labels[j]].insert(i);
            }
        }
    }

    // destroy temporary STL mappings
    markingMap.clear();
    finalMarkingReachableMap.clear();

    status("stored %d inner markings (%d final, %d bad, %d inevitable deadlocks)",
           stats.markings, stats.final_markings, stats.bad_states, stats.inevitable_deadlocks);

    if (stats.final_markings == 0) {
        message("%s: %s", _cimportant_("warning"), _cwarning_("no final marking found"));
    }
}

void InnerMarking::finalize() {
    for (InnerMarking_ID i = 0; i < stats.markings; ++i) {
        delete inner_markings[i];
    }
    delete[] inner_markings;

    status("InnerMarking: deleted %d objects", stats.markings);

    stats.bad_states = 0;
    stats.final_markings = 0;
    stats.markings = 0;
    stats.inevitable_deadlocks = 0;
}


/***************
 * CONSTRUCTOR *
 ***************/

InnerMarking::InnerMarking(const InnerMarking_ID& myId,
                           const std::vector<Label_ID>& _labels,
                           const std::vector<InnerMarking_ID>& _successors,
                           const bool& _is_final)
    : is_final(_is_final), is_waitstate(0), is_bad(0),
      out_degree(_successors.size()), successors(new InnerMarking_ID[out_degree]),
      labels(new Label_ID[out_degree]) {
    assert(_labels.size() == out_degree);
    assert(out_degree < UCHAR_MAX);

    if (++stats.markings % 50000 == 0) {
        message("%8d inner markings", stats.markings);
    }

    // copy given STL vectors to C arrays
    std::copy(_labels.begin(), _labels.end(), labels);
    std::copy(_successors.begin(), _successors.end(), successors);

    // knowing all successors, we can determine the type of the marking...
    determineType(myId);
}


InnerMarking::~InnerMarking() {
    delete[] labels;
    delete[] successors;
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
 The type is determined by checking the labels of the leaving transitions as
 well as the fact whether this marking is a final marking. For the further
 processing, it is sufficient to distinguish three types:

 - the marking is a bad state (is_bad) -- then a knowledge containing
   this inner marking can immediately be considered insane
 - the marking is a final marking (is_final) -- this is needed to distinguish
   deadlocks from final markings
 - the marking is a waitstate (is_waitstate) -- a waitstate is a marking of
   the inner of the net that can only be left by firing a transition that is
   connected to an input place

 This function also implements the detection of inevitable deadlocks. A
 marking is an inevitable deadlock, if it is a deadlock or all its successor
 markings are inevitable deadlocks. The detection exploits the way LoLA
 returns the reachability graph: when a marking is returned, we know that
 the only successors we have not considered yet (i.e. those markings where
 successors[i] == NULL) are also predecessors of this marking and cannot be
 a reason for this marking to be a deadlock. Hence, this marking is an
 inevitable deadlock if all known successors are deadlocks.

 \note except is_final, all types are initialized with 0, so it is sufficient
       to only set values to 1
 */
void InnerMarking::determineType(const InnerMarking_ID& myId) {
    bool is_transient = false;

    // deadlock: no successor markings and not final
    if (out_degree == 0 and not is_final) {
        ++stats.bad_states;
        is_bad = 1;
    }

    if (is_final) {
        ++stats.final_markings;
    }

    // when only deadlocks are considered, we don't care about final markings
    finalMarkingReachableMap[myId] = true;

    // variable to detect whether this marking has only deadlocking successors
    // standard: "true", otherwise evaluate noDeadlockDetection flag
    bool deadlock_inevitable = true;
    for (uint8_t i = 0; i < out_degree; ++i) {
        // if a single successor is not a deadlock, everything is OK
        if (markingMap[successors[i]] != NULL and
                deadlock_inevitable and
                not markingMap[successors[i]]->is_bad) {
            deadlock_inevitable = false;
        }

        // if we have not seen a successor yet, we can't be a deadlock
        if (markingMap[successors[i]] == NULL) {
            deadlock_inevitable = false;
        }

        // a tau or sending (sic!) transition makes this marking transient
        if (SILENT(labels[i]) or RECEIVING(labels[i])) {
            is_transient = true;
        }
    }

    // deadlock cannot be avoided any more -- treat this marking as deadlock
    if (not is_final and
            not is_bad and
            deadlock_inevitable) {
        is_bad = 1;
        ++stats.inevitable_deadlocks;
    }

    // draw some last conclusions
    if (not(is_transient or is_bad)) {
        is_waitstate = 1;
    }
}
