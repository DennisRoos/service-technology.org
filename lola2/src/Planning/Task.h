#pragma once

#include <cmdline.h>
#include <Core/Dimensions.h>
#include <Stores/Store.h>


class StatePredicate;
class SimpleProperty;
class FireListCreator;
class ChooseTransition;
class DFSExploration;
class NetState;


class Task
{
private:
    StatePredicate* sp;
    NetState* ns;
    ChooseTransition* choose;
    enum_search search;
    FireListCreator* flc;
    SimpleProperty* p;
    DFSExploration* exploration;
    int number_of_threads;
    formula_t formulaType;
    void setNet();
    void setFormula();

public:
    Store<void>* s;

    Task();
    ~Task();

    bool getResult();
    void interpreteResult(bool);

    void setStore();
    void setProperty();

    void printWitness();
    void printMarking();

    void printDot();

    // this function is needed to allow the random walk in the main function
    NetState* getNetState();
};
