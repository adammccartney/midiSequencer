#pragma once

#include "ofMain.h"
#include <vector>
#include <map>


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
   float _len;

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

    vector<vector<ofVec2f>> keypoints; // a (pianoroll) key has 4 ofVec2f pts 
    char name;
    Timespan& timespan;
    string toString();
    void setID();
    int getID();
    float getWidth() { return _width; }
    
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

//----------------------------------------------------------------------------
//

enum class PitchClass {
    c, cs, d, ds, e, f, fs, g, gs, a, as, b
};

//----------------------------------------------------------------------------
//

struct NumberedPitch {
    // numbered according to midi note values (0, 127) = (C-2, G8)
    NumberedPitch(int v) : val { v }{}
    NumberedPitch(); // default constructor
    int val;
    void transpose(int n);
};


//-----------------------------------------------------------------------------

struct PitchClassSegment {
    PitchClassSegment(vector<PitchClass> pc) : pclasses { pc } {}
    PitchClassSegment(); // default
    vector<PitchClass> pclasses;
};

//-----------------------------------------------------------------------------

struct PitchSegment {
    PitchSegment(vector<NumberedPitch> p) : pitches { p } {}
    PitchSegment(); // default
    vector<NumberedPitch> pitches;
    void transpose(int n); // transpose pitches by n semitones
};

//-----------------------------------------------------------------------------
//

//TODO: make this generic so it can be used for floats (microtones) too
//TODO: test this to make sure it works as expected
//
struct IntervalSegment{
    // construct using an integer list (number of semitones)
    IntervalSegment(vector<int> v) : intervals { v } {}
    vector<int> intervals;
};

class IntervalSegmentManager{
    
    using ModeIntervalMap = std::map<string, IntervalSegment>;

public:

    IntervalSegmentManager();
    IntervalSegment getIntervalSegment(string mode) const;    

private:
    static ModeIntervalMap mintMap;
};

//-----------------------------------------------------------------------------

class MidiPitchContainer{
    // construct using a root note and interval segment
    MidiPitchContainer(const PitchClass &root, const IntervalSegment &mode);

    MidiPitchContainer get();

    void makePitchClassSegment();
    void makePitchSegment();
    PitchClassSegment getPitchClassSegment();
    PitchSegment getPitchSegment();

private:    
    PitchClass _root;
    IntervalSegment _mode;
    int _minpitch;
    PitchClassSegment pclasses; // this will hold the info for graphic
    PitchSegment pitches;       // this holds data for sonic
    const int MIDCOFFSET = 60;
};


//-----------------------------------------------------------------------------
//

class HarmonicField{

public:
    HarmonicField();
    ~HarmonicField();

    void setup();
    void update();

    void makeScale(vector<float> intervalsegment);
    vector<float> getIntervalSegment(string modename);
    
    bool state;
    vector <float> scale;
    vector <int> pitches; // these are going to be midi vals
    float root;
    string mode;

};

