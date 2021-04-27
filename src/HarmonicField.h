#pragma once

#include "ofMain.h"
#include <vector>
#include <map>
#include <queue>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <iterator>

//----------------------------------------------------------------------------
// constants used for range checking and making default constructors

    
inline extern const int MIDCOFFSET { 60 };
inline extern const int MIDIMAX { 127 };
inline extern const int MINFIELDS { 1 };
inline extern const char DNAME { 'X' };


//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

class TimespanGraph{
   // timespan acts mostly as a reference to the HarmonicFields

public:

   TimespanGraph(const int numharmonicfields);
   TimespanGraph();
   // compiler generates default destructor

   void draw();

   float getLength();
   ofVec2f getStart() { return _start; }
   ofVec2f getEnd() { return _end; }

   const int numhfields;

private:
   ofVec2f _start;
   ofVec2f _end;
   float _len;
};

//-----------------------------------------------------------------------------
// constant for default timespan, needed for harmonifieldgraph default
// constructors. It makes sense to do this because if we don't insist that
// there is a sensible default, there will be a bunch of divide by null or 0
// errors 

inline extern TimespanGraph TSGDEFAULT { 1 };

//-----------------------------------------------------------------------------
//

class HarmonicFieldGraph{

public:
    HarmonicFieldGraph(char n, TimespanGraph& ts); 
    HarmonicFieldGraph();
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
    int getXPos() { return x; } 
    int getYPos() { return y; }
    
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

PitchClass operator++(PitchClass& pc);
PitchClass operator--(PitchClass& pc);

//----------------------------------------------------------------------------
//

class NumberedPitchClass {

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
    
public:
    // numbered according to midi note values (0, 127) = (C-2, G8)
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
// Note

class Note{

public:

    Note(NumberedPitch &np, float rtime, float prob)  // relative temporal position 
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
    float _rtime;
    float _probability;

};

//-----------------------------------------------------------------------------
//TODO: make this generic so it can be used for floats (microtones) too

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
// Larger Classes used to as types of factories and managers for coordinating
// lower level logical types (pitch classes, pitches, notes, intervals) with
// harmonic information. 
// These are the core logic classes of the app and do most of the heavy
// lifting. 
//

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
    const vector<int> OCTAVE_OFFSETS {0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120};
};

//-----------------------------------------------------------------------------
// Logic class for Quantizer Fields
//

class HarmonicField{

public:
    HarmonicField(vector<NumberedPitch> &pdata) 
        : _pitchset { pdata } {}

    //void setup(); // intialize on opening app, listen to gui
    //void draw();  // update according to changes from gui 
    
    const vector<NumberedPitch> getPitchSet() const { return _pitchset; }

private:
    vector<NumberedPitch> _pitchset;

};


//-----------------------------------------------------------------------------
// Pitch Quantizer 

class HarmonicFieldManager{

    public:
        HarmonicFieldManager();
        HarmonicFieldManager(HarmonicField &hfield, HarmonicFieldGraph &hfgraph);
        NumberedPitch getQuantizedPitch(NumberedPitch inpitch);
        int getProb() { return _probability; }
        int getRtime() { return _rtime; }

        void setup() { _hfgraph.setup(); }

    private:
        HarmonicField _hfield;
        HarmonicFieldGraph _hfgraph;
        vector<NumberedPitch> _pitchset;
        int _probability;
        int _rtime;    // x position as proportion of timespan

};

//-----------------------------------------------------------------------------
// Manager for pitch quantizer 
//

class QuantizedPitchManager{
    // Coordinates the incoming and outgoing messages
    //
public:
    QuantizedPitchManager(const vector<HarmonicFieldManager> &hfm);
    QuantizedPitchManager();

    void setup() { for(int i = 0; i < numFields(); i++) _vhfm[i].setup(); }
    
    //void draw();  // create an array of four quantized pitches on the heap
                    // make sure these get destroyed once they are sent via osc
    
    void processMidiNote(/*const ofxMidiIn &in*/const int &midiVal); // test with midi note 
    NumberedPitch getOriginalPitch() { return _pitch; }
    int numFields(){ return _numfields; }
    NumberedPitch getQuantizedPitch(int hfindex, NumberedPitch p);

    Note getNote() { return _notes.front(); }
    void popNote(); 

    int getRval(int n); // gets xcoord of nth harmonic field graph
    int getProb(int n); // gets ycoord of nth harmonic field graph

private:
    Note makeNote(NumberedPitch &p, float &prob, float &rtime);
    NumberedPitch _pitch;
    int _numfields; // set by size of constructor's vector
    vector<HarmonicFieldManager> _vhfm; 
    queue<Note> _notes;
};


