#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(100, 100, 100);
	ofEnableSmoothing();
	camera.setCursorDraw(true);

	data.insert(pair<string, float&>("projector", projectorSelection));
	data.insert(pair<string, float&>("position.x", position.x));
	data.insert(pair<string, float&>("position.y", position.y));
	data.insert(pair<string, float&>("position.z", position.z));
	data.insert(pair<string, float&>("resolution.x", resolution.x));
	data.insert(pair<string, float&>("resolution.y", resolution.y));
	data.insert(pair<string, float&>("throwRatio", throwRatio));
	data.insert(pair<string, float&>("lensOffset.x", lensOffset.x));
	data.insert(pair<string, float&>("lensOffset.y", lensOffset.y));
	dataCursor = data.begin();
	
	position = ofVec3f(1.0f, 0.0f, 3.0f);
	resolution = ofVec2f(1024, 768);
	throwRatio = 2.0f;
	lensOffset = ofVec2f(0.0f, 0.5f);
	
	//insert dataPoints into a defined region
	const ofVec3f lbf(-2.0f, -1.0f, -1.0f);
	const ofVec3f rtb(2.0f, 1.0f, 1.0f);
	const ofVec2f blockSize(0.02f, 0.05f);
	const int pointsPerBlockXY = 4;
	const ofVec2f blockSizeStep = blockSize / pointsPerBlockXY;
	const int blocks = 1000;
	ofVec3f block;
	dataPoints.clear();
	for (int i=0; i<blocks; i++) {
		block = ofVec3f(ofRandom(lbf.x, rtb.x), ofRandom(lbf.y, rtb.y), ofRandom(lbf.z, rtb.z));
		for (float x=-blockSize.x; x<blockSize.x; x+= blockSizeStep.x)
			for (float y=-blockSize.y; y<blockSize.y; y+= blockSizeStep.y)
				if (ofRandom(1.0f) > 0.7f)
					dataPoints.addVertex(block + ofVec3f(x, y, 0));
	}

	updateProjector();

	ofSetLogLevel("ofxProCamSolver", OF_LOG_NOTICE);
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
	camera.begin();
	
	ofPushStyle();
	ofSetColor(200, 100, 100);
	ofDrawGrid(10,5,true);
	ofPopStyle();

	for (int i=0; i<PROJECTOR_COUNT; i++) {
		projector[i].draw();
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(projector[i].getPosition());
		ofScale(0.01,-0.01,0.01);
		ofSetColor(255);
		ofDrawBitmapString(ofToString(i),0,5);

		if (floor(projectorSelection + 0.5f) == i)
			ofRect(-5, -5, 10, 2);
		ofPopMatrix();
		ofPopStyle();

	}

	dataPoints.drawVertices();
	camera.end();
	
	drawSelection();
}

//--------------------------------------------------------------
void testApp::drawSelection() {
	int nItems = data.size();
	string instruction = "[UP]/[DOWN] to move selection, [LEFT]/[RIGHT] to edit";
	
	int stringWidth = 0;
	map<string, float&>::iterator it;
	for(it = data.begin(); it != data.end(); it++) {
		stringWidth = MAX(stringWidth, (it->first).length());
	}
	
	ofPushStyle();
	ofFill();
	ofSetColor(200, 100, 100);
	int y = ofGetHeight() - nItems*10 - 100;
	ofRect(20, y, MAX(stringWidth, instruction.length()) * 8 + 40, nItems*10 + 50 + 20);
	
	ofSetColor(255, 255, 255);
	y += 20;
	ofDrawBitmapString(instruction, 40, y);
	y += 20;
	ofSetColor(200, 200, 200);
	string result;
	for(it = data.begin(); it != data.end(); it++) {
		result = dataCursor == it ? "> " : "  ";
		result += it->first;
		while (result.length() < stringWidth+4)
			result.push_back(' ');
		result += ofToString(it->second, 2);
		ofDrawBitmapString(result, 40, y);
		y += 10;
	}
	y+= 10;
	ofSetColor(255);
	ofDrawBitmapString("[q] = save correspondence file", 40, y);
	y+=10;
	ofDrawBitmapString("[s] = solve", 40, y);
	ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if (key==OF_KEY_LEFT)
		dataCursor->second += -0.1;
	if (key==OF_KEY_RIGHT)
		dataCursor->second += +0.1;
	
	if (key==OF_KEY_UP) {
		if (dataCursor == data.begin())
			dataCursor = data.end();
		dataCursor--;
	}
	if (key==OF_KEY_DOWN) {
		dataCursor++;
		if (dataCursor == data.end())
			dataCursor = data.begin();
	}
	
	updateProjector();
	
	if (key=='q')
		this->saveCorrespondences();
	if (key=='s')
		this->solve();

	if (key=='c')
		camera.toggleCursorDraw();
	if (key=='f')
		ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::updateProjector()	{
	for (int i=0; i<PROJECTOR_COUNT; i++) {
		projector[i].setWidth(this->resolution.x);
		projector[i].setHeight(this->resolution.y);
		projector[i].setProjection(this->throwRatio, this->lensOffset);
		projector[i].setPosition(this->position + ofVec3f(ofMap(i, 0, PROJECTOR_COUNT, -2.0f, 2.0f), 0,0));
		//projector[i].lookAt(ofVec3f(0,0,0));
	}
}

//--------------------------------------------------------------
void testApp::solve() {
	for (int i=0; i<PROJECTOR_COUNT; i++) {
		cout << "Projector " << i << endl;
		cout << projector[i].getViewProjectionMatrix() << endl << endl;
	}
	this->makeCorrespondences();
	ofxProCamSolver::Solver solver;
	solver.solve(this->correspondences);
}

//--------------------------------------------------------------
void testApp::makeCorrespondences() {
	uint32_t count = dataPoints.getNumVertices();

	correspondences.clear();
	const ofVec3f *point = dataPoints.getVerticesPointer();
	ofVec2f* xy = new ofVec2f[PROJECTOR_COUNT];

	ofRectangle bounds(-1,-1,2,2);
	for (int i=0; i<count; i++, point++) {
		for (int j=0; j<PROJECTOR_COUNT; j++)
			xy[j] = projector[j].getViewProjectionMatrix() * *point;

		for (int j=0; j<PROJECTOR_COUNT; j++)
			for (int k=j+1; k<PROJECTOR_COUNT; k++)
				if (bounds.inside(xy[j]) && bounds.inside(xy[k]))
					if (ofRandom(1.0f) > 0.7)
						correspondences.push_back((Correspondenced)Correspondencef(j, xy[j], k, xy[k]));
	}

	delete[] xy;

}
//--------------------------------------------------------------
void testApp::saveCorrespondences() {
	makeCorrespondences();

	uint32_t count = correspondences.size();

	ofstream fileOut;
	fileOut.open(ofToDataPath("correspondences"), ios::binary);
	fileOut.write((char*)&count, sizeof(uint32_t));
	fileOut.write((char*)&correspondences[0], sizeof(Correspondenced) * count);
	fileOut.close();

	for (int i=0; i<PROJECTOR_COUNT; i++) {
		cout << "--------" << endl;
		cout << "Camera " << i << endl;
		cout << "--------" << endl;
		cout << projector[i].getViewProjectionMatrix() << endl;
		cout << "--------" << endl;
		cout << endl;
	}
}