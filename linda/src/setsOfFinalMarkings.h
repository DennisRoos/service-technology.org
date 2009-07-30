#ifndef SETSOFFINALMARKINGS_H_
#define SETSOFFINALMARKINGS_H_

#include <cstdlib>
#include <map>
#include <vector>
#include <pnapi/pnapi.h>
#include "helpers.h"


class PartialMarking {
public:

	std::map<const pnapi::Place*,int> values;
	bool defines(const pnapi::Place* place) {return values.find(place) != values.end();}
	bool isEquivalentTo(PartialMarking* other);

	static PartialMarking* merge(PartialMarking* m1,PartialMarking* m2);

	void output();
	std::string toString();

};

class SetOfPartialMarkings {

public:

	std::vector<PartialMarking*> partialMarkings;

	bool protectedInsert(PartialMarking* partialMarking);
	bool contains(PartialMarking* partialMarking);
	void add(SetOfPartialMarkings* aSet);

	static SetOfPartialMarkings* unite(std::vector<SetOfPartialMarkings*>& sets);
	static SetOfPartialMarkings* intersect(SetOfPartialMarkings* set1, SetOfPartialMarkings* set2);
	static SetOfPartialMarkings* unite(SetOfPartialMarkings* set1, SetOfPartialMarkings* set2);
	static SetOfPartialMarkings* create(const pnapi::formula::Formula* const src, unsigned int bound);

	void output();

};

#endif

