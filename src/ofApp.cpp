#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(120);
    ofSetFrameRate(60);


    const int nhfields = hfgrphs.size() + 1; // setting up this constant  
    timespan.setNumHfields(nhfields); // critical function, cleanup?
    
    for(auto i = 0; i < hfgrphs.size(); i++){
        // set up some naming conventions for hfield parameters
        hfgrphs[i]->timespan = &timespan;
        hfgrphs[i]->setup();
        mainGroup.add(hfgrphs[i]->params);
    }
    gui.setup(mainGroup);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    timespan.draw();
    for(auto i = 0; i < hfgrphs.size(); i++){
        hfgrphs[i]->draw();
    }
    gui.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
