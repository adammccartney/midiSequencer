#include "HarmonicField.h"


Timespan::Timespan(const int nhf)
    : numHfields{nhf}
{
    _start.x = 0.0;
    _start.y = ofGetHeight() / 2.0;
    _end.x   = ofGetWidth();
    _end.y   = ofGetHeight() / 2.0;
    _len     = _start.distance(_end);
}

Timespan::~Timespan(){}

float Timespan::getLength(){ return _len; }

void Timespan::draw(){
    ofDrawLine(_start.x, _start.y, _end.x, _end.y);
}

//-----------------------------------------------------------------------------
// 

HarmonicFieldGraph::HarmonicFieldGraph(char n, Timespan& ts)
    :name{n}, timespan{ts}
{
    this->setID();
}

HarmonicFieldGraph::~HarmonicFieldGraph(){}

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


//-----------------------------------------------------------------------------
//

IntervalSegmentManager::ModeIntervalMap 
IntervalSegmentManager::mintMap = []
{
    IntervalSegmentManager::ModeIntervalMap mimap
        // stores the interval steps needed to create a specific scale or mode
    {
        {"major", IntervalSegment(vector<int>{2, 2, 1, 2, 2, 2, 1})},//"M2 M2 m2 M2 M2 M2 m2"
        {"minor", IntervalSegment(vector<int>{2, 2, 1, 2, 2, 2, 1})},//"M2 m2 M2 M2 m2 M2 M2"
        {"hminor", IntervalSegment(vector<int>{2, 2, 1, 2, 2, 2, 1})},//"M2 m2 M2 M2 M2 m2 M2"
        {"dorian", IntervalSegment(vector<int>{2, 1, 2, 2, 2, 1, 2})},//"M2 m2 M2 M2 M2 m2 M2"
        {"phrygian", IntervalSegment(vector<int>{1, 2, 2, 2, 1, 2, 2})},//"m2 M2 M2 M2 m2 M2 M2"
        {"lydian", IntervalSegment(vector<int>{2, 2, 2, 1, 2, 2, 1})},//"M2 M2 M2 m2 M2 M2 m2"
        {"mixolydian", IntervalSegment(vector<int>{2, 2, 1, 2, 2, 1, 2})},//"M2 M2 m2 M2 M2 m2 M2"
        {"locrian", IntervalSegment(vector<int>{1, 2, 2, 2, 1, 2, 2})},//"m2 M2 M2 M2 m2 M2 M2"
        {"maj7", IntervalSegment(vector<int>{4, 3, 4})},//"M3 m3 M3"
        {"D7", IntervalSegment(vector<int>{4, 3, 3})},//"M3 m3 m3"
        {"6", IntervalSegment(vector<int>{3, 5})},//"m3 P4"
        {"m6", IntervalSegment(vector<int>{4, 5})},//"M3 P4"
        {"64", IntervalSegment(vector<int>{5, 4})},//"P4 M3"
        {"m64", IntervalSegment(vector<int>{5, 3})},//"P4 m3"
        {"65", IntervalSegment(vector<int>{4, 3, 2})},//"M3 m3 M2"
        {"m65", IntervalSegment(vector<int>{3, 4, 1})},//"m3 M3 m2"
        {"54", IntervalSegment(vector<int>{5, 2})},//"P4 M2"
        {"m54", IntervalSegment(vector<int>{5, 1})},//"P4 m2"
        {"42", IntervalSegment(vector<int>{2, 3})},//"M2 m3"
        {"m42", IntervalSegment(vector<int>{1, 4})},//"m2 M3"
    };
    return mimap;
}();

//-----------------------------------------------------------------------------

void NumberedPitch::transpose(int n)
{
    val += n;
}


//-----------------------------------------------------------------------------

void PitchSegment::transpose(int n)
{
    for(int i = 0; i < pitches.size(); i++){
        pitches[i].transpose(n);
    }
}


//-----------------------------------------------------------------------------

MidiPitchContainer::MidiPitchContainer(const PitchClass &root, const IntervalSegment &mode)
   : _root { root }, _mode { mode } 
{
    _minpitch = (int)root- MIDCOFFSET; 
} 

//-----------------------------------------------------------------------------
//

void MidiPitchContainer::makePitchClassSegment()
{
    
}

