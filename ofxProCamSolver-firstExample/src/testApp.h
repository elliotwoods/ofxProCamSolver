#pragma once


// NOTICE!!!!!!!!!!!
//due to name conflicts
//	ofxProCamSolver MUST BE ABOVE
//	ofMain in your includes
#include "ofxProCamSolver.h"


#include "ofMain.h"

#include "ofxGrabCam.h"
#include "ofxRay.h"

#include <stdint.h>

#define PROJECTOR_COUNT 5
#define BLOCK_COUNT 400

using namespace ofxProCamSolver;

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void drawSelection();

	void keyPressed  (int key);
	void updateProjector();
	
	ofxGrabCam camera;
	map<string, float&> data;
	map<string, float&>::iterator dataCursor;
	
	ofProjector projector[PROJECTOR_COUNT];
	vector<ofVec2f> pixels;


	////
	//correspondences
	////
	//
	void solve();
	void makeCorrespondences();
	void saveCorrespondences();
	ofMesh dataPoints; // points in 3d to be used to generate correspondences
	CorrespondenceSetd correspondences;
	//
	////


	//settings
	float projectorSelection;
	ofVec3f position;
	ofVec2f resolution;
	float throwRatio;
	ofVec2f lensOffset;

};
