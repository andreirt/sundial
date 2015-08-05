#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"



class ofApp : public ofBaseApp{
    ofVideoGrabber* grabber;
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
    float latitude, longitude;
    
    std::map<string, string> ptStrings;
    std::map<string, string> enStrings;
    std::map<string, string> currentStrings;

    int cameraWidth;
    int cameraHeight;

    int intervalToSaveImage;

    //----------------------- Interface

    ofxUICanvas *gui;
    ofxUICanvas *cameraPanel;
    ofxUICanvas *imagePanel;

    int selectedCameraIndex;
    bool hideButtonReleased;
    
    const static float MAX_STRENGTH_AROUND_PIXEL;
    const static string SUPPORT_BUTTON_NAME;
    const static string CHANGE_LOCALE_BUTTON_NAME;
    
    const static string ENGLISH_LABEL;
    const static string PORTUGUESE_LABEL;
    
    int currentLocale;
    const static int LOCALE_ENGLISH = 0;
    const static int LOCALE_PORTUGUESE = 1;
    
    ofxUILabel* titleLabel;
    ofxUILabelButton* changeLocaleButton;

    const static long MILLISECONDS_PER_HOUR;


    ofxUIDropDownList* cameraList;
    ofxUITextInput* cameraWidthTextInput;
    ofxUITextInput* cameraHeightTextInput;
    ofxUITextInput* intervalToSaveTextInput;
    
    ofxUILabel* latitudeLabel;
    ofxUILabel* longitudeLabel;
    
    ofxUILabel* minutesLabel;
    
    ofxUILabel* cameraWidthLabel;
    ofxUILabel* cameraHeightLabel;
    
    ofxUILabel* imageRotationLabel;
    
    ofxUIDropDownList* pickCameraLabel;

    std::vector<ofxUITextInput*> textInputs;

    ofxUIToggle* zeroRotationToggle;
    ofxUIToggle* ninetyRotationToggle;
    ofxUIToggle* oneHundredEightyRotationToggle;
    ofxUIToggle* twoHundredSeventyRotationToggle;
    int rotations;
    
    ofxUILabel* credits1Label;
    ofxUILabel* credits2Label;
    ofxUILabel* credits3Label;
    ofxUILabel* credits4Label;
    ofxUILabel* credits5Label;
    
    ofxUILabelButton* clearButton;
    
    ofxUILabelButton* saveButton;
    ofxUILabelButton* cancelButton;
    ofxUILabel* supportLabel;

    ofxUIToggle* showAtStartupToggle;
    bool showAtStartup;

    ofxUIToggle* fullScreenToggle;
    bool fullScreen;

    ofxUIToggle* saveImageToggle;
    bool saveImage;

    ofxUITextInput* latitudeTextInput;
    ofxUITextInput* longitudeTextInput;

    //----------------------- Interface

    public:
        void exit();
        void guiEvent(ofxUIEventArgs &e);
        void cameraPanelEvent(ofxUIEventArgs &e);
        void imagePanelEvent(ofxUIEventArgs &e);
        void reset();
        void cancelConfigurationChanges();
        void applyConfigurationChanges();
        void hideConfigurationPanel();
        void showConfigurationPanel();
        void unfocusAllTextInputs(ofxUITextInput* except);
        void checkTextInputFocus(ofxUIEventArgs &e);

		void setup();
		void update();
		void draw();
        void saveCurrentImage();
        float getShadowWidth();
        float getShadowPositionAndOffset();
        float getShadowAlpha(int i, float shadowWidth);
        ofColor getBackgroundColor();
        void getSunTime();
        void clearImage();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
};
