#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "HarmonicField.h"
#include "ofxMidi.h"
#include "ofxOsc.h"

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
        TimespanGraph timespangraph{constants::GLOBAL_CONST_NUM_HFIELDS};  // timespangraph with 4 subsections (harmonic fields)
        HarmonicFieldGraph hfgraph1{'a', timespangraph}; 
        HarmonicFieldGraph hfgraph2{'b', timespangraph};  
        HarmonicFieldGraph hfgraph3{'c', timespangraph};  
        HarmonicFieldGraph hfgraph4{'d', timespangraph};
        const vector <HarmonicFieldGraph*> hfgrphs {&hfgraph1, &hfgraph2, &hfgraph3, &hfgraph4} ;
        HarmonicField hfield1;
        HarmonicField hfield2;
        HarmonicField hfield3;
        HarmonicField hfield4;
        const vector <HarmonicField*> hfields {&hfield1, &hfield2, &hfield3, &hfield4}; 
        IntervalSegmentManager intsegman;
        IntervalSegment intseg1;
        IntervalSegment intseg2;
        IntervalSegment intseg3;
        IntervalSegment intseg4;
        const vector <IntervalSegment*> intseg {&intseg1, &intseg2, &intseg3, &intseg4}; 
        PitchSetManager psman1;
        PitchSetManager psman2;
        PitchSetManager psman3;
        PitchSetManager psman4;
        const vector <PitchSetManager*> psmanager {&psman1, &psman2, &psman3, &psman4}; 
        HarmonicFieldManager hfman1;
        HarmonicFieldManager hfman2;
        HarmonicFieldManager hfman3;
        HarmonicFieldManager hfman4;
        const vector <HarmonicFieldManager*> hfmanager {&hfman1, &hfman2, &hfman3, &hfman4};
        QuantizedPitchManager qpmanager {hfmanager};

        // Midi setup
        void drawMidiMessages();
        void newMidiMessage(ofxMidiMessage& eventArgs);
        
        ofxMidiIn midiIn;
        std::vector<ofxMidiMessage> midiMessages;
        std::size_t maxMessages = 10; //< max number of messages to keep track of

        // Osc setup
        ofxOscSender sender;
};
