#include "HarmonicField.h"

//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

TimespanGraph::TimespanGraph()
    : numhfields { MINFIELDS } {} 

TimespanGraph::TimespanGraph(const int numharmonicfields)
    : numhfields{numharmonicfields}
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

HarmonicFieldGraph::HarmonicFieldGraph()
    :name{DNAME}, timespan{TSGDEFAULT} {}

//-----------------------------------------------------------------------------
//

void HarmonicFieldGraph::setup(){
    // use _pos to determine position on timeline 
    // represents a subsection on the horizontal axis
    
    // setup member variables to handle location and positioning
    length    = timespan.getLength() / timespan.numhfields;
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
// Interval segment manager: used to handle the interval distribution needed to
// construct a harmonic field in a specific tonality.
// i.e. The interval values stored here are an abstraction and describe a path
// of n steps through an octave. 

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
// Higher level logic classes, factories and managers
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

int QuantizedPitchManager::getRval(int n) // gets xcoord of nth harmonic field graph
{
    if((0 <= n) && (n < _numfields)) // in range, proceed
        return _vhfm[n].getRtime();
    else
        throw std::out_of_range("HField index out of range");
}

int QuantizedPitchManager::getProb(int n) // gets ycoord of nth harmonic field graph
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
    // quantized pitches are pushed on to a FiFo stack // queue 
    // quantizers work in a pipeline and modify pitch continuously 
    // q1 = quantize(p1) 
    // q2 = quantize(q1)
    // q3 = quantize(q2)
    // q4 = quantize(q3)
    NumberedPitch np { midiVal }; 
    _pitch = NumberedPitch { np } ; // set this as our starting pitch
    NumberedPitch qp0, qp1, qp2, qp3;
    
    float rval0, rval1, rval2, rval3;
    float prob0, prob1, prob2, prob3;
    qp0 = getQuantizedPitch(0, np); // start with incoming pitch at index 0
    rval0 = getRval(0);
    prob0 = getProb(0);
    Note n0 { qp0, rval0, prob0 };
    _notes.push(n0);
    qp1 = getQuantizedPitch(1, qp0);
    rval1 = getRval(1);
    prob1 = getProb(1);
    Note n1 { qp1, rval1, prob1 };
    _notes.push(n1);
    qp2 = getQuantizedPitch(2, qp1);
    rval2 = getRval(2);
    prob2 = getProb(2);
    Note n2 { qp2, rval2, prob2 };
    _notes.push(n2);
    qp3 = getQuantizedPitch(3, qp2);
    rval3 = getRval(2);
    prob3 = getProb(2);
    Note n3 { qp3, rval3, prob3 };
    _notes.push(n3);


    // TODO: fix the segfault in this loop, 
    // likely cause is trying to access an address that's out of range
    // it should be possible to wrap the above logic 
    // in a for loop along the following lines:
    //vector<NumberedPitch> qpitches { np, qp0, qp1, qp2, qp3 };
    //vector<float> rvals;
    //vector<float> probs;
    //for(int i = 0; i < qpitches.size(); i++){
    //    qpitches[i + 1] = getQuantizedPitch(i, qpitches[i]); 
    //    rvals[i] = getRval(i);
    //    probs[i] = getProb(i);
    //    Note n { qpitches[i+1], rvals[i], probs[i] };
    //    _notes.push(n);
    //}
}


void QuantizedPitchManager::popNote()
{
    _notes.pop();
}


