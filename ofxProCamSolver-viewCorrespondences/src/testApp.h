#pragma once


// NOTICE!!!!!!!!!!!
//due to name conflicts
//	ofxProCamSolver MUST BE ABOVE
//	ofMain in your includes
#include "ofxProCamSolver.h"

#include "ofMain.h"

#include <stdint.h>

using namespace ofxProCamSolver;

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
protected:
	void load();
	CorrespondenceSetd dataSet;
	set<int> cameraIndices;
	
	set<int>::iterator indexLeft, indexRight;
};
