/*
 * PitchManger.h
 *
 * description: creates and manages a set containing the discrete pitches 
 *
 * input: *PitchField
 * process: read pitchclasses, make set within midi note bounds (0-127)
 * output: none
 *
 * */
#pragma once

#include <vector>
#include <map>

using namespace::std;

//----------------------------------------------------------------------------
// constants

#define MIDCOFFSET 60;

//----------------------------------------------------------------------------
// Pitch and Interval Datatypes 
//

enum class PitchClass {
    c, cs, d, ds, e, f, fs, g, gs, a, as, b
};

//----------------------------------------------------------------------------
//

class NumberedPitchClass {

public:
    // numbered according to midi note values in the middle c octave (0-11)
    NumberedPitchClass(PitchClass pc) : name { pc }{ init(); }
    // compiler makes simple constructor by default
    PitchClass name;
    void init();
    
    int asInt() { return val; }

    void transpose(int n) { val += n; }

private:
    int val;
};


//----------------------------------------------------------------------------
//

class NumberedPitch {
    
public:
    // numbered according to midi note values (0, 127) = (C-2, G8)
    NumberedPitch(int v) : val { v }{ init(); }
    // compiler makes simple destructor by default
    
    PitchClass name;
    void init();
    
    int asInt() { return val; }

    void transpose(int n) { val += n; }

private:
    int val;
};


//-----------------------------------------------------------------------------

//struct PitchClassSet {
//    PitchClassSet(vector<NumberedPitchClass> pc) : pclasses { pc }{}
//    PitchClassSet();
//    // compiler makes simple destructor by default
//    vector<NumberedPitchClass> pclasses;
//};
//
////-----------------------------------------------------------------------------
//
//struct PitchSet {
//    PitchSet(vector<NumberedPitch> p) : pitches { p } {}
//    PitchSet();
//    // compiler makes simple constructor by default
//    
//    vector<NumberedPitch> pitches;
//};

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
    int _minpitch;
    vector<NumberedPitchClass> _pcset; // this will hold the info for graphic
    vector<NumberedPitch> _pitches;       // this holds data for sonic
    const vector<int> OCTAVE_OFFSETS {0, 12, 24, 36, 48, 60, 72, 84, 96, 108};
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

