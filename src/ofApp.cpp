#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(60);
    effect = 0;
    
    // Connect to the fcserver
    opcClient.setup("127.0.0.1", 7890);
    
    // You define the stage size and draw into the stage
    opcClient.setupStage(500, 500);
    
    defaultEffects.setup(opcClient.getStageCenterX(), opcClient.getStageCenterY(), opcClient.getStageWidth(), opcClient.getStageHeight());
    
    for (int x = 0; x < 8; x++) {
        strips.push_back(ofxNeoPixelStrip(((opcClient.getStageWidth()/8)/2)+(x*(opcClient.getStageWidth()/32)), opcClient.getStageCenterY()-(30*5), 33, 10));
    }

    // Setup Syphon
    dir.setup();
    ofAddListener(dir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    ofAddListener(dir.events.serverRetired, this, &ofApp::serverRetired);
    
    directoryId = -1;
    
    syphonClient.setup();
    
    
}
//--------------------------------------------------------------
void ofApp::update()
{
    ofSetWindowTitle("FADECANDY W/ SYPHON");
    
    
    opcClient.update();
    
    // Now Draw the effects to the stage
    opcClient.beginStage();
    
    if (syphonActive) {
        if (dir.isValidIndex(directoryId))
            ofEnableAlphaBlending();
            ofSetColor(ofColor::white);
            syphonClient.draw(0, 0);
    }
    else {
        defaultEffects.draw(effect);
    }
    
    opcClient.endStage();
    
    // New Get Method
    for (int i = 0; i < strips.size(); i++) {
        opcClient.getStagePixels(strips[i].getPixelCoordinates(), strips[i].colors);
    }
    
    // If the client is not connected do not try and send information
    if (!opcClient.isConnected()) {
        // Will continue to try and reconnect to the Pixel Server
        opcClient.tryConnecting();
    }
    else {
        // Write out the data
        for (int i = 0; i < strips.size(); i++) {
            opcClient.writeChannel(i, strips[i].colorData());
        }
    }
}
//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0);
    opcClient.drawStage(hide);
    
    // Show the grabber area and Draw the output
    for (int i = 0; i < strips.size(); i++) {
        strips[i].drawGrabRegion(hide);
        strips[i].draw((opcClient.getStageWidth()+25)+(i*25), 10);
    }

    // Report Messages
    stringstream ss;
    ss << "Press Left and Right to Change Effect Mode" << endl;
    ss << "FPS: " << ofToString((int)(ofGetFrameRate())) << endl;
    ofDrawBitmapStringHighlight(ss.str(), 5,ofGetHeight()-30);
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == OF_KEY_LEFT) {
        effect--;
    }
    if (key == OF_KEY_RIGHT) {
        effect++;
    }
    if (key == ' ') {
        hide = !hide;
    }
    if (key == 's') {
        syphonActive = !syphonActive;
    }
    if (key == 'c') {
        //press any key to move through all available Syphon servers
        if (dir.size() > 0)
        {
            directoryId++;
            if(directoryId > dir.size() - 1)
                directoryId = 0;
            
            syphonClient.set(dir.getDescription(directoryId));
            string serverName = syphonClient.getServerName();
            string appName = syphonClient.getApplicationName();
            
            if(serverName == ""){
                serverName = "null";
            }
            if(appName == ""){
                appName = "null";
            }
        }
        else
        {

        }

    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    
}
//--------------------------------------------------------------
void ofApp::exit()
{
    // Close Connection
    opcClient.close();
}
//--------------------------------------------------------------
void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    directoryId = 0;
}
//--------------------------------------------------------------
void ofApp::serverUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Updated")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    directoryId = 0;
}
//--------------------------------------------------------------
void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    directoryId = 0;
}
