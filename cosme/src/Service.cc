/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "Service.h"

ServiceMarking::ServiceMarking(const std::multimap<label_id_t, std::pair<label_id_t, og_service_index_t> > &successors,
                               unsigned is_Final) {

    std::multimap<label_id_t, std::pair<label_id_t, og_service_index_t> >::const_iterator it;
    label_index_t i = 0;

    this->mOutDegree = successors.size();
    this->mSuccessors = new og_service_index_t[this->mOutDegree];
    this->mLabels = new label_id_t[this->mOutDegree];

    for (it = successors.begin(); it != successors.end(); it++, i++) {
        this->mLabels[i] = it->first;
        this->mSuccessors[i] = it->second.second;
    }

    this->mIsFinal = is_Final;

    this->mEnabledTransitions = NULL;
}

ServiceMarking::~ServiceMarking() {
    delete[] this->mSuccessors;
    delete[] this->mLabels;
    delete this->mEnabledTransitions;
}

std::string ServiceMarking::toString() const {
    std::stringstream tmpStr;
    tmpStr << std::endl;
    if (this->mIsFinal) {
        tmpStr << " F: 1";
    } else {
        tmpStr << " F: 0";
    }
    tmpStr << std::endl;
    if (this->mEnabledTransitions != NULL) {
        tmpStr << " en(q): " << this->mEnabledTransitions->toString() << std::endl;
    }
    for (label_index_t i = 0; i < this->mOutDegree; i++) {
        tmpStr << " " << static_cast<unsigned int>(this->mLabels[i]);
        tmpStr << " -> ";
        tmpStr << static_cast<unsigned int>(this->mSuccessors[i]) << std::endl;
    }
    return tmpStr.str();
}

///
///

Service::Service(const std::map<og_service_index_t, ServiceMarking*> &markings,
                 const std::set<label_id_t> &interface) {

    this->mMarkings = new ServiceMarking*[markings.size()];
    this->mSize = markings.size();

    std::map<og_service_index_t, ServiceMarking*>::const_iterator it;

    for (it = markings.begin(); it != markings.end(); it++) {
        this->mMarkings[it->first] = it->second;
    }

    std::set<label_id_t>::const_iterator it2;

    for (it2 = interface.begin(); it2 != interface.end(); it2++) {
        this->mInterface.insert((*it2));
    }

}

Service::~Service() {}

void Service::finalize() {
    for (og_service_index_t i = 0; i < this->mSize; i++) {
        delete this->mMarkings[i];
    }
    delete[] this->mMarkings;
    mInterface.clear();
    status("Service: deleted %d markings", this->mSize);
}

void Service::calculateBitSets(Label& interface) {

    for (og_service_index_t i = 0; i < this->mSize; i++) {
        this->mMarkings[i]->mEnabledTransitions = new BitSetC(interface.size(), true);
        for (label_index_t j = 0; j < this->mMarkings[i]->outDegree(); j++) {
            this->mMarkings[i]->mEnabledTransitions->setBit(this->mMarkings[i]->label(j));
        }
    }
}

std::string Service::toString() const {
    std::stringstream tmpStr;
    tmpStr << "\nStates: " << this->mSize << std::endl;
    for (og_service_index_t i = 0; i < this->mSize; i++) {
        tmpStr << "State " << i << ": ";
        tmpStr << this->mMarkings[i]->toString();
    }
    return tmpStr.str();
}
