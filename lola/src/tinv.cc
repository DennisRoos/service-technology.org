/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include "tinv.H"
#ifdef DISTRIBUTE
#include "distribute.h"
#endif

#if defined(PREDUCTION) || defined(CYCLE) || defined(STRUCT) || defined(SWEEP)
unsigned int NrOfEquations;

inline long int ggt(long int a, long int b) {
    while (true) {
        a %= b;
        if (!a) {
            return b;
        }
        b %= a;
        if (!b) {
            return a;
        }
    }
}

void printeq(equation* e) {
    for (summand* s = e->sum; s ; s = s->next) {
        cout << s->value << "*" << (s->var ? s->var->name : "nix") << " ";
    }
}

//! \todo initialize next with NULL?
summand::summand(Node* t, long int k) : var(t), value(k) {}

equation::equation(Node* p) : sum(NULL) {
    summand** anchor;

    // traverse incoming arcs (preset of p)
    for (unsigned int i = 0; i < p->NrOfArriving; ++i) {
        Node* t = p->ArrivingArcs[i]->Source;
        long int k = p->ArrivingArcs[i]->Multiplicity;
        summand* s = new summand(t, k);

        // find position to add new summand to list
        for (anchor = &sum; *anchor; anchor = &((*anchor)->next)) {
            if ((*anchor)->var->nr > t->nr) {
                break;
            }
        }
        s->next = *anchor;
        *anchor = s;
    }

    // traverse leaving arcs (postset of p)
    for (unsigned int i = 0; i < p->NrOfLeaving; ++i) {
        Node* t = p->LeavingArcs[i]->Destination;
        long int k = p->LeavingArcs[i]->Multiplicity;

        for (anchor = &sum; *anchor; anchor = &((*anchor)->next)) {
            if ((*anchor)->var->nr >= t->nr) {
                break;
            }
        }
        if ((*anchor) && (*anchor)->var == t) {
            (*anchor)->value -= k;
            if ((*anchor)->value == 0) {
                summand* tmp;
                tmp = *anchor;
                *anchor = tmp->next;
                delete tmp;
            }
        } else {
            summand* s = new summand(t, -k);
            s->next = *anchor;
            *anchor = s;
        }
    }

    if (!sum) {
        return;
    }

#ifndef SWEEP
    long int k = sum->value;
    for (summand* s = sum->next; s; s = s->next) {
        k = ggt(k, s->value);
    }
    if (k != 1) {
        for (summand* s = sum; s; s = s->next) {
            s->value /= k;
        }
    }
#endif

    ++NrOfEquations;
     
}

equation** esystem, **newesystem;
unsigned int* lastconsidered;

void equation::apply() {
    // use this to eliminate smallest var in next
    // this and next expected to have same smallest var

#ifdef DISTRIBUTE
    progress();
#endif
    long int k = ggt(sum->value, next->sum->value);
    long int fthis = next->sum->value / k;
    long int fnext = sum->value / k;

    summand* s = next->sum->next;
    delete next->sum;
    next->sum = s;
    s = sum->next;

    summand** anchor = &(next->sum);
    while (s && *anchor) {
        if (s->var->nr < (*anchor)->var->nr) {
            summand* ss = new summand(s->var, -fthis * s->value);
            ss->next = *anchor;
            *anchor = ss;
            anchor = &(ss->next);
            s = s->next;
        } else {
            if (s->var->nr > (*anchor)->var->nr) {
                (*anchor)->value *= fnext;
                anchor = &((*anchor)->next);
            } else {
                (*anchor)->value *= fnext;
                (*anchor)->value -= fthis * s->value;
                if (!((*anchor)->value)) {
                    summand* ss = *anchor;
                    *anchor = ss->next;
                    delete ss;
                } else {
                    anchor = &((*anchor)->next);
                }
                s = s->next;
            }
        }
    }
    while (s) {
        *anchor = new summand(s->var, - fthis * s->value);
        anchor = & ((*anchor)->next);
        *anchor = NULL;
        s = s->next;
    }
    while (*anchor) {
        (*anchor)->value *= fnext;
        anchor = &((*anchor)->next);
    }
    if (!(next->sum)) {
        equation* tmp = next;
        next = next->next;
        delete tmp;
        return;
    }
    k = next->sum->value;
    for (s = next->sum->next; s; s = s->next) {
        k = ggt(k, s->value);
    }
    for (s = next->sum; s; s = s->next) {
        s->value /= k;
    }
    equation* tmp = next;
    next = next->next;
#ifdef SWEEP
    if (tmp->sum->var) {
#endif
        tmp->next = esystem[tmp->sum->var->nr];
        esystem[tmp->sum->var->nr] = tmp;
#ifdef SWEEP
    } else {
        tmp->next = esystem[Places[0]->cnt];
        esystem[Places[0]->cnt] = tmp;
    }
#endif
}

#if defined(CYCLE) || defined(STRUCT)
void tsolve() {
    int i;
    unsigned int round;
    equation* e;
    summand** anchor;

    // step 0: all no-change transitions must be set to "cyclic"
    for (i = 0; i < Transitions[0]->cnt; i++) {
        if (!(Transitions[i]->IncrPlaces[0]) && !(Transitions[i]->DecrPlaces[0])) {
            Transitions[i]->cyclic = true;
        }
    }

    round = 1;
    esystem = new equation * [ Transitions[0]->cnt];
    newesystem = new equation * [ Transitions[0]->cnt];
    lastconsidered = new unsigned int [ Transitions[0]->cnt];
    NrOfEquations = 0;

    // load equations into esystem
    for (i = 0; i < Transitions[0]->cnt; i++) {
        esystem[i] = newesystem[i] = NULL;
        lastconsidered[i] = 0;
    }

    for (i = 0; i < Places[0]->cnt; i++) {
        e = new equation(Places[i]);
        if (!e->sum) {
            delete e;
            continue;
        }
        e->next = esystem[e->sum->var->nr];
        esystem[e->sum->var->nr] = e;
        NrOfEquations ++;

    }
    if (NrOfEquations == 0) {
        delete [] esystem;
        delete [] newesystem;
        delete [] lastconsidered;
        return;
    }

    while (1) {
        // Matrix->obere Dreiecksform

        for (i = 0; i < Transitions[0]->cnt; i++) {
            while (esystem[i] && esystem[i]->next) {
                esystem[i]->apply();
            }
        }

        // extract variables and re-organize esystem
        bool newcyclic;
        newcyclic = false;
        for (i = Transitions[i]->cnt - 1; i >= 0; i--) {
            if (!esystem[i]) {
                continue;
            }
            // consider equation with largest head variable first
            // look for unconsidered parameter variables and remove
            // already selected cycle transitions
            anchor = & (esystem[i]->sum->next);
            while (*anchor) {
                if (((Transition*)((*anchor)->var))->cyclic) {
                    // remove
                    summand* tmp;
                    tmp = * anchor;
                    * anchor = tmp->next;
                    delete tmp;
                } else {
                    if (lastconsidered[(* anchor)->var->nr] < round) {
                        // unconsidered parameter->select head var
                        newcyclic = ((Transition*)(esystem[i]->sum->var))->cyclic = true;
                        lastconsidered[(* anchor)->var->nr] = round;
                    }
                    anchor = & ((*anchor)->next);
                }
            }
            lastconsidered[esystem[i]->sum->var->nr] = round;
            if (((Transition*)(esystem[i]->sum->var))->cyclic) {
                unsigned int pos;
                summand* tmp;
                tmp = esystem[i]->sum;
                esystem[i]->sum = tmp->next;
                pos = esystem[i]->sum->var->nr;
                esystem[i]->next = newesystem[pos];
                newesystem[pos] = esystem[i];
                esystem[i] = NULL;
                delete tmp;
            } else {
                newesystem[i] = esystem[i];
                esystem[i] = NULL;
            }
        }
        if (!newcyclic) {
            for (i = 0; i < Transitions[i]->cnt; i++) {
                while (newesystem[i]) {
                    equation* e;
                    while (newesystem[i]->sum) {
                        summand* tmp;
                        tmp = newesystem[i]->sum;
                        newesystem[i]->sum = newesystem[i]->sum->next;
                        delete tmp;
                    }
                    e = newesystem[i];
                    newesystem[i] = newesystem[i]->next;
                    delete e;
                }
            }
            delete [] esystem;
            delete [] newesystem;
            delete [] lastconsidered;
            return;
        } else {
            equation** tmp;
            tmp = esystem;
            esystem = newesystem;
            newesystem = tmp;
            round ++;
        }

    }
}
#endif

#ifdef PREDUCTION
void psolve() {
    esystem = new equation*[Places[0]->cnt];
    NrOfEquations = 0;

    // load equations into esystem
    for (int i = 0; i < Places[0]->cnt; ++i) {
        esystem[i] = NULL;
        Places[i]->nr = i;
    }

    for (int i = 0; i < Transitions[0]->cnt; ++i) {
        equation* e = new equation(Transitions[i]);
        if (!e->sum) {
            delete e;
            continue;
        }
        e->next = esystem[e->sum->var->nr];
        esystem[e->sum->var->nr] = e;
        ++NrOfEquations;
    }

    if (NrOfEquations == 0) {
        delete[] esystem;
        return;
    }

    // Matrix->obere Dreiecksform
    for (int i = 0; i < Places[0]->cnt; ++i) {
        while (esystem[i] && esystem[i]->next) {
            esystem[i]->apply();
        }
    }

    // set head variables as significant and remove equations
    for (int i = 0; i < Places[0]->cnt; ++i) {
        if (esystem[i]) {
            Places[i]->significant = true;
            while (esystem[i]->sum) {
                summand* s = esystem[i]->sum;
                esystem[i]->sum = esystem[i]->sum->next;
                delete s;
            }
            delete esystem[i];
        }
    }
    delete[] esystem;
}
#endif

#ifdef SWEEP
int ProgressSpan;
int ZeroProgress;
int MonotoneProgress;

void progress_measure() {
    int i;
    summand* s;
    summand** anchor;
    equation** reference;
    long int* value;                // progress values as rational numbers
    long int* denominator;
    long int g;

    esystem = new equation * [ Places[0]->cnt + Transitions[0]->cnt]; // entry = list of all equations where p is smallest summand != 0
    reference = new equation * [Transitions[0]->cnt]; // entry = the equation belonging to t
    value = new long int [Transitions[0]->cnt];
    denominator = new long int [Transitions[0]->cnt];
    NrOfEquations = 0;

    // load equations into esystem
    for (i = 0; i < Places[0]->cnt ; i++) {
        esystem[i] = NULL;
        Places[i]->nr = i;
    }
    esystem[Places[0]->cnt] = NULL;

    for (i = 0; i < Transitions[0]->cnt; i++) {
        Transitions[i]->nr = Places[0]->cnt + i;  // all transitions get same number; this way all
        // equations that cancel out all places are collected
        // in bucket esystem[Places[0]->cnt]].
        reference[i] = new equation(Transitions[i]);
        for (anchor = &(reference[i]->sum); *anchor; anchor = &((*anchor)->next));
        *anchor = new summand(Transitions[i], 1);
        (*anchor)->next = NULL;
        reference[i]->next = esystem[reference[i]->sum->var->nr];
        esystem[reference[i]->sum->var->nr] = reference[i];
        NrOfEquations ++;

    }
    if (NrOfEquations == 0) {
        delete [] esystem;
        return;
    }
    // Matrix->obere Dreiecksform

    for (i = 0; i < Places[0]->cnt; i++) {
        while (esystem[i] && esystem[i]->next) {
            esystem[i]->apply();
        }
    }
    for (i = 0; i < Transitions[0]->cnt; i++) {
        if (reference[i]->sum->var->nr < Places[0]->cnt) {
            // among the linear independent transitions->progress value 1
            value[i] = 1;
            denominator[i] = 1;
        } else {
            // linear dependent
            value[i] = 0;
            for (s = reference[i]->sum; s; s = s->next) {
                if (((Transition*) s->var) == Transitions[i]) {
                    denominator[i] = s->value;
                } else {
                    value[i] -= s->value;
                }
            }
            if (value[i] == 0) {
                denominator[i] = 1;
            } else {
                g = ggt(value[i], denominator[i]);
                value[i] /= g;
                denominator[i] /= g;
                if (denominator[i] < 0) {
                    denominator[i] *= -1;
                    value[i] *= -1;
                }
            }


        }
    }
    for (i = 0; i < Places[0]->cnt + Transitions[0]->cnt; i++) {
        if (esystem[i]) {
            while (esystem[i]->sum) {
                s = esystem[i]->sum;
                esystem[i]->sum = esystem[i]->sum->next;
                delete s;
            }
            delete esystem[i];
        }
    }
    delete [] esystem;
    // multiply all values with gcd
    long int gcd = 1;
    for (i = 0; i < Transitions[i]->cnt; i++) {
        g = ggt(gcd, denominator[i]);
        gcd = (gcd / g);
        gcd = gcd * denominator[i];     // computes lcm
    }
    int MinProgress, MaxProgress;
    ZeroProgress = 0;
    if (Transitions[0]) {
        MinProgress = MaxProgress = value[0] * (gcd / denominator[0]);
    }
    for (i = 0; i < Transitions[i]->cnt; i++) {
        long int p;
        p = Transitions[i]->progress_value =  value[i] * (gcd / denominator[i]);
        cout << Transitions[i]->name << " : " << Transitions[i]->progress_value << endl;
        if (!p) {
            ZeroProgress = 1;
        }
        if (p < MinProgress) {
            MinProgress = p;
        }
        if (p > MaxProgress) {
            MaxProgress = p;
        }
    }
    ProgressSpan = MaxProgress - MinProgress + 10;
    if (MinProgress < 0) {
        MonotoneProgress = 0;
    } else {
        MonotoneProgress = 1;
    }
}
#endif

#endif
