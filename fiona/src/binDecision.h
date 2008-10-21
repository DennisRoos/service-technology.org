/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    binDecision.h
 *
 * \brief   functions for representing a Petri net's state in a tree
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef BINDECISION_H
#define BINDECISION_H

using namespace std;

class oWFN;
class State;


/**
 * binDecision records form a binary decision tree
 */
class binDecision {
    public:
        // [LUHME XV]
        int bitnr;   ///< position of the first differing bit on the path from root to the current vect
        binDecision* nextold; ///< next branch off the current vector
        binDecision* nextnew; ///< next branch off the next branch of current vector

        /// actual bit vector
        unsigned char* vector;

        // [LUHME XV] kann m�glicherweise raus?
        /// backlink to previous decision
        binDecision* prev;

        // [LUHME XV] kann m�glicherweise raus?
        /// link to state record for this state
        State* state;

        /// constructor (2 parameters)
        binDecision(int b, long int bitVectorSize);

        /// destructor
        ~binDecision();

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(binDecision)
#define new NEW_NEW
};


/// DESCRIPTION
void binDelete(binDecision** Bucket, long int BitVectorSize);

/// DESCRIPTION
void binDeleteAll(binDecision* d);

/// DESCRIPTION
void binDeleteAll(oWFN* PN, int h);

/// DESCRIPTION
State* binInsert(oWFN* PN);

/// DESCRIPTION
State* binInsert(binDecision** Bucket, oWFN* PN);

/// DESCRIPTION
State* binSearch(oWFN* PN);

/// DESCRIPTION
State* binSearch(binDecision* Bucket, oWFN* PN);


/// turns an integer into a binary number
void inttobits(unsigned char* bytepos,
               int bitpos,
               int nrbits,
               unsigned int value);

/// computes the logarithm to base 2 of m
int logzwo(int m);

#endif //BINDECISION_H
