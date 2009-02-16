#include <sstream>
#include <fstream>
#include "test.h"
#include "parser.h"

using std::stringstream;

using pnapi::io::owfn;
using pnapi::io::meta;
using pnapi::io::InputError;
using pnapi::io::CREATOR;
using pnapi::io::INPUTFILE;
using pnapi::io::OUTPUTFILE;

int main(int argc, char * argv[])
{
  stringstream stream, test;
  PetriNet net;
  Place & p1 = net.createPlace("in", Place::INPUT);
  Place & p2 = net.createPlace("out", Place::OUTPUT);
  Place & p3 = net.createPlace("", Place::INTERNAL, 4, 8);
  Place & p4 = net.createPlace();
  Transition & t1 = net.createTransition();
  Transition & t2 = net.createTransition();
  net.createArc(p1, t1, 3);
  net.createArc(t1, p3);
  net.createArc(p3, t2);
  net.createArc(t2, p2);

  begin_test("io::operator>>() [Petri net OWFN input]");
  stream << owfn << net;
  //cout << owfn << net;
  try { stream >> owfn >> net; }
  catch (InputError e) { cout << endl << e << endl; assert(false); }
  test << owfn << net;
  //cout << owfn << net;
  assert(stream.str() == test.str());
  end_test();

  return 0;
}
