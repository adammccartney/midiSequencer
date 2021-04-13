#include "Quantizer.h"
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
    modeIntervalMap[MINOR]=vector<int>{2, 2, 1, 2, 2, 2, 1};//"M2 m2 M2 M2 m2 M2 M2"
    modeIntervalMap[HMINOR]=vector<int>{2, 2, 1, 2, 2, 2, 1};//"M2 m2 M2 M2 M2 m2 M2"
    modeIntervalMap[DORIAN]=vector<int>{2, 1, 2, 2, 2, 1, 2};//"M2 m2 M2 M2 M2 m2 M2"
    modeIntervalMap[PHRYGIAN]=vector<int>{1, 2, 2, 2, 1, 2, 2};//"m2 M2 M2 M2 m2 M2 M2"
    modeIntervalMap[LYDIAN]=vector<int>{2, 2, 2, 1, 2, 2, 1};//"M2 M2 M2 m2 M2 M2 m2"
    modeIntervalMap[MIXOLYDIAN]=vector<int>{2, 2, 1, 2, 2, 1, 2};//"M2 M2 m2 M2 M2 m2 M2"
    modeIntervalMap[LOCRIAN]=vector<int>{1, 2, 2, 2, 1, 2, 2};//"m2 M2 M2 M2 m2 M2 M2"
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
    val = (int)name;
}

//-----------------------------------------------------------------------------


int NumberedPitch::operator==(NumberedPitch n)
{
    if(asInt() == n.asInt())
        return 1;
    else
        return 0;
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
//

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
//

int main()
{
   IntervalSegmentManager ism;
   IntervalSegment major{ism.modeIntervalMap[MAJOR]};
   PitchSetManager pcman{PitchClass::c, major}; 
   
   vector <NumberedPitchClass> testpc { pcman.getPitchClassSet() };
   for(int i = 0; i < testpc.size(); i++){
       cout << testpc[i].asInt() << '\t';
   }
   cout << '\n';

   vector <NumberedPitch> testpitches { pcman.getPitchSet() };
   for(int i = 0; i < testpitches.size(); i++){
       cout << testpitches[i].asInt() << '\t';
   }
   cout << '\n';


   // test operator== 
   NumberedPitch c { 60 };
   NumberedPitch d { 62 };
   NumberedPitch cc { 60 };
   if(c == d){
       cout << "is equal, c; " << c.asInt() << " = d; " << d.asInt() << '\n';
   }
   else{ 
       cout << "is not equal" << '\n';
   }
   
   // test quantizer 
   // create a harmonic field with the above PitchSetManager
   vector <NumberedPitch> npitches { pcman.getPitchSet() };
   
   HarmonicField cionian { npitches };

   NumberedPitch cs { 61 };
   NumberedPitch d1 { cionian.getQuantizedPitch(cs) };
   cout << "d: " << d1.asInt() << '\n'; 

   // setup for larger test
   //vector<NumberedPitch> chrom { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 };
   //vector<NumberedPitch> target { 60, 62, 62, 64, 64, 65, 67, 67, 69, 69, 71, 71};


   
   return 0;
}



