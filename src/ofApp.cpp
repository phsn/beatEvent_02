#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    receiver.setup(PORT);
    sender.setup(HOST, PORT);
    
    ofAddListener(beatSyncThread.tickEvent, this, &ofApp::onTick);
    ofAddListener(beatSyncThread.tick8Event, this, &ofApp::onTick8);
    ofAddListener(beatSyncThread.tick16Event, this, &ofApp::onTick16);
    ofAddListener(beatSyncThread.barEvent, this, &ofApp::onBar);
    ofAddListener(beatSyncThread.bpmChange, this, &ofApp::onBPMChange);
    
    beatSyncThread.start();
    
    BPM = 126;
    beatStart = ofGetElapsedTimef();
    tickTime = 60.0f/BPM;
    tickSent = false;
    syncDelay = tickTime;
    
    ofSetCircleResolution(64);
    circleColor = ofColor(ofRandom(255.0f),ofRandom(255.0f),ofRandom(255.0f),255.0);
    circleSize = ofGetHeight()/2-20;
    circleOffset = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    bTime = ofGetElapsedTimef() - beatStart;
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        //cout << m.getAddress() << "\t\t" << ofToString(m.getArgAsFloat(0)) << endl;
        
        if(ofGetElapsedTimef()-lastSync >= syncDelay) {
            if(m.getAddress() == "/Sync/x" && m.getArgAsFloat(0) == 1) {
                beatSyncThread.syncBeat();
                lastSync = ofGetElapsedTimef();
            }
        }
        if(m.getAddress() == "/Tap/x" && m.getArgAsFloat(0) == 1) {
            beatSyncThread.tap();
            //cout << "TAP || BPM = " << beatSyncThread.getBPM() << endl;
        }
        if(m.getAddress() == "/BPM_Slider/x") {
            beatSyncThread.setBPM(int(m.getArgAsFloat(0)*300.0f));
            //cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
        }
        if(m.getAddress() == "/BPM_Mult/x") {
            
            if(   int(m.getArgAsFloat(0)) == 0
               && int(m.getArgAsFloat(1)) == 0
               && int(m.getArgAsFloat(2)) == 0
               && int(m.getArgAsFloat(3)) == 0) {
                
                beatSyncThread.resetMult();
                
            } else {
                if(int(m.getArgAsFloat(0)) == 1) {
                    beatSyncThread.multBPM(0.25);
                } else if(int(m.getArgAsFloat(1)) == 1) {
                    beatSyncThread.multBPM(0.5);
                } else if(int(m.getArgAsFloat(2)) == 1) {
                    beatSyncThread.multBPM(2);
                } else if(int(m.getArgAsFloat(3)) == 1) {
                    beatSyncThread.multBPM(4);
                }
            };
            
            //cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
            
        }
        if(m.getAddress() == "/BPM_MultBar/x") {
            
            float multbarVal = (1.0f-m.getArgAsFloat(0))*6.0f+1.0f;
            //cout << "MULTBAR " << multbarVal << endl;
            if(int(multbarVal) == 1) {
                
                beatSyncThread.resetMult();
                
            } else {
                
                beatSyncThread.multBPM(multbarVal);
                
            };
            
            //cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
            
        }
        if(m.getAddress() == "/BPM_MultBar_Zero/x") {
            
            float multbarVal = (1.0f-m.getArgAsFloat(0));
            //cout << "MULTBAR " << multbarVal << endl;
            if(int(multbarVal) == 1) {
                
                beatSyncThread.resetMult();
                
            } else {
                
                beatSyncThread.multBPM(multbarVal);
                
            };
            
            cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
            
        }
        if(m.getAddress() == "/clearTaps/x" && m.getArgAsFloat(0) == 1) {
            beatSyncThread.clearTaps();
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    circleOffset += (t_circleOffset-circleOffset)*0.5;
    circleSize += (t_circleSize-circleSize)*0.4;
    
    circleColor = ofColor(circleColor.r + (t_circleColor.r - circleColor.r)*0.4,
                          circleColor.g + (t_circleColor.g - circleColor.g)*0.4,
                          circleColor.b + (t_circleColor.b - circleColor.b)*0.4,
                          255.0);
    
    ofSetColor(circleColor);
    ofCircle(ofGetWidth()/2+circleOffset, ofGetHeight()/2,circleSize);
}

//--------------------------------------------------------------
void ofApp::onTick(ofVec2f &tObj) {
    //circleColor = ofColor(ofRandom(255.0f),ofRandom(255.0f),ofRandom(255.0f),255.0);
    t_circleOffset = ofRandomf()*100.0f;
    
    ofxOscMessage m;
    m.setAddress("/BeatState/x");
    m.addFloatArg((tObj.y)/3.0f);
    sender.sendMessage(m);
    
    //cout << "TICK " << (tObj.y)+1 << endl;
    
}

//--------------------------------------------------------------
void ofApp::onTick8(ofVec2f &tObj) {
    //t_circleOffset = sin(ofGetElapsedTimef()*PI*BPM*4)*300;
    if(int(tObj.y)%2 == 0) {
        t_circleSize = ofRandom((ofGetHeight()/2-20));
    } else {
        t_circleSize = ofGetHeight()/2-20;
    }
    //t_circleSize = ofGetHeight()/2-20;
}


//--------------------------------------------------------------
void ofApp::onTick16(ofVec2f & tObj){
    //centerColor = ofColor(150,0,0);
    
    //cout << "TICK " << tObj.y << endl;
}


//--------------------------------------------------------------
void ofApp::onBar(ofVec2f &bObj) {
    //circleColor = ofColor(ofRandom(255.0f),ofRandom(255.0f),ofRandom(255.0f),255.0);
    t_circleColor = ofColor(ofRandom(255.0f),ofRandom(255.0f),ofRandom(255.0f),255.0);
    
}


//--------------------------------------------------------------
void ofApp::onBPMChange(ofVec2f &tObj) {
    //circleColor = ofColor(ofRandom(255.0f),ofRandom(255.0f),ofRandom(255.0f),255.0);
    
    
    beatSyncThread.setBPM(tObj.x);
    
    ofxOscMessage m;
    m.setAddress("/BPM_Slider/x");
    m.addFloatArg(tObj.x/300.0f);
    sender.sendMessage(m);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'c') {
        beatSyncThread.stop();
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
    //beatSyncThread.setBPM(x/4);
    //cout << x/4 << endl;
    //beatSyncThread.setBPM(BPM);
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
