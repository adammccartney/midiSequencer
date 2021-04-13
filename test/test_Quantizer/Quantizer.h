/* Quantizer.h: sets up a simple pitch quantizer 
 *
 * input: midi note in the range 0-127
 * process: use floor function to find nearest midi note in
 * *PitchManager->pitchClassSet, 
 * then transposes pitch -12 in order to find relevant octave 
 * *PitchManager->pitchSet
 * output: midi note in the range 0-127
 *
 */

#pragma once

#include <vector>
#include <map>

using namespace::std;

//----------------------------------------------------------------------------
// constants

#define MIDCOFFSET 60;
#define MIDIMAX 127;

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
    // numbered according to midi note values in the middle c octave (60-72)
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
    NumberedPitch(int v) : val { v }{ }
    // compiler makes simple destructor by default
    
    void init();
    
    int asInt() { return val; }

    void transpose(int n) { val += n; }
    
    int operator==(NumberedPitch n);

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
    HarmonicField(vector<NumberedPitch> &pdata) : pitchset { pdata } {}

    //void setup();
    //void update();

    //int searchPitch(NumberedPitch &inpitch);
    //int findPitch(const NumberedPitch &inpitch);
    NumberedPitch getQuantizedPitch(NumberedPitch &inpitch);

private:
    vector<NumberedPitch> pitchset;
    //vector<NumberedPitch>::iterator p;

};

