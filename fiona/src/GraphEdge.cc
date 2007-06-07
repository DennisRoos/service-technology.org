/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
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
 * \file    graphEdge.cc
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

#include "mynew.h"
#include "graphEdge.h"
#include <string>
#include "vertex.h"
#include "enums.h"

GraphEdge::GraphEdge(vertex* dstNodeP, const string& labelP) :
    dstNode(dstNodeP),
    label(labelP),
    nextElement(NULL) {
}

void GraphEdge::setNextElement(GraphEdge* newNextElement) {
    nextElement = newNextElement;
}

GraphEdge* GraphEdge::getNextElement() const {
    return nextElement;
}

void GraphEdge::setDstNode(vertex* newDstNode) {
    dstNode = newDstNode;
}

vertex * GraphEdge::getDstNode() const {
    return dstNode;
}

string GraphEdge::getLabel() const {
    return label;
}

GraphEdgeType GraphEdge::getType() const {
    assert(label.size() != 0);
    switch (label[0]) {
        case '?': return RECEIVING;
        case '!': return SENDING;
        default : assert(false); // This should never happen.
    }
}

