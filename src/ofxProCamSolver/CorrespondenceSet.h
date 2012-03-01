#pragma once
#include "Correspondence.h"
#include <vector>
#include <fstream>
#include <stdint.h>
#include <set>

#include "ofFileUtils.h"
#include "ofSystemUtils.h"
#include "ofLog.h"


namespace ofxProCamSolver {

	template<typename T>
	class CorrespondenceSet_ : public vector<Correspondence_<T> > {
	public:
		CorrespondenceSet_() { };
		CorrespondenceSet_(size_type _Count) : vector<Correspondence_<T> >(_Count) { };

		template<typename O>
		CorrespondenceSet_(vector<O> other) : vector<Correspondence_<T> >(other.begin(), other.end()) { };
		
		void save(string filename="");
		void load(string filename="");

		set<int> getViewIndices() const; // get a set of indices of all camera/projectors in correspondence set
	};

	typedef CorrespondenceSet_<double> CorrespondenceSetd;
	typedef CorrespondenceSet_<float> CorrespondenceSetf;
}