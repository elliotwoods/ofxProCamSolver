#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0, 0, 0);
	ofSetWindowShape(1280, 640);
	indexLeft = cameraIndices.end();
	indexRight = cameraIndices.end();
	load();
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
	ofRectangle viewLeft = ofGetCurrentViewport();
	viewLeft.width /= 2.0f;
	ofRectangle viewRight = viewLeft;
	viewRight.x += viewLeft.width;

	ofPushStyle();
	ofNoFill();
	ofSetColor(255);
	ofRect(viewLeft);
	ofRect(viewRight);
	ofPopStyle();

	if (!(indexLeft == cameraIndices.end() || indexRight == cameraIndices.end())) {
		CorrespondenceSetd::iterator it;
		ofMesh pointsLeft, pointsRight;
		for (it = dataSet.begin(); it != dataSet.end(); it++) {
			if (it->cameraID1 == *indexLeft && it->cameraID2 == *indexRight) {
				pointsLeft.addVertex(it->getXY1());
				pointsLeft.addColor(ofFloatColor((it->xy2[0] + 1.0f) / 2.0f, (1.0f - it->xy2[1]) / 2.0f, 0));

				pointsRight.addVertex(it->getXY2());
				pointsRight.addColor(ofFloatColor((it->xy1[0] + 1.0f) / 2.0f, (1.0f - it->xy1[1]) / 2.0f, 0));
			}
		}
		
		ofPushView();
		glPointSize(5.0f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		ofViewport(viewLeft);
		pointsLeft.drawVertices();
		ofViewport(viewRight);
		pointsRight.drawVertices();

		ofPopView();
	}

	if (indexLeft != cameraIndices.end())
		ofDrawBitmapString(ofToString(*indexLeft), viewLeft.x + 10, viewLeft.height - 10);
	if (indexRight != cameraIndices.end())
		ofDrawBitmapString(ofToString(*indexRight), viewRight.x + 10, viewRight.height - 10);

	stringstream message;
	message << "Cameras {";
	set<int>::iterator it;
	for (it = cameraIndices.begin(); it != cameraIndices.end(); it++) {
		if (it != cameraIndices.begin())
			message << ", ";
		message << *it;
	}
	message << "}" << endl;
	
	ofDrawBitmapString(message.str(), 20, 30);

}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if (key == ' ' || key == 'l')
		load();

	if (cameraIndices.size() == 0) {
		indexLeft = cameraIndices.end();
		indexRight = cameraIndices.end();
		return;
	}
		

	if (key==OF_KEY_LEFT)
		if (indexLeft != cameraIndices.begin())
			indexLeft--;
	if (key==OF_KEY_RIGHT)
		if (indexLeft != cameraIndices.end()) {
			indexLeft++;
			if (indexLeft == cameraIndices.end())
				indexLeft--;
		}

	if (key==OF_KEY_UP)
		if (indexRight != cameraIndices.begin())
			indexRight--;
	if (key==OF_KEY_DOWN)
		if (indexRight != cameraIndices.end()) {
			indexRight++;
			if (indexRight == cameraIndices.end())
				indexRight--;
		}

}

//--------------------------------------------------------------
void testApp::load() {
	//load correspondences
	this->dataSet.load();

	//get view indices
	this->cameraIndices = this->dataSet.getViewIndices();

	//set selection
	indexLeft = cameraIndices.begin();
	indexRight = indexLeft;
	indexRight++;
}