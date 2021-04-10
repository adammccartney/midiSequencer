#pragma once

#include "ofMain.h"
#include <vector>
#include <map>


//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

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
// Pitch and Interval Datatypes 
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
    void transpose(int n) { val += n; }
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

struct IntervalSegment {
    // construct using an integer list (number of semitones)
    IntervalSegment(vector<int> v) : intervals { v }{}
    //IntervalSegment(const IntervalSegment& is);
    vector<int> intervals;
};

enum Mode { MAJOR = 0, MINOR, HMINOR, DORIAN, PHRYGIAN, LYDIAN, MIXOLYDIAN, 
    LOCRIAN, MAJ7, DOM7, MAJ6, MIN6, SIX4, MINSIX4, SIX5, MINSIX5,
    FIVE4, MINFIVE4, FOUR2, DIMFOUR2, MODECOUNT };

class IntervalSegmentManager{

public:
    IntervalSegmentManager();
    static map <Mode, vector<int>> modeIntervalMap;

private:
    static void makeMap();
};

//-----------------------------------------------------------------------------

class PitchManager{
    // Interface class to manage the allocation of pitch data
    PitchManager(const PitchClass &root, const IntervalSegment &mode);

    PitchManager get();

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
// Logic class for Quantizer Fields
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

