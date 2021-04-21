#pragma once

#include "ofMain.h"
#include <vector>
#include <map>

//----------------------------------------------------------------------------
// constants

    
inline extern const int MIDCOFFSET { 60 };
inline extern const int MIDIMAX { 127 };


//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

class TimespanGraph{
   // timespan acts mostly as a reference to the HarmonicFields

public:

   TimespanGraph(const int numharmonicfields);
   // compiler generates default destructor

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
    HarmonicFieldGraph(char n, TimespanGraph& ts); 
    // compiler generates default destructor

    void setup();
    void draw();
    void update();

    void updatePoints();

    ofParameterGroup params;
    ofParameter<int> x;
    ofParameter<int> y;

    vector<vector<ofVec2f>> keypoints; // a (pianoroll) key has 4 ofVec2f pts 
    char name;
    TimespanGraph& timespan;
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

enum PitchClass {
    c, cs, d, ds, e, f, fs, g, gs, a, as, b
};


//----------------------------------------------------------------------------
//

class NumberedPitchClass {

public:
    // simple data class to give some functionalits to PitchClass
    NumberedPitchClass(PitchClass pc) : name { pc }{ init(); }
    // compiler makes simple constructor by default
    PitchClass name;
    void init();

    void setVal(PitchClass& pc) { _val = int(pc); }
    int asInt() { return _val; }
    
    void transpose(int n);
    
    bool operator==(NumberedPitchClass& npc);
    

private:
    int _val;
};


//----------------------------------------------------------------------------
//

class NumberedPitch {
    
public:
    // numbered according to midi note _values (0, 127) = (C-2, G8)
    NumberedPitch(int v) : _val { v }{ }
    NumberedPitch();
    // compiler makes simple destructor by default
    
    void init();
    void setVal(int p) { _val = p; }
    
    int asInt() const { return _val; } 
    void transpose(int n);
    
    bool operator==(NumberedPitch n);

private:
    int _val;
};


//----------------------------------------------------------------------------
//

class QuantizedPitch {

public:
    QuantizedPitch(NumberedPitch &np, ofParameterGroup &params);

private:
    NumberedPitch _pitch;
    ofParameterGroup _params;

};

//-----------------------------------------------------------------------------
//
//TODO: make this generic so it can be used for non 12-tet tunings

struct IntervalSegment {
    // construct using an integer list (number of semitones)
    IntervalSegment(vector<int> v) : intervals { v }{}
    //IntervalSegment(const IntervalSegment& is);
    vector<int> intervals;
};

enum Mode { MAJOR = 0, MINOR, HMINOR, DORIAN, PHRYGIAN, LYDIAN, MIXOLYDIAN, 
    LOCRIAN, MAJ7, DOM7, MAJ6, MIN6, SIX4, MINSIX4, SIX5, MINSIX5,
    FIVE4, FOUR2, MODECOUNT };

class IntervalSegmentManager{

public:
    IntervalSegmentManager();
    static map <Mode, vector<int>> modeIntervalMap;

private:
    static void makeMap();
};

//-----------------------------------------------------------------------------

class PitchSetManager{

public:
    // Interface class to manage the subset of relevant steps for quantizer
    PitchSetManager(const PitchClass &root, const IntervalSegment &mode);

    void makePitchClassSet();
    void makePitchSet();

    vector<NumberedPitchClass> getPitchClassSet() { return _pcset; }
    vector<NumberedPitch> getPitchSet() { return _pitches; }

private:    
    PitchClass _root;
    IntervalSegment _mode;
    int _minpitchval;
    vector<NumberedPitchClass> _pcset; // this will hold the info for graphic
    vector<NumberedPitch> _pitches;       // this holds data for sonic
    const vector<int> OCTAVE_OFFSETS {0, 12, 24, 36, 48, 60, 72, 84, 96, 108};
};

//-----------------------------------------------------------------------------
// Logic class for Quantizer Fields
//

class HarmonicField{

public:
    // test constructor
    HarmonicField(vector<NumberedPitch> &pdata) : _pitchset { pdata } {}
    // constructor should work with a reference to the HarmonicFieldGraph 
    // 
    // HarmonicField(const HarmonicFieldGraph &hfgraph); 
    
    const vector<NumberedPitch> getPitchSet() const { return _pitchset; }

private:
    vector<NumberedPitch> _pitchset;

};

//-----------------------------------------------------------------------------
// Pitch Quantizer 
//
class PitchQuantizer{

public:
    PitchQuantizer(const HarmonicField &hfield);
    NumberedPitch getQuantizedPitch(const NumberedPitch &inpitch);

private:
    const vector<NumberedPitch> _pitchset;

};

//-----------------------------------------------------------------------------
// Manager for pitch quantizer 
//

class QuantizedPitchManager{
    // Listens for incoming midi notes, upon the arrival of a new note, 
    // runs the draw() function to create 
public:
    void setup(); // set up listeners
    void draw();  // quantize pitches and route to output

private:
    const vector<PitchQuantizer> _pquantizers;

};

//-----------------------------------------------------------------------------
// Initialization steps for creating a group of quantized notes
//
// 1. IntervalSegmentManager ism; // this is a static class
// 2. IntervalSegment major{ism.modeIntervalMap[MAJOR]};
// 3. PitchSetManager pcman{PitchClass::c, major}; 
// 4. HarmonicField hfield{pcman.getPitchSet()};
// 5. PitchQuantizer pq{hfeld};
// 6. NumberePitch newpitch = pq.getQuantizedPitch();
//
