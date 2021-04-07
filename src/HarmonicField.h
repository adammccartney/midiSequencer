#pragma once

#include "ofMain.h"
#include <vector>


class HarmonicFieldGraph{

public:
    HarmonicFieldGraph();
    ~HarmonicFieldGraph();

    void setup();
    void draw();
    void update();

    ofParameterGroup params;
    ofParameter<int> x;
    ofParameter<int> y;

    //HarmonicField* field;
    vector<vector<ofVec2f>> keypoints; // each key contains 4 ofVec2f points
    void setTimespanIntersect(float& xTimespan);

private:
    const float _width = 20.0;
    const float _height = 5.0;
    const int _numtones = 12; // 12 chromatic tones

};

/*
class HarmonicField{

public:
    HarmonicField();
    ~HarmonicField();
    void setup();
    void update();
    void makescale(vector<float> intervalsegment);
    vector<float> getintervalsegment(string modename);
    
    bool state;
    vector <float> scale;
    float relpos;
    float root;
    string mode; 
};
*/
