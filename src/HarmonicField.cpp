#include "HarmonicField.h"
#include <chrono>

//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

TimespanGraph::TimespanGraph(const int numharmonicfields)
    : numHfields{numharmonicfields}
{
    _start.x = 0.0;
    _start.y = ofGetHeight() / 2.0;
    _end.x   = ofGetWidth();
    _end.y   = ofGetHeight() / 2.0;
    _len     = _start.distance(_end);
}

float TimespanGraph::getLength(){ return _len; }

void TimespanGraph::draw(){
    ofDrawLine(_start.x, _start.y, _end.x, _end.y);
}

//-----------------------------------------------------------------------------
// 

HarmonicFieldGraph::HarmonicFieldGraph(char n, TimespanGraph& ts)
    :name{n}, timespan{ts}
{
    this->setID();
}

//-----------------------------------------------------------------------------
//

void HarmonicFieldGraph::setup(){
    // use _pos to determine position on timeline 
    // represents a subsection on the horizontal axis
    
    // setup member variables to handle location and positioning
    length    = timespan.getLength() / timespan.numHfields;
    hfxOrigin = timespan.getStart().x + ( length * (getID() - 1)); // from 0 
    localMin  = hfxOrigin;
    localMax  = hfxOrigin + length - getWidth(); 

    params.setName(toString());
    params.add(x.set("x", hfxOrigin, localMin, localMax));
    params.add(y.set("y", ofGetHeight() / 2, 0, ofGetHeight())); 

    updatePoints();
}

void HarmonicFieldGraph::updatePoints(){
    // this grabs info from the gui Sliders and updates the shape's data 

    keypoints.clear();
    
    // this part is setup of the basic keyboard shape
    for(int i = 0; i < _numtones; i++){
        vector <ofVec2f> vec{ ofVec2f(x, y - i*10),
                              ofVec2f(x + _width, y - i*10),
                              ofVec2f(x + _width, y - i*10 - _height),
                              ofVec2f(x, y - i*10 - _height)};
        keypoints.push_back(vec);
    }
}

void HarmonicFieldGraph::draw(){

    updatePoints();
   
    // draw paths according to points
    for(int i = 0; i < (int)keypoints.size(); i++){
        ofPath npath;
        npath.moveTo(keypoints[i][0]);
        npath.lineTo(keypoints[i][1]);
        npath.lineTo(keypoints[i][2]);
        npath.lineTo(keypoints[i][3]);
        npath.close(); 
        // color according to piano keyboard
        PitchClass x = (PitchClass)i;
        if((x == PitchClass::cs) || (x == PitchClass::ds) || 
                (x == PitchClass::fs) || (x == PitchClass::gs) || 
                (x == PitchClass::as)){
            npath.setStrokeColor(ofColor::black);
            npath.setFillColor(ofColor::black);
            npath.setFilled(true);
            npath.setStrokeWidth(2);
        }else{
            npath.setStrokeColor(ofColor::black);
            npath.setFillColor(ofColor::white);
            npath.setFilled(true);
            npath.setStrokeWidth(2);
        }
        npath.draw();
    }
}

void HarmonicFieldGraph::update(){}

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
//-----------------------------------------------------------------------------
// Operators for pitch class enum 
//

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

//-----------------------------------------------------------------------------
// NumberedPitchClass Members functions

void NumberedPitchClass::init(){
    _val = (int)name;
}

void NumberedPitchClass::NumberedPitchClass::transpose(int n)
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
// Numbered Pitch member functions

// Default constructor
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



//----------------------------------------------------------------------------
//

QuantizedPitch::QuantizedPitch(NumberedPitch &np, ofParameterGroup &params)
    : _pitch { np }, _params { params }
{
    //TODO: make sure that params are getting put to good use

}

//-----------------------------------------------------------------------------
// Interval segment manager: used to handle the interval distribution needed to
// construct a harmonic field in a specific tonality.
// i.e. The interval values stored here are an abstraction and describe a path
// of n steps through an octave. 

IntervalSegmentManager::IntervalSegmentManager(){ makeMap(); }

map<Mode, vector<int>> IntervalSegmentManager::modeIntervalMap;

void IntervalSegmentManager::makeMap() {
    // sum of all interval steps from any root should equal octave
    // we're using 12-tone equal temperament 
    // so 12 steps ... like an alcoholic in recovery or a dodecahedron
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
    modeIntervalMap[FOUR2]=vector<int>{2, 3};//"M2 m3"
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
// Pitch Quantizer Class
//
PitchQuantizer::PitchQuantizer(const HarmonicField &hfield)
    // pitchset is just a reference for the quantizer so it knows what values
    // to use while processing an incoming pitch
    : _pitchset { hfield.getPitchSet() }{}

//-----------------------------------------------------------------------------
// 

NumberedPitch PitchQuantizer::getQuantizedPitch(const NumberedPitch &inpitch){
    // Find inpitch in this objects _pitchset
    // if not found tranpose up semitone and recurse
    //
    NumberedPitch n { inpitch.asInt() }; // initialize a new pitch
    
    auto p = find(_pitchset.begin(), _pitchset.end(), n);

    if(p != _pitchset.end()){
        return *p;
    }
    else{
        n.transpose(+1);
        return getQuantizedPitch(n);
    }
}


//-----------------------------------------------------------------------------
// 

void QuantizedPitchManager::setup(){
    // for x in harmonicfieldgraphs
    //   
}

