#pragma once

#include <math.h>       /* round, floor, ceil, trunc */
#include "ofThread.h"

/// This is a simple example of a ThreadedObject created by extending ofThread.
/// It contains data (count) that will be accessed from within and outside the
/// thread and demonstrates several of the data protection mechanisms (aka
/// mutexes).

class beatThread: public ofThread
{
public:
    /// Create a ThreadedObject and initialize the member
    /// variable in an initialization list.
    beatThread(){
    }
    
    ofEvent<ofVec2f> tickEvent;
    ofEvent<ofVec2f> tick8Event;
    ofEvent<ofVec2f> tick16Event;
    ofEvent<ofVec2f> tickRhythmEvent;
    ofEvent<ofVec2f> barEvent;
    ofEvent<ofVec2f> bpmChange;
    ofEvent<ofVec2f> rhythmMaskChange;

    
    /// Start the thread.
    void start()
    {
        SYNC = false;
        beatStart = ofGetElapsedTimef();
        
        barStart = beatStart;
        tickStart = barStart;
        tick8Start = tickStart;
        
        tickTime = 60.0f/mBPM;
        tickSent = false;
        // Mutex blocking is set to true by default
        // It is rare that one would want to use startThread(false).
        startThread();
    }
    
    /// Signal the thread to stop.  After calling this method,
    /// isThreadRunning() will return false and the while loop will stop
    /// next time it has the chance to.
    void stop()
    {
        stopThread();
    }
    
    void syncBeat() {
        SYNC = true;
    }
    
    void setBPM(int beats) {
        BPM = beats;
        mBPM = BPM;
        tickTime = 60.0f/mBPM;
    }
    
    void multBPM(float mult) {
        mBPM = int(float(BPM)*mult);
        tickTime = 60.0f/mBPM;
    }
    
    void resetMult() {
        mBPM = BPM;
        tickTime = 60.0f/mBPM;
    }
    
    int getBPM() {
        return BPM;
    }
    
    vector<bool> getRhythmMask() {
        return rhythmMask;
    }
    
    
    void setRhythmMask(vector<bool> newMask) {
        rhythmMask.clear();
        for(int i=0; i < newMask.size(); i++) {
            rhythmMask.push_back(newMask[i]);
        }
    }
    
    /// Our implementation of threadedFunction.
    void threadedFunction()
    {
        for(int i=0; i < 16; i++) {
            rhythmMask.push_back(false);
        }
        while(isThreadRunning())
        {
            // Attempt to lock the mutex.  If blocking is turned on,
            if(lock())
            {
                if(SYNC) {
                    beatStart = ofGetElapsedTimef()-tickTime*(5.0f/8.0f);
                    barStart = beatStart;
                    tickStart = beatStart;
                    tick8Start = tickStart;
                    tick16Start = tickStart;
                    lastRhythmTap = 0;
                    beatState = 0;
                    beat8State = 0;
                    beat16State = 0;
                    SYNC = false;
                }
                
                if(ofGetElapsedTimef()-tick16Start >= tickTime/4.0f) {
                    this->tick();
                }
                
                
                sleep(10);
                unlock();
            }
            else
            {
                // If we reach this else statement, it means that we could not
                // lock our mutex, and so we do not need to call unlock().
                // Calling unlock without locking will lead to problems.
                ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
            }
        }
    }
    
    void tick() {
        
        //if(ofGetElapsedTimef()-lastTick > tickTime/8.0f) {
        ofVec2f tData = ofVec2f(tick16Start, beat16State);
        ofNotifyEvent(tick16Event, tData, this);
        
        if(ofGetElapsedTimef()-tick8Start >= tickTime/2.0f) {
            
            ofVec2f tData = ofVec2f(tick8Start, beat8State);
            ofNotifyEvent(tick8Event, tData, this);
            
            if(ofGetElapsedTimef()-tickStart >= tickTime) {
                tData = ofVec2f(tickStart, beatState);
                ofNotifyEvent(tickEvent, tData, this);
                if(beatState == 0) {
                    ofVec2f bData = ofVec2f(tickStart, beatState);
                    ofNotifyEvent(barEvent, bData, this);
                    if(ofGetElapsedTimef()-lastRhythmTap >= tickTime*4) {
                        recordRhythm = false;
                    }
                }
                beatState = (beatState+1)%4;
                if(beatState == 0) barStart = tickStart;
                tickStart += tickTime;
                //cout << "TICK " << beatState << endl;
            }
            
            beat8State = (beat8State+1)%8;
            tick8Start += tickTime/2.0f;
            
            
        }
        
        if(rhythmMask[int(beat16State)%16]) {
            ofVec2f tData = ofVec2f(tick16Start, beat16State);
            ofNotifyEvent(tickRhythmEvent, tData, this);
        }
        
        beat16State = (beat16State+1)%16;
        tick16Start += tickTime/4.0f;
        
        //}
        
    }
    
    void rhythmTap() {
        float t = ofGetElapsedTimef()-tickTime*(3.0f/4.0f);
        if(recordRhythm) {
            if(t >= recordStart) {
                if(firstRecord) {
                    rhythmMask.clear();
                    for(int i=0; i < 16; i++) {
                        rhythmMask.push_back(false);
                    }
                    firstRecord = false;
                }
                if(t >= tickTime*4.0f) {
                    recordStart = barStart;
                }
                float deltaTime = t-recordStart;
                int R_beatState = int(round(16.0f*deltaTime/(tickTime*4.0f))+8)%16;
                rhythmMask[R_beatState] = true;
                //cout << R_beatState << endl;
            }
            ofVec2f tData = ofVec2f(t, beatState);
            ofNotifyEvent(rhythmMaskChange, tData, this);
        } else {
            recordRhythm = true;
            firstRecord = true;
            recordStart = barStart+4*tickTime;
            lastRhythmTap = t;
        }
    }
    
    void tap() {
        float t = ofGetElapsedTimef();
        if(t-lastTap >= tickTime*4) {
            tapTimes.clear();
        }
        tapTimes.push_back(t);
        lastTap = t;
        
        float timeDiff = 60.0f/BPM;
        int count = 1;
        for(int i=1; i < tapTimes.size(); i++) {
            if(i >= 0) {
                timeDiff += tapTimes[i]-tapTimes[i-1];
                count++;
            } else {
                break;
            }
        }
        timeDiff /= float(count);
        
        BPM = int(60.0f/timeDiff);
        tickTime = 60.0f/BPM;
        ofVec2f eData = ofVec2f(BPM,1.0);
        ofNotifyEvent(bpmChange, eData, this);
        
        
    }
    
    void clearTaps() {
        tapTimes.clear();
    }
    
protected:
    
    int BPM=120;
    int mBPM=120;
    int beatState;
    int beat8State;
    int beat16State;
    
    
    float bTime;
    float tickTime;
    float beatStart;
    float barStart;
    float tickStart;
    float tick8Start;
    float tick16Start;
    bool tickSent;
    float lastTick;
    float last16Tick;
    float lastTap;
    vector<float> tapTimes;
    
    vector<bool> rhythmMask;
    float lastRhythmTap;
    bool recordRhythm = false;
    float recordStart;
    bool firstRecord = false;
    
    bool SYNC;
    
};
