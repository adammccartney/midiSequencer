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
    // compiler makes simple destructor by default
    
    void init();

    void setVal(PitchClass& pc) { _val = int(pc); }
    
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
    //



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
    IntervalSegment minfive4{ism.modeIntervalMap[MINFIVE4]}; 
    IntervalSegment four2{ism.modeIntervalMap[FOUR2]}; 
    IntervalSegment dimfour2{ism.modeIntervalMap[DIMFOUR2]}; 
};


class PitchManagerTest : public::testing::Test {

protected:
    void SetUp() override {} 

    IntervalSegmentManager ism;

    IntervalSegment major{ism.modeIntervalMap[MAJOR]}; 
    PitchSetManager cionian{PitchClass::c, major};

    IntervalSegment hminor{ism.modeIntervalMap[HMINOR]};
    PitchSetManager cshminor{PitchClass::cs, hminor};

    IntervalSegment minor{ism.modeIntervalMap[MINOR]}; 
    PitchSetManager daeolian{PitchClass::d, minor};

    IntervalSegment dorian{ism.modeIntervalMap[DORIAN]};
    PitchSetManager ddorian{PitchClass::d, dorian};

    IntervalSegment phrygian{ism.modeIntervalMap[PHRYGIAN]};
    PitchSetManager dsphrygian{PitchClass::ds, phrygian};

    IntervalSegment lydian{ism.modeIntervalMap[LYDIAN]};
    PitchSetManager elydian{PitchClass::e, lydian};
    
    IntervalSegment mixolydian{ism.modeIntervalMap[MIXOLYDIAN]};
    PitchSetManager emixolydian{PitchClass::e, mixolydian};

    IntervalSegment locrian{ism.modeIntervalMap[LOCRIAN]};
    PitchSetManager flocrian{PitchClass::f, locrian};

};
