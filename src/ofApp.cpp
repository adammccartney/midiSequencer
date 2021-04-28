#include "ofApp.h"
#include "HarmonicField.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
    ofBackground(238, 255, 153);
    ofSetFrameRate(60);
	ofSetLogLevel(OF_LOG_VERBOSE);

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
			if(message.status == MIDI_NOTE_ON ||
			   message.status == MIDI_NOTE_OFF) {
				text << "\tpitch: " << message.pitch;
                // this is where we need to make a call to
                // QuantizedPitchManager::processMidiNote(const int &note)
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
    switch(key) {
		case '?':
			midiIn.listInPorts();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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


