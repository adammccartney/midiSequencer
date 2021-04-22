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
#include "gtest/gtest.h"

using namespace::std;

//----------------------------------------------------------------------------
// constants

inline extern const int MIDCOFFSET { 60 };
inline extern const int MIDIMAX { 127 };

//-----------------------------------------------------------------------------
// Mock of functions for testing
// N.B: The graphics classes are only here to model some behavior, it's not
// practical to wrap the whole openFrameworks application into this testing
// framework, so graphics classes are modelled here locally, the actual
// versions leverages openFrameworks and all of it's nicely contstructed types
// and functions

inline extern const int numberHFields { 4 };
inline extern const int LENGTH { 240 };
inline extern int ofGetHeight() { return 120; }
inline extern int ofGetWidth() { return 240; }

// Constants to model incoming parameters from the GUI
inline extern int xpos1 { LENGTH / 2 };
inline extern int ypos1 { LENGTH / 2 };
inline extern int xpos2 { LENGTH / 4 };
inline extern int ypos2 { LENGTH / 4 };
inline extern int xpos3 { LENGTH / 6 };
inline extern int ypos3 { LENGTH / 6 };
inline extern int xpos4 { LENGTH / 8 };
inline extern int ypos4 { LENGTH / 8 };

// Mock ofVec2f class that mimics the same in OpenFrameworks app
template<typename T>
class ofVec2f{
    public:
    ofVec2f();
    ofVec2f(T x, T y);
    string toString();
    void setX(T x);
    void setY(T y);
    T getX() { return _x;  } 
    T getY() { return _y;  }
    
    protected:
    T _x;
    T _y;
};

template<typename T>
inline ofVec2f<T>::ofVec2f(T x, T y)
{
    _x = x;
    _y = y;
}

template<typename T>
inline ofVec2f<T>::ofVec2f(){}

template<typename T>
inline string ofVec2f<T>::toString()
{
    return "x: " + to_string(_x) + " y: " + to_string(_y);
}

template<typename T>
inline void ofVec2f<T>::setX(T x)
{
    _x = x;
}

template<typename T>
inline void ofVec2f<T>::setY(T y)
{
    _y = y;
}

//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

class TimespanGraph{
   // timespan acts mostly as a reference to the HarmonicFields

public:

   TimespanGraph(const int numharmonicfields);
   // compiler generates default destructor

   //void draw();

   float getLength();
   ofVec2f<float> getStart() { return _start; }
   ofVec2f<float> getEnd() { return _end; }

   const int numHfields;

private:
   ofVec2f<float> _start;
   ofVec2f<float> _end;
   float _len;
};

class HarmonicFieldGraph{

public:
    HarmonicFieldGraph(char n, TimespanGraph& ts); 
    // compiler generates default destructor

    //void setup();
    //void draw();
    //void update();

    //void updatePoints();

    //ofParameterGroup params;
    //ofParameter<int> x;
    //ofParameter<int> y;
    int x;
    int y;

    //vector<vector<ofVec2f>> keypoints; // a (pianoroll) key has 4 ofVec2f pts 
    char name;
    TimespanGraph& timespan;
    string toString();
    void setID();
    int getID();
    float getWidth() { return _width; }

    void setPos(float _x, float _y) { x = _x; y = _y; } 
    float getXPos() { return x; } 
    float getYPos() { return y; }
    
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
// Note

class Note{

public:

    Note(NumberedPitch &np, float rtime, float prob)  // relative temporal position 
        : _pitch { np }, _rtime { rtime }, _probability { prob } { } 
    Note();

    void setAll(NumberedPitch &p, float &rtime, float &prob);
    NumberedPitch getPitch() { return _pitch; }
    float getRtime() { return _rtime; }
    float getProb() { return _probability; }

private:
    NumberedPitch _pitch;
    float _rtime;
    float _probability;

};

//-----------------------------------------------------------------------------
// Pitch Quantizer 

class HarmonicFieldManager{

    public:
        HarmonicFieldManager();
        HarmonicFieldManager(HarmonicField &hfield, HarmonicFieldGraph &hfgraph);
        NumberedPitch getQuantizedPitch(NumberedPitch inpitch);
        float getProb() { return _probability; }
        float getRtime() { return _rtime; }

    private:
        vector<NumberedPitch> _pitchset;
        float _probability;
        float _rtime;    // x position as proportion of timespan

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
    ~QuantizedPitchManager(){ /*delete[] _notes;*/ }
    
    //void draw();  // create an array of four quantized pitches on the heap
                    // make sure these get destroyed once they are sent via osc
    
    void processMidiNote(/*const ofxMidiIn &in*/const int &midiVal); // test with midi note 
    NumberedPitch getOriginalPitch() { return _pitch; }
    int numFields(){ return _numfields; }
    NumberedPitch getQuantizedPitch(int &hfindex, NumberedPitch p);

private:
    Note makeNote(NumberedPitch &p, float &prob, float &rtime);
    NumberedPitch _pitch;
    int _numfields;
    vector<HarmonicFieldManager> _vhfm; 
    vector<Note> _notes;
};


//-----------------------------------------------------------------------------
// Unit Tests 
//

class TimespanGraphTest : public::testing::Test {

protected:
    void SetUp() override {}

    // this is a gui class and visuals are currently being tested via the
    // construction of ofApp in the main function of the openframeworks program
    TimespanGraph timespanTest { numberHFields };
};

class HarmonicFieldGraphTest : public::testing::Test {

protected:
    void SetUp() override {}
    TimespanGraph tspanTest { numberHFields };
    HarmonicFieldGraph hfieldg1 {'a', tspanTest};
    HarmonicFieldGraph hfieldg2 {'b', tspanTest};
    HarmonicFieldGraph hfieldg3 {'c', tspanTest};
    HarmonicFieldGraph hfieldg4 {'d', tspanTest};
};

class NumberedPitchClassTest : public::testing::Test {

protected:
    void SetUp() override {}

    // void TearDown() override {}

    NumberedPitchClass npc1{PitchClass::c}; 
    NumberedPitchClass npc2{PitchClass::cs}; 
    NumberedPitchClass npc3{PitchClass::d}; 
    NumberedPitchClass npc4{PitchClass::ds}; 
    NumberedPitchClass npc5{PitchClass::e}; 
    NumberedPitchClass npc6{PitchClass::f}; 
    NumberedPitchClass npc7{PitchClass::fs}; 
    NumberedPitchClass npc8{PitchClass::g}; 
    NumberedPitchClass npc9{PitchClass::gs}; 
    NumberedPitchClass npc10{PitchClass::a}; 
    NumberedPitchClass npc11{PitchClass::as}; 
    NumberedPitchClass npc12{PitchClass::b}; 
};

class NumberedPitchTest : public::testing::Test {

protected:
    void SetUp() override {}

    // void TearDown() override {} // compiler handles destructor
    
    NumberedPitch np1{60};
    NumberedPitch np2{61};
    NumberedPitch np3{62};
    NumberedPitch np4{63};
};


class IntervalSegmentTest : public::testing::Test {

protected:
    void SetUp() override {}

    // void TearDown() override {}

    IntervalSegmentManager ism;
    IntervalSegment major{ism.modeIntervalMap[MAJOR]}; 
    IntervalSegment minor{ism.modeIntervalMap[MINOR]}; 
    IntervalSegment hminor{ism.modeIntervalMap[HMINOR]}; 
    IntervalSegment dorian{ism.modeIntervalMap[DORIAN]}; 
    IntervalSegment phrygian{ism.modeIntervalMap[PHRYGIAN]}; 
    IntervalSegment lydian{ism.modeIntervalMap[LYDIAN]}; 
    IntervalSegment mixolydian{ism.modeIntervalMap[MIXOLYDIAN]}; 
    IntervalSegment locrian{ism.modeIntervalMap[LOCRIAN]}; 
    IntervalSegment maj7{ism.modeIntervalMap[MAJ7]}; 
    IntervalSegment dom7{ism.modeIntervalMap[DOM7]}; 
    IntervalSegment maj6{ism.modeIntervalMap[MAJ6]}; 
    IntervalSegment min6{ism.modeIntervalMap[MIN6]}; 
    IntervalSegment six4{ism.modeIntervalMap[SIX4]}; 
    IntervalSegment minsix4{ism.modeIntervalMap[MINSIX4]}; 
    IntervalSegment six5{ism.modeIntervalMap[SIX5]}; 
    IntervalSegment minsix5{ism.modeIntervalMap[MINSIX5]}; 
    IntervalSegment five4{ism.modeIntervalMap[FIVE4]}; 
    IntervalSegment four2{ism.modeIntervalMap[FOUR2]}; 
};


class PitchManagerTest : public::testing::Test {

protected:
    void SetUp() override {} 

    const int octavest = 12;
    vector<NumberedPitchClass> npcvec;
    vector<NumberedPitch> dist;
    const int numoctaves = 2;
    int pitchval {0};
 
    NumberedPitchClass c {PitchClass::c};
    NumberedPitchClass cs {PitchClass::cs};
    NumberedPitchClass d {PitchClass::d};
    NumberedPitchClass ds {PitchClass::ds};
    NumberedPitchClass e {PitchClass::e};
    NumberedPitchClass f {PitchClass::f};
    NumberedPitchClass fs {PitchClass::fs};
    NumberedPitchClass g {PitchClass::g};
    NumberedPitchClass gs {PitchClass::gs};
    NumberedPitchClass a {PitchClass::a};
    NumberedPitchClass as {PitchClass::as};
    NumberedPitchClass b {PitchClass::b};
    
    IntervalSegmentManager ism;

    IntervalSegment major{ism.modeIntervalMap[MAJOR]}; 
    PitchSetManager cionian{PitchClass::c, major};
    vector<NumberedPitchClass> cmajorloc {c, d, e, f, g, a, b}; // used locally

    IntervalSegment hminor{ism.modeIntervalMap[HMINOR]};
    PitchSetManager cshminor{PitchClass::cs, hminor};
    vector<NumberedPitchClass> cshminorloc {cs, ds, e, fs, gs, a, c};

    IntervalSegment minor{ism.modeIntervalMap[MINOR]}; 
    PitchSetManager daeolian{PitchClass::d, minor};
    vector<NumberedPitchClass> dminorloc {d, e, f, g, a, as, c};

    IntervalSegment dorian{ism.modeIntervalMap[DORIAN]};
    PitchSetManager ddorian{PitchClass::d, dorian};
    vector<NumberedPitchClass> ddorloc {d, e, f, g, a, b, c};

    IntervalSegment phrygian{ism.modeIntervalMap[PHRYGIAN]};
    PitchSetManager dsphrygian{PitchClass::ds, phrygian};
    vector<NumberedPitchClass> dsphryloc {ds, e, fs, gs, as, b, cs};

    IntervalSegment lydian{ism.modeIntervalMap[LYDIAN]};
    PitchSetManager elydian{PitchClass::e, lydian};
    vector<NumberedPitchClass> elydianloc {e, fs, gs, as, b, cs, ds};
    
    IntervalSegment mixolydian{ism.modeIntervalMap[MIXOLYDIAN]};
    PitchSetManager emixolydian{PitchClass::e, mixolydian};
    vector<NumberedPitchClass> emixolydianloc {e, fs, gs, a, b, cs, d};

    IntervalSegment locrian{ism.modeIntervalMap[LOCRIAN]};
    PitchSetManager flocrian{PitchClass::f, locrian};
    vector<NumberedPitchClass> flocrianloc {f, fs, gs, as, b, cs, ds};

    IntervalSegment maj7{ism.modeIntervalMap[MAJ7]};
    PitchSetManager fsmaj7{PitchClass::fs, maj7};
    vector<NumberedPitchClass> fsmaj7loc {fs, as, cs, f};

    IntervalSegment dom7{ism.modeIntervalMap[DOM7]};
    PitchSetManager fsdom7{PitchClass::fs, dom7};
    vector<NumberedPitchClass> fsdom7loc {fs, as, cs, e};

    IntervalSegment maj6{ism.modeIntervalMap[MAJ6]};
    PitchSetManager gmaj6{PitchClass::g, maj6};
    vector<NumberedPitchClass> gmaj6loc {g, b, e};
    
    IntervalSegment min6{ism.modeIntervalMap[MIN6]};
    PitchSetManager gmin6{PitchClass::g, min6};
    vector<NumberedPitchClass> gmin6loc {g, as, e};

    IntervalSegment six4{ism.modeIntervalMap[SIX4]};
    PitchSetManager gsix4{PitchClass::g, six4};
    vector<NumberedPitchClass> gsix4loc {g, c, e};

    IntervalSegment minsix4{ism.modeIntervalMap[MINSIX4]};
    PitchSetManager gsminsix4{PitchClass::gs, minsix4};
    vector<NumberedPitchClass> gsminsix4loc {gs, cs, e};

    IntervalSegment six5{ism.modeIntervalMap[SIX5]};
    PitchSetManager asix5{PitchClass::a, six5};
    vector<NumberedPitchClass> asix5loc {a, c, ds, f};

    IntervalSegment minsix5{ism.modeIntervalMap[MINSIX5]};
    PitchSetManager aminsix5{PitchClass::a, six5};
    vector<NumberedPitchClass> aminsix5loc {a, c, ds, f};

    IntervalSegment five4{ism.modeIntervalMap[FIVE4]};
    PitchSetManager asfive4{PitchClass::as, five4};
    vector<NumberedPitchClass> asfive4loc {as, ds, f};

    IntervalSegment four2{ism.modeIntervalMap[FOUR2]};
    PitchSetManager bfour2{PitchClass::b, four2};
    vector<NumberedPitchClass> bfour2loc {b, cs, e};
};

class NoteTest :public::testing::Test {

protected:
    void SetUp() override {} 
    ofVec2f<float> h1 { 20.0, 20.0 };
    ofVec2f<float> h2 { 30.0, 50.0 };
    ofVec2f<float> h3 { 40.0, 60.0 };
    ofVec2f<float> h4 { 50.0, 70.0 };
    NumberedPitch c { 60 };
    NumberedPitch d { 62 };
    NumberedPitch f { 65 };
    NumberedPitch e { 64 };
    float h1x = h1.getX();
    Note c1 { c, h1.getX(), h1.getY() };
    Note d1 { d, h2.getX(), h2.getY() };
    Note f1 { f, h3.getX(), h3.getY() };
    Note e1 { e, h4.getX(), h4.getY() };
};

class HarmonicFieldTest : public::testing::Test {

protected:
    void SetUp() override {} 

    IntervalSegmentManager ism;
    IntervalSegment major{ism.modeIntervalMap[MAJOR]};
    PitchSetManager cmajman{PitchClass::c, major};// cmajor man...

    // Create a harmonic field
    vector<NumberedPitch> npitches { cmajman.getPitchSet() };
    HarmonicField cmajor { npitches };
};

class HarmonicFieldManagerTest : public::testing::Test{

protected:
    void SetUp() override {} 
    // mock up a timeline and four harmonic field graphs
    TimespanGraph timespangraph{4};  // timespangraph with 4 subsections (harmonic fields)
    HarmonicFieldGraph hfield1{'a', timespangraph}; 
    HarmonicFieldGraph hfield2{'b', timespangraph};  
    HarmonicFieldGraph hfield3{'c', timespangraph};  
    HarmonicFieldGraph hfield4{'d', timespangraph};

    IntervalSegmentManager ism;
    IntervalSegment major{ism.modeIntervalMap[MAJOR]};
    PitchSetManager cmajman{PitchClass::c, major};// cmajor man...
    IntervalSegment minor{ism.modeIntervalMap[MINOR]};
    PitchSetManager dminman{PitchClass::d, minor};// dminor man...
    IntervalSegment lydian{ism.modeIntervalMap[LYDIAN]};
    PitchSetManager flydman{PitchClass::f, lydian};// flydian man...
    IntervalSegment phrygian{ism.modeIntervalMap[PHRYGIAN]};
    PitchSetManager ephryman{PitchClass::c, major};// ephrygian man...

    vector<NumberedPitch> cpitches { cmajman.getPitchSet() };
    HarmonicField cmajor { cpitches };
    vector<NumberedPitch> dpitches { dminman.getPitchSet() };
    HarmonicField dminor { dpitches };
    vector<NumberedPitch> fpitches { flydman.getPitchSet() };
    HarmonicField flydian { fpitches };
    vector<NumberedPitch> epitches { ephryman.getPitchSet() };
    HarmonicField ephrygian { epitches };

    HarmonicFieldManager cmanager{cmajor, hfield1}; 
    HarmonicFieldManager dmanager{dminor, hfield2}; 
    HarmonicFieldManager fmanager{flydian, hfield3}; 
    HarmonicFieldManager emanager{ephrygian, hfield4}; 
};


class QuantizedPitchManagerTest : public::testing::Test{
protected:
    void SetUp() override {}
    // Copying all the setup like this might violate the principle of DRY code
    // maybe tests can be wet? TODO: find out if there is a way to stay DRY
    TimespanGraph timespangraph{4};  // timespangraph with 4 subsections (harmonic fields)
    HarmonicFieldGraph hfield1{'a', timespangraph}; 
    HarmonicFieldGraph hfield2{'b', timespangraph};  
    HarmonicFieldGraph hfield3{'c', timespangraph};  
    HarmonicFieldGraph hfield4{'d', timespangraph};

    IntervalSegmentManager ism;
    IntervalSegment major{ism.modeIntervalMap[MAJOR]};
    PitchSetManager cmajman{PitchClass::c, major};// cmajor man...
    IntervalSegment minor{ism.modeIntervalMap[MINOR]};
    PitchSetManager dminman{PitchClass::d, minor};// dminor man...
    IntervalSegment lydian{ism.modeIntervalMap[LYDIAN]};
    PitchSetManager flydman{PitchClass::f, lydian};// flydian man...
    IntervalSegment phrygian{ism.modeIntervalMap[PHRYGIAN]};
    PitchSetManager ephryman{PitchClass::c, major};// ephrygian man...

    vector<NumberedPitch> cpitches { cmajman.getPitchSet() };
    HarmonicField cmajor { cpitches };
    vector<NumberedPitch> dpitches { dminman.getPitchSet() };
    HarmonicField dminor { dpitches };
    vector<NumberedPitch> fpitches { flydman.getPitchSet() };
    HarmonicField flydian { fpitches };
    vector<NumberedPitch> epitches { ephryman.getPitchSet() };
    HarmonicField ephrygian { epitches };

    HarmonicFieldManager cman{cmajor, hfield1}; 
    HarmonicFieldManager dman{dminor, hfield2}; 
    HarmonicFieldManager fman{flydian, hfield3}; 
    HarmonicFieldManager eman{ephrygian, hfield4}; 

    vector<HarmonicFieldManager> hfmanagers { cman, dman, fman, eman };
    QuantizedPitchManager notemaker { hfmanagers };
};
