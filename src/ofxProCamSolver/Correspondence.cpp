#include "Correspondence.h"

namespace ofxProCamSolver {
	template<typename T>
	Correspondence_<T>::Correspondence_(int cameraID1, const ofVec2f& xy1, int cameraID2, const ofVec2f& xy2) {
		this->cameraID1 = cameraID1;
		this->cameraID2 = cameraID2;
		
		if (sizeof(T) == 4) {
			//float. this is optimised out as always true or always false
			*(ofVec2f*)(&this->xy1) = xy1;
			*(ofVec2f*)(&this->xy2) = xy2;
		} else {
			this->xy1[0] = xy1.x;
			this->xy1[1] = xy1.y;
			this->xy2[0] = xy2.x;
			this->xy2[1] = xy2.y;
		}

		this->weight = 0.0;
	}
	
	template<typename T>
	ofVec2f Correspondence_<T>::getXY1() const {
		return ofVec2f(xy1[0], xy1[1]);
	}

	template<typename T>
	ofVec2f Correspondence_<T>::getXY2() const {
		return ofVec2f(xy2[0], xy2[1]);
	}

	template class Correspondence_<float>;
	template class Correspondence_<double>;
}