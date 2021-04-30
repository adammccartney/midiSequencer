#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "HarmonicField.h"
#include "ofxMidi.h"

//----------------------------------------------------------------------------
// Setup for Osc, define socket info

// send host (aka IP address) 
#define HOST "localhost"

// send port
#define PORT 12345

//----------------------------------------------------------------------------
// The app takes a midi note in, processes by sending through four harmonic
// fields that quantize the original pitch according to their parameters

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);

		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);

        // Gui setup
        // There are four input controls per harmonic field 
        ofxPanel gui;
        ofParameter<int> hfield1Slider;
        ofParameter<string> hfield1mode;
        ofParameter<string> hfield1root;
        ofParameter<int> hfield2Slider;
        ofParameter<string> hfield2mode;
        ofParameter<string> hfield2root;
        ofParameter<int> hfield3Slider;
        ofParameter<string> hfield3mode;
        ofParameter<string> hfield3root;
        ofParameter<int> hfield4Slider;
        ofParameter<string> hfield4mode;
        ofParameter<string> hfield4root;
        ofParameterGroup mainGroup;

        // Setup harmonic field graphs and harmonic fields
        //
        // Timespan Graph is what coordinates the horizontal positioning 
        // of the Harmonic Field objects, the timespan is also necessary 
        // for drawing the GUI objects below
        TimespanGraph timespangraph{constants::GLOBAL_CONST_NUM_HFIELDS};  // timespangraph with 4 subsections (harmonic fields)
        // 4 Harmonic Field Graph instantces. Position of these graphs in
        // the display window depends on the sequential position (indicated by
        // the name)  
        HarmonicFieldGraph hfgraph1{'a', timespangraph}; 
        HarmonicFieldGraph hfgraph2{'b', timespangraph};  
        HarmonicFieldGraph hfgraph3{'c', timespangraph};  
        HarmonicFieldGraph hfgraph4{'d', timespangraph};
        // Pointers to these graphs are pushed on to a vector and accessed
        // within the main ofApp::setup() and ofApp::update() functions
        const vector <HarmonicFieldGraph*> hfgrphs {&hfgraph1, &hfgraph2, &hfgraph3, &hfgraph4} ;
        // Static method for looking up modes
        IntervalSegmentManager intsegman;
        // 4 IntervalSegment instances to associate a mode with a hfield
        IntervalSegment intseg1;
        IntervalSegment intseg2;
        IntervalSegment intseg3;
        IntervalSegment intseg4;
        // intervalsegments accessed and populated in ofApp::update()
        const vector <IntervalSegment*> intseg {&intseg1, &intseg2, &intseg3, &intseg4}; 
        PitchSetManager psman1;
        PitchSetManager psman2;
        PitchSetManager psman3;
        PitchSetManager psman4;
        // pitchsetmanagers accessed and populated in ofApp::update()
        const vector <PitchSetManager*> psmanager {&psman1, &psman2, &psman3, &psman4}; 
        // 4 Harmonic Field instances that are analogous to the Harmonic Field
        // Graph
        HarmonicField hfield1;
        HarmonicField hfield2;
        HarmonicField hfield3;
        HarmonicField hfield4;
        // harmonicfields accessed and populated in ofApp::update()
        const vector <HarmonicField*> hfields {&hfield1, &hfield2, &hfield3, &hfield4}; 
        HarmonicFieldManager hfman1;
        HarmonicFieldManager hfman2;
        HarmonicFieldManager hfman3;
        HarmonicFieldManager hfman4;
        // harmonicfieldmanagers accessed and populated in ofApp::update()
        const vector <HarmonicFieldManager*> hfmanager {&hfman1, &hfman2, &hfman3, &hfman4};
        // qpmanager accessed within ofApp::drawMidiMessages() 
        QuantizedPitchManager qpmanager {hfmanager};

        // Midi setup
        void drawMidiMessages();
        void newMidiMessage(ofxMidiMessage& eventArgs);
        
        ofxMidiIn midiIn;
        std::vector<ofxMidiMessage> midiMessages;
        std::size_t maxMessages = 10; //< max number of messages to keep track of

        // Osc setup
        ofxOscSender sender;

        // help menu
        ofTrueTypeFont vagRounded;
        string helpstring;
        string modestring;
        string guistring;
        string commstring;
        string rootstring;
        string statestring;
};
