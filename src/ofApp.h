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

    int cameraWidth;
    int cameraHeight;

    int intervalToSaveImage;

    //----------------------- Interface

    ofxUICanvas *gui;
    ofxUICanvas *cameraPanel;
    ofxUICanvas *imagePanel;

    int selectedCameraIndex;
    bool hideButtonReleased;

    const static string CAMERA_WIDTH_LABEL;
    const static string CAMERA_HEIGHT_LABEL;
    const static string LATITUDE_LABEL;
    const static string LONGITUDE_LABEL;
    const static string SAVE_IMAGE_LABEL;
    const static string SAVE_LABEL;
    const static string CANCEL_LABEL;
    const static string RESET_IMAGE_LABEL;
    const static string SUPPORT_BUTTON_NAME;

    const static string ZERO_DEGREES_LABEL;
    const static string NINETY_DEGREES_LABEL;
    const static string ONE_HUNDRED_EIGHTY_DEGREES_LABEL;
    const static string TWO_HUNDRED_SEVENTY_DEGREES_LABEL;

    const static int MILLISECONDS_PER_HOUR;


    ofxUIDropDownList* cameraList;
    ofxUITextInput* cameraWidthTextInput;
    ofxUITextInput* cameraHeightTextInput;
    ofxUITextInput* intervalToSaveTextInput;

    std::vector<ofxUITextInput*> textInputs;

    ofxUIToggle* zeroRotationToggle;
    ofxUIToggle* ninetyRotationToggle;
    ofxUIToggle* oneHundredEightyRotationToggle;
    ofxUIToggle* twoHundredSeventyRotationToggle;
    int rotations;

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
