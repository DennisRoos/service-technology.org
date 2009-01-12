#ifndef SERVICEAUTOMATON_H
#define SERVICEAUTOMATON_H

#ifndef HASHSIZE
#define HASHSIZE 65535
#endif

#include <list>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include "state.h"

using std::list;
using std::ostream;
using std::set;
using std::string;
using std::vector;
using std::map;

namespace pnapi
{

class PetriNet;
class Transition;

class Automaton
{
public:

  /// constructor with Petri net - initial marking taken from place marks
  Automaton(PetriNet &n);

  /// standard destructor
  virtual ~Automaton();

  /// initializes the service automaton
  void initialize();

  /// creates the service automaton
  void createAutomaton();

  /// friend operation <<
  friend ostream &operator <<(ostream &os, const Automaton &sa);

  /// calculates the hash value
  unsigned int hashValue(State &s);

private:
  vector<set<State *> > hashTable;
  vector<unsigned int> primes;

  list<string> in;
  list<string> out;
  unsigned int first;
  list<unsigned int> finals;

  map<Transition *, string> reasons;

  PetriNet &net;
  Marking &initialmarking;

  void initHashTable();

  void fillPrimes();
  bool isPrime(unsigned int &p) const;

  void dfs(State &i);
};

/*** overloaded operator << ***/

ostream &operator <<(ostream &os, const Automaton &sa);

}

#endif /* SERVICEAUTOMATON_H */
