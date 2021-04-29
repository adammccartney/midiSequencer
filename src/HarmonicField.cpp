#include "HarmonicField.h"

//-----------------------------------------------------------------------------
// Graphics Ćlasses
//

TimespanGraph::TimespanGraph(const int numharmonicfields)
    : numhfields{numharmonicfields}
{
    _start.x = 0.0;
    _start.y = ofGetHeight() / 2.0;
    _end.x   = ofGetWidth();
    _end.y   = ofGetHeight() / 2.0;
    _len     = _start.distance(_end);
}

TimespanGraph::TimespanGraph()
    : numhfields { constants::GLOBAL_CONST_NUM_HFIELDS} {}

float TimespanGraph::getLength() const { return _len; }

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
    : name { constants::GLOBAL_CONST_HFGNAME}, 
      timespan { constants::GLOBAL_CONST_DEFTIMESPAN } {}

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

    Mode defaultmode = Mode::MAJOR;
    PitchClass defaultroot = PitchClass::c;

    params.setName(toString());
    params.add(x.set("x", hfxOrigin, localMin, localMax));
    params.add(y.set("y", ofGetHeight() / 2, 0, ofGetHeight())); 
    params.add(mode.set("MODE", defaultmode, Mode::MAJOR, Mode::FOUR2));
    params.add(root.set("ROOT", defaultroot, PitchClass::c, PitchClass::b));


    update();
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

void HarmonicFieldGraph::update(){
    updatePoints();
}

void HarmonicFieldGraph::draw(){

    update();
   
    // draw paths according to points
    for(int i = 0; i < constants::GLOBAL_CONST_CHROMA; i++){  // 12 chrom steps
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
            npath.setFilled(_filldata[i]); // visualizes harmonic info 
            npath.setStrokeWidth(2); 
        }else{
            npath.setStrokeColor(ofColor::white);
            npath.setFillColor(ofColor::white);
            npath.setFilled(_filldata[i]);
            npath.setStrokeWidth(2);
        }
        npath.draw();
    }
}

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

void HarmonicFieldGraph::setFillData(const vector<int> &filldata){
    for(auto i = 0; i < filldata.size(); i++){
        if(filldata[i] == 1){
            _filldata[i] = true;
        }
        else
            _filldata[i] = false;
    }
    // pass this info to GUI
}

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
    if(((_val + n) > 0) && (n < constants::GLOBAL_CONST_MIDIMAX)){ // the request is in range
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
// IntervalSegment
// Constructors

IntervalSegment::IntervalSegment(){}

IntervalSegment::IntervalSegment(vector<int> v) : intervals { v }{}

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
// 
//
// PitchSetManager
// handles construction of pitch and pitch class sets, given ref to root & mode

// Constructors
//
PitchSetManager::PitchSetManager(PitchClass &root, IntervalSegment &mode)
   : _root { root }, _mode { mode } 
{
    _minpitchval = (int)root; 
    makePitchClassSet();
    makePitchSet();
} 

PitchSetManager::PitchSetManager() : _mode {} {}

PitchSetManager::PitchSetManager(const PitchSetManager& n)
    : _root { n._root }, _mode { n._mode }
{
    _minpitchval = (int)n._root;
    makePitchClassSet();
    makePitchSet();
}

PitchSetManager& PitchSetManager::operator=(const PitchSetManager& n)
{
    _root =  n._root;
    _mode = n._mode;
    _minpitchval = (int)n._root;
    makePitchClassSet();
    makePitchSet();
    return *this;
}

void PitchSetManager::init(PitchClass &root, IntervalSegment &intseg)
{
    setRoot(root);
    setIntseg(intseg);
    _minpitchval = (int)root;
    makePitchClassSet();
    makePitchSet();
}

//-----------------------------------------------------------------------------
// Member Functions

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
    _pcset = { pcset }; 
}

////-----------------------------------------------------------------------------
////

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
// HarmonicField

HarmonicField::HarmonicField(){}

//-----------------------------------------------------------------------------
// HarmonicFieldManager 
//combines logic HarmonicField and HarmonicFieldGraph, makes Notes for output 

HarmonicFieldManager::HarmonicFieldManager(HarmonicField &hfield, HarmonicFieldGraph &hfgraph)
    // pitchset is just a reference for the quantizer so it knows what values
    // to use while processing an incoming pitch
    : _hfield { hfield }, 
      _hfgraph { hfgraph }, 
      _pitchset { hfield.getPitchSet() }, 
      _pitchclassset { hfield.getPitchClassSet() }
{
    _rtime = hfgraph.getXPos();
    _probability = hfgraph.getYPos();
}

HarmonicFieldManager::HarmonicFieldManager(){}

void HarmonicFieldManager::init(HarmonicField &hfield, HarmonicFieldGraph &hfgraph)
{
    _pitchset      = hfield.getPitchSet();
    _pitchclassset = hfield.getPitchClassSet();
    _rtime         = hfgraph.getXPos();
    _probability   = hfgraph.getYPos();
}

void HarmonicFieldManager::setFillData(){
    // beginning of pipe to harmonic field graph
    // communicates info on the harmonic instance 
    
    // initialize zero shading
    for(auto i = 0; i < _filldata.size(); i++){
        _filldata[i] = 0;
    }
    int pcindex = 0;
    // read pitchclasses 
    for(auto i = 0; i < _pitchclassset.size(); i++){
        // "turn on" pitchclass shading for whatever pitch classes are present
        pcindex = _pitchclassset[i].asInt();
        _filldata[pcindex] = 1; // turn shading on for specific "key" / pitchclass 
    }
    // display the keys shaded according to tonality
    _hfgraph.setFillData(_filldata);
}

////-----------------------------------------------------------------------------

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
// QuantizedPitchManager
// constructors 

QuantizedPitchManager::QuantizedPitchManager(){} 

QuantizedPitchManager::QuantizedPitchManager(const vector<HarmonicFieldManager*> &vhfm)
    : _numfields { constants::GLOBAL_CONST_NUM_HFIELDS }, _vhfm { vhfm } {}


//-----------------------------------------------------------------------------
//

NumberedPitch QuantizedPitchManager::getQuantizedPitch(int hfindex, NumberedPitch p)
// calls the get QuantizedPitch function of a specific hfield
// returns quantized pitch
{
    NumberedPitch qp;
    qp.setPitch(_vhfm[hfindex]->getQuantizedPitch(p));
    return qp;
}

//-----------------------------------------------------------------------------
// X/Y Coordinate getters
// usage: gui coordinates contain rhythmic and probabilty info about a note 

int QuantizedPitchManager::getRval(int n) // gets xcoord of nth harmonic field graph
{
    if((0 <= n) && (n < _numfields)) // in range, proceed
        return _vhfm[n]->getRtime();
    else
        throw std::out_of_range("HField index out of range");
}

int QuantizedPitchManager::getProb(int n) // gets ycoord of nth harmonic field graph
{
    if((0 <= n) && (n < _numfields)) // in range, proceed
        return _vhfm[n]->getProb();
    else
        throw std::out_of_range("HField index out of range");
}

//-----------------------------------------------------------------------------
// Here the incoming midi note is quantized and pushed to the stack of notes in
// the object


void QuantizedPitchManager::processMidiNote(const int &midiVal, ofxOscMessage &m)
{
    NumberedPitch tmp { midiVal };
    _pitch = NumberedPitch { tmp } ; // set this as our starting pitch
   
    int rvalue, probability;
    NumberedPitch qp;
    for(int i = 0; i < constants::GLOBAL_CONST_NUM_HFIELDS; i++){
        qp.setPitch(getQuantizedPitch(i, tmp)); 
        rvalue = getRval(i);
        probability = getProb(i);
        Note n { qp, rvalue, probability };
        _notes.push(n);
        tmp.setPitch(qp);
    }
    // send the osc message
    NumberedPitch np;
    int qmidival, rval, prob;
    for(int i = 0; i < constants::GLOBAL_CONST_NUM_HFIELDS; i++){
        Note n { getNote() };
        np        = n.getPitch();
        qmidival  = np.asInt();
        rval      = n.getRtime();
        prob      = n.getProb();
        m.setAddress("/test");
        m.addIntArg(qmidival);
        m.addIntArg(rval);
        m.addIntArg(prob);
        popNote(); // pop the note from queue as soon as we extract the vals
    }
}


