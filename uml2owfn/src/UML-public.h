/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2007, 2008, 2009  Dirk Fahland and Martin Znamirowski

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*
 * UML-public.h
 *
 *  Created on: 01.12.2008
 *      Author: Dirk Fahland
 *
 *  declarations of classes and functions for use outside of internal-representation.cc
 */


#ifndef UMLPUBLIC_H_
#define UMLPUBLIC_H_

class FlowContentElement;
class FlowContentConnection;
class Pin;
class PinCombination;
class InputCriterion;
class OutputCriterion;
class FlowContentNode;
class Task;
class Process;
class SimpleTask;
class Decision;
class Fork;
class Join;
class Merge;
class AtomicCFN;

class UmlProcessStatistics;


// flags to set the characteristics of the process (for filtering)
// take parameter processCharacteristics from options.h
#define UML_PC(x) (1 << x)



#endif /* UMLPUBLIC_H_ */
