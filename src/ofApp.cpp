#include "ofApp.h"
#include "HarmonicField.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
    ofBackground(238, 255, 153);
    ofSetFrameRate(60);
	ofSetLogLevel(OF_LOG_VERBOSE);
    vagRounded.load("vag.ttf", 24);

	// print input ports to console
    midiIn.listInPorts();

	// open port by number (you may need to change this)
	midiIn.openPort(1);
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);

	// add ofApp as a listener
	midiIn.addListener(this);
	
    // print received messages to the console
	midiIn.setVerbose(true);

    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, PORT);

    for(auto i = 0; i < constants::GLOBAL_CONST_NUM_HFIELDS; i++){
        // set up some naming conventions for hfield parameters
        hfgrphs[i]->setup();
        mainGroup.add(hfgrphs[i]->params);
    }
    // send back shading data
    gui.setup(mainGroup);

}

//--------------------------------------------------------------
void ofApp::update(){

    int tmpmode, tmproot;
    for(auto i = 0; i < constants::GLOBAL_CONST_NUM_HFIELDS; i++){
        // use info from gui to update logic
        tmpmode = hfgrphs[i]->getMode();
        tmproot = hfgrphs[i]->getRoot();
        
        auto root = PitchClass(tmproot);
        auto mode = Mode(tmpmode);
        // first make segment
        intseg[i]->intervals = intsegman.modeIntervalMap[mode];
        // use segment to make PitchSets
        psmanager[i]->init(root, *intseg[i]);
        // update logic from 
        hfields[i]->init(*psmanager[i]);
        // unify pitchset data with GUI data 
        hfmanager[i]->init(*hfields[i], *hfgrphs[i]);
        hfmanager[i]->setFillData();
        hfgrphs[i]->setFillData(hfmanager[i]->getFillData());
    }
    
}


void ofApp::drawMidiMessages(){
    for(unsigned int i = 0; i < midiMessages.size(); ++i) {

		ofxMidiMessage &message = midiMessages[i];
		int x = 10;
		int y = i*40 + 40;

		// draw the last recieved message contents to the screen,
		// this doesn't print all the data from every status type
		// but you should get the general idea
		stringstream text;
		text << ofxMidiMessage::getStatusString(message.status);
		while(text.str().length() < 16) { // pad status width
			text << " ";
		}

		ofSetColor(127);
		if(message.status < MIDI_SYSEX) {
			text << "chan: " << message.channel;
            if(message.status == MIDI_NOTE_ON) {
                ofxOscMessage m; // each incoming midi note on gets once osc msg
                qpmanager.processMidiNote(message.pitch, m);
				sender.sendMessage(m, false);
            }
			if(message.status == MIDI_NOTE_ON ||
			   message.status == MIDI_NOTE_OFF) {
				text << "\tpitch: " << message.pitch;
                ofDrawRectangle(x + ofGetWidth()*0.2, y + 12,
					ofMap(message.pitch, 0, 127, 0, ofGetWidth()*0.2), 10);
				text << "\tvel: " << message.velocity;
				ofDrawRectangle(x + (ofGetWidth()*0.2 * 2), y + 12,
					ofMap(message.velocity, 0, 127, 0, ofGetWidth()*0.2), 10);
			}
			if(message.status == MIDI_CONTROL_CHANGE) {
				text << "\tctl: " << message.control;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12,
					ofMap(message.control, 0, 127, 0, ofGetWidth()*0.2), 10);
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2 * 2, y + 12,
					ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}
			else if(message.status == MIDI_PROGRAM_CHANGE) {
				text << "\tpgm: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12,
					ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}
			else if(message.status == MIDI_PITCH_BEND) {
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12,
					ofMap(message.value, 0, MIDI_MAX_BEND, 0, ofGetWidth()*0.2), 10);
			}
			else if(message.status == MIDI_AFTERTOUCH) {
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12,
					ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}
			else if(message.status == MIDI_POLY_AFTERTOUCH) {
				text << "\tpitch: " << message.pitch;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12,
					ofMap(message.pitch, 0, 127, 0, ofGetWidth()*0.2), 10);
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2 * 2, y + 12,
					ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}
			text << " "; // pad for delta print
		}
		else {
			text << message.bytes.size() << " bytes ";
		}

		text << "delta: " << message.deltatime;
		ofSetColor(0);
		ofDrawBitmapString(text.str(), x, y);
		text.str(""); // clear
	} // draw midi messages
}

//--------------------------------------------------------------
void ofApp::draw(){

    drawMidiMessages(); 
    timespangraph.draw();
    for(auto i = 0; i < hfgrphs.size(); i++){
        hfgrphs[i]->draw();
    }
    gui.draw();

    // Draw help strings 
    ofSetHexColor(0xffffff);
    vagRounded.drawString(helpstring, 98, 498 );
    
    ofSetColor(255,122,220);
    vagRounded.drawString(helpstring, 100, 500 );

    ofSetHexColor(0xffffff);
    vagRounded.drawString(modestring, 98, 448 );
    
    ofSetColor(255,122,220);
    vagRounded.drawString(modestring, 100, 450 );
    
    ofSetHexColor(0xffffff);
    vagRounded.drawString(guistring, 98, 498 );
    
    ofSetColor(255,122,220);
    vagRounded.drawString(guistring, 100, 500 );
    
    ofSetHexColor(0xffffff);
    vagRounded.drawString(commstring, 88, 498 );
    
    ofSetColor(255,122,220);
    vagRounded.drawString(commstring, 90, 500 );
    
    ofSetHexColor(0xffffff);
    vagRounded.drawString(rootstring, 98, 448 );
    
    ofSetColor(255,122,220);
    vagRounded.drawString(rootstring, 100, 450 );

    ofSetHexColor(0xffffff);
    vagRounded.drawString(statestring, 98, 448 );
    
    ofSetColor(255,122,220);
    vagRounded.drawString(statestring, 100, 450 );
}

//--------------------------------------------------------------
void ofApp::exit(){

	// clean up
	midiIn.closePort();
	midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// add the latest message to the message queue
	midiMessages.push_back(msg);

	// remove any old messages if we have too many
	while(midiMessages.size() > maxMessages) {
		midiMessages.erase(midiMessages.begin());
	}
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == '?'){
			midiIn.listInPorts();
    }else if(key ==  'c'){
            commstring += "Opens midi port 1, receives messages\n";
            commstring += "quantizes according to harmonic fields\n";
            commstring += "sends four notes out via Osc on port 12345\n";
            commstring += "Osc Message contents: midi val, x pos, y pos\n";
    }else if(key == 'g'){
            guistring += "Four params per harmonic field are set via gui:\n";
            guistring += "x position can be used for temporal position\n";
            guistring += "y position can be used for note probability\n";
            guistring += "mode sets the interval segment used to create scale\n";
            guistring += "root sets the root pitch of scale\n";
    }else if(key == 'h'){
            helpstring += "Help Menu: \n(c)omms \n(g)ui \n(m)ode \n";
    }else if(key == 'm'){
            modestring += "0: major\t 9: dom7\n";
            modestring += "1: minor\t 10: maj6\n"; 
            modestring += "2: Hminor\t 11: min6\n";
            modestring += "3: dorian\t 12: six4\n";
            modestring += "4: phrygian\t 13: minsix4\n";
            modestring += "5: lydian\t 14: six5\n";
            modestring += "6: mixolydian\t 15: minsix5\n";
            modestring += "7: locrian\t 16: five4\n";
            modestring += "8: maj7\t 17: four2\n";
	}else if(key == 'r'){
            rootstring += "0: c\t  6: fs\n";
            rootstring += "1: cs\t 7: g\n";
            rootstring += "2: d\t  8: gs\n";
            rootstring += "3: ds\t 9: a\n";
            rootstring += "4: e\t  10: as\n";
            rootstring += "5: f\t  11: b\n";
    }else if(key == 's'){
            statestring += "state can be saved as xml via the gui\n";
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 'c'){
            commstring.clear(); 
    }else if(key == 'g'){
            guistring.clear();
    }else if(key == 'h'){
            helpstring.clear();
    }else if(key == 'm'){
            modestring.clear();
    }else if(key == 'r'){
            rootstring.clear();
    }else if(key == 's'){
            statestring.clear();
    }
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}


