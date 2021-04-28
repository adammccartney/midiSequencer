#include "testMidiSequencer.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <chrono>


using namespace std;

// N.B.: these tests avoid using any code that has to directly leverage
// openFrameworks and uses mock-up logic instead to test that the interface
// classes are working properly
//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

TimespanGraph::TimespanGraph()
    : numhfields { MINFIELDS } {} 

TimespanGraph::TimespanGraph(const int numharmonicfields)
    : numhfields{numharmonicfields}
{
    //_start.x = 0.0;
    _start.setX(0.0);
    //_start.y = ofGetHeight() / 2.0;
    _start.setY(ofGetHeight() / 2.0);
    //_end.x   = ofGetWidth();
    _end.setX(ofGetWidth());
    _end.setY(ofGetHeight() / 2.0);
    //_end.y   = ofGetHeight() / 2.0;
    //_len     = _start.distance(_end);
    _len = LENGTH;
}

float TimespanGraph::getLength(){ return _len; }

//void TimespanGraph::draw(){
//    // remove anything that's really trying to draw for the time being
//    //ofDrawLine(_start.x, _start.y, _end.x, _end.y);
//}

//-----------------------------------------------------------------------------
// 

HarmonicFieldGraph::HarmonicFieldGraph(char n, TimespanGraph& ts)
    :name{n}, timespan{ts}
{
    this->setID();
}

HarmonicFieldGraph::HarmonicFieldGraph()
    :name{DNAME}, timespan{TSGDEFAULT} {}

//-----------------------------------------------------------------------------
//

void HarmonicFieldGraph::setup()
{
//void HarmonicFieldGraph::setup(){
//    // use _pos to determine position on timeline 
//    // represents a subsection on the horizontal axis
//    
//    // setup member variables to handle location and positioning
//    length    = timespan.getLength() / timespan.numHfields;
//    hfxOrigin = timespan.getStart().x + ( length * (getID() - 1)); // from 0 
//    localMin  = hfxOrigin;
//    localMax  = hfxOrigin + length - getWidth(); 
//
//    //params.setName(toString());
//    //params.add(x.set("x", hfxOrigin, localMin, localMax));
//    //params.add(y.set("y", ofGetHeight() / 2, 0, ofGetHeight())); 
//
//    //updatePoints();

      // mock setup for testing logic
      length    = timespan.getLength() / timespan.numhfields;
      hfxOrigin = timespan.getStart().getX() + ( length * (getID() - 1));
      localMin  = hfxOrigin;
      localMax  = hfxOrigin + length - getWidth();

      setPos(hfxOrigin, localMax);
}
//
//void HarmonicFieldGraph::updatePoints(){
//    // this grabs info from the gui Sliders and updates the shape's data 
//
//    keypoints.clear();
//    
//    // this part is setup of the basic keyboard shape
//    for(int i = 0; i < _numtones; i++){
//        vector <ofVec2f> vec{ ofVec2f(x, y - i*10),
//                              ofVec2f(x + _width, y - i*10),
//                              ofVec2f(x + _width, y - i*10 - _height),
//                              ofVec2f(x, y - i*10 - _height)};
//        keypoints.push_back(vec);
//    }
//}

//void HarmonicFieldGraph::draw(){
//
//    updatePoints();
//   
//    // draw paths according to points
//    for(int i = 0; i < (int)keypoints.size(); i++){
//        ofPath npath;
//        npath.moveTo(keypoints[i][0]);
//        npath.lineTo(keypoints[i][1]);
//        npath.lineTo(keypoints[i][2]);
//        npath.lineTo(keypoints[i][3]);
//        npath.close(); 
//        // color according to piano keyboard
//        PitchClass x = (PitchClass)i;
//        if((x == PitchClass::cs) || (x == PitchClass::ds) || 
//                (x == PitchClass::fs) || (x == PitchClass::gs) || 
//                (x == PitchClass::as)){
//            npath.setStrokeColor(ofColor::black);
//            npath.setFillColor(ofColor::black);
//            npath.setFilled(true);
//            npath.setStrokeWidth(2);
//        }else{
//            npath.setStrokeColor(ofColor::black);
//            npath.setFillColor(ofColor::white);
//            npath.setFilled(true);
//            npath.setStrokeWidth(2);
//        }
//        npath.draw();
//    }
//}

//void HarmonicFieldGraph::update(){}

string HarmonicFieldGraph::toString(){
    string str = "Hfield ";
    str.push_back(name);
    return str;
}


//------------------------------------------------------------
// assign a numeric value to harmonic field based on name
//
void HarmonicFieldGraph::setID(){
    switch(this->name){
        case 'a':
            _id = 1;
            break;
        case 'b':
            _id = 2;
            break;
        case 'c':
            _id = 3;
            break;
        case 'd':
            _id = 4;
            break;
        case 'e':
            _id = 5;
            break;
        default:
            _id = -1; // negative val means name is not valid
    }
}

int HarmonicFieldGraph::getID(){ return _id; }


//----------------------------------------------------------------------------
// Pitch and Interval Datatypes 
//

IntervalSegmentManager::IntervalSegmentManager(){ makeMap(); }

map<Mode, vector<int>> IntervalSegmentManager::modeIntervalMap;

void IntervalSegmentManager::makeMap() { 
    // sum of all interval steps from any root should equal octave
    // that's the same number of sides as a dodecahedron
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

NumberedPitch::NumberedPitch()
{
    _val = 0;
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
        throw std::invalid_argument("Invalid transposition");
}

//-----------------------------------------------------------------------------
// Manager for pitch sets

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
// Note

Note::Note(const Note& n)
{
    _pitch = n.getPitch();
    _rtime = n.getRtime(); 
    _probability = n.getProb();
}

Note::Note(){}

Note& Note::operator=(const Note& n)
{
    _pitch = n.getPitch();
    _rtime = n.getRtime(); 
    _probability = n.getProb();
    return *this;
}

void Note::setAll(NumberedPitch &p, float &rtime, float &prob)
{
    _pitch = p;
    _rtime = rtime;
    _probability = prob;
}

string Note::toString()
{
    return "Pitch: " + to_string(getPitch().asInt()) + 
           " rel time: " + to_string(getRtime()) + 
           " prob: " + to_string(getProb()) + '\n';
}

//-----------------------------------------------------------------------------
// HarmonicFieldManager brings together logic 
// from HarmonicField and HarmonicFieldGraph in order to make Notes 

HarmonicFieldManager::HarmonicFieldManager(HarmonicField &hfield, HarmonicFieldGraph &hfgraph)
    // pitchset is just a reference for the quantizer so it knows what values
    // to use while processing an incoming pitch
    : _hfield { hfield }, _hfgraph { hfgraph }, _pitchset { hfield.getPitchSet() } 
{
    _rtime = hfgraph.getXPos();
    _probability = hfgraph.getYPos();
}


//-----------------------------------------------------------------------------

NumberedPitch HarmonicFieldManager::getQuantizedPitch(NumberedPitch inpitch){
    
    auto p = find(_pitchset.begin(), _pitchset.end(), inpitch);
    if(p != _pitchset.end()){
        return *p;
    }
    else{
        inpitch.transpose(+1);
        return getQuantizedPitch(inpitch);
    }
}

//-----------------------------------------------------------------------------
//

QuantizedPitchManager::QuantizedPitchManager(){} // default constructor 

//-----------------------------------------------------------------------------
// Constructor

QuantizedPitchManager::QuantizedPitchManager(const vector<HarmonicFieldManager> &vhfm)
    : _numfields { (int)vhfm.size() } // syntax possibly a crime against humanity
{
    for(int i = 0; i < _numfields; i++)
    {
        _vhfm.push_back(vhfm[i]);
    }
}

//-----------------------------------------------------------------------------
//

NumberedPitch QuantizedPitchManager::getQuantizedPitch(int hfindex, NumberedPitch p)
// calls the get QuantizedPitch function of a specific hfield
// returns quantized pitch
{
    NumberedPitch qp;
    qp.setPitch(_vhfm[hfindex].getQuantizedPitch(p));
    return qp;
}

//-----------------------------------------------------------------------------
// Coordinate getters
// usage: gui coordinates contain rhythmic and probabilty info about a note 

float QuantizedPitchManager::getRval(int n) // gets xcoord of nth harmonic field graph
{
    if((0 <= n) && (n < _numfields)) // in range, proceed
        return _vhfm[n].getRtime();
    else
        throw std::out_of_range("HField index out of range");
}

float QuantizedPitchManager::getProb(int n) // gets ycoord of nth harmonic field graph
{
    if((0 <= n) && (n < _numfields)) // in range, proceed
        return _vhfm[n].getProb();
    else
        throw std::out_of_range("HField index out of range");
}

//-----------------------------------------------------------------------------
// Here the incoming midi note is quantized and pushed to the stack of notes in
// the object

void QuantizedPitchManager::processMidiNote(const int &midiVal)
{
    NumberedPitch tmp { midiVal };
    _pitch = NumberedPitch { tmp } ; // set this as our starting pitch
   
    vector<float> rvals;
    vector<float> probs;
    NumberedPitch qp;
    for(int i = 0; i < 4; i++){ // magic constant replaced in main program
        qp.setPitch(getQuantizedPitch(i, tmp)); 
        rvals.push_back(getRval(i));
        probs.push_back(getProb(i));
        Note n { qp, rvals[i], probs[i] };
        _notes.push(n);
        tmp.setPitch(qp);
    }
}


void QuantizedPitchManager::popNote()
{
    _notes.pop();
}


//-----------------------------------------------------------------------------
// TEST FUNCTIONS 
//

TEST_F(TimespanGraphTest, SetsDimensionsCorrectly){
    ofVec2f<float> start = timespanTest.getStart();
    EXPECT_EQ(start.getX(), 0.0);
    ofVec2f<float> end = timespanTest.getEnd();
    EXPECT_EQ(end.getY(), 60.0);
    float length = timespanTest.getLength();
    EXPECT_EQ(length, 240);
}

TEST_F(HarmonicFieldGraphTest, ReturnsPosition){
    hfieldg1.setPos(xpos1, ypos1);
    hfieldg2.setPos(xpos2, ypos2);
    hfieldg3.setPos(xpos3, ypos3);
    hfieldg4.setPos(xpos4, ypos4);

    EXPECT_EQ(hfieldg1.getXPos(), 120);
    EXPECT_EQ(hfieldg1.getYPos(), 120);
    EXPECT_EQ(hfieldg2.getXPos(), 60);
    EXPECT_EQ(hfieldg2.getYPos(), 60);
    EXPECT_EQ(hfieldg3.getXPos(), 40);
    EXPECT_EQ(hfieldg3.getYPos(), 40);
    EXPECT_EQ(hfieldg4.getXPos(), 30);
    EXPECT_EQ(hfieldg4.getYPos(), 30);
};

// UsesPositionToConstructMessage

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

    np3.transpose(63); // value is 62, we want to see if we've included the upper octave in our range
    n = np3.asInt();
    EXPECT_EQ(n, 125);
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

TEST_F(NoteTest, GettersWork)
{
    NumberedPitch c { 60 };
    NumberedPitch d { 62 };
    NumberedPitch f { 65 };
    NumberedPitch e { 64 };
    EXPECT_EQ(c1.getPitch().asInt(), c.asInt());
    EXPECT_EQ(d1.getPitch().asInt(), d.asInt());
    EXPECT_EQ(f1.getPitch().asInt(), f.asInt());
    EXPECT_EQ(e1.getPitch().asInt(), e.asInt());
    EXPECT_EQ(c1.getRtime(), 20.0);
    EXPECT_EQ(d1.getRtime(), 30.0);
    EXPECT_EQ(f1.getRtime(), 40.0);
    EXPECT_EQ(e1.getRtime(), 50.0);
    EXPECT_EQ(c1.getProb(), 20.0);
    EXPECT_EQ(d1.getProb(), 50.0);
    EXPECT_EQ(f1.getProb(), 60.0);
    EXPECT_EQ(e1.getProb(), 70.0);
}

//TEST_F(NoteTest, CopyConstructorWorks)
//{
//    NumberedPitch c { 60 };
//    float xloc { 0.5 };
//    float yloc { 0.7 };
//    Note n { c, xloc, yloc };
//    Note n2 { n };
//    EXPECT_EQ(n.getPitch().asInt(), n2.getPitch().asInt());
//}

TEST_F(HarmonicFieldManagerTest, QuantizesSinglePitch)
{
    NumberedPitch cs { 61 };
    NumberedPitch q1 { cmanager.getQuantizedPitch(cs) };
    EXPECT_EQ(q1.asInt(), 62);
    NumberedPitch neg { -10000 };
    NumberedPitch large { 10000 };
    NumberedPitch borderlineneg { -1 };
    NumberedPitch borderlinepos { 128 };
    try {
        NumberedPitch q2 { cmanager.getQuantizedPitch(neg) };
        NumberedPitch q3 { cmanager.getQuantizedPitch(large) }; // think these three might be redundant
        NumberedPitch q4 { cmanager.getQuantizedPitch(borderlineneg) };
        NumberedPitch q5 { cmanager.getQuantizedPitch(borderlinepos) };
        FAIL() << "Expected std::invalid_argument";
    }
    catch(std::invalid_argument & err) {
        EXPECT_EQ(err.what(), std::string("Invalid transposition"));
    }
    catch(...) {
        FAIL() << "Expected std::invalid_argument";
    }
}

TEST_F(HarmonicFieldManagerTest, DoesNotQuantize)
// if the value is equal to one specified by the field's pichset, leave as is
{
    NumberedPitch c4 { 60 };
    NumberedPitch d4 { 62 };
    NumberedPitch f4 { 65 };
    NumberedPitch f8 { 125 };
    NumberedPitch np;
    np.setPitch(dmanager.getQuantizedPitch(c4));
    EXPECT_EQ(np.asInt(), 60);
    np.setVal(0);
    np.setPitch(dmanager.getQuantizedPitch(d4));
    EXPECT_EQ(np.asInt(), 62);
    np.setVal(0);
    np.setPitch(dmanager.getQuantizedPitch(f4));
    EXPECT_EQ(np.asInt(), 65);
    np.setVal(0);
    np.setPitch(dmanager.getQuantizedPitch(f8));
    EXPECT_EQ(np.asInt(), 125);
    
}

TEST_F(QuantizedPitchManagerTest, AccessToLocalHfields)
{
    NumberedPitch tespitch1 { 61 }; // cs
    NumberedPitch tespitch2 { 59 }; // b
    NumberedPitch tespitch3 { 63 }; // ds
    NumberedPitch tespitch4 { 66 }; // fs
    vector<NumberedPitch> origins { 61, 59, 63, 66 };
    vector <int> targets {62, 60, 64, 67};
    NumberedPitch qpitch;
    for(int i = 0; i < notemaker.numFields(); i++){
        qpitch.setPitch(notemaker.getQuantizedPitch(i, origins[i]));
        EXPECT_EQ(qpitch.asInt(), targets[i]);
    }
}

TEST_F(QuantizedPitchManagerTest, InitializesPitchFromMidiNote)
{
    int C4 { 60 };
    notemaker.processMidiNote(C4);
    NumberedPitch np; 
    np.setPitch(notemaker.getOriginalPitch());
    EXPECT_EQ(np.asInt(), C4);
}

TEST_F(QuantizedPitchManagerTest, QuantizedValuesPushedToNotes)
{
    // harmonic fields are cmajor | dminor | flydian | ephrygian
    NumberedPitch testpitch1 { 61 }; // 
    notemaker.processMidiNote(testpitch1.asInt());
    vector<int> expected { 62, 62, 62, 62 };

    Note tn; 
    testQueue(tn, expected);

    testpitch1.setVal(59); // b 
    vector<int> expected2 { 59, 60, 60, 60 };
    notemaker.processMidiNote(testpitch1.asInt());
    testQueue(tn, expected2);

    testpitch1.setVal(63); // ds 
    vector<int> expected3 { 64, 64, 64, 64 };
    notemaker.processMidiNote(testpitch1.asInt());
    testQueue(tn, expected3);
    
    testpitch1.setVal(66); // fs 
    vector<int> expected4 { 67, 67, 67, 67 };
    notemaker.processMidiNote(testpitch1.asInt());
    testQueue(tn, expected4);
}

TEST_F(QuantizedPitchManagerTest, NoteRvalueSetCorrectly)
{
    notemaker.setup();
    NumberedPitch testpitch1 { 61 }; // 
    notemaker.processMidiNote(testpitch1.asInt());
    EXPECT_EQ(notemaker.getRval(0), 17.0); 
    EXPECT_EQ(notemaker.getRval(1), 41.0);
    EXPECT_EQ(notemaker.getRval(2), 65.0); 
    EXPECT_EQ(notemaker.getRval(3), 89.0); 
}

TEST_F(QuantizedPitchManagerTest, NoteProbSetCorrectly)
{
    notemaker.setup();
    NumberedPitch testpitch1 { 61 };
    notemaker.processMidiNote(testpitch1.asInt());
    EXPECT_EQ(notemaker.getProb(0), 19);
    EXPECT_EQ(notemaker.getProb(1), 43);
    EXPECT_EQ(notemaker.getProb(2), 67);
    EXPECT_EQ(notemaker.getProb(3), 91);
}

//-----------------------------------------------------------------------------
//
//

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



