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
 * \file    lexer_owfn_wrap.h
 *
 * \brief   compatibility wrapper for the lexer
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "lexer_owfn.h"

// #defines YY_FLEX_HAS_YYLEX_DESTROY for newer versions of flex. If
// YY_FLEX_HAS_YYLEX_DESTROY is defined, you should call yylex_destroy() after
// parsing is complete.

#if (YY_FLEX_MAJOR_VERSION > 2) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION > 5)) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION == 5) && \
     (YY_FLEX_SUBMINOR_VERSION >= 31))
#define YY_FLEX_HAS_YYLEX_DESTROY
#endif
