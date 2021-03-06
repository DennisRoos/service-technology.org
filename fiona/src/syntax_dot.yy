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

%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <stdlib.h>
#include <stdio.h>
#include <stack>
#include "dot2tex.h"
#include "GastexGraph.h"
int dot_yyerror(char const *s);
int dot_yylex(void);

using namespace std;

GasTexGraph* gastexGraph;

GasTexNode dummyNode;			// for temporary storage of node values
GasTexEdge dummyEdge(NULL, NULL);	// for temporary storage of edge values

GasTexNode defaultNode;                 // for setting default values in a node statement
GasTexEdge defaultEdge;                 // for setting default values in an edge statement

stack<GasTexNode> defaultNodeStack;     // for subgraph statements 
stack<GasTexEdge> defaultEdgeStack;     // for subgraph statements 

bool firstNode = true;

void parseAttrStr(string lhs, string rhs) {
    if (lhs == "label") {
        dummyNode.label = rhs.substr(1, rhs.length() - 2);
        dummyEdge.label = rhs.substr(1, rhs.length() - 2);
    } else if (lhs == "pos") {
        dummyNode.posX = 
        atoi(rhs.substr(1, rhs.find_first_of(",")).c_str());
        dummyNode.posY = 
        atoi(rhs.substr(rhs.find_first_of(",") + 1, rhs.length()).c_str());
    } else if (lhs == "width") {
        dummyNode.width = atof(rhs.substr(1, rhs.length() - 2).c_str());
    } else if (lhs == "height") {
        dummyNode.height = atof(rhs.substr(1, rhs.length() - 2).c_str());
    } else if (lhs == "peripheries") {
        if (rhs == "2") {
            dummyNode.peripheries = 2;
        }
    } else if (lhs == "color") {
        dummyNode.color = rhs;
        dummyEdge.color = rhs;
    } else if (lhs == "fillcolor") {
        dummyNode.fillcolor = rhs;
    } else if (lhs == "fontcolor") {
        dummyNode.fontcolor = rhs;
        dummyEdge.fontcolor = rhs;
    } else if (lhs == "fontname") {
        dummyNode.fontname = rhs;
        dummyEdge.fontname = rhs;
    } else if (lhs == "fontsize") {
        dummyNode.fontsize = atoi(rhs.c_str());
        dummyEdge.fontsize = atoi(rhs.c_str());
    } else if (lhs == "shape") {
        dummyNode.shape = rhs;
    } else if (lhs == "style") {
        dummyNode.style = rhs;
        dummyEdge.style = rhs;
    } else if (lhs == "labelfontcolor") {
        dummyEdge.labelfontcolor = rhs;
    } else if (lhs == "labelfontname") {
        dummyEdge.labelfontname = rhs;
    } else if (lhs == "labelfontsize") {
        dummyEdge.labelfontsize = atoi(rhs.c_str());
    } else if (lhs == "headlabel") {
        dummyEdge.headlabel = rhs;
    } else if (lhs == "headport") {
        dummyEdge.headport = rhs;
    } else if (lhs == "taillabel") {
        dummyEdge.taillabel = rhs;
    } else if (lhs == "tailport") {
        dummyEdge.tailport = rhs;
    } else if (lhs == "bb") {
        BoundingBox* bb = new BoundingBox;

        rhs = rhs.substr(1, rhs.length()-1);
        bb->x = atoi(rhs.substr(0, rhs.find_first_of(",")).c_str());
        rhs = rhs.substr(rhs.find_first_of(",")+1, rhs.length()-1); 
        bb->y = atoi(rhs.substr(0, rhs.find_first_of(",")).c_str());
        rhs = rhs.substr(rhs.find_first_of(",")+1, rhs.length()-1); 
        bb->width = atoi(rhs.substr(0, rhs.find_first_of(",")).c_str());
        rhs = rhs.substr(rhs.find_first_of(",")+1, rhs.length()-1); 
        bb->height = atoi(rhs.substr(0, rhs.length()-1).c_str());

        gastexGraph->boundingBoxes.insert(bb);
    }
} 

%}

// Bison options
%name-prefix="dot_yy"

%union{
    int     	int_val;
    string*	str_val;
    struct {
        int cNode;
        int cEdge;
    } meta;
}

%start	graph 

%type <str_val>    graph 
%type <meta>      stmt_list
%type <meta>      stmt
%type <meta>      attr_stmt
%type <str_val>    attr_list
%type <str_val>    a_list
%type <str_val>    node_stmt
%type <str_val>    node_id
%type <str_val>    port 
%type <str_val>    compass_pt 
%type <str_val>    edge_stmt
%type <str_val>    edgeRHS

%left KW_DIGRAPH
%left KW_GRAPH
%left KW_SUBGRAPH
%left KW_NODE
%left KW_EDGE
%left  <str_val> KW_ID
%token <str_val> KW_STRING
%token <int_val> KW_INT
%left KW_LBRACE
%left KW_RBRACE
%left KW_LBRACKET
%left KW_RBRACKET
%left KW_LPARAN
%left KW_RPARAN
%left KW_COMMA
%left KW_SEMICOLON
%left KW_COLON
%left KW_EQUAL
%left KW_AT
%left KW_EDGEOP
%left KW_STRICT
%left KW_N
%left KW_NE
%left KW_E
%left KW_SE
%left KW_S
%left KW_SW
%left KW_W
%left KW_NW


%%

graph:  /* empty */ 
        KW_GRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_GRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_STRICT KW_GRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_STRICT KW_GRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_DIGRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_DIGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_STRICT KW_DIGRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_STRICT KW_DIGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
;

stmt_list: /* empty */
        stmt
            {
                $$.cNode = $1.cNode;
                $$.cEdge = $1.cEdge;
            }
        | stmt KW_SEMICOLON
            { 
                $$.cNode = $1.cNode;
                $$.cEdge = $1.cEdge;
            }
        | stmt stmt_list 
            {
                $$.cNode = $1.cNode + $2.cNode;
                $$.cEdge = $1.cEdge + $2.cEdge;
            }
        | stmt KW_SEMICOLON stmt_list 
            { 
                $$.cNode = $1.cNode + $3.cNode;
                $$.cEdge = $1.cEdge + $3.cEdge;
            }
;

stmt:   /* empty */
        attr_stmt
            {
                $$.cNode = $1.cNode;
                $$.cEdge = $1.cEdge;
            }
        | node_stmt
            {
                $$.cNode = 0;
                $$.cEdge = 0;
            } 
        | edge_stmt
            { 
                $$.cNode = 0;
                $$.cEdge = 0;
            }
        | KW_ID KW_EQUAL KW_ID
            { 
                $$.cNode = 0;
                $$.cEdge = 0;
            }
        | subgraph 
            {
                $$.cNode = 0;
                $$.cEdge = 0;
            }
;

subgraph: /* empty */
        KW_LBRACE stmt_list KW_RBRACE
            {
                for (int i=0; i<$2.cNode; ++i) {
                    defaultNodeStack.pop();
                }
                for (int i=0; i<$2.cEdge; ++i) {
                    defaultEdgeStack.pop();
                }
            }
        | KW_SUBGRAPH KW_LBRACE stmt_list KW_RBRACE
            {
                for (int i=0; i<$3.cNode; ++i) {
                    defaultNodeStack.pop();
                }
                for (int i=0; i<$3.cEdge; ++i) {
                    defaultEdgeStack.pop();
                }
            }
        | KW_SUBGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
                for (int i=0; i<$4.cNode; ++i) {
                    defaultNodeStack.pop();
                }
                for (int i=0; i<$4.cEdge; ++i) {
                    defaultEdgeStack.pop();
                }
            }
;

attr_stmt: /* empty */ 
        KW_GRAPH attr_list 
            {
                if (!defaultNodeStack.empty()) 
                    dummyNode = defaultNodeStack.top();
                else
                    dummyNode.setToDefault(); 
                if (!defaultEdgeStack.empty()) 
                    dummyEdge = defaultEdgeStack.top();
                else
                    dummyEdge.setToDefault(); 
                ($$).cNode = 0;
                ($$).cEdge = 0;
            }
        | KW_NODE attr_list
            {
                defaultNode = dummyNode;
                defaultNodeStack.push(defaultNode);
                if (!defaultEdgeStack.empty()) 
                    dummyEdge = defaultEdgeStack.top();
                else
                    dummyEdge.setToDefault(); 
                ($$).cNode = 1;
                ($$).cEdge = 0;
            }
        | KW_EDGE attr_list
            { 
                if (!defaultNodeStack.empty()) 
                    dummyNode = defaultNodeStack.top();
                else
                    dummyNode.setToDefault(); 
                defaultEdge = dummyEdge; 
                defaultEdgeStack.push(defaultEdge);
                ($$).cNode = 0;
                ($$).cEdge = 1;
            }
;

attr_list: /* empty */
        KW_LBRACKET KW_RBRACKET
            {
            }
        | KW_LBRACKET a_list KW_RBRACKET
            { 
            }
        | KW_LBRACKET KW_RBRACKET attr_list
            {
            } 
        | KW_LBRACKET a_list KW_RBRACKET attr_list
            { 
            }
;

a_list: /* empty */ 
        KW_ID
            {
            }
        | KW_ID KW_EQUAL KW_ID
            {
                parseAttrStr((*$1), (*$3));
            }
        | KW_ID KW_COMMA 
            {
            }
        | KW_ID a_list
            {
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA
            {
                parseAttrStr((*$1), (*$3));
            }
        | KW_ID KW_EQUAL KW_ID a_list 
            {
                parseAttrStr((*$1), (*$3));
            }
        | KW_ID KW_COMMA a_list
            {
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA a_list
            {
                parseAttrStr((*$1), (*$3));
            }
;

node_stmt: /* empty */
        node_id
            {
                GasTexNode* node = new GasTexNode(dummyNode);

                if (gastexGraph->style == GasTexGraph::STYLE_OG) {
                    if (firstNode) {
                        node->isInitial = true;
                        firstNode = false;
                    }
                }

                gastexGraph->addNode(node);

                if (!defaultNodeStack.empty()) 
                    dummyNode = defaultNodeStack.top();
                else
                    dummyNode.setToDefault(); 
            }
        | node_id attr_list        
            {
                GasTexNode* node = new GasTexNode(dummyNode);

                if (gastexGraph->style == GasTexGraph::STYLE_OG) {
                    if (firstNode) {
                        node->isInitial = true;
                        firstNode = false;
                    }

                    if (node->peripheries == 2) {
                        node->isFinal = true;
                    }

                } else if (gastexGraph->style == GasTexGraph::STYLE_OWFN) {

                    if (node->fillcolor == "black") {
                        node->isInitial = true;
                   }

                    if (node->fillcolor == "gray") {
                        node->isFinal = true;
                    }
                }

                gastexGraph->addNode(node);

                if (!defaultNodeStack.empty()) 
                    dummyNode = defaultNodeStack.top(); 
                else
                    dummyNode.setToDefault(); 
            }
;

node_id: /* empty */
        KW_ID 
            {
                dummyNode.id = (*$1);
            }
        | KW_ID port 
            {
                dummyNode.id = (*$1);
            }
;

port: /* empty */
        KW_COLON KW_ID
            {
            }
        | KW_COLON KW_ID KW_COLON compass_pt
            {
            }
        | KW_COLON compass_pt
            {
            }
;

compass_pt: /* empty */
        KW_N {}
        | KW_NE {}
        | KW_E {}
        | KW_SE {}
        | KW_S {}
        | KW_SW {}
        | KW_W {}
        | KW_NW {}
;

edge_stmt: /* empty */
        node_id edgeRHS
            {
                GasTexEdge* edge = new GasTexEdge;
               
                (*edge) = dummyEdge;
                edge->srcNode = gastexGraph->getNode(*$1);
                edge->destNode = gastexGraph->getNode(*$2);

                gastexGraph->addEdge(edge);

                if (!defaultEdgeStack.empty()) 
                    dummyEdge = defaultEdgeStack.top();
                else
                    dummyEdge.setToDefault(); 
            }
        | node_id edgeRHS attr_list
            {
                GasTexEdge* edge = new GasTexEdge;

                (*edge) = dummyEdge;
                edge->srcNode = gastexGraph->getNode(*$1);
                edge->destNode = gastexGraph->getNode(*$2);

                gastexGraph->addEdge(edge);

                if (!defaultEdgeStack.empty()) 
                    dummyEdge = defaultEdgeStack.top(); 
                else
                    dummyEdge.setToDefault(); 
            }
        | subgraph edgeRHS
            {
            }
        | subgraph edgeRHS attr_list
            {
            }
;

edgeRHS: /* empty */
        KW_EDGEOP node_id
            {
                (*$$) = (*$2);
            }
        | KW_EDGEOP node_id edgeRHS
            {
                (*$$) = (*$2);
            }
        | KW_EDGEOP subgraph
            {
            }
        | KW_EDGEOP subgraph edgeRHS
            {
            }
;

%%

int dot_yyerror(string s) {
  extern int dot_yylineno;	// defined and maintained in lex.c
  extern char *dot_yytext;	// defined and maintained in lex.c
  
  cerr << "ERROR: " << s << " at symbol \"" << dot_yytext;
  cerr << "\" on line " << dot_yylineno << endl;
  setExitCode(EC_PARSE_ERROR);
}

int yyerror(char const *s) {
  return yyerror(string(s));
}

