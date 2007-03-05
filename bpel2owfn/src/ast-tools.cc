/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    ast-tools.cc
 *
 * \brief   unparse helper tools
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2006/02/08
 *
 * \date    \$Date: 2007/03/05 14:30:34 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.62 $
 *
 * \ingroup debug
 * \ingroup creation
 */




#include <cmath>
#include <cassert>
#include <utility>

#include "options.h"
#include "helpers.h"
#include "ast-details.h"
#include "ast-tools.h"

using std::pair;
using std::cerr;
using std::endl;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/// number of spaces to be added as indention
unsigned int indent = 0;

/// number of spaces to be added when indention is increased
unsigned int indentStep = 4;

namespace globals
{
  extern map<string, unsigned int> ASTE_linkIdMap;
}




/******************************************************************************
 * Functions for the Petri net unparsers
 *****************************************************************************/

/*!
 * \brief generates transition and places to throw a fault
 *
 * This functions generates a subnet to model the throwing of a fault. It
 * models faults in any control flow of the BPEL process as well as in any
 * patterns used.
 *
 * \param p1  the place in positive control flow from which the control flow
 *            enters the negative control flow
 *
 * \param p2  the place of the pattern on which a token shall be produced which
 *            can only be removed by stopping the pattern
 *
 * \param p1name  name of place p1 which is used to label the generated
 *                transitions (e.g. if p1 is called "running" generated
 *                transitions end with this name (e.g. "throwFault.running")
 *
 * \param prefix  prefix of the pattern to label generated places and
 *                transitions
 *
 * \param id  identifier of the caller activity
 *
 * \param negativeControlFlow  signals where the activity is located:
 *                             - 0: inside a scope or the process
 *                             - 1: inside a fault handler
 *                             - 2: inside a compensation handler
 *
 * \param preventFurtherFaults  controls what happens to further faults
 *                               - true: these faults are prevented (standard)
 *                               - false: these faults are suppressed
 *
 * \return a pointer to the (first) generated fault transition
 *
 * \ingroup creation
*/
Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix, kc::integer id,
    int negativeControlFlow, bool preventFurtherFaults)
{
  extern string currentScope;	// introduced in bpel-unparse.k
  extern PetriNet PN;		// introduced in main.c
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  assert(p1 != NULL);
  assert(p2 != NULL);


  // no fault transitions in case of "communicationonly" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;


  // if attribute "exitOnStandardFault" is set to "yes", the process should
  // terminate rather than handling the fault
  if (ASTEmap[ASTEmap[id->value]->parentScopeId]->attributes["exitOnStandardFault"] == "yes")
  {
    Transition *t1 = PN.newTransition(prefix + "exitOnStandardFault." + p1name);
    PN.newArc(p1, t1);
    PN.newArc(t1, p2);
    PN.newArc(t1, PN.findPlace("1.internal.exit"));

    return t1;
  }


  // fault handling for the `new' patterns
    switch (negativeControlFlow)
    {
      case(0): // activity in scope or process
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = PN.newTransition(prefix + "throwFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.fault_in"));

	  return t1;
	}

      case(1): // activity in fault handlers
      case(4): // <rethrow> activity
	{
	  // The parent scope is just the scope of the handler or the
	  // activity. Thus, we are interested in the parent's parent.
          unsigned int parentId = ASTEmap[ASTEmap[id->value]->parentScopeId]->parentScopeId;

	  Transition *t1;
	  if (negativeControlFlow == 4)
	    t1 = PN.newTransition(prefix + "rethrow");
	  else
	    t1 = PN.newTransition(prefix + "rethrowFault." + p1name);

	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);

	  if (ASTEmap[id->value]->parentScopeId == 1)
	    PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.exit"));
	  else
	    PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.fault_in"));

	  return t1;
	}

      case(2): // activity in compensation handler
	{
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

          // parent scope of a compensation handler shouldn't be the process :)
	  assert(parentId != 1);

	  Transition *t1 = PN.newTransition(prefix + "throwCHFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.ch_fault_in"));

	  return t1;
	}

      case(3): // activity in termination handler
        {
          unsigned int parentId = ASTEmap[id->value]->parentScopeId;

	  Transition *t1 = PN.newTransition(prefix + "signalFault." + p1name);
	  PN.newArc(p1, t1);
	  PN.newArc(t1, p2);
	  PN.newArc(t1, PN.findPlace(toString(parentId) + ".internal.terminationHandler.inner_fault"));
        }
    }
    return NULL;
}





/*!
 * \brief generates a subnet to stop
 *
 * Generates a transition and places to stop the activity, i.e. a
 * transition moving a token on the "stop" place to "stopped".
 *
 * \param p  the place in control flow from which the token is move to
 *           "stop"
 *
 * \param p_name  name of place p which is used to label the generated
 *                transition (e.g. if p1 is called "running" generated
 *                transition end with this name (e.g. "stop.running")
 *
 * \param prefix  prefix of the pattern to label generated transition
 *
 * \return a pointer to the stop transition
 *
 * \ingroup creation
 */
Transition *stop(Place *p, string p_name, string prefix)
{
  extern PetriNet PN;	// introduced in main.c

  assert(p != NULL);

  // no stop transitions in case of "nano" parameter
  if (parameters[P_COMMUNICATIONONLY])
    return NULL;
  
  Transition *stopTransition = PN.newTransition(prefix + "stopped." + p_name);
  PN.newArc(PN.findPlace(prefix + "stop"), stopTransition);
  PN.newArc(stopTransition, PN.findPlace(prefix + "stopped"));
  PN.newArc(p, stopTransition);

  return stopTransition;
}





/*!
 * \brief   returns a string used for identation.
 * \ingroup debug
 */
string inString()
{
  string result = "";

  for(unsigned int i=0; i<indent; i++)
    result += " ";

  return result;
}





/*!
 * \brief prints name of activity that is unparsed
 *
 * Prints a debug trace message containing the (opening) tag name of the
 * activity of the given id. If myindent is set to true, the enclosed
 * elements are indented.
 *
 * \see #footer
 *
 * \ingroup debug
*/
void header(int id, bool myindent)
{
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  trace(TRACE_DEBUG, "[PNU]" + inString() + "<" + ASTEmap[id]->activityTypeName() + " id=" + toString(id) + ">\n");

  if (myindent)
    indent += indentStep;
}





/*!
 * \brief prints name of activity that is unparsed
 * \overload
 * \see header(int id, bool myindent)
 * \ingroup debug
*/
void header(kc::integer id, bool myindent)
{
  header(id->value, myindent);
}





/*!
 * \brief prints name of activity that is unparsed
 *
 * Prints a debug trace message containing the (closing) tag name of the
 * activity of the given id. If myindent is set to true, the indentation is
 * reduced.
 *
 * \see #header

 * \ingroup debug
 */
void footer(int id, bool myindent)
{
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  if (myindent)
    indent -= indentStep;

  trace(TRACE_DEBUG, "[PNU]" + inString() + "</" + ASTEmap[id]->activityTypeName() + " id=" + toString(id) + ">\n");
}





/*!
 * \brief prints name of activity that is unparsed
 * \overload
 * \see footer(int id, bool myindent)
 * \ingroup debug
 */
void footer(kc::integer id, bool myindent)
{
  footer(id->value, myindent);
}





/*!
 * \brief add a subnet for dead-path elimination
 *
 * Creates arcs to set links on dead paths to false.
 *
 * \param t	transition that invokes DPE
 * \param id	the identifier of the AST node
 *
 * \ingroup creation
 */
void dpeLinks(Transition *t, int id)
{
  ENTER("[ASTT]");

  extern PetriNet PN;	// introduced in main.c
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  assert(t != NULL);
  assert(ASTEmap[id] != NULL);

  for (set<unsigned int>::iterator linkID = ASTEmap[id]->enclosedSourceLinks.begin();
      linkID != ASTEmap[id]->enclosedSourceLinks.end();
      linkID++)
  {
    assert(ASTEmap[*linkID] != NULL);
    string linkName = ASTEmap[*linkID]->linkName;
    PN.newArc(t, PN.findPlace("!link." + linkName));
  }

  LEAVE("[ASTT]");
}





/*!
 * \param loop_bounds	    a vector holding the maximal indices of the ancestor loops
 * \param loop_identifiers  a vector holding the identifiers of the ancestor loops
 * \param prefix            the prefix of the calling <scope>
 * \param my_max	    the maximal index of the direct parent loop
 */
void process_loop_bounds(const vector<unsigned int> &loop_bounds, const vector<unsigned int> &loop_identifiers, string prefix, unsigned int my_max)
{
  extern PetriNet PN;	// introduced in main.c

  vector<unsigned int> current_index(loop_bounds.size(), 1);
  unsigned int number = 1;

  // count the possible permutations of indices
  for (unsigned int i = 0; i < loop_bounds.size(); i++)
    number *= loop_bounds[i];

  // create transitions, places and arcs
  for (unsigned int i = 1; i <= number; i++)
  {
    Place *p1 = PN.newPlace(prefix + "c" + toString(current_index));
    Place *p2 = PN.newPlace(prefix + "!c" + toString(current_index));
    p2->mark(my_max);

    Transition *t14 = PN.newTransition(prefix + "t14." + toString(i));
    PN.newArc(PN.findPlace(prefix + "final1"), t14);
    PN.newArc(PN.findPlace(prefix + "!Successful"), t14, READ);
    PN.newArc(t14, PN.findPlace(prefix + "final"));

    Transition *t15 = PN.newTransition(prefix + "t15." + toString(i));
    PN.newArc(t15, p1);
    PN.newArc(p2, t15);
    PN.newArc(PN.findPlace(prefix + "final1"), t15);
    PN.newArc(PN.findPlace(prefix + "Successful"), t15);
    PN.newArc(t15, PN.findPlace(prefix + "!Successful"));
    PN.newArc(t15, PN.findPlace(prefix + "final"));

    Transition *t16 = PN.newTransition(prefix + "t16." + toString(i));
    PN.newArc(p2, t16, READ, my_max);
    PN.newArc(PN.findPlace(prefix + "compensated1"), t16);
    PN.newArc(t16, PN.findPlace(prefix + "compensated"));

    Transition *t17 = PN.newTransition(prefix + "t17." + toString(i));
    PN.newArc(p1, t17);
    PN.newArc(t17, p2);
    PN.newArc(PN.findPlace(prefix + "compensate"), t17);
    PN.newArc(t17, PN.findPlace(prefix + "ch_initial"));

    PN.mergePlaces(prefix + "compensated1", prefix + "compensate");

    // connect transitions with counters of ancestor loops
    for (unsigned i = 0; i < loop_identifiers.size(); i++)
    {
      Place *p3 = PN.findPlace(toString(loop_identifiers[i]) + ".internal.count." + toString(current_index[i]));
      assert(p3 != NULL);

      PN.newArc(p3, t15, READ);
      PN.newArc(p3, t16, READ);
      PN.newArc(p3, t17, READ);
    }

    // generate next index
    next_index(current_index, loop_bounds);
  }
}





/******************************************************************************
 * ACTIVITY RELATIONS
 *****************************************************************************/

namespace {
    map< pair< unsigned int, unsigned int >, activityRelationType > activityRelationMap;
}


/*!
 * \brief Returns the relationship between to activities.
 *
 * Two activities are always in some kind of relationship. Either conflicting, concurrent, or consecutively.
 * This method returns this relationship as analyzed in postprocessing.
 *
 * \param a	the id of the first activity
 * \param b	the id of the second activity
 * \returns     the relationship of a and b
 *
 * \ingroup creation
 */
activityRelationType activityRelation(unsigned int a, unsigned int b)
{
    ENTER("activityRelation");
    return activityRelationMap[pair<unsigned int,unsigned int>(a,b)];
    LEAVE("activityRelation");
}

/*!
 * \brief Defines two activities as conflicting.
 *
 * \param a	the id of the first activity
 * \param b	the id of the second activity
 *
 * \ingroup creation
 */
void conflictingActivities(unsigned int a, unsigned int b)
{
    ENTER("conflictingActivities");
    activityRelationMap[pair<unsigned int, unsigned int>(a,b)] = AR_CONFLICT;
    activityRelationMap[pair<unsigned int, unsigned int>(b,a)] = AR_CONFLICT;
    LEAVE("conflictingActivities");
}

/*!
 * \brief For two sets of activities define each pair of an a and a b activity as conflicting.
 *
 * \param a	the first set activity ids
 * \param b	the second set activity ids
 *
 * \ingroup creation
 */
void enterConflictingActivities( set< unsigned int > a, set< unsigned int > b )
{
  ENTER("enterConflictingActivities");
  for ( set< unsigned int >::iterator id1 = a.begin();
        id1 != a.end();
        id1++ )
  {
    for ( set< unsigned int >::iterator id2 = b.begin();
          id2 != b.end();
          id2++ )
    {
      conflictingActivities( *id1, *id2 );
    }
  }
  LEAVE("enterConflictingActivities");
}

/*!
 * \brief Defines two activities as enclodes.
 *
 * \param a	the id of the first activity
 * \param b	the id of the second activity
 *
 * \ingroup creation
 */
void enclosedActivities( unsigned int a, unsigned int b )
{
    ENTER("enclosedActivities");
    activityRelationMap[pair<unsigned int, unsigned int>(a,b)] = AR_ENCLOSES;
    activityRelationMap[pair<unsigned int, unsigned int>(b,a)] = AR_DESCENDS;
    LEAVE("enclosedActivities");
}

/*!
 * \brief For two sets of activities define each pair of an a and a b activity as enclosed.
 *
 * \param a	the first set activity ids
 * \param b	the second set activity ids
 *
 * \ingroup creation
 */
void enterEnclosedActivities( unsigned int a, set< unsigned int > b )
{
  ENTER("enterEnclosedActivities");
  for ( set< unsigned int >::iterator id = b.begin();
        id != b.end();
        id++ )
  {
    enclosedActivities( a, *id );
  }
  LEAVE("enterEnclosedActivities");
}

/******************************************************************************
 * Functions for checking SA00070 and SA00071
 *****************************************************************************/

/*!
 * \brief Checks for Static Analysis item SA00070.
 *
 * \param id  the id of the item to be checked
 *
 */
void check_SA00070( unsigned int id )
{
  ENTER("check_SA00070");

  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h
  
  // go through all enclosed activities and look if there have sources or targets
  for ( set< unsigned int >::iterator activities = ASTEmap[ id ]->enclosedActivities.begin();
          activities != ASTEmap[ id ]->enclosedActivities.end();
          activities++ )
  {
    // whether it's source or target doesn't matter - the link must be defined inside the element that is checked
    set< unsigned int > links = setUnion( ASTEmap[ *activities ]->sourceLinks, ASTEmap[ *activities ]->targetLinks );

    // so check every source and target
    for ( set< unsigned int>::iterator link = links.begin(); link != links.end(); link++ )
    {
      // get the id of the <link> element
      unsigned int linkId = globals::ASTE_linkIdMap[ ASTEmap[ *link ]->linkName ];
      
      // look if the linkId is inside the set of enclosed activities
      bool internal = false;
      for ( set< unsigned int >::iterator activity = ASTEmap[ id ]->enclosedActivities.begin();
            activity != ASTEmap[ id ]->enclosedActivities.end();
            activity++ )
      {
        // is this the right id?
        if ( *activity == linkId )
        {
          // yeah, everything is fine
          internal = true;
        }
      }
      // the linkId doesn't belong to the set of enclosed activities, so trigger the error
      if ( !internal )
      {
        SAerror( 70, ASTEmap[ *link ]->linkName, ASTEmap[ id ]->attributes[ "referenceLine" ] );
      }
    }
  }
  LEAVE("check_SA00070");
}

/*!
 * \brief Checks for Static Analysis item SA00071.
 *
 * \param id  the id of the item to be checked
 *
 */
void check_SA00071( unsigned int id )
{
  ENTER("check_SA00071");

  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h
 
  for ( set< unsigned int >::iterator activities = ASTEmap[ id ]->enclosedActivities.begin();
          activities != ASTEmap[ id ]->enclosedActivities.end();
          activities++ )
  {
    // whether it's source or target doesn't matter - the link must be defined inside the element that is checked
    set< unsigned int > targets = ASTEmap[ *activities ]->targetLinks;

    // so check every source and target
    for ( set< unsigned int>::iterator link = targets.begin(); link != targets.end(); link++ )
    {
      // get the id of the <link> element
      unsigned int linkId = globals::ASTE_linkIdMap[ ASTEmap[ *link ]->linkName ];
      
      // look if the linkId is inside the set of enclosed activities
      bool internal = false;
      for ( set< unsigned int >::iterator activity = ASTEmap[ id ]->enclosedActivities.begin();
            activity != ASTEmap[ id ]->enclosedActivities.end();
            activity++ )
      {
        set< unsigned int > sources = ASTEmap[ *activity ]->enclosedSourceLinks;

        // is this the right id?
        if ( sources.find( linkId ) != sources.end() )
        {
          // yeah, everything is fine
          internal = true;
        }
      }
      // the linkId doesn't belong to the set of enclosed activities, so trigger the error
      if ( !internal )
      {
        SAerror( 71, ASTEmap[ linkId ]->linkName, ASTEmap[ id ]->attributes[ "referenceLine" ] );
      }
    }
  }

  LEAVE("check_SA00071");
}


/******************************************************************************
 * Functions for the XML (pretty) unparser defined in bpel-unparse-xml.k
 *****************************************************************************/

/*!
 * \brief adds whitespace to indent output
 * \see #inup #indown
 * \ingroup debug
 */
void in()
{
  for(unsigned int i=0; i<indent; i++)
    *output << " ";
}





/*!
 * \brief increases indention#
 * \see #indown #in
 * \ingroup debug
 */
void inup()
{
  in();
  indent += indentStep;
}





/*!
 * \brief decreases indention
 * \see #inup #in
 * \ingroup debug
 */
void indown()
{
  indent -= indentStep;
  in();
}





/*!
 * \brief Lists attributes for use in pretty printer mode
 *
 * \param id  the id of the activity, whose attributes should be printed
 *
 */
void listAttributes ( unsigned int id )
{
  extern map<unsigned int, ASTE*> ASTEmap; // introduced in bpel-unparse-tools.h

  string result = "";
  for (map< string, string >::iterator attribute = ASTEmap[ id ]->attributes.begin(); attribute != ASTEmap[ id ]->attributes.end(); attribute++ )
  {
    if ( attribute->second != "" &&
         attribute->first != "referenceLine" &&
         !( (attribute->first == "exitOnStandardFaults" ||
             attribute->first == "createInstance" ||
             attribute->first == "initializePartnerRole" ||
             attribute->first == "initiate" ||
             attribute->first == "isolated" ||
             attribute->first == "surpressJoinFailure" ||
             attribute->first == "validate")
            && attribute->second == "no" )
      )
    {
      result += " " + attribute->first + "=\""+ attribute->second +"\"";
    }
  }
  *output << result;
}





/******************************************************************************
 * Other functions
 *****************************************************************************/

/*!
 * The function increases the indices in the vector and propagates resulting
 * carries. For example, if the index vector [3,2] is increased and the maximal
 * bounds are [5,2] the resulting vector is [4,1].
 *
 * \param current_index  vector holding the current indices
 * \param max_index      vector holding the upper bounds of the indices
 *
 * \post Index vector is increased according to the described rules.
 *
 * \invariant Each index lies between 1 and its maximal value, i.e., 1 and the
 *            maximal value can be reached.
 */
void next_index(vector<unsigned int> &current_index, const vector<unsigned int> &max_index)
{
  assert(current_index.size() == max_index.size());

  for (unsigned int i = 0; i < current_index.size(); i++)
  {
    if (current_index[i] < max_index[i])
    {
      current_index[i]++;
      break;
    }
    else
      current_index[i] = 1;
  }
}
