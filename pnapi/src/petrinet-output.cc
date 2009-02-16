/*!
 * \file    petrinet-output.cc
 *
 * \brief   Petri Net API: file output
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          last changes of: $Author$
 *
 * \since   2006-03-16
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#include <cassert>
#include <iostream>
#include <iomanip>
#include <ostream>

#include "config.h"
#include "petrinet.h"
#include "io.h"

using std::ostream;
using std::endl;
using std::cerr;
using std::setw;
using std::right;
using std::left;
using std::deque;
using std::set;
using std::string;

using pnapi::io::operator<<;
using pnapi::io::owfn;
using pnapi::io::stat;

using pnapi::io::util::operator<<;
using pnapi::io::util::delim;

namespace pnapi
{

  /*!
   */
  void PetriNet::output_gastex(ostream & os) const
  {
    // TODO: implement
  }


  /*!
   */
  void PetriNet::output_owfn(ostream & os) const
  {
    os  //< output everything to this stream

      << "{" << endl
      << "  generated by: " 
      << getMetaInformation(os, io::CREATOR, PACKAGE_STRING)  << endl
      << "  input file:   " 
      << getMetaInformation(os, io::INPUTFILE)                << endl
      << "  invocation:   " 
      << getMetaInformation(os, io::INVOCATION)               << endl
      << "  net size:     " 
      << stat << *this << owfn                                << endl
      << "}" << endl 
      << endl

      << "PLACE"      << endl
      << mode(io::util::PLACE_CAPACITY) << delim("; ")
      << "  INTERNAL" << endl
      << "    " << internalPlaces_ << ";" << endl << endl
      << mode(io::util::PLACE) << delim(", ")
      << "  INPUT"    << endl
      << "    " << inputPlaces_    << ";" << endl << endl
      << "  OUTPUT"   << endl
      << "    " << outputPlaces_   << ";" << endl
      << endl

      << (interfacePlacesByPort_.empty() ? "" : "PORTS") << endl
      << interfacePlacesByPort_
      << endl
  
      << mode(io::util::PLACE_TOKEN)
      << "INITIALMARKING" << endl
      << "  " << io::util::filterMarkedPlaces(internalPlaces_) << ";" << endl 
      << endl

      << "FINALCONDITION" << endl 
      << "  " << condition_ << ";" << endl << endl 
      << endl

      << delim("\n")
      << transitions_ << endl
      << endl 

      << "{ END OF FILE ‘" << getMetaInformation(os, io::OUTPUTFILE) << "’ }"
      << endl;
  }


  /*!
   * Creates a DOT output (see http://www.graphviz.org) of the Petri
   * net. It uses the digraph-statement and adds labels to transitions,
   * places and arcs if neccessary.
   */
  void PetriNet::output_dot(ostream & os) const
  {
    bool interface = true;

    os  //< output everything to this stream

      << delim("\n")

      << "digraph N {" << endl
      << " graph [fontname=\"Helvetica\" nodesep=0.25 ranksep=\"0.25\""
      << " fontsize=10 remincross=true label=\""
      //<< (reduced ? "structurally reduced " : "")
      << "Petri net" 
      //<< " generated from " << filename 
      << "\"]" 
      << endl

      // REMEMBER The table size of the INOUT transitions depends on the size 
      // of a node!
      // So a width of .3 (in) results in 21 pixel table width 
      // ( 0.3 in * 72 dpi ).
      << " node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\""
      << " height=\".3\" label=\"\" style=filled fillcolor=white]" << endl
      << " edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none"
      << " weight=\"20.0\"]" << endl
      << endl

      << " // places" << endl
      << " node [shape=circle]" << endl
      << internalPlaces_ << endl
      << (interface ? interfacePlaces_ : set<Place *>()) << endl
      << endl

      << " // transitions" << endl
      << " node [shape=box]" << endl
      << transitions_ << endl
      << endl

      << mode(io::util::ARC)
      << " // arcs" << endl
      << " edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal"
      << " color=black]" << endl
      << arcs_
      << endl

      << "}" 
      << endl;
  }

      /*
    // the inner of the net
    (*output) << "\n // cluster the inner of the net" << endl;
    (*output) << " subgraph cluster1\n {\n ";
    for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
      //FIXME: (*output) << " t" << (*t)->id << " t" << (*t)->id << "_l";
    (*output) << "\n ";
    for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
    {
      if ((*p)->getTokenCount() > 0)
        ;// FIXME: (*output) << " p" << (*p)->id;
      else
        ;// FIXME:(*output) << " p" << (*p)->id << " p" << (*p)->id << "_l";
    }

    if (draw_interface)
      (*output) << "\n  label=\"\" style=\"dashed\"" << endl;
    else
      (*output) << "\n  label=\"\" style=invis" << endl;
      << " }" << endl;
  }
      */

  /* FIXME: use interfacePlacesByPort_
    // draw the ports
    for (map<string, set<Place *> >::const_iterator port = ports.begin();
         port != ports.end(); port++)
    {
        (*output) << " // cluster for port " << port->first << endl;
        (*output) << " subgraph cluster_" << port->first << "\n {\n";
        (*output) << "  label=\"" << port->first << "\";" << endl;
        (*output) << "  style=\"filled,rounded\"; bgcolor=grey95  labelloc=t;" << endl;
        //(*output) << "  rankdir=TB;" << endl;

        for (set<Place*>::const_iterator place = port->second.begin();
             place != port->second.end(); place++)
        {
            // FIXME:(*output) << "  p" + util::toString((*place)->id) << ";" << endl;
            // FIXME:(*output) << "  p" + util::toString((*place)->id) << "_l;" << endl;

            // make the port more compact
            for (set<Place*>::const_iterator place2 = port->second.begin();
                 place2 != port->second.end(); place2++)
            {
                if ( (*place) != (*place2) )
		  ;// FIXME:(*output) << "  p" + util::toString((*place)->id) + " -> p" + util::toString((*place2)->id) + " [style=invis];" << endl;
            }
        }

        (*output) << " }" << endl << endl;
    }
  */





/******************************************************************************
 * Functions to print information of the net and its nodes
 *****************************************************************************/


/*!
 * \brief   info file output
 *
 *          Prints information about the generated Petri net. In particular,
 *          for each place and transition all roles of the history are printed
 *          to understand the net and maybe LoLA's witness pathes later.
 *
 * \todo    Put this to the nodes.
 */
void PetriNet::output_info(ostream & os) const
{
  ostream * output = &os;

  assert(output != NULL);

  // the places
  (*output) << "PLACES:\nID\tTYPE\t\tROLES\n";

  // the internal places
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    (*output) << (*p)->getName() << "\tinternal";

    deque<string> names = (*p)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << "\t" + (*p)->prefix + *role + "\n";
	(*output) << "\t" + *role + "\n";
      else
        //(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t\t\t" + *role + "\n";
  }

  // the input places
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
  {
    (*output) << (*p)->getName() << "\tinput   ";

    deque<string> names = (*p)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << "\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t" + *role + "\n";
      else
        //(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t\t\t" + *role + "\n";
  }

  // the output places
  for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
  {
    (*output) << (*p)->getName() << "\toutput  ";

    deque<string> names = (*p)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << "\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t" + *role + "\n";
      else
        //(*output) << "\t\t\t" + (*p)->prefix + *role + "\n";
        (*output) << "\t\t\t" + *role + "\n";
  }

  // the transitions
  (*output) << "\nTRANSITIONS:\n";
  (*output) << "ID\tROLES\n";

  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    (*output) << (*t)->getName() + "\t";

    deque<string> names = (*t)->getNameHistory();
    for (deque<string>::iterator role = names.begin(); role != names.end(); role++)
      if (role == names.begin())
        //(*output) << (*t)->prefix + *role + "\n";
        (*output) << *role + "\n";
      else
        //(*output) << "\t" + (*t)->prefix + *role + "\n";
        (*output) << "\t" + *role + "\n";
  }
}



/******************************************************************************
 * Petri net file formats
 *****************************************************************************/

/*!
 * \brief   PNML (Petri Net Markup Language) output
 *
 *          Outputs the net in PNML (Petri Net Markup Language).
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \note    - See https://savannah.gnu.org/task/?6263
 *          - The complete syntax can be found at
 *            http://www.petriweb.org/specs/epnml-1.1/pnmldef.pdf
 */
void PetriNet::output_pnml(ostream & os) const
{
  ostream * output = &os;

  assert(output != NULL);

  (*output) << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl << endl;
  (*output) << "<!--" << endl;
  //(*output) << "  Petri net created by " << getPackageString() << " reading file " << globals::filename << "." << endl;
  (*output) << "  See http://www.gnu.org/software/bpel2owfn for more details." << endl;
  (*output) << "-->" << endl << endl;

  (*output) << "<pnml>" << endl;
  (*output) << "  <net id=\"bpel-net\" type=\"owfn\">" << endl << endl;


  // places
  (*output) << "<!-- input places -->" << endl;
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*p)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "      <type>" << endl;
    (*output) << "        <text>input</text>" << endl;
    (*output) << "      </type>" << endl;
    (*output) << "    </place>" << endl << endl;
  }

  (*output) << "<!-- output places -->" << endl;
  for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*p)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "      <type>" << endl;
    (*output) << "        <text>output</text>" << endl;
    (*output) << "      </type>" << endl;
    (*output) << "    </place>" << endl << endl;
  }

  (*output) << "<!-- internal places -->" << endl;
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <place id=\"" << (*p)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*p)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    if ((*p)->getTokenCount() > 0)
    {
      (*output) << "      <initialMarking>" << endl;
      (*output) << "        <text>" << (*p)->getTokenCount() << "</text>" << endl;
      (*output) << "      </initialMarking>" << endl;
    }
    (*output) << "    </place>" << endl << endl;
  }


  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
#ifdef USING_BPEL2OWFN
    (*output) << "    <transition id=\"" << (*t)->getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "    <transition id=\"" << (*t)->getName() << "\">" << endl;
#endif
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" << (*t)->getName() << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "    </transition>" << endl << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++, arcNumber++)
  {
    (*output) << "    <arc id=\"a" << arcNumber << "\" ";
#ifdef USING_BPEL2OWFN
    (*output) << "source=\"" << (*f)->getSourceNode().getName() << "\" ";
    (*output) << "target=\"" << (*f)->getTargetNode().getName() << "\">" << endl;
#endif
#ifndef USING_BPEL2OWFN
    (*output) << "source=\"" << (*f)->getSourceNode().getName() << "\" ";
    (*output) << "target=\"" << (*f)->getTargetNode().getName() << "\">" << endl;
#endif
    (*output) << "      <inscription>\n        <text>" << (*f)->getWeight() << "</text>\n      </inscription>" << endl;
    (*output) << "    </arc>" << endl;
  }
  (*output) << endl;

  (*output) << "    <toolspecific tool=\"owfn\" version=\"1.0\">" << endl;
  (*output) << "      <finalmarking xmlns=\"http://www.informatik.hu-berlin.de/top/tools4bpel\">" << endl;
  (*output) << "        <text>";

  /* FIXME
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
    if ((*p)->isFinal)
      (*output) << (*p)->getName() << " ";
  */

  (*output) << "</text>" << endl;
  (*output) << "      </finalmarking>" << endl;
  (*output) << "    </toolspecific>" << endl;
  (*output) << "  </net>" << endl;
  (*output) << "</pnml>" << endl;
}





/*!
 * \brief   low-level PEP output
 *
 *          Outputs the net in low-level PEP notation.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo
 *          - Add syntax reference.
 *          - Does PEP also work with non-safe Petri nets?
 */
void PetriNet::output_pep(ostream & os) const
{
  ostream * output = &os;

  assert(output != NULL);

  // header
  (*output) << "PEP" << endl << "PTNet" << endl << "FORMAT_N" << endl;

  // places (only internal)
  (*output) << "PL" << endl;
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << (*p)->id << "\"" << (*p)->getName() << "\"80@40";
    if ((*p)->getTokenCount() > 0)
      (*output) << "M" << (*p)->getTokenCount();
    (*output) << "k1" << endl;
  }

  // transitions
  (*output) << "TR" << endl;
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
    ;// FIXME:(*output) << (*t)->id << "\"" << (*t)->getName() << "\"80@40" << endl;

  // arcs from transitions to places (no output places)
  /* FIXME
  (*output) << "TP" << endl;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
    if (((*f)->getSourceNode()->nodeType) == TRANSITION)
      if ( outputPlaces_.find(static_cast<Place*>((*f)->getTargetNode())) == outputPlaces_.end())
        (*output) << (*f)->getSourceNode()->id << "<" << (*f)->getTargetNode()->id << "w" << (*f)->getWeight() << endl;

  // arcs from places to transitions
  (*output) << "PT" << endl;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
    if (((*f)->getSourceNode()->nodeType) == PLACE)
      if ( inputPlaces_.find(static_cast<Place*>((*f)->getSourceNode())) == inputPlaces_.end())
        (*output) << (*f)->getSourceNode()->id << ">" << (*f)->getTargetNode()->id << "w" << (*f)->getWeight() << endl;
  */
}





/*!
 * \brief   INA output
 *
 *          Outputs the net in INA format.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \node    The complete syntax can be found at
 *          http://www2.informatik.hu-berlin.de/lehrstuehle/automaten/ina/node14.html
 *
 * \todo    Check whether arc weights are in the correct order.
 */
void PetriNet::output_ina(ostream & os) const
{
  ostream * output = &os;

  assert(output != NULL);

  // net header
  //(*output) << "P   M   PRE,POST  NETZ 1:" << globals::filename.substr(0, globals::filename.find_first_of(".")) << endl;

  // places (only internal)
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << setw(3) << (*p)->id << " " << (*p)->getTokenCount() << "     ";

    for (set<Node*>::iterator t = (*p)->getPreset().begin(); t != (*p)->getPreset().end(); t++)
    {
      if (t != (*p)->getPreset().begin())
        (*output) << " ";

      // FIXME:(*output) << (*t)->id << ":" << arc_weight(*t, *p);
    }

    (*output) << ", ";

    for (set<Node*>::iterator t = (*p)->getPostset().begin(); t != (*p)->getPostset().end(); t++)
    {
      if (t != (*p)->getPostset().begin())
        (*output) << " ";

      // FIXME:(*output) << (*t)->id << ":" << arc_weight(*p, *t);
    }

    (*output) << endl;
  }

  (*output) << "@" << endl;


  // place names
  (*output) << "place nr.             name capacity time" << endl;

  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << right << setw(8) << (*p)->id << ": ";
    (*output) << left << setw(16) << (*p)->getName();
    (*output) << "       oo    0" << endl;
  }

  (*output) << "@" << endl;


  // transition names
  (*output) << "trans nr.             name priority time" << endl;

  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    // FIXME:(*output) << right << setw(8) << (*t)->id << ": ";
    (*output) << left << setw(16) << (*t)->getName();
    (*output) << "        0    0" << endl;
  }

  (*output) << "@" << endl;

  // info (ignored after last '@')
  //(*output) << "generated by: " << getPackageString() << endl;
  //(*output) << "input file:   `" << globals::filename << "'" << endl;
  //(*output) << "net size:     " << information() << endl;
}





/*!
 * \brief   SPIN output
 *
 *          Outputs the net in SPIN format.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo    Add syntax reference.
 */
void PetriNet::output_spin(ostream & os) const
{
  ostream * output = &os;

  assert(output != NULL);
  // net header
  (*output) << "/* NO 1-safe */" << endl;

  // places (only internal)
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    // FIXME:(*output) << "byte p" << (*p)->id << "=" << (*p)->getTokenCount() << ";" << endl;
  }

  (*output) << "int {" << endl;
  (*output) << "\tdo" << endl;


  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    int follower=0;
    (*output) << "\t::atomic { (";

    if ((*t)->getPreset().empty())
    {
      (*output) << "0";
    }
    else
    {
      for (set<Node *>::iterator p = (*t)->getPreset().begin(); p != (*t)->getPreset().end(); p++)
      {
        if(follower)
        {
          (*output) << "&&";
        }
        else
        {
          follower=1;
        }
        // FIXME:(*output) << "(p" << (*p)->id << ">=" << arc_weight(*p,*t) << ")";
      }
    }

    (*output) << ") -> ";

    for (set<Node *>::iterator p = (*t)->getPreset().begin(); p != (*t)->getPreset().end(); p++)
    {
      // FIXME:(*output) << "p" << (*p)->id << "=p" << (*p)->id << "-" << arc_weight(*p,*t) << ";";
    }

    (*output) << endl << "\t\t\t";

    for (set<Node *>::iterator p = (*t)->getPostset().begin(); p != (*t)->getPostset().end(); p++)
    {
      // FIXME:(*output) << "p" << (*p)->id << "=p" << (*p)->id << "+" << arc_weight(*t,*p) << ";";
    }

    (*output) << " }" << endl;

  }

  (*output) << "\tod;" << endl;
  (*output) << "}" << endl;
}





/*!
 * \brief   APNN (Abstract Petri Net Notation) output
 *
 *          Outputs the net in APNN (Abstract Petri Net Notation).
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo    Add syntax reference.
*/
void PetriNet::output_apnn(ostream & os) const
{
  ostream * output = &os;

  assert(output != NULL);

  //(*output) << "\\beginnet{" << globals::filename << "}" << endl << endl;

  // places (only internal)
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    (*output) << "  \\place{" << (*p)->getName() << "}{";
    if ((*p)->getTokenCount() > 0)
      (*output) << "\\init{" << (*p)->getTokenCount() << "}";
    (*output) << "}" << endl;
  }
  (*output) << endl;

  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    (*output) << "  \\transition{" << (*t)->getName() << "}{}" << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++, arcNumber++)
  {
    /* FIXME
    // ignore input places
    if ((*f)->getSourceNode()->nodeType == PLACE)
      if ( inputPlaces_.find(static_cast<Place*>((*f)->getSourceNode())) != inputPlaces_.end())
        continue;

    // ignore output places
    if ((*f)->getTargetNode()->nodeType == PLACE)
      if ( outputPlaces_.find(static_cast<Place*>((*f)->getTargetNode())) != outputPlaces_.end())
        continue;
    */

    (*output) << "  \\arc{a" << arcNumber << "}{ ";
    (*output) << "\\from{" << (*f)->getSourceNode().getName() << " } ";
    (*output) << "\\to{" << (*f)->getTargetNode().getName() << "} \\weight{" << (*f)->getWeight() << "} }" << endl;
  }
  (*output) << endl;

  (*output) << "\\endnet" << endl;
}





/*!
 * \brief   LoLA-output
 *
 *          Outputs the net in LoLA-format.
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 *
 * \todo    Add syntax reference.
 */
void PetriNet::output_lola(ostream & os) const
{
  ostream * output = &os;

  assert(output != NULL);

  //(*output) << "{ Petri net created by " << getPackageString() << " reading " << globals::filename << " }" << endl << endl;

  {
    /************************
     * STANDARD LOLA FORMAT *
     ************************/

    // places (only internal)
    (*output) << "PLACE" << endl;
    unsigned int count = 1;
    for (set<Place *>::iterator p = places_.begin(); p != places_.end(); count++, p++)
    {
      (*output) << "  " << (*p)->getName();

      if (count < places_.size())
        (*output) << "," << endl;
    }
    (*output) << endl << ";" << endl << endl << endl;
  }


  // from here on, both standard LoLA and I/O-annotated LoLA formats are equal


  // initial marking
  (*output) << "MARKING" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
  {
    if ((*p)->getTokenCount() > 0)
    {
      if (count++ != 1)
        (*output) << "," << endl;

      (*output) << "  " << (*p)->getName() << ":\t" << (*p)->getTokenCount();
    }
  }
  (*output) << endl << ";" << endl << endl << endl;


  // transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    (*output) << "TRANSITION " << (*t)->getName() << endl;

    (*output) << "CONSUME" << endl;
    count = 1;
    for (set<Node *>::iterator pre = (*t)->getPreset().begin(); pre != (*t)->getPreset().end(); count++, pre++)
    {
      // ignore input places
      // FIXME: if ( (*pre)->nodeType == PLACE )
        if ( inputPlaces_.find(static_cast<Place*>(*pre)) != inputPlaces_.end())
          continue;

	(*output) << "  " << (*pre)->getName() << ":\t" << findArc(**pre, **t)->getWeight();

      if (count < (*t)->getPreset().size())
        (*output) << "," << endl;
    }
    (*output) << ";" << endl;

    (*output) << "PRODUCE" << endl;
    count = 1;
    for (set<Node *>::iterator post = (*t)->getPostset().begin(); post != (*t)->getPostset().end(); count++, post++)
    {
      // ignore output places
      //FIXME: if ( (*post)->nodeType == PLACE )
        if ( outputPlaces_.find(static_cast<Place*>(*post)) != outputPlaces_.end())
          continue;

	(*output) << "  " << (*post)->getName() << ":\t" << findArc(**t, **post)->getWeight();

      if (count < (*t)->getPostset().size())
        (*output) << "," << endl;
    }

    (*output) << ";" << endl << endl;
  }
  (*output) << "{ END OF FILE }" << endl;
}

}
