#include <errno.h>

#include <map>
#include <list>

#include <InputOutput/InputOutput.h>
#include <InputOutput/Reporter.h>
#include <Planning/Task.h>
#include <cmdline.h>

#include <Parser/ast-system-k.h>
#include <Parser/ast-system-rk.h>
#include <Parser/ast-system-unpk.h>

#include <Net/Net.h>
#include <Net/Marking.h>
#include <Net/NetState.h>
#include <Net/Transition.h>

#include <Exploration/DFSExploration.h>
#include <Exploration/ParallelExploration.h>

#include <Exploration/RandomWalk.h>
#include <Exploration/Firelist.h>
#include <Exploration/SimpleProperty.h>
#include <Exploration/Deadlock.h>
#include <Exploration/FirelistStubbornDeadlock.h>
#include <Exploration/FirelistStubbornStatePredicate.h>
#include <Exploration/StatePredicateProperty.h>
#include <Exploration/ChooseTransition.h>
#include <Exploration/ChooseTransitionHashDriven.h>

#include <Formula/StatePredicate.h>

#include <Stores/BinStore.h>
#include <Stores/BinStore2.h>
#include <Stores/NetStateEncoder/BitEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <Stores/NetStateEncoder/SimpleCompressedEncoder.h>
#include <Stores/VectorStores/SuffixTreeStore.h>
#include <Stores/VectorStores/VSTLStore.h>
#include <Stores/VectorStores/VBloomStore.h>
#include <Stores/PluginStore.h>
#include <Stores/BitStore.h>
#include <Stores/BloomStore.h>
#include <Stores/EmptyStore.h>
#include <Stores/SIBinStore2.h>
#include <Stores/STLStore.h>
#include <Stores/Store.h>
#include <Stores/ThreadSafeStore.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;

// the parsers
extern int ptformula_parse();
extern int ptformula_lex_destroy();

// input file
extern FILE* ptformula_in;

// code to parse from a string
struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE ptformula__scan_string(const char* yy_str);
extern void ptformula__delete_buffer(YY_BUFFER_STATE);

/// printer-function for Kimiwtu's output on stdout
// LCOV_EXCL_START
void myprinter(const char* s, kc::uview v)
{
    printf("%s", s);
}
// LCOV_EXCL_STOP

extern kc::tFormula TheFormula;

Task::Task() : sp(NULL), ns(NULL), p(NULL), s(NULL), flc(NULL), exploration(NULL), choose(NULL), search(args_info.search_arg), number_of_threads(args_info.threads_arg)
{
    setFormula();
    setNet();
}

Task::~Task()
{
    delete ns;
    delete s;
    delete p;
    delete sp;
    delete flc;
    delete exploration;
}

void Task::setNet()
{
    ns = NetState::createNetStateFromInitial();
    Transition::checkTransitions(*ns);
}

void Task::setFormula()
{
    if (not args_info.formula_given)
    {
        return;
    }

    StatePredicate* result = NULL;
    Input* formulaFile = NULL;

    // Check if the paramter of --formula is a file that we can open: if that
    // works, parse the file. If not, parse the string.
    FILE* file;
    if ((file = fopen(args_info.formula_arg, "r")) == NULL and errno == ENOENT)
    {
        YY_BUFFER_STATE my_string_buffer = ptformula__scan_string(args_info.formula_arg);
    }
    else
    {
        fclose(file);
        formulaFile = new Input("formula", args_info.formula_arg);
        ptformula_in = *formulaFile;
    }

    // parse the formula
    ptformula_parse();

    // restructure the formula: remove arrows and handle negations and tautologies
    TheFormula = TheFormula->rewrite(kc::arrows);
    TheFormula = TheFormula->rewrite(kc::neg);

    // check temporal status
    TheFormula->unparse(myprinter, kc::temporal);
    formulaType = TheFormula->type;

    switch (formulaType)
    {
        case (FORMULA_REACHABLE):
            rep->status("checking reachability");
            TheFormula = TheFormula->rewrite(kc::reachability);
            break;
        case (FORMULA_INVARIANT):
            rep->status("checking invariance");
            TheFormula = TheFormula->rewrite(kc::reachability);
            break;
        case (FORMULA_IMPOSSIBLE):
            rep->status("checking impossibility");
            TheFormula = TheFormula->rewrite(kc::reachability);
            break;
        case (FORMULA_LIVENESS):
            rep->status("checking liveness");
            break;
        case (FORMULA_FAIRNESS):
            rep->status("checking fairness");
            break;
        case (FORMULA_STABILIZATION):
            rep->status("checking stabilization");
            break;
        case (FORMULA_EVENTUALLY):
            rep->status("checking eventual occurrence");
            break;
        case (FORMULA_INITIAL):
            rep->status("checking initial satisfiability");
            break;
        case (FORMULA_MODELCHECKING):
            rep->status("checking CTL");
            break;
    }

    // restructure the formula: again tautoglies and simplification
    TheFormula = TheFormula->rewrite(kc::neg);
    TheFormula = TheFormula->rewrite(kc::leq);
    TheFormula = TheFormula->rewrite(kc::sides);
    TheFormula = TheFormula->rewrite(kc::lists);

    // debug
    // TheFormula->print();
    // TheFormula->unparse(myprinter, kc::out);

    if (formulaType == FORMULA_REACHABLE or
            formulaType == FORMULA_INVARIANT or
            formulaType == FORMULA_IMPOSSIBLE or
            formulaType == FORMULA_INITIAL)
    {
        // copy restructured formula into internal data structures
        TheFormula->unparse(myprinter, kc::internal);
        result = TheFormula->formula;
    }
    else
    {
        rep->message("check not yet implemented");
        rep->abort(ERROR_COMMANDLINE);
    }

    assert(result);
    rep->status("processed formula with %d atomic propositions", result->countAtomic());

    // tidy parser
    ptformula_lex_destroy();
    //delete TheFormula;

    if (args_info.formula_given)
    {
        delete formulaFile;
    }

    sp = result;
}

void Task::setStore()
{
    if (args_info.search_arg == search_arg_findpath)
    {
        s = new EmptyStore();
    }
    else
    {
        // choose a store
        switch (args_info.store_arg)
        {
            case store_arg_bin:
                s = new BinStore();
                break;
            case store_arg_bloom:
                s = new BloomStore(args_info.hashfunctions_arg);
                break;
            case store_arg_stl:
                s = new STLStore();
                break;
            case store_arg_bit:
                s = new BitStore();
                break;
            case store_arg_bin2:
                s = new BinStore2(number_of_threads);
                break;
            case store_arg_psbbin:
                s = new PluginStore(new BitEncoder(number_of_threads), new SuffixTreeStore(),number_of_threads);
                break;
            case store_arg_pscbin:
                s = new PluginStore(new CopyEncoder(number_of_threads), new SuffixTreeStore(),number_of_threads);
                break;
            case store_arg_pssbin:
                s = new PluginStore(new SimpleCompressedEncoder(number_of_threads), new SuffixTreeStore(),number_of_threads);
                break;
            case store_arg_psbstl:
                s = new PluginStore(new BitEncoder(number_of_threads), new VSTLStore(number_of_threads),number_of_threads);
                break;
            case store_arg_pscstl:
                s = new PluginStore(new CopyEncoder(number_of_threads), new VSTLStore(number_of_threads),number_of_threads);
                break;
            case store_arg_pssstl:
                s = new PluginStore(new SimpleCompressedEncoder(number_of_threads), new VSTLStore(number_of_threads),number_of_threads);
                break;
            case store_arg_psbbloom:
                s = new PluginStore(new BitEncoder(number_of_threads), new VBloomStore(number_of_threads,args_info.hashfunctions_arg),number_of_threads);
                break;
            case store_arg_pscbloom:
                s = new PluginStore(new CopyEncoder(number_of_threads), new VBloomStore(number_of_threads,args_info.hashfunctions_arg),number_of_threads);
                break;
            case store_arg_pssbloom:
                s = new PluginStore(new SimpleCompressedEncoder(number_of_threads), new VBloomStore(number_of_threads,args_info.hashfunctions_arg),number_of_threads);
                break;
            case store_arg_tsbin2:
                s = new ThreadSafeStore(new SIBinStore2(number_of_threads), number_of_threads);
                break;
        }
    }
}

void Task::setProperty()
{
    // choose a simple property
    switch (args_info.check_arg)
    {
            //        case check_arg_none:
            //            return EXIT_SUCCESS;

        case check_arg_full:
            p = new SimpleProperty();
            flc = new FireListCreator();
            break;

        case check_arg_deadlock:
            p = new Deadlock();
            flc = new FireListStubbornDeadlockCreator();
            break;

        case check_arg_statepredicate:
            p = new StatePredicateProperty(sp);
            flc = new FirelistStubbornStatePredicateCreator();
            break;
    }


    // set the correct exploration algorithm
    switch (args_info.check_arg)
    {
        case check_arg_full:
        case check_arg_deadlock:
        case check_arg_statepredicate:
            if (number_of_threads == 1)
            {
                exploration = new DFSExploration();
            }
            else
            {
                exploration = new ParallelExploration();
            }
            break;
            // now there is only one, but who knows...
    }
}

bool Task::getResult()
{
    assert(ns);
    assert(s);
    assert(p);
    assert(exploration);
    assert(flc);

    bool result;
    switch (args_info.search_arg)
    {
        case search_arg_depth:
            result = exploration->depth_first(*p, *ns, *s, * flc, number_of_threads);
            break;

        case search_arg_findpath:
            if (args_info.retrylimit_arg == 0)
            {
                rep->status("starting infinite tries of depth %d", args_info.depthlimit_arg);
            }
            else
            {
                rep->status("starting at most %d tries of depth %d", args_info.retrylimit_arg, args_info.depthlimit_arg);
            }

            choose = new ChooseTransitionHashDriven();
            result = exploration->find_path(*p, *ns, args_info.retrylimit_arg, args_info.depthlimit_arg, *flc->createFireList(p), *((EmptyStore*)s), *choose);
            delete choose;
            break;

        default:
            assert(false);
            break;
    }

    return result;
}

void Task::interpreteResult(bool result)
{
    // in case AG is used, the result needs to be negated
    if (formulaType == FORMULA_INVARIANT or formulaType == FORMULA_IMPOSSIBLE)
    {
        result = not result;
    }

    // make result three-valued
    trinary_t final_result = result ? TRINARY_TRUE : TRINARY_FALSE;

    // if the Bloom store did not find anything, the result is unknown
    if (args_info.store_arg == store_arg_bloom)
    {
        switch (args_info.check_arg)
        {
            case (check_arg_deadlock):
            case (check_arg_statepredicate):
                if (not result)
                {
                    final_result = TRINARY_UNKNOWN;
                }
                break;
        }
    }

    switch (final_result)
    {
        case (TRINARY_TRUE):
            rep->message("result: %s", rep->markup(MARKUP_GOOD, "yes").str());
            break;
        case (TRINARY_FALSE):
            rep->message("result: %s", rep->markup(MARKUP_BAD, "no").str());
            break;
        case (TRINARY_UNKNOWN):
            rep->message("result: %s", rep->markup(MARKUP_WARNING, "unknown").str());
            break;
    }
}

void Task::printWitness()
{
    rep->message("%s", rep->markup(MARKUP_IMPORTANT, "witness path:").str());
    index_t c;
    index_t* f;
    while (p->stack.StackPointer > 0)
    {
        index_t t = p->stack.topTransition();
        rep->message("%s", Net::Name[TR][t]);
        p->stack.pop(& c, & f);
    }
}

void Task::printMarking()
{
    rep->message("%s", rep->markup(MARKUP_IMPORTANT, "witness state:").str());
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        if (ns->Current[p] > 0)
        {
            rep->message("%s : %d", Net::Name[PL][p], ns->Current[p]);
        }
    }
}


NetState* Task::getNetState()
{
    return ns;
}


void Task::printDot()
{
    FILE *o = stdout;
    std::map<index_t, unsigned int> conditions;
    std::map<index_t, unsigned int> events;
    
    fprintf(o, "digraph d {\n");
    fprintf(o, "rankdir=LR;\n\n");

    // put initial marking into a cluster
    fprintf(o, "subgraph m0 {\n");
    fprintf(o, "  rank=same;\n");
    for (index_t i = 0; i < Net::Card[PL]; ++i)
    {
        if (Marking::Initial[i] > 0)
        {
            fprintf(o, "  p%d_%d [label=\"%s\" shape=circle color=green]\n", i, conditions[i], Net::Name[PL][i]);
        }
    }
    fprintf(o, "}\n\n");
    
    // print the witness path
    std::list<index_t> path;
    index_t c;
    index_t* f;
    while (p->stack.StackPointer > 0)
    {
        index_t i = p->stack.topTransition();
        path.push_front(i);
        p->stack.pop(& c, & f);
    }

    for (std::list<index_t>::iterator it = path.begin(); it != path.end(); ++it) {
        index_t i = *it;

        fprintf(o, "  t%d_%d [label=\"%s\" shape=box]\n", i, events[i], Net::Name[TR][i]);

        for (index_t pre = 0; pre < Net::CardArcs[TR][PRE][i]; ++pre)
        {
            fprintf(o, "  p%d_%d -> t%d_%d\n", Net::Arc[TR][PRE][i][pre], conditions[Net::Arc[TR][PRE][i][pre]], i, events[i]);
        }

        for (index_t post = 0; post < Net::CardArcs[TR][POST][i]; ++post)
        {
            fprintf(o, "  t%d_%d -> p%d_%d\n", i, events[i], Net::Arc[TR][POST][i][post], ++conditions[Net::Arc[TR][POST][i][post]]);
        }

        fprintf(o, "  subgraph post_t%d_%d {\n", i, events[i]);
        fprintf(o, "    rank=same;\n");
        for (index_t post = 0; post < Net::CardArcs[TR][POST][i]; ++post)
        {
            fprintf(o, "    p%d_%d [label=\"%s\" shape=circle]\n", Net::Arc[TR][POST][i][post], conditions[Net::Arc[TR][POST][i][post]], Net::Name[PL][Net::Arc[TR][POST][i][post]]);
        }
        fprintf(o, "}\n\n");

        events[i]++;
    }
/*
    // put the target marking into a cluster
    fprintf(o, "\nsubgraph mf {\n");
    fprintf(o, "  rank=same;\n");
    for (index_t i = 0; i < Net::Card[PL]; ++i)
    {
        if (ns->Current[i] > 0)
        {
            fprintf(o, "  p%d_%d [label=\"%s\" shape=\"circle\"]\n", i, conditions[i], Net::Name[PL][i]);
        }
    }
    fprintf(o, "}\n\n");
*/

    fprintf(o, "}\n");
}