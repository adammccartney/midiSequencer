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

    void setVal(int p) { _val = p; }
    
    int asInt() { return _val; }

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


//-----------------------------------------------------------------------------
// Unit Tests 
//

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
