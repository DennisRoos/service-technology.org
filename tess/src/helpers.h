/*****************************************************************************\
 Tess -- Selecting Test Cases for Services

 Copyright (c) 2010 Kathrin Kaschner

 Tess is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tess is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Tess.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef HELPERS_H_
#define HELPERS_H_

#include <vector>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include "Node.h"
#include "TestCase.h"
//#include "types.h"
//#include "Formula.h"


using namespace std;

void addLabel(string label, char io);

string intToString(int i);
string dnfToString(list< set<int> >);
list< set<int> > putIdIntoList(int id);
void initGlobalVariables();
list< set<int> > minimizeDnf(list< set<int> > originalDnf);
string setToString(set<int> myset);
void printAllPartialTestCases(map<Node*, vector<TestCase*> >* allPartialTestCases, ostream& o);
void printTestCases(vector<TestCase*> testCases, ostream& o);

bool finalInDnf(const list<set<int> > assignments);

struct MatchingNodes{
	Node* ogNode;
	TNode* testNode;
};

#endif
