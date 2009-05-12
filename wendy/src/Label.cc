/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

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


#include "Label.h"
#include "InnerMarking.h"
#include "cmdline.h"

using std::set;
using std::string;
using pnapi::Node;
using pnapi::Place;
using pnapi::Transition;

extern gengetopt_args_info args_info;


/******************
 * STATIC MEMBERS *
 ******************/

Label_ID Label::first_receive = 0;
Label_ID Label::last_receive = 0;
Label_ID Label::first_send = 0;
Label_ID Label::last_send = 0;
Label_ID Label::first_sync = 0;
Label_ID Label::last_sync = 0;
Label_ID Label::async_events = 0;
Label_ID Label::sync_events = 0;
Label_ID Label::events = 0;

std::map<Label_ID, string> Label::id2name;
std::map<string, Label_ID> Label::name2id;


/******************
 * STATIC METHODS *
 ******************/

void Label::initialize() {
    // ASYNCHRONOUS RECEIVE EVENTS (?-step for us)
    first_receive = 1;

    const set<Place*> outputPlaces( InnerMarking::net->getOutputPlaces() );
    for (set<Place*>::const_iterator p = outputPlaces.begin(); p != outputPlaces.end(); ++p) {
        ++events;
        id2name[events] = "?" + (*p)->getName();

        const set<Node*> preset( (*p)->getPreset() );
        for (set<Node*>::const_iterator t = preset.begin(); t != preset.end(); ++t) {
            name2id[(*t)->getName()] = events;
        }
    }
    
    last_receive = events;


    // ASYNCHRONOUS RECEIVE SEND (!-step for us)
    first_send = events+1;

    const set<Place*> inputPlaces( InnerMarking::net->getInputPlaces() );

    for (set<Place*>::const_iterator p = inputPlaces.begin(); p != inputPlaces.end(); ++p) {
        ++events;
        id2name[events] = "!" + (*p)->getName();

        const set<Node*> postset( (*p)->getPostset() );
        for (set<Node*>::const_iterator t = postset.begin(); t != postset.end(); ++t) {
            name2id[(*t)->getName()] = events;
        }
    }

    last_send = events;


    // SYNCHRONOUS EVENTS (#-step for us)
    first_sync = events+1;

    std::map<string, Label_ID> sync_labels;
    const set<string> sync_label_names( InnerMarking::net->getSynchronousLabels() );
    for (set<string>::const_iterator l = sync_label_names.begin(); l != sync_label_names.end(); ++l) {
        sync_labels[*l] = ++events;
        id2name[events] = "#" + *l;
    }

    const set<Transition*> trans( InnerMarking::net->getSynchronizedTransitions() );
    for (set<Transition*>::const_iterator t = trans.begin(); t != trans.end(); ++t) {
        name2id[(*t)->getName()] = sync_labels[*(*t)->getSynchronizeLabels().begin()];
    }

    last_sync = events;
    async_events = last_send;
    sync_events = last_sync - async_events;

    if (args_info.verbose_given) {
        for (unsigned int i = first_receive; i <= last_sync; ++i) {
            fprintf(stderr, "%s: label with id %2d is '%s'\n", PACKAGE, i, id2name[i].c_str());
        }

        fprintf(stderr, "%s: initialized labels for %d events (%d asynchronous, %d synchronous)\n",
            PACKAGE, events, async_events, sync_events);
    }
}
