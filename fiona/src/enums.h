// *****************************************************************************\
// * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg      *
// *                                                                           *
// * This file is part of Fiona.                                               *
// *                                                                           *
// * Fiona is free software; you can redistribute it and/or modify it          *
// * under the terms of the GNU General Public License as published by the     *
// * Free Software Foundation; either version 2 of the License, or (at your    *
// * option) any later version.                                                *
// *                                                                           *
// * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
// * more details.                                                             *
// *                                                                           *
// * You should have received a copy of the GNU General Public License along   *
// * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
// * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
// *****************************************************************************/

/*!
 * \file	enums.h
 *
 * \brief	all enums
 * 
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 */

#ifndef ENUMS_H_
#define ENUMS_H_

#include "mynew.h"

enum vertexColor {BLACK, RED, BLUE};		//!< BLACK == vertex has not been checked; RED == bad vertex; BLUE == good one
enum edgeType {sending, receiving}; 		//!< ENUM possible types of an edge
enum analysisResult {TERMINATE, CONTINUE};  //!< needed as feedback of the "analysis" function, whether this node is an end node or not



#endif /*ENUMS_H_*/
