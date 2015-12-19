#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "beatThread.h"

#define PORT 8000
#define HOST "192.168.178.73"
//#define HOST "10.44.63.158"

#define BEAT_BUFFER 131072

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void handleOSC(ofxOscMessage m);
    
    void onTickRhythm(ofVec2f & tObj);
    void onTick16(ofVec2f & tObj);
    void onTick8(ofVec2f & tObj);
    void onTick(ofVec2f & tObj);
    void onBar(ofVec2f & bObj);
    void onBPMChange(ofVec2f & tObj);
    void onRhythmChange(ofVec2f & tObj);

    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
    ofxOscReceiver receiver;
    ofxOscSender sender;
    
    beatThread beatSyncThread;
    float lastSync;
    float syncDelay;
    
    int BPM;
    float bTime;
    float tickTime;
    float beatStart;
    bool tickSent;
    
    ofColor circleColor;
    ofColor t_circleColor;
    float circleSize;
    float t_circleSize;
    float circleOffset;
    float t_circleOffset;
    
};
