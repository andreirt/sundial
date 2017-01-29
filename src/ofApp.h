#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxImGui.h"
#include <map>

class ofApp : public ofBaseApp{
private:
    ofXml settings;
    ofxImGui::Gui*gui;
    GLuint imageButtonID;
    static float latitude;
    static float longitude;
    static int intervalToSaveImage;
    static int degreesButtonValue;
    static bool saveImageToggle;
    static bool showAtStartup;
    static bool fullscreen;
    static bool configurationPanelShow;
    static int currentResolution;
    const char* deviceResolution[5];
    const static long MILLISECONDS_PER_HOUR;
    const static float MAX_STRENGTH_AROUND_PIXEL;
    
    ofVideoGrabber* grabber;
    int selectedCameraIndex;
    ofFbo imageFbo;
    ofFbo finalFbo;
    ofFbo shadowFbo;
    ofImage screenImage;
    long lastDrawnPixel;
    long cameraPixelsLength;
    float scale;
    float lastSavedImageTime;
    ofColor  color;
    long currentTime, sunrise, sunset; //in milliseconds
    
    int cameraWidth;
    int cameraHeight;
    int rotations;
    float lastTimeImageWasSaved;
    
    std::map<string, string> ptStrings;
    std::map<string, string> enStrings;
    std::map<string, string> currentStrings;
    const static string CHANGE_LOCALE_BUTTON_NAME;
    const static string ENGLISH_LABEL;
    const static string PORTUGUESE_LABEL;
    const static int LOCALE_ENGLISH = 0;
    const static int LOCALE_PORTUGUESE = 1;
    int currentLocale;
    string changeLocaleLabel;
    
public:
    void setup();
    void reset();
    void update();
    void draw();
    void keyPressed(int key);
    void windowResized(int w, int h);
    
    void saveCurrentImage();
    float getShadowWidth();
    float getShadowPositionAndOffset();
    float getShadowAlpha(int i, float shadowWidth);
    ofColor getBackgroundColor();
    void getSunTime();
    void clearImage();
 
    void cancelConfigurationChanges();
    void applyConfigurationChanges();
    void hideConfigurationPanel();
    void showConfigurationPanel();
    void changeLocale();
    void saveXmlSettings();
    void loadXmlSettings();
    void setFullscreen();
    void selectResolution();
    
    void mouseReleased(ofMouseEventArgs&);
};
