#include "HarmonicField.h"

HarmonicFieldGraph::HarmonicFieldGraph(){}
HarmonicFieldGraph::~HarmonicFieldGraph(){}

void HarmonicFieldGraph::setup(){
    // point for x origin is where field crosses timespan 
    // setup involves first getting the x coordinate for all ofPath
    // this x coordinate is the point of intersection with the timespan
    params.setName("hfield params");
    params.add(x.set("x", ofGetWidth() / 2, 0, ofGetWidth()));
    params.add(y.set("y", ofGetHeight() / 2, 0, ofGetHeight())); 
    
    // this part is setup of the basic keyboard shape
    for(int i = 0; i < _numtones; i++){
        keypoints[i][0] = ofVec2f(x, y - i*10);
        keypoints[i][1] = ofVec2f(x + _width, y - i*10);
        keypoints[i][2] = ofVec2f(x + _width, y - i*10 - _height);
        keypoints[i][3] = ofVec2f(x, y - i*10 - _height);
    }
}

/*
// old drawing loop
for(int i = 0; i < _numtones; i++){
        ofPath npath;
        npath.moveTo(x, y - i*10);
        npath.lineTo(x + _width, y - i*10);
        npath.lineTo(x + _width, y - i*10 - _height);
        npath.lineTo(x, y - i*10 - _height);
        npath.close();
        // color according to piano keyboard
        if((i == 1) || (i == 3) || (i == 6) || (i == 8) || (i == 10)){
            npath.setStrokeColor(ofColor::black);
            npath.setFillColor(ofColor::black);
            npath.setFilled(true);
            npath.setStrokeWidth(2);
        }else{
            npath.setStrokeColor(ofColor::black);
            npath.setFillColor(ofColor::white);
            npath.setFilled(true);
            npath.setStrokeWidth(2);
        }
        tones.push_back(npath);
    }
*/

void HarmonicFieldGraph::draw(){
    
    // draw paths according to points
    for(size_t i = 0; i < keypoints.size(); i++){
        ofPath npath;
        npath.moveTo(keypoints[i][0]);
        npath.lineTo(keypoints[i][1]);
        npath.lineTo(keypoints[i][2]);
        npath.lineTo(keypoints[i][3]);
        npath.close(); 
        // color according to piano keyboard
        if((i == 1) || (i == 3) || (i == 6) || (i == 8) || (i == 10)){
            npath.setStrokeColor(ofColor::black);
            npath.setFillColor(ofColor::black);
            npath.setFilled(true);
            npath.setStrokeWidth(2);
        }else{
            npath.setStrokeColor(ofColor::black);
            npath.setFillColor(ofColor::white);
            npath.setFilled(true);
            npath.setStrokeWidth(2);
        }
        npath.draw();
    }
}

void HarmonicFieldGraph::update(){}

