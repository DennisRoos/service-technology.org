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


#include "net.H"
#include "graph.H"
#include "formula.H"
#include <climits>
#ifdef STUBBORN

void insert_up(State*, formula*);
void insert_global_down(formula*);

extern unsigned int* checkstart;
extern Transition* LastAttractor;  ///< Last transition in list of


void stubbornclosure() {
    Transition* current;

    current->NrStubborn = 0;
    for (current = current -> StartOfStubbornList; current; current = current -> NextStubborn) {
#ifdef EXTENDEDCTL
        if (!(current -> pathrestriction[TemporalIndex])) {
            continue;
        }
#endif
        if (current -> enabled) {
            current -> NrStubborn ++;
#if defined(RELAXED)
            // condition SPP1 from Kristensen/Valmari (Petri nets 2000)
            if (current -> down) {
                insert_up(NULL, F);
            }
#endif
        }
        for (int i = 0; current -> mustbeincluded[i]; i++) {
            if (!current -> mustbeincluded[i]->instubborn) {
                current -> mustbeincluded[i]->instubborn = true;
                current -> mustbeincluded[i]->NextStubborn  = NULL;
                current-> EndOfStubbornList -> NextStubborn = current -> mustbeincluded[i];
                current -> EndOfStubbornList = current -> mustbeincluded[i];
            }
        }
    }
}


void sortscapegoats() {
    unsigned int i, j;
    unsigned int firstunknown, firstnz, tmp;
    unsigned int tmpp;
    // sort places with only invisible pre-transitions to the
    // beginning of the list used for enabledness-check, s.t.
    // these places become scapegoat more likely


    // count number of invisible pre-transitions
    for (i = 0; i < Globals::Places[0]->cnt; i++) {
        for (j = 0; Globals::Places[i]->PreTransitions[j]; j++) {
            if (Globals::Places[i]->PreTransitions[j]->visible) {
                Globals::Places[i]->visible ++;
            }
        }
    }

    // sort PrePlaces and Pre such that places without
    // visible pretransitions move to the beginning

    for (i = 0; i < Globals::Transitions[i]->cnt; i++) {
        // sort lists of transition i
        firstnz = 0;
        firstunknown = 0;
        while (Globals::Transitions[i]->PrePlaces[firstunknown] < Globals::Places[0]->cnt) {
            if (!Globals::Places[Globals::Transitions[i]->PrePlaces[firstunknown]] -> visible) {
                // swap firstnz <--> firstunknown
                tmp = Globals::Transitions[i]->Pre[firstunknown];
                tmpp = Globals::Transitions[i]->PrePlaces[firstunknown];
                Globals::Transitions[i]->Pre[firstunknown] = Globals::Transitions[i]->Pre[firstnz];
                Globals::Transitions[i]->PrePlaces[firstunknown] = Globals::Transitions[i]->PrePlaces[firstnz];
                Globals::Transitions[i]->Pre[firstnz] = tmp;
                Globals::Transitions[i]->PrePlaces[firstnz] = tmpp;
                firstnz ++;
            }
            firstunknown ++;
        }
    }
}








void stubborninsert(Transition* t) {
    if (t -> instubborn) {
        return;
    }
    t -> instubborn = true;
    t -> NextStubborn = NULL;
    if (t -> StartOfStubbornList) {
        t -> EndOfStubbornList -> NextStubborn = t;
        t -> EndOfStubbornList = t;
    } else {
        t -> StartOfStubbornList = t -> EndOfStubbornList = t;
    }
}


void insert_down(State*, formula*);



void insert_up(State* s, formula* f) {
    State* ss;
    unsigned int i;

    switch (f -> type) {
        case neq:
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f) ->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case eq:
            if (Globals::CurrentMarking[((atomicformula*) f) ->p->index] < ((atomicformula*) f)  -> k) {
                for (i = 0; i < ((atomicformula*) f) ->p->NrOfArriving; i++) {
                    stubborninsert(((atomicformula*) f) ->p->ArrivingArcs[i]->tr);
                }
            } else {
                for (i = 0; i < ((atomicformula*) f) ->p->NrOfLeaving; i++) {
                    stubborninsert(((atomicformula*) f) ->p->LeavingArcs[i]->tr);
                }
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f) ->p->ArrivingArcs[i]->tr);
            }
            break;
        case conj:
#ifdef MODELCHECKING
            for (i = checkstart[f->index];;) {
                if (!(s -> value[((booleanformula*) f) -> sub[i] -> index])) {
                    break;
                }
                i += 1;
                i %= ((booleanformula*) f) -> cardsub;
                if (i == checkstart[f->index]) {
                    break;
                }
            }
#else
            for (i = 0; i < ((booleanformula*) f)->cardsub; i++) {
                if (!(((booleanformula*) f) -> sub[i] -> value)) {
                    break;
                }
            }
#endif
            insert_up(s, ((booleanformula*) f) ->sub[i]);
            break;
        case disj:
            for (i = 0; i < ((booleanformula*) f) -> cardsub; i++) {
                insert_up(s, ((booleanformula*) f) ->sub[i]);
            }
            break;
        case neg:
            insert_down(s, ((unarybooleanformula*) f)->sub);
            break;
#ifdef MODELCHECKING
        case ef:
            break;
        case ag:
            for (ss = s; (ss->value[((unarytemporalformula*) f)->element->index]); ss = ss -> witness[f -> tempindex]) {
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
            }
            insert_up(ss, ((unarytemporalformula*) f) ->element);
            break;
        case eg:
            if (s -> value[((unarytemporalformula*) f) ->element->index]) {
                insert_down(s, ((unarytemporalformula*) f) ->element);
            } else {
                insert_up(s, ((unarytemporalformula*) f) ->element);
            }
            break;
        case eu:
            if (s -> value[((untilformula*) f) ->hold->index]) {
                insert_down(s, ((untilformula*) f) ->hold);
            } else {
                insert_up(s, ((untilformula*) f) ->hold);
                insert_up(s, ((untilformula*) f) -> goal);
            }
            break;
        case af:
            for (ss = s; s->checkmin[f->tempindex] < UINT_MAX; ss = ss ->
                                                                    witness[f->tempindex]) {
                ss -> checkmin[f->tempindex] = UINT_MAX;
                for (i = 0; ss ->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]
                                   ->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_up(ss, ((unarytemporalformula*) f) ->element);
            }
            for (ss = s; ss -> checkmin[f->tempindex] == UINT_MAX; ss =
                        ss -> witness[f->tempindex]) {
                ss -> checkmin[f->tempindex] = 0;
            }
            break;
        case au:
            for (ss = s; (s->checkmin[f->tempindex] < UINT_MAX) && (s->value[((untilformula*) f) ->hold->index]); ss = ss ->
                    witness[f->tempindex]) {
                ss -> checkmin[f->tempindex] = UINT_MAX;
                for (i = 0; ss ->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]
                                   ->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_up(ss, ((untilformula*) f)->goal);
            }
            if (!ss->value[((untilformula*) f) ->hold->index]) {
                insert_up(ss, ((untilformula*) f) ->hold);
            }
            for (ss = s; ss -> checkmin[f->tempindex] == UINT_MAX; ss =
                        ss -> witness[f->tempindex]) {
                ss -> checkmin[f->tempindex] = 0;
            }
            break;
        case ax:
        case ex:
            for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
                stubborninsert(Globals::Transitions[i]);
            }
            break;
#endif
        default:
            cout << "feature not implemented (yet)\n";
    }
}


void insert_global_up(formula* f) {
    unsigned int i;

    switch (f -> type) {
        case neq:
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f) ->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case eq:
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f) ->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < ((atomicformula*) f) ->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f) ->p->ArrivingArcs[i]->tr);
            }
            break;
        case conj:
        case disj:
            for (i = 0; i < ((booleanformula*) f) -> cardsub; i++) {
                insert_global_up(((booleanformula*) f) ->sub[i]);
            }
            break;
        case neg:
            insert_global_down(((unarybooleanformula*) f)->sub);
            break;
        default:
            cout << "feature not implemented (yet)\n";
    }
}

void insert_down(State* s, formula* f) {
    State* ss;
    unsigned int i;

    switch (f -> type) {
        case neq:
            if (Globals::CurrentMarking[((atomicformula*) f)->p->index] < ((atomicformula*) f)->k) {
                for (i = 0; i < ((atomicformula*) f)->p->NrOfArriving; i++) {
                    stubborninsert(((atomicformula*) f)->p->ArrivingArcs[i]->tr);
                }
            } else {
                for (i = 0; i < ((atomicformula*) f)->p->NrOfLeaving; i++) {
                    stubborninsert(((atomicformula*) f)->p->LeavingArcs[i]->tr);
                }
            }
            break;
        case eq:
            for (i = 0; i < ((atomicformula*) f)->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < ((atomicformula*) f)->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f)->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < ((atomicformula*) f)->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f)->p->LeavingArcs[i]->tr);
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < ((atomicformula*) f)->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f)->p->ArrivingArcs[i]->tr);
            }
            break;
        case disj:
            for (i = checkstart[f->index];;) {
#ifdef MODELCHECKING
                if (s -> value[((booleanformula*) f) -> sub[i]->index]) {
                    break;
                }
#else
                if (((booleanformula*) f)  -> sub[i]->value) {
                    break;
                }
#endif
                i += 1;
                i %= ((booleanformula*) f) -> cardsub;
                if (i == checkstart[f->index]) {
                    break;
                }
            }
            insert_down(s, ((booleanformula*) f) ->sub[i]);
            break;
        case conj:
            for (i = 0; i < ((booleanformula*) f) -> cardsub; i++) {
                insert_down(s, ((booleanformula*) f) ->sub[i]);
            }
            break;
        case neg:
            insert_up(s, ((unarybooleanformula*) f) ->sub);
            break;
#ifdef MODELCHECKING
        case ef:
            for (ss = s; !(ss->value[((unarytemporalformula*) f) ->element->index]); ss = ss -> witness[f -> tempindex]) {
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
            }
            insert_down(ss, ((unarytemporalformula*) f) ->element);
            break;
        case ag:
            break;
        case eg:
            for (ss = s; (ss->checkmin[f->tempindex] < UINT_MAX); ss = ss -> witness[f -> tempindex]) {
                ss -> checkmin[f->tempindex] = UINT_MAX;
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_down(ss, ((unarytemporalformula*) f) ->element);
            }
            for (ss = s; (ss->checkmin[f->tempindex] < UINT_MAX); ss = ss -> witness[f -> tempindex]) {
                ss -> checkmin[f->tempindex] = 0;
            }
            break;
        case eu:
            for (ss = s; !(ss->value[((untilformula*) f)->goal->index]); ss = ss -> witness[f -> tempindex]) {
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_down(ss, ((untilformula*) f) ->hold);
            }
            insert_down(ss, ((untilformula*) f) ->goal);
            break;
        case af:
            if (s->value[((unarytemporalformula*) f) ->element->index]) {
                insert_down(s, ((unarytemporalformula*) f) ->element);
            } else {
                insert_up(s, ((unarytemporalformula*) f) ->element);
            }
            break;
        case au:
            if (s->value[((untilformula*) f) ->goal->index]) {
                insert_down(s, ((untilformula*) f) ->goal);
            } else {
                insert_up(s, ((untilformula*) f)->goal);
            }
            break;

        case ax:
        case ex:
            for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
                stubborninsert(Globals::Transitions[i]);
            }
            break;
#endif
        default:
            cout << "feature not implemented (yet)\n";
    }
}


void insert_global_down(formula* f) {
    // unused: State* ss;
    unsigned int i;

    switch (f -> type) {
        case neq:
            for (i = 0; i < ((atomicformula*) f)->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < ((atomicformula*) f)->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f)->p->LeavingArcs[i]->tr);
            }
            break;
        case eq:
            for (i = 0; i < ((atomicformula*) f)->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < ((atomicformula*) f)->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f)->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < ((atomicformula*) f)->p->NrOfLeaving; i++) {
                stubborninsert(((atomicformula*) f)->p->LeavingArcs[i]->tr);
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < ((atomicformula*) f)->p->NrOfArriving; i++) {
                stubborninsert(((atomicformula*) f)->p->ArrivingArcs[i]->tr);
            }
            break;
        case disj:
        case conj:
            for (i = 0; i < ((booleanformula*) f) -> cardsub; i++) {
                insert_global_down(((booleanformula*) f) ->sub[i]);
            }
            break;
        case neg:
            insert_global_up(((unarybooleanformula*) f) ->sub);
            break;
        default:
            cout << "feature not implemented (yet)\n";
    }
}

Transition** stubbornfirelist(State* s, formula* f) {
    Transition** result;
    Transition* t;
    int i;

    t -> StartOfStubbornList = NULL;
    insert_up(s, f);
    stubbornclosure();
    i = Globals::Transitions[0]->NrStubborn;
    result = new Transition * [Globals::Transitions[0]->NrStubborn + 1];
    for (t = Globals::Transitions[0]->StartOfStubbornList, i = 0; t; t = t -> NextStubborn) {
        t -> instubborn = false;
        if (t -> enabled
#ifdef EXTENDEDCTL
                && (t -> pathrestriction[TemporalIndex])
#endif
           ) {
            result[i++] = t;
        }
    }
    result[i] = NULL;
    CardFireList = i;
    return result;
}

Transition** stubbornfirelistpredicate() {
    if (F -> value) {
        return NULL;
    }
    return stubbornfirelist(NULL, F);
}

Transition** stubbornfirelistneg(State* s, formula* f) {
    Transition** result;
    Transition* t;
    int i;

    t -> StartOfStubbornList = NULL;
    insert_down(s, f);
    stubbornclosure();
    result = new Transition * [Globals::Transitions[0]->NrStubborn + 1];
    for (t = Globals::Transitions[0]->StartOfStubbornList, i = 0; t; t = t -> NextStubborn) {
        t -> instubborn = false;
        if (t -> enabled
#ifdef EXTENDEDCTL
                && (t -> pathrestriction[TemporalIndex])
#endif
           ) {
            result[i++] = t;
        }
    }
    result[i] = NULL;
    return result;
}

Transition** stubbornfirelistctl() {
    Transition** result, * current, *start;

    // stubborn does not work with X-operators
    if (xoperators) {
        return firelist();
    }

    // try for all enabled invisible transitions if there is
    // a stubborn superset without other enabled transitions
    for (start = Globals::Transitions[0]->StartOfEnabledList; start; start = start -> NextEnabled) {
        if (start -> visible) {
            continue;
        }
#ifdef EXTENDEDCTL
        if (!(start -> pathrestriction[TemporalIndex])) {
            continue;
        }
#endif
        start -> StartOfStubbornList = start -> EndOfStubbornList = start;
        start -> instubborn = true;
        start -> NextStubborn = NULL;
        for (current = current -> StartOfStubbornList; current; current = current -> NextStubborn) {
            for (int i = 0; current -> mustbeincluded[i]; i++) {
#ifdef EXTENDEDCTL
                if (!(current->mustbeincluded[i] -> pathrestriction[TemporalIndex])) {
                    continue;
                }
#endif
                if (current->mustbeincluded[i] -> enabled) {
                    if (current->mustbeincluded[i] != start) {
                        Transition* t;
                        for (t = t->StartOfStubbornList; t; t = t -> NextStubborn) {
                            t -> instubborn = false;
                        }
                        goto nextstart;
                    }
                }
                if (!current -> mustbeincluded[i]->instubborn) {
                    current -> mustbeincluded[i]->instubborn = true;
                    current -> mustbeincluded[i]->NextStubborn  = NULL;
                    current-> EndOfStubbornList -> NextStubborn = current -> mustbeincluded[i];
                    current -> EndOfStubbornList = current -> mustbeincluded[i];
                }
            }
        }
        result = new Transition*[2];
        result[0] = start;
        result[1] = NULL;
        for (current = current -> StartOfStubbornList; current; current = current -> NextStubborn) {
            current -> instubborn = false;
        }
        return result;
nextstart:
        ;
    }

    return firelist();
}


Transition** stubbornfireliststatic() {
    Transition** result;
    Transition* t;
    unsigned int i;
    // computes a stubborn superset of a static attractor set.
    // The attractor set is given as initial segment of the stubborn
    // set list, terminated by LastAttractor

    if (LastAttractor) {
        LastAttractor -> NextStubborn = NULL;
        Globals::Transitions[0]->EndOfStubbornList = LastAttractor;
        stubbornclosure();
    }
    result = new Transition * [Globals::Transitions[0]->NrStubborn + 1];
    i = 0;
    if (LastAttractor) {
        for (t = Globals::Transitions[0]->StartOfStubbornList;; t = t -> NextStubborn) {
            if (t -> enabled) {
                result[i++] = t;
            }
            if (t == LastAttractor) {
                break;
            }
        }
        for (t = t -> NextStubborn; t; t = t -> NextStubborn) {
            t -> instubborn = false;
            if (t -> enabled) {
                result[i++] = t;
            }
        }
    }
    result[i] = NULL;
    CardFireList = Globals::Transitions[0]-> NrStubborn;
    return result;
}


Transition** stubbornfirelistnogoal() {
    Transition** result;
    Transition* t;
    unsigned int i;
    // computes a stubborn superset of a single enabled transition.
    // We use the first element of enabling list

    Globals::Transitions[0]->NrStubborn = 0;
    if (Globals::Transitions[0]->StartOfEnabledList) {
        Globals::Transitions[0]->StartOfEnabledList -> NextStubborn = NULL;
        Globals::Transitions[0]->StartOfStubbornList =
            Globals::Transitions[0]->EndOfStubbornList = Globals::Transitions[0]->StartOfEnabledList;
        Globals::Transitions[0]->StartOfStubbornList->instubborn = true;
        stubbornclosure();
    } else {
        Globals::Transitions[0]->StartOfStubbornList = NULL;
    }
    result = new Transition * [Globals::Transitions[0]->NrStubborn + 1];
    i = 0;
    for (t = Globals::Transitions[0]->StartOfStubbornList; t; t = t -> NextStubborn) {
        t -> instubborn = false;
        if (t -> enabled) {
            result[i++] = t;
        }
    }
    result[i] = NULL;
    CardFireList = Globals::Transitions[0]-> NrStubborn;
    return result;
}

Transition** stubbornfirelistreach() {
    Transition** Attr;
    Transition** result;
    Transition* t;
    unsigned int i;
    // computes a stubborn superset of an attractor set for
    // the reachability of target_marking.
    // Attractor set generation is controlled by the comparison
    // between target and current marking.

    for (i = 0; i < Globals::Places[0]->cnt; i++) {
        if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
            break;
        }
    }
    if (i >= Globals::Places[0]->cnt) { // target_marking found!
        return NULL;
    }
    if (Globals::CurrentMarking[i] > Globals::Places[i]->target_marking) {
        Attr = Globals::Places[i]->PostTransitions;
    } else {
        Attr = Globals::Places[i]->PreTransitions;
    }
    Globals::Transitions[0]->StartOfStubbornList = NULL;
    for (i = 0; Attr[i]; i++) {
        Attr[i]->NextStubborn = Globals::Transitions[0]->StartOfStubbornList;
        Globals::Transitions[0]->StartOfStubbornList = Attr[i];
        Attr[i]->instubborn = true;
    }
    Globals::Transitions[0]->EndOfStubbornList = Attr[0];
    stubbornclosure();
    result = new Transition * [Globals::Transitions[0]->NrStubborn + 1];
    i = 0;
    for (t = Globals::Transitions[0]->StartOfStubbornList; t; t = t -> NextStubborn) {
        if (t -> enabled) {
            result[i++] = t;
        }
        t -> instubborn = false;
    }
    result[i] = NULL;
    CardFireList = Globals::Transitions[0]-> NrStubborn;
    return result;
}

unsigned int StubbStamp = 0;

void NewStubbStamp() {
    if (StubbStamp < UINT_MAX) {
        StubbStamp ++;
    } else {
        unsigned int i;
        for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
            Globals::Transitions[i]-> stamp = 0;
        }
        StubbStamp = 1;
    }
}

#define MINIMUM(X,Y) ((X) < (Y) ? (X) : (Y))
Transition** tsccstubbornlist() {
    Transition** result;
    unsigned int maxdfs;
    Transition* current, * next;

    // computes stubborn set without goal orientation.
    // The TSCC based optimisation is included

    // 1. start with enabled transition
    if (Globals::Transitions[0]->TarjanStack =  Globals::Transitions[0]->StartOfEnabledList) {
        maxdfs = 0;
        NewStubbStamp();
        Globals::Transitions[0]->TarjanStack -> nextontarjanstack = Globals::Transitions[0]->TarjanStack;
        Globals::Transitions[0]->TarjanStack -> stamp  = StubbStamp;
        Globals::Transitions[0]->TarjanStack -> dfs = Globals::Transitions[0]->TarjanStack -> min = maxdfs++;
        Globals::Transitions[0]->TarjanStack -> mbiindex = 0;
        current = Globals::Transitions[0]->TarjanStack;
        Globals::Transitions[0]->CallStack = current;
        current -> nextoncallstack = NULL;
    } else {
        result = new Transition * [1];
        result[0] = NULL;
        CardFireList = 0;
        return result;
    }
    while (current) {
        if (next = current->mustbeincluded[current->mbiindex]) {
            // Successor exists
            if (next->stamp == StubbStamp) {
                // already visited
                if (next -> nextontarjanstack) {
                    // next still on stack
                    current -> min = MINIMUM(current -> min, next -> dfs);
                }
                current -> mbiindex++;
            } else {
                // not yet visited
                next -> nextontarjanstack = Globals::Transitions[0]->TarjanStack;
                Globals::Transitions[0]->TarjanStack = next;
                next -> min = next -> dfs = maxdfs++;
                next -> stamp = StubbStamp;
                next -> mbiindex = 0;
                next -> nextoncallstack = current;
                Globals::Transitions[0]->CallStack = next;
                current = next;
            }
        } else {
            // no more successors -> scc detection and backtracking
            if (current -> dfs == current -> min) {
                // remove all states behind current from Tarjanstack;
                // if enabled -> final sequence
                while (1) {
                    if (Globals::Transitions[0]->TarjanStack -> enabled) {
                        // final sequence
                        unsigned int cardstubborn;
                        Transition* t;

                        cardstubborn = 0;
                        for (t = Globals::Transitions[0]->TarjanStack;; t = t -> nextontarjanstack) {
                            if (t -> enabled) {
                                cardstubborn ++;
                            }
                            if (t == current) {
                                break;
                            }
                        }
                        result = new Transition * [cardstubborn + 2];
                        cardstubborn = 0;
                        for (t = Globals::Transitions[0]->TarjanStack;; t = t -> nextontarjanstack) {
                            if (t -> enabled) {
                                result[cardstubborn++] = t;
                            }
                            if (t == current) {
                                result[cardstubborn] = NULL;
                                CardFireList = cardstubborn;
                                return(result);
                            }
                        }
                    } else {
                        if (Globals::Transitions[0]->TarjanStack == current) {
                            break;
                        }
                        Globals::Transitions[0]->TarjanStack = Globals::Transitions[0]->TarjanStack -> nextontarjanstack;
                    }
                }
            }
            // backtracking to previous state
            next = current -> nextoncallstack;
            next -> min = MINIMUM(current -> min, next -> min);
            next -> mbiindex++;
            current = next;
        }
    }
}

Transition** relaxedstubbornset() {
#if defined(WITHFORMULA) && ! defined(MODELCHECKING)
    Transition** firstattempt;
    unsigned int i;

#if !defined(LIVEPROP) || (defined(RELAXED) && defined(TWOPHASE))
    if (F -> value) {
        return NULL;
    }
#endif
    firstattempt = tsccstubbornlist();
#if defined(LIVEPROP) && (!defined(RELAXED) || !defined(TWOPHASE))
    if (F -> value) {
        return firstattempt;
    }
#endif
    for (i = 0; firstattempt[i]; i++) {
        if (firstattempt[i] -> down) {
            break;
        }
    }
    if (firstattempt[i]) {
        delete [] firstattempt;
        return stubbornfirelistpredicate();
    }
    return firstattempt;

#endif
}

Transition** structreachstubbornset() { // used: relaxed reachability, cycle detection by t-inv
#if defined(WITHFORMULA) && ! defined(MODELCHECKING) && defined(STRUCT)
    Transition** firstattempt;
    unsigned int i;
    // unused: unsigned int down;
    Transition** result;
    Transition* t;

    if (F -> value) {
        return NULL;
    }
    firstattempt = tsccstubbornlist();
    for (i = 0; firstattempt[i]; i++) {
        if (firstattempt[i] -> cyclic) {
            break;
        }
    }
    if (firstattempt[i]) {
        // cycle detected, return stubborn closure of global UP set

        //delete [] firstattempt;
        t -> StartOfStubbornList = NULL;
        insert_global_up(F);
        stubbornclosure();
        result = new Transition * [Globals::Transitions[0]->NrStubborn + 5];
        for (t = Globals::Transitions[0]->StartOfStubbornList, i = 0; t; t = t -> NextStubborn) {
            t -> instubborn = false;
            if (t -> enabled) {
                result[i++] = t;
            }
        }
        result[i] = NULL;
        return result;
    }
    return firstattempt;

#endif
}

#ifdef STRUCT
Transition** structstubbornset() {
    Transition** reduced;
    int Cycle;
    int i;


    reduced = tsccstubbornlist();
    Cycle = 0;
    for (i = 0; reduced[i]; i++) {
        if (reduced[i] -> cyclic) {
            Cycle = 1;
            break;
        }
    }
    if (Cycle) {
        delete [] reduced;
        return firelist();
    }
    return reduced;
}
#endif





#endif












