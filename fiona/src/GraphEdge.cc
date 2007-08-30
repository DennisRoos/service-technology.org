/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider                                               *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    GraphEdge.cc
 *
 * \brief   functions for traversing IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include <string>
#include <stdexcept>
#include <cassert>
#include "mynew.h"
#include "GraphEdge.h"
#include "GraphNode.h"

using namespace std;

GraphEdge::GraphEdge(GraphNode* dstNodeP, const std::string& labelP) :
    dstNode(dstNodeP), label(labelP) {
}


std::string GraphEdge::getLabel() const {
    return label;
}


GraphEdgeType GraphEdge::getType() const {
    assert(label.size() != 0);
    switch (label[0]) {
        case '?':
            return RECEIVING;
        case '!':
            return SENDING;
        default:
            // This should never happen.
            assert(false);
            throw new std::invalid_argument("Cannot determine type of this GraphEdge with label '" + label + "'.");
    }
}


GraphNode* GraphEdge::getDstNode() const {
    return dstNode;
}
