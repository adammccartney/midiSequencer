#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include <vector>
#include <map>
#include <queue>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <iterator>

//----------------------------------------------------------------------------
// see near end of file for constants

//----------------------------------------------------------------------------
// PitchClass and Mode enums: basic elements for all further construction
//
enum Mode { 
// Musical Mode refers to an abstract set of harmonic proportions.
// This list is somewhat arbitrary, it features the 7 common church modes, the
// harmonic minor and then a selection of specific tonal harmonies that are
// named according to Figured Bass logic 
    MAJOR = 0, MINOR, HMINOR, DORIAN, PHRYGIAN, LYDIAN, MIXOLYDIAN, 
    LOCRIAN, MAJ7, DOM7, MAJ6, MIN6, SIX4, MINSIX4, SIX5, MINSIX5,
    FIVE4, FOUR2, MODECOUNT };

enum PitchClass {
// PitchClass is a common musical term, it refers to the 12 chromatic pitches,
// starting at c=0 and ascending until b=11
    c, cs, d, ds, e, f, fs, g, gs, a, as, b
};

PitchClass operator++(PitchClass& pc);
PitchClass operator--(PitchClass& pc);


//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

class TimespanGraph{
   // TimespanGraph is a horizontal line that is used to calculate the
   // positioning for the Harmonic Field Graph instances  

public:

   TimespanGraph(const int numharmonicfields);
   TimespanGraph();
   // compiler generates default destructor

   void draw();

   float getLength() const;
   ofVec2f getStart() const { return _start; }
   ofVec2f getEnd() const { return _end; }

   const int numhfields;

private:
   ofVec2f _start;
   ofVec2f _end;
   float _len;
};


//-----------------------------------------------------------------------------
//

class HarmonicFieldGraph{
// Allows user to visualize their input as they set parameters for musical mode
// and root, along with rhythmic and probabilistic information 

public:
    HarmonicFieldGraph(char n, TimespanGraph& ts); 
    HarmonicFieldGraph();
    // compiler generates default destructor

    void setup();
    void draw();
    void update();

    void updateHarmonicFandFill();

    ofParameterGroup params;
    ofParameter<int> x;
    ofParameter<int> y;
    ofParameter<int> mode;
    ofParameter<int> root;

    vector<vector<ofVec2f>> keypoints; // a (pianoroll) key has 4 ofVec2f pts 
    char name;
    const TimespanGraph& timespan;
    string toString();
    void setID();
    int getID() { return _id; }
    float getWidth() { return _width; }
    int getXPos() { return x; } 
    int getYPos() { return y; }
    int getMode(){ return mode; }
    int getRoot(){ return root; }
    void setFillData(const vector<int> &filldata);

    float hfxOrigin;
    float localMin;
    float localMax;
    float length;

private:
    const float _width = 20.0;
    const float _height = 5.0;
    const int _numtones = 12; // 12 chromatic tones
    int _id;
    vector<bool> _filldata {false, false, false, false, false, false, 
        false, false, false, false, false, false};

};

//----------------------------------------------------------------------------
// Pitch and Interval Datatypes 
//

class NumberedPitchClass {
// Refers to one of twelve possible chromatic pitch classes 

public:
    // numbered according to midi note values in the middle c octave (60-72)
    NumberedPitchClass(PitchClass pc) : name { pc }{ init(); }
    NumberedPitchClass();
    // compiler makes simple destructor by default
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
// Refers to one of 128 possible pitches: midi note values (0, 127) = (C-2, G8)
    
public:
    NumberedPitch(int v) : _val { v }{ }
    NumberedPitch();
    // compiler makes simple destructor by default
    
    void init();

    void setVal(int p) { _val = p; } // candidate for template function
    void setPitch(NumberedPitch p) { _val = p.asInt(); }
    
    int asInt() const { return _val; }

    void transpose(int n);
    
    bool operator==(NumberedPitch n);

private:
    int _val;
};

//-----------------------------------------------------------------------------

class Note{
// Note: this is an important class for the output, as it is used to store
// information about the incoming midi note after it has been quantized, along
// with the x y position data that is taken from the position of harmonic field
// graphs on the gui. this x y data is intended to be used as timing and
// probability info
public:

    Note(NumberedPitch &np, int rtime, int prob)  // relative temporal position 
        : _pitch { np }, _rtime { rtime }, _probability { prob } { } 
    Note();

    Note(const Note& n); // copy constructor 
    Note& operator=(const Note& n);

    void setAll(NumberedPitch &p, float &rtime, float &prob);
    NumberedPitch getPitch() const { return _pitch; }
    auto getRtime() const  { return _rtime; }
    auto getProb() const { return _probability; }
    string toString();

private:
    NumberedPitch _pitch;
    int _rtime;
    int _probability;

};

//-----------------------------------------------------------------------------
// Classes used to set up on of the basic construction components for musical
// scales. A scale is constructed using a set of intervals (represented by
// integers) and a specific root note (represented by a PitchClass

class IntervalSegmentManager{
// Interval segment manager: used to handle the interval distribution needed to
// construct a harmonic field in a specific tonality.
// i.e. The interval values stored here are an abstraction and describe a path
// of n steps through an octave. 

public:
    IntervalSegmentManager();
    static map <Mode, vector<int>> modeIntervalMap;

private:
    static void makeMap();
};

struct IntervalSegment {
    // Interval Segment is simply a wrapper for an integer vector,
    // this works closely with the IntervalSegmentManager
    // construct using an integer list (number of semitones)
    IntervalSegment(vector<int> v);
    IntervalSegment();
    //IntervalSegment(const IntervalSegment& is);
    vector<int> intervals;
};

//-----------------------------------------------------------------------------
// Larger Classes used to as types of factories and managers for coordinating
// lower level logical types (pitch classes, pitches, notes, intervals) with
// harmonic information. 
// These are the core logic classes of the app and do most of the heavy
// lifting. 
//

class PitchSetManager{
// handles construction of pitch and pitch class sets, given ref to root & mode
public:
    // Interface class to manage the subset of relevant steps for quantizer
    PitchSetManager(PitchClass &root, IntervalSegment &mode);
    PitchSetManager();
    PitchSetManager(const PitchSetManager& n); // copy constructor 
    PitchSetManager& operator=(const PitchSetManager& n);

    void setRoot(PitchClass &root) { _root = root; }
    void setIntseg(IntervalSegment &mode) { _mode = mode; }
    void init(PitchClass &root, IntervalSegment &intseg);
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
    const vector<int> OCTAVE_OFFSETS {0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120};
};

////-----------------------------------------------------------------------------
//// Logic class for Quantizer Fields
////

class HarmonicField{
// provides access to sets of NumberedPitchClass and NumberedPitch 
public:
    HarmonicField(PitchSetManager &psman) 
        : _psman { psman } {}
    HarmonicField();

    void init(PitchSetManager &psman) { _psman = psman; }

    vector<NumberedPitch> getPitchSet() { return _psman.getPitchSet(); }
    vector<NumberedPitchClass> getPitchClassSet() { return _psman.getPitchClassSet(); }

private:
    PitchSetManager _psman;

};

//-----------------------------------------------------------------------------

class HarmonicFieldManager{
// quantize a Pitch according to field attributes
    public:
        HarmonicFieldManager(HarmonicField &hfield, HarmonicFieldGraph &hfgraph);
        HarmonicFieldManager();
        HarmonicFieldManager& operator=(const HarmonicFieldManager& n);
        
        NumberedPitch getQuantizedPitch(NumberedPitch inpitch);
        int getProb() { return _probability; }
        int getRtime() { return _rtime; }
        void setFillData();
        vector<int> getFillData() { return _filldata;  }
        void init(HarmonicField &hfield, HarmonicFieldGraph &hfgraph);

    private:
        HarmonicField _hfield;
        HarmonicFieldGraph _hfgraph;
        vector<NumberedPitch> _pitchset;
        vector<NumberedPitchClass> _pitchclassset;
        int _probability;
        int _rtime;    // x position as proportion of timespan
        vector<int> _filldata {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

};

////-----------------------------------------------------------------------------
//// Manager for pitch quantizer 
////

class QuantizedPitchManager{
// Coordinate the incoming (midi) and outgoing (osc) messages
public:
    QuantizedPitchManager(const vector<HarmonicFieldManager*> &hfm);
    QuantizedPitchManager();

    void processMidiNote(const int &midiVal, ofxOscMessage &m); // test with midi note 
    NumberedPitch getOriginalPitch() { return _pitch; }
    NumberedPitch getQuantizedPitch(int hfindex, NumberedPitch p);

    Note getNote() { return _notes.front(); }
    void popNote() { _notes.pop(); } 

    int getRval(int n); // gets xcoord of nth harmonic field graph
    int getProb(int n); // gets ycoord of nth harmonic field graph

private:
    Note makeNote(NumberedPitch &p, float &prob, float &rtime);
    NumberedPitch _pitch;
    int _numfields; // set by size of constructor's vector
    const vector<HarmonicFieldManager*> _vhfm; 
    queue<Note> _notes;                
};


//----------------------------------------------------------------------------
// constants used for range checking and making default constructors
namespace constants
{
const int GLOBAL_CONST_MIDIMAX { 127 };
const int GLOBAL_CONST_CHROMA  { 12 };
const int GLOBAL_CONST_NUM_HFIELDS { 4 };
const TimespanGraph GLOBAL_CONST_DEFTIMESPAN {};
const char GLOBAL_CONST_HFGNAME { 'X' };
} // namespace constants


