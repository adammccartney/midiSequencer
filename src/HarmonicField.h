#pragma once

#include "ofMain.h"
#include <vector>


class Timespan{
   // timespan acts mostly as a reference to the HarmonicFields

public:

   Timespan(const int nhf);
   ~Timespan();

   void draw();

   float getLength();
   ofVec2f getStart() { return _start; }
   ofVec2f getEnd() { return _end; }

   const int numHfields;

private:
   ofVec2f _start;
   ofVec2f _end;

};

class HarmonicFieldGraph{

public:
    HarmonicFieldGraph(char n, Timespan& ts); 
    ~HarmonicFieldGraph();

    void setup();
    void draw();
    void update();

    void updatePoints();

    ofParameterGroup params;
    ofParameter<int> x;
    ofParameter<int> y;

    vector<vector<ofVec2f>> keypoints; // each key contains 4 ofVec2f points
    char name;
    Timespan& timespan;
    string toString();
    void setID();
    int getID();
    
    float hfxOrigin;
    float localMin;
    float localMax;
    float length;

private:
    const float _width = 20.0;
    const float _height = 5.0;
    const int _numtones = 12; // 12 chromatic tones
    int _id;

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
