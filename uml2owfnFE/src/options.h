/*****************************************************************************\
  UML2OWFN -- Translating Websphere Business Object Models to oWFNs

  Copyright (C) 2007, 2008  Martin Znamirowski,
                            Dirk Fahland
\*****************************************************************************/

/*!
 * \file    options.h
 *
 * \brief   evaluation of command-line options
 *
 * \author  Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2008/05/07
 *
 * \date    \$Date: 2008/05/09 12:38:16 $
 *
 * \note    This file is part of the tool UML2OWFN and was created at the
 *          Humboldt-Universitšt zu Berlin. See
 *
 * \version \$Revision: 1.00 $
 */





#ifndef OPTIONS_H
#define OPTIONS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <map>
#include <set>
#include <list>
#include <string>
#include <iostream>
#include <cstdlib>
#include <climits>

using std::map;
using std::set;
using std::list;
using std::istream;
using std::ostream;
using std::string;





/******************************************************************************
 * Data structures
 *****************************************************************************/

/*!
 * \brief enumeration of the possible options
 */
typedef enum
{
   O_HELP,		///< show help screen
   O_VERSION,		///< show version information
   O_INPUT,		///< read input file
   O_OUTPUT,		///< write output file
   O_FORMAT,		///< choose output file format (see #possibleFormats)
   O_PARAMETER,		///< set various parameters (see #possibleParameters)
   O_DEBUG,		///< set the debug level
   O_REDUCTION,		///< set level of structural reduction
   O_CHECKINTEST	///< guess
} possibleOptions;


/*!
 * \brief enumeration of the possible output file formats
 */
typedef enum
{
  F_LOLA,		///< LoLA Petri net
  F_OWFN,		///< Fiona open workflow net
  F_DOT,		///< Graphviz dot format
  F_PEP,		///< low-level PEP notation
  F_APNN,		///< Abstract Petri Net Notation
  F_INA,		///< INA Petri net
  F_SPIN,		///< SPIN Petri net
  F_INFO,		///< BPEL2oWFN information file
  F_PNML		///< Petri Net Markup Language
} possibleFormats;





/*!
 * \brief enumeration of the possible parameters
 */
typedef enum
{
// <Dirk.F start>
  P_SOUNDNESS		///< transform the net s.t. it can be checked for soundness
// <Dirk.F end>
} possibleParameters;





/******************************************************************************
 * External functions
 *****************************************************************************/

// from options.cc

extern void closeOutput(ostream *file);
extern void parse_command_line(int argc, char* argv[]);
extern ostream *openOutput(string name);





/******************************************************************************
 * External variables
 *****************************************************************************/

// from options.cc

extern bool createOutputFile;
extern set<string> inputfiles;
extern string log_filename;
extern istream *input;
extern ostream *output;
extern ostream *log_output;
extern map<possibleOptions,    bool> options;
extern map<possibleParameters, bool> parameters;
extern map<possibleFormats,    bool> formats;
extern map<possibleFormats,  string> suffixes;

extern int frontend_debug;
extern int frontend__flex_debug;
extern FILE *frontend_in;





#endif
