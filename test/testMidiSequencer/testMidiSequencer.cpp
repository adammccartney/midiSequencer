#include "testMidiSequencer.h"
#include <iostream>
#include <algorithm>
#include <iterator>


using namespace std;

//----------------------------------------------------------------------------
// Pitch and Interval Datatypes 
//

IntervalSegmentManager::IntervalSegmentManager(){ makeMap(); }

map<Mode, vector<int>> IntervalSegmentManager::modeIntervalMap;

void IntervalSegmentManager::makeMap() {
    modeIntervalMap[MAJOR]=vector<int>{2, 2, 1, 2, 2, 2, 1}; //"M2 M2 m2 M2 M2 M2 m2"
    modeIntervalMap[MINOR]=vector<int>{2, 1, 2, 2, 1, 2, 2};//"M2 m2 M2 M2 m2 M2 M2"
    modeIntervalMap[HMINOR]=vector<int>{2, 1, 2, 2, 1, 3, 1};//"M2 m2 M2 M2 M2 m2 M2"
    modeIntervalMap[DORIAN]=vector<int>{2, 1, 2, 2, 2, 1, 2};//"M2 m2 M2 M2 M2 m2 M2"
    modeIntervalMap[PHRYGIAN]=vector<int>{1, 2, 2, 2, 1, 2, 2};//"m2 M2 M2 M2 m2 M2 M2"
    modeIntervalMap[LYDIAN]=vector<int>{2, 2, 2, 1, 2, 2, 1};//"M2 M2 M2 m2 M2 M2 m2"
    modeIntervalMap[MIXOLYDIAN]=vector<int>{2, 2, 1, 2, 2, 1, 2};//"M2 M2 m2 M2 M2 m2 M2"
    modeIntervalMap[LOCRIAN]=vector<int>{1, 2, 2, 1, 2, 2, 2};//"m2 M2 M2 M2 m2 M2 M2"
    modeIntervalMap[MAJ7]=vector<int>{4, 3, 4};//"M3 m3 M3"
    modeIntervalMap[DOM7]=vector<int>{4, 3, 3};//"M3 m3 m3"
    modeIntervalMap[MAJ6]=vector<int>{3, 5};//"m3 P4"
    modeIntervalMap[MIN6]=vector<int>{4, 5};//"M3 P4"
    modeIntervalMap[SIX4]=vector<int>{5, 4};//"P4 M3"
    modeIntervalMap[MINSIX4]=vector<int>{5, 3};//"P4 m3"
    modeIntervalMap[SIX5]=vector<int>{4, 3, 2};//"M3 m3 M2"
    modeIntervalMap[MINSIX5]=vector<int>{3, 4, 1};//"m3 M3 m2"
    modeIntervalMap[FIVE4]=vector<int>{5, 2};//"P4 M2"
    modeIntervalMap[MINFIVE4]=vector<int>{5, 1};//"P4 m2"
    modeIntervalMap[FOUR2]=vector<int>{2, 3};//"M2 m3"
    modeIntervalMap[DIMFOUR2]=vector<int>{1, 4};//"m2 M3"
}

//-----------------------------------------------------------------------------

void NumberedPitchClass::init(){
    _val = (int)name;
}


PitchClass operator++(PitchClass& pc)
{
    pc = (pc == PitchClass::b) ? PitchClass::c : PitchClass(int(pc) + 1);
    return pc;
} 

PitchClass operator--(PitchClass& pc)
{
    pc = (pc == PitchClass::c) ? PitchClass::b : PitchClass(int(pc) - 1);
    return pc;
} 

void NumberedPitchClass::transpose(int n)
{
    if(n < 0){
        n = abs(n);
        for(int i = 0; i < n; i++) --name;
    }
    else
        for(int i = 0; i < n; i++) ++name;
    setVal(name); // resets the integer value 
}
//-----------------------------------------------------------------------------
//

bool NumberedPitch::operator==(NumberedPitch n)
{
    if(asInt() == n.asInt())
        return true;
    else
        return false;
}

//-----------------------------------------------------------------------------
//

void NumberedPitch::transpose(int n)
{
    // don't go process requests to go below "midi=0" or above "midi=127"
    if(((_val + n) > 0) && (n < MIDIMAX)){ // the request is in range
        _val += n;
    }
    else
        throw std::invalid_argument("not a valid transposition");
}

//-----------------------------------------------------------------------------
//

void PitchSetManager::makePitchClassSet()
// set of pitch classes from a root and given scale in octave above mid c
{
    vector<NumberedPitchClass> pcset;
    NumberedPitchClass temppc { _root };
    pcset.push_back(temppc);
    for(int i = 0; i < _mode.intervals.size(); i++){
        // move through intervals
        // increment the value 
        temppc.transpose(_mode.intervals[i]);
        pcset.push_back(temppc); 
    }
    //TODO: figure out how this compiles, specifically if both copies persist
    _pcset = { pcset }; 
}


//-----------------------------------------------------------------------------
//

void PitchSetManager::makePitchSet()
{
    // makes a set of pitches across a span of 10 octaves
    // retrieve vector of pitch classes and access them in the root octave
    int offset = 0;
    int pitchval = 0;
    vector<NumberedPitch> npv;
    for(int i = 0; i < OCTAVE_OFFSETS.size(); i++){
        for(int j = 0; j < _pcset.size() - 1; j++){ // don't duplicate the octave
            offset = OCTAVE_OFFSETS[i];
            pitchval = _pcset[j].asInt() + offset;
            NumberedPitch np { pitchval };
            npv.push_back(np);
        }
    }
   _pitches = { npv }; 
}


//-----------------------------------------------------------------------------

PitchSetManager::PitchSetManager(const PitchClass &root, const IntervalSegment &mode)
   : _root { root }, _mode { mode } 
{
    _minpitchval = (int)root - MIDCOFFSET; 
    makePitchClassSet();
    makePitchSet();
} 


//-----------------------------------------------------------------------------
// TODO: fix bug in pitchdata.getPitchSet() ... write a unit test!

NumberedPitch HarmonicField::getQuantizedPitch(NumberedPitch &inpitch){
    cout << "Entering getQP with inpitch: " << inpitch.asInt() << '\n';
    NumberedPitch n { inpitch.asInt() }; // initialize a new pitch
    
    cout << pitchset.size() << '\n';
    for(int i = 0; i < pitchset.size(); i++){
        cout << pitchset[i].asInt() << '\t';
    }
    cout << '\n';
    auto p = find(pitchset.begin(), pitchset.end(), n);
    if(p != pitchset.end()){
        return *p;
    }
    else{
        n.transpose(+1);
        cout << "n as int: " << n.asInt() << '\n';
        return getQuantizedPitch(n);
    }
}

//-----------------------------------------------------------------------------
// TEST FUNCTIONS 
//
 
TEST_F(NumberedPitchClassTest, ReturnsCorrectInt){
    int n = npc1.asInt();
    EXPECT_EQ(n, 0);

    n = npc2.asInt();
    EXPECT_EQ(n, 1); 
    
    n = npc3.asInt();
    EXPECT_EQ(n, 2); 
    
    n = npc4.asInt();
    EXPECT_EQ(n, 3); 

    n = npc5.asInt();
    EXPECT_EQ(n, 4); 

    n = npc6.asInt();
    EXPECT_EQ(n, 5); 

    n = npc7.asInt();
    EXPECT_EQ(n, 6); 

    n = npc8.asInt();
    EXPECT_EQ(n, 7); 

    n = npc9.asInt();
    EXPECT_EQ(n, 8); 

    n = npc10.asInt();
    EXPECT_EQ(n, 9); 

    n = npc11.asInt();
    EXPECT_EQ(n, 10); 
    
    n = npc12.asInt();
    EXPECT_EQ(n, 11); 
}


TEST_F(NumberedPitchClassTest, TransposesCorrectly){    
    npc1.transpose(6);
    int n = npc1.asInt(); // tritone
    EXPECT_EQ(n, 6);

    npc2.transpose(7);
    n = npc2.asInt(); // perfect fifth above cs
    EXPECT_EQ(n, 8); 

    npc3.transpose(12);
    n = npc3.asInt(); // perfect octave above d
    EXPECT_EQ(n, 2); 
    
    npc4.transpose(-5); 
    n = npc4.asInt(); // major third below ds
    EXPECT_EQ(n, 10); // PitchClass::as  

    npc5.transpose(-24); 
    n = npc5.asInt(); // double octave below e
    EXPECT_EQ(n, 4); // PitchClass::e
}



TEST_F(NumberedPitchTest, OperatorWorksCorrectly){
    ASSERT_NE(np4==(NumberedPitch(64)), 1);
}

TEST_F(NumberedPitchTest, TransposesCorrectly){
    np1.transpose(12);
    int n = np1.asInt();
    EXPECT_EQ(n, 72);
    
    np2.transpose(-12);
    n = np2.asInt();
    EXPECT_EQ(n, 49);
    
    np3.transpose(0);
    n = np3.asInt();
    EXPECT_EQ(n, 62);
}

TEST_F(IntervalSegmentTest, IntervalSegmentsContainRightNumSteps)
{
    int n = 0;
    n = major.intervals.size();
    EXPECT_EQ(n, 7);
    n = minor.intervals.size();
    EXPECT_EQ(n, 7);
    n = hminor.intervals.size();
    EXPECT_EQ(n, 7);
    n = dorian.intervals.size();
    EXPECT_EQ(n, 7);
    n = phrygian.intervals.size();
    EXPECT_EQ(n, 7);
    n = lydian.intervals.size();
    EXPECT_EQ(n, 7);
    n = mixolydian.intervals.size();
    EXPECT_EQ(n, 7);
    n = locrian.intervals.size();
    EXPECT_EQ(n, 7);
    n = maj7.intervals.size();
    EXPECT_EQ(n, 3);
    n = dom7.intervals.size();
    EXPECT_EQ(n, 3);
    n = maj6.intervals.size();
    EXPECT_EQ(n, 2);
    n = min6.intervals.size();
    EXPECT_EQ(n, 2);
    n = six4.intervals.size();
    EXPECT_EQ(n, 2);
    n = minsix4.intervals.size();
    EXPECT_EQ(n, 2);
    n = six5.intervals.size();
    EXPECT_EQ(n, 3);
    n = minsix5.intervals.size();
    EXPECT_EQ(n, 3);
    n = five4.intervals.size();
    EXPECT_EQ(n, 2);
    n = four2.intervals.size();
    EXPECT_EQ(n, 2);
    n = dimfour2.intervals.size();
    EXPECT_EQ(n, 2);
}

TEST_F(PitchManagerTest, PitchClassSetMadeCorrectly)
{
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
    
    vector<NumberedPitchClass> cmajor {c, d, e, f, g, a, b};
    vector<NumberedPitchClass> cionianconstr { cionian.getPitchClassSet() };
    ASSERT_NE(cionianconstr.size()==cmajor.size(), 1);
    for(int i = 0; i < cmajor.size(); i++){
        EXPECT_EQ(cionianconstr[i].asInt(), cmajor[i].asInt());
    }  
   
    vector<NumberedPitchClass> cshmin {cs, ds, e, fs, gs, a, c};
    vector<NumberedPitchClass> cshminconstr { cshminor.getPitchClassSet() }; 
    ASSERT_NE(cshminconstr.size()==cshmin.size(), 1);
    for(int i = 0; i < cshmin.size(); i++){
        EXPECT_EQ(cshminconstr[i].asInt(), cshmin[i].asInt());
    }  

    vector<NumberedPitchClass> dminor {d, e, f, g, a, as, c};
    vector<NumberedPitchClass> daeolianconstr { daeolian.getPitchClassSet() };
    ASSERT_NE(daeolianconstr.size()==dminor.size(), 1);
    for(int i = 0; i < dminor.size(); i++){
        EXPECT_EQ(daeolianconstr[i].asInt(), dminor[i].asInt());
    }  
    
    vector<NumberedPitchClass> ddorloc {d, e, f, g, a, b, c};
    vector<NumberedPitchClass> ddorianconstr { ddorian.getPitchClassSet() };
    ASSERT_NE(ddorianconstr.size()==ddorloc.size(), 1);
    for(int i = 0; i < ddorloc.size(); i++){
        EXPECT_EQ(ddorianconstr[i].asInt(), ddorloc[i].asInt());
    }  
    
    vector<NumberedPitchClass> dsphryloc {ds, e, fs, gs, as, b, cs};
    vector<NumberedPitchClass> dsphrygconstr { dsphrygian.getPitchClassSet() };
    ASSERT_NE(dsphrygconstr.size()==dsphryloc.size(), 1);
    for(int i = 0; i < dsphryloc.size(); i++){
        EXPECT_EQ(dsphrygconstr[i].asInt(), dsphryloc[i].asInt());
    }  
    
    vector<NumberedPitchClass> elydianloc {e, fs, gs, as, b, cs, ds};
    vector<NumberedPitchClass> elydianconstr { elydian.getPitchClassSet() };
    ASSERT_NE(elydianconstr.size()==elydianloc.size(), 1);
    for(int i = 0; i < elydianloc.size(); i++){
        EXPECT_EQ(elydianconstr[i].asInt(), elydianloc[i].asInt());
    }  
    vector<NumberedPitchClass> emixolydianloc {e, fs, gs, a, b, cs, d};
    vector<NumberedPitchClass> emixolydianconstr { emixolydian.getPitchClassSet() };
    ASSERT_NE(emixolydianconstr.size()==emixolydianloc.size(), 1);
    for(int i = 0; i < emixolydianloc.size(); i++){
        EXPECT_EQ(emixolydianconstr[i].asInt(), emixolydianloc[i].asInt());
    }  
    vector<NumberedPitchClass> flocrianloc {f, fs, gs, as, b, cs, ds};
    vector<NumberedPitchClass> flocrianconstr { flocrian.getPitchClassSet() };
    ASSERT_NE(flocrianconstr.size()==flocrianloc.size(), 1);
    for(int i = 0; i < flocrianloc.size(); i++){
        EXPECT_EQ(flocrianconstr[i].asInt(), flocrianloc[i].asInt());
    }  
}

//-----------------------------------------------------------------------------
//
//

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



