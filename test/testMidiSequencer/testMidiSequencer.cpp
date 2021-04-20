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

void IntervalSegmentManager::makeMap() { // sum of all interval steps from any root should equal octave
    modeIntervalMap[MAJOR]=vector<int>{2, 2, 1, 2, 2, 2, 1}; //"M2 M2 m2 M2 M2 M2 m2"
    modeIntervalMap[MINOR]=vector<int>{2, 1, 2, 2, 1, 2, 2};//"M2 m2 M2 M2 m2 M2 M2"
    modeIntervalMap[HMINOR]=vector<int>{2, 1, 2, 2, 1, 3, 1};//"M2 m2 M2 M2 m2 m3 m2"
    modeIntervalMap[DORIAN]=vector<int>{2, 1, 2, 2, 2, 1, 2};//"M2 m2 M2 M2 M2 m2 M2"
    modeIntervalMap[PHRYGIAN]=vector<int>{1, 2, 2, 2, 1, 2, 2};//"m2 M2 M2 M2 m2 M2 M2"
    modeIntervalMap[LYDIAN]=vector<int>{2, 2, 2, 1, 2, 2, 1};//"M2 M2 M2 m2 M2 M2 m2"
    modeIntervalMap[MIXOLYDIAN]=vector<int>{2, 2, 1, 2, 2, 1, 2};//"M2 M2 m2 M2 M2 m2 M2"
    modeIntervalMap[LOCRIAN]=vector<int>{1, 2, 2, 1, 2, 2, 2};//"m2 M2 M2 M2 m2 M2 M2"
    modeIntervalMap[MAJ7]=vector<int>{4, 3, 4, 1};//"M3 m3 M3"
    modeIntervalMap[DOM7]=vector<int>{4, 3, 3, 2};//"M3 m3 m3"
    modeIntervalMap[MAJ6]=vector<int>{4, 5, 3};//"M3 P4 m3"
    modeIntervalMap[MIN6]=vector<int>{3, 6, 3};//"m3 P4 m3"
    modeIntervalMap[SIX4]=vector<int>{5, 4, 3};//"P4 M3 m3"
    modeIntervalMap[MINSIX4]=vector<int>{5, 3, 4};//"P4 m3 M3"
    modeIntervalMap[SIX5]=vector<int>{3, 3, 2, 4};//"m3 m3 M2 M3"
    modeIntervalMap[MINSIX5]=vector<int>{4, 3, 2, 3};//"M3 m3 M2 M3"
    modeIntervalMap[FIVE4]=vector<int>{5, 2, 5};//"P4 M2"
    modeIntervalMap[FOUR2]=vector<int>{2, 3, 7};//"M2 m3"
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
    setVal(name); 
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
    // do not process requests below "midi=0" or above "midi=127"
    if(((_val + n) > 0) && (n < MIDIMAX)){ // the request is in range
        _val += n;
    }
    else
        throw std::invalid_argument("not a valid transposition");
}

//-----------------------------------------------------------------------------
//

void PitchSetManager::makePitchClassSet()
// makes a set of pitch classes using the members root and interval segment 
{
    vector<NumberedPitchClass> pcset;
    NumberedPitchClass temppc { _root };
    pcset.push_back(temppc); // append root first  
    for(int i = 0; i < _mode.intervals.size() - 1; i++){ // don't take last step 
        temppc.transpose(_mode.intervals[i]);
        pcset.push_back(temppc); 
    }
    //TODO: figure out how this compiles, specifically if both copies persist
    _pcset = { pcset }; 
}


//-----------------------------------------------------------------------------
//

void PitchSetManager::makePitchSet()
// makes a set of pitches across a span of 10 octaves using the member pitchset
{
    int offset = 0;
    int pitchval = 0;
    vector<NumberedPitch> npv;
    for(int i = 0; i < OCTAVE_OFFSETS.size(); i++){
        for(int j = 0; j < _pcset.size(); j++){ 
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
    _minpitchval = (int)root; 
    makePitchClassSet();
    makePitchSet();
} 


//-----------------------------------------------------------------------------

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
    ASSERT_NE(np4==(NumberedPitch(64)), true);
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
    EXPECT_EQ(n, 4);
    n = dom7.intervals.size();
    EXPECT_EQ(n, 4);
    n = maj6.intervals.size();
    EXPECT_EQ(n, 3);
    n = min6.intervals.size();
    EXPECT_EQ(n, 3);
    n = six4.intervals.size();
    EXPECT_EQ(n, 3);
    n = minsix4.intervals.size();
    EXPECT_EQ(n, 3);
    n = six5.intervals.size();
    EXPECT_EQ(n, 4);
    n = five4.intervals.size();
    EXPECT_EQ(n, 3);
    n = four2.intervals.size();
    EXPECT_EQ(n, 3);
}

TEST_F(PitchManagerTest, AllIntervalsSumToOctave)
{
    // tests that all interval steps within a constructed PitchSet equal 12
    const int octave = 12;
    int sum;
    for(int modeInt = MAJOR; modeInt != MODECOUNT; modeInt++){
        sum = 0;
        IntervalSegmentManager ism;
        Mode mode = static_cast<Mode>(modeInt);
        IntervalSegment is{ism.modeIntervalMap[mode]};
        for(int i = 0; i < is.intervals.size(); i++){ 
                 sum += is.intervals[i];
        }
        EXPECT_EQ(sum, octave);
    }
}

TEST_F(PitchManagerTest, PitchClassSetMadeCorrectly)
{
    vector<NumberedPitchClass> cionianconstr { cionian.getPitchClassSet() };
    EXPECT_EQ(cionianconstr.size(), cmajorloc.size());
    for(int i = 0; i < cmajorloc.size(); i++){
        EXPECT_EQ(cionianconstr[i].asInt(), cmajorloc[i].asInt());
    }  
   
    vector<NumberedPitchClass> cshminconstr { cshminor.getPitchClassSet() }; 
    EXPECT_EQ(cshminconstr.size(), cshminorloc.size());
    for(int i = 0; i < cshminorloc.size(); i++){
        EXPECT_EQ(cshminconstr[i].asInt(), cshminorloc[i].asInt());
    }  

    vector<NumberedPitchClass> daeolianconstr { daeolian.getPitchClassSet() };
    EXPECT_EQ(daeolianconstr.size(), dminorloc.size());
    for(int i = 0; i < dminorloc.size(); i++){
        EXPECT_EQ(daeolianconstr[i].asInt(), dminorloc[i].asInt());
    }  
    
    vector<NumberedPitchClass> ddorianconstr { ddorian.getPitchClassSet() };
    EXPECT_EQ(ddorianconstr.size(), ddorloc.size());
    for(int i = 0; i < ddorloc.size(); i++){
        EXPECT_EQ(ddorianconstr[i].asInt(), ddorloc[i].asInt());
    }  
    
    vector<NumberedPitchClass> dsphrygconstr { dsphrygian.getPitchClassSet() };
    EXPECT_EQ(dsphrygconstr.size(), dsphryloc.size());
    for(int i = 0; i < dsphryloc.size(); i++){
        EXPECT_EQ(dsphrygconstr[i].asInt(), dsphryloc[i].asInt());
    }  
    
    vector<NumberedPitchClass> elydianconstr { elydian.getPitchClassSet() };
    EXPECT_EQ(elydianconstr.size(), elydianloc.size());
    for(int i = 0; i < elydianloc.size(); i++){
        EXPECT_EQ(elydianconstr[i].asInt(), elydianloc[i].asInt());
    }  
    vector<NumberedPitchClass> emixolydianconstr { emixolydian.getPitchClassSet() };
    EXPECT_EQ(emixolydianconstr.size(), emixolydianloc.size());
    for(int i = 0; i < emixolydianloc.size(); i++){
        EXPECT_EQ(emixolydianconstr[i].asInt(), emixolydianloc[i].asInt());
    }  
    vector<NumberedPitchClass> flocrianconstr { flocrian.getPitchClassSet() };
    EXPECT_EQ(flocrianconstr.size(), flocrianloc.size());
    for(int i = 0; i < flocrianloc.size(); i++){
        EXPECT_EQ(flocrianconstr[i].asInt(), flocrianloc[i].asInt());
    }  
    vector<NumberedPitchClass> fsmaj7constr { fsmaj7.getPitchClassSet() };
    EXPECT_EQ(fsmaj7constr.size(), fsmaj7loc.size());
    for(int i = 0; i < fsmaj7loc.size(); i++){
        EXPECT_EQ(fsmaj7constr[i].asInt(), fsmaj7loc[i].asInt());
    }  
    vector<NumberedPitchClass> fsdom7constr { fsdom7.getPitchClassSet() };
    EXPECT_EQ(fsdom7constr.size(), fsdom7loc.size());
    for(int i = 0; i < fsdom7loc.size(); i++){
        EXPECT_EQ(fsdom7constr[i].asInt(), fsdom7loc[i].asInt());
    }  
    vector<NumberedPitchClass> gmaj6constr { gmaj6.getPitchClassSet() };
    EXPECT_EQ(gmaj6constr.size(), gmaj6loc.size());
    for(int i = 0; i < gmaj6loc.size(); i++){
        EXPECT_EQ(gmaj6constr[i].asInt(), gmaj6loc[i].asInt());
    }  
    vector<NumberedPitchClass> gmin6constr { gmin6.getPitchClassSet() };
    EXPECT_EQ(gmin6constr.size(), gmin6loc.size());
    for(int i = 0; i < gmin6loc.size(); i++){
        EXPECT_EQ(gmin6constr[i].asInt(), gmin6loc[i].asInt());
    }  
    vector<NumberedPitchClass> gsix4constr { gsix4.getPitchClassSet() };
    EXPECT_EQ(gsix4constr.size(), gsix4loc.size());
    for(int i = 0; i < gsix4loc.size(); i++){
        EXPECT_EQ(gsix4constr[i].asInt(), gsix4loc[i].asInt());
    }
    vector<NumberedPitchClass> gsminsix4constr { gsminsix4.getPitchClassSet() };
    EXPECT_EQ(gsminsix4constr.size(), gsminsix4loc.size());
    for(int i = 0; i < gsminsix4loc.size(); i++){
        EXPECT_EQ(gsminsix4constr[i].asInt(), gsminsix4loc[i].asInt());
    }  
    vector<NumberedPitchClass> asix5constr { asix5.getPitchClassSet() };
    EXPECT_EQ(asix5constr.size(), asix5loc.size());
    for(int i = 0; i < asix5loc.size(); i++){
        EXPECT_EQ(asix5constr[i].asInt(), asix5loc[i].asInt());
    }  
    vector<NumberedPitchClass> aminsix5constr { aminsix5.getPitchClassSet() };
    EXPECT_EQ(aminsix5constr.size(), aminsix5loc.size());
    for(int i = 0; i < aminsix5loc.size(); i++){
        EXPECT_EQ(aminsix5constr[i].asInt(), aminsix5loc[i].asInt());
    }  
    vector<NumberedPitchClass> asfive4constr { asfive4.getPitchClassSet() };
    EXPECT_EQ(asfive4constr.size(), asfive4loc.size());
    for(int i = 0; i < asfive4loc.size(); i++){
        EXPECT_EQ(asfive4constr[i].asInt(), asfive4loc[i].asInt());
    }  
    vector<NumberedPitchClass> bfour2constr { bfour2.getPitchClassSet() };
    EXPECT_EQ(bfour2constr.size(), bfour2loc.size());
    for(int i = 0; i < bfour2loc.size(); i++){
        EXPECT_EQ(bfour2constr[i].asInt(), bfour2loc[i].asInt());
    }  
}

TEST_F(PitchManagerTest, PitchSetMadeCorrectly)
{
    // tests that the octave distribution works

    npcvec = cmajorloc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { cionian.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = cshminorloc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { cshminor.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = dminorloc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { daeolian.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();


    npcvec = dsphryloc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { dsphrygian.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = elydianloc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { elydian.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = emixolydianloc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { emixolydian.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();


    npcvec = flocrianloc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { flocrian.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = fsmaj7loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { fsmaj7.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = fsdom7loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { fsdom7.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = gmaj6loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { gmaj6.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = gmin6loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { gmin6.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = gsix4loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { gsix4.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = gsminsix4loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { gsminsix4.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = asix5loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { asix5.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();
    
    npcvec = aminsix5loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { aminsix5.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();
 
    npcvec = asfive4loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { asfive4.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();

    npcvec = bfour2loc;
    for(int i = 0; i < numoctaves; i++){
        for(int j = 0; j < npcvec.size(); j++){
            pitchval = npcvec[j].asInt() + (i * octavest); 
            NumberedPitch np{pitchval};
            dist.push_back(np);
        }
    } 
    for(int i = 0; i < dist.size(); i++){
        vector<NumberedPitch> npv { bfour2.getPitchSet() };
        EXPECT_EQ(dist[i].asInt(), npv[i].asInt()); 
    }
    npcvec.clear();
    dist.clear();
}

//-----------------------------------------------------------------------------
//
//

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



