
// ----------------------------
// ------------ ofxProCamSolver
// ----------------------------
// -- Kimchi and Chips
// -- www.kimchiandchips.com
// -- Daniel Tang, Elliot Woods
// ----------------------------

//NB: we use the words 'cameras' and 'projectors' interchangeably

#include "ProCamSolver/RPSolver.h"
#include "ofxProCamSolver/Correspondence.h"
#include "ofxProCamSolver/CorrespondenceSet.h"
#include "ofxRay/src/ofxRay.h"
#include <assert.h>

namespace ofxProCamSolver {
	class Solver {
	public:
		float solve(const CorrespondenceSetf &correspondences);
		float solve(const CorrespondenceSetd &correspondences);
	protected:
	};

}