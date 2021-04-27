#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "HarmonicField.h"
#include "ofxMidi.h"

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
        ofParameterGroup sliderGroup;
        ofParameter<int> hfield1Slider;
        ofParameter<int> hfield2Slider;
        ofParameter<int> hfield3Slider;
        ofParameter<int> hfield4Slider;
        ofParameterGroup mainGroup;

        TimespanGraph timespangraph{4};  // timespangraph with 4 subsections (harmonic fields)
        HarmonicFieldGraph hfield1{'a', timespangraph}; 
        HarmonicFieldGraph hfield2{'b', timespangraph};  
        HarmonicFieldGraph hfield3{'c', timespangraph};  
        HarmonicFieldGraph hfield4{'d', timespangraph};
        const vector <HarmonicFieldGraph*> hfgrphs {&hfield1, &hfield2, &hfield3, &hfield4} ;
        
        // Midi setup
        void newMidiMessage(ofxMidiMessage& eventArgs);
        
        ofxMidiIn midiIn;
        std::vector<ofxMidiMessage> midiMessages;
        std::size_t maxMessages = 10; //< max number of messages to keep track of
};
