#pragma once
#include "ofVec2f.h"
#include <vector>

namespace ofxProCamSolver {

	template<typename T>
	class Correspondence_ {
	public:
		Correspondence_() { };

		///Copy from other type
		template <typename O>
		Correspondence_(const Correspondence_<O>& other) {
			this->cameraID1 = other.cameraID1;
			this->xy1[0] = other.xy1[0];
			this->xy1[1] = other.xy1[1];

			this->cameraID2 = other.cameraID2;
			this->xy2[0] = other.xy2[0];
			this->xy2[1] = other.xy2[1];
		}

		Correspondence_(int cameraID1, const ofVec2f& xy1, int cameraID2, const ofVec2f& xy2);

		ofVec2f getXY1() const;
		ofVec2f getXY2() const;

		////
		//data
		////
		int cameraID1;
		T xy1[2];
		int cameraID2;
		T xy2[2];
		T weight; // to keep consistent spacing with RPSolver
		////

	};

	typedef Correspondence_<float> Correspondencef;
	typedef Correspondence_<double> Correspondenced;
}