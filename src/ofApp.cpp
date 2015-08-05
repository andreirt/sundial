#include "ofApp.h"
#include <sys/time.h>
#include <math.h>

const string ofApp::ENGLISH_LABEL = "English";
const string ofApp::PORTUGUESE_LABEL = "Portugues";

const float ofApp::MAX_STRENGTH_AROUND_PIXEL = .15;
const string ofApp::SUPPORT_BUTTON_NAME = "support";
const string ofApp::CHANGE_LOCALE_BUTTON_NAME = "changeLocale";

const long ofApp::MILLISECONDS_PER_HOUR = 60*60*1000;



//--------------------------------------------------------------
void ofApp::setup(){
    this->lastSavedImageTime = 0;
    this->lastDrawnPixel = -1;
    this->selectedCameraIndex = 0;
    this->hideButtonReleased = false;
    
    // reads strings.xml
    ofFile stringsFile;
    stringsFile.open("strings.xml");
    ofBuffer stringsBuffer = stringsFile.readToBuffer();
    
    ofXml strings;
    strings.loadFromBuffer( stringsBuffer );
    strings.setTo("data");
    
    int numberOfStrings = strings.getNumChildren();
    for (int i = 0; i < numberOfStrings; i++) {
        strings.setToChild(i);
        
        string tagName = strings.getName();
        
        strings.setTo("pt");
        this->ptStrings[tagName ] = strings.getValue();
        
        strings.setTo("../en");
        this->enStrings[tagName ] = strings.getValue();
        strings.setToParent();
        
        strings.setToParent();
    }
    
    this->currentStrings = this->ptStrings;

    this->grabber = new ofVideoGrabber();

    this->gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());
    this->gui->setWidgetSpacing(10);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);
    gui->setFontSize(OFX_UI_FONT_LARGE, 18);

    ofxUIColor backgroundColor = ofxUIColor::white;
    ofxUIColor fillColor = ofxUIColor::black;
    ofxUIColor fillHightlightColor = ofxUIColor::black;
    ofxUIColor outline = ofxUIColor::black;
    ofxUIColor outlineHighlight = ofxUIColor::red;
    ofxUIColor paddedColor = ofxUIColor::blue;
    ofxUIColor paddedOutlineColor = ofxUIColor::orange;

    this->gui->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );

    this->titleLabel = this->gui->addLabel("title", this->currentStrings["sundial"], OFX_UI_FONT_LARGE);
    this->changeLocaleButton = new ofxUILabelButton(ofApp::CHANGE_LOCALE_BUTTON_NAME, true, 200, 30, 500, 10);
    this->gui->addWidgetRight(this->changeLocaleButton);
    this->changeLocaleButton->getLabelWidget()->setLabel(ENGLISH_LABEL);
    this->gui->addSpacer();

    this->cameraPanel = new ofxUICanvas(0, 0, 300, 260);
    this->cameraPanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->cameraPanel->setWidgetSpacing(10);
    this->gui->addWidgetDown(this->cameraPanel);

    // lista as câmeras conectadas a este computador
    vector< ofVideoDevice > devices = this->grabber->listDevices();

    vector<string> *cameras = new vector<string>();
    vector<ofVideoDevice>::iterator it;

    for (it = devices.begin(); it != devices.end(); ++it) {
        ofVideoDevice device = *it;
        cameras->push_back(device.deviceName);
    }

    this->pickCameraLabel = this->cameraList = this->cameraPanel->addDropDownList(this->currentStrings["pickCamera"], *cameras, 300, 10);
    this->cameraList->setAllowMultiple(false);
    this->cameraList->setDrawOutline(true);
    this->cameraList->setAutoClose(true);

    this->cameraWidthLabel = new ofxUILabel(180, this->currentStrings["cameraWidth"], OFX_UI_FONT_SMALL);
    this->cameraPanel->addWidgetDown( cameraWidthLabel );
    this->cameraWidthTextInput = new ofxUITextInput("CameraWidth", "1920", 80, 18) ;
    this->cameraWidthTextInput->setOnlyNumericInput(true);
    this->cameraWidthTextInput->setDrawOutline(true);
    this->cameraWidthTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( cameraWidthTextInput );
    this->textInputs.push_back(this->cameraWidthTextInput);

    this->cameraHeightLabel = new ofxUILabel(170, this->currentStrings["cameraHeight"], OFX_UI_FONT_SMALL);
    this->cameraPanel->addWidgetDown( cameraHeightLabel );
    this->cameraHeightTextInput = new ofxUITextInput("CameraHeight", "1080", 80, 18);
    this->cameraHeightTextInput->setOnlyNumericInput(true);
    this->cameraHeightTextInput->setDrawOutline(true);
    this->cameraHeightTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( this->cameraHeightTextInput );
    this->textInputs.push_back(this->cameraHeightTextInput);

    this->rotations = 0;

    this->imageRotationLabel = this->cameraPanel->addLabel(this->currentStrings["imageRotation"], OFX_UI_FONT_SMALL);

    this->zeroRotationToggle = new ofxUIToggle(this->currentStrings["zeroDegress"], true, 16, 16);
    this->zeroRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->zeroRotationToggle);

    this->ninetyRotationToggle = new ofxUIToggle(this->currentStrings["ninetyDegress"], true, 16, 16);
    this->ninetyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->ninetyRotationToggle);

    this->oneHundredEightyRotationToggle = new ofxUIToggle(this->currentStrings["oneHundredEightyDegress"], true, 16, 16);
    this->oneHundredEightyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->oneHundredEightyRotationToggle);

    this->twoHundredSeventyRotationToggle = new ofxUIToggle(this->currentStrings["twoHundredSeventyDegress"], true, 16, 16);
    this->twoHundredSeventyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->twoHundredSeventyRotationToggle);

    this->imagePanel = new ofxUICanvas(320, 0, 430, 260);
    this->imagePanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->imagePanel->setWidgetSpacing(10);
    this->imagePanel->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );

    this->latitudeLabel = new ofxUILabel(170, this->currentStrings["latitude"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( latitudeLabel );
    this->latitudeTextInput = new ofxUITextInput("Latitude", "-23.55149", 80, 18) ;
    this->latitudeTextInput->setOnlyNumericInput(true);
    this->latitudeTextInput->setDrawOutline(true);
    this->latitudeTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( latitudeTextInput );
    this->textInputs.push_back(this->latitudeTextInput);

    this->longitudeLabel = new ofxUILabel(170, this->currentStrings["longitude"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( longitudeLabel );
    this->longitudeTextInput = new ofxUITextInput("Longitude", "46.63051", 80, 18) ;
    this->longitudeTextInput->setOnlyNumericInput(true);
    this->longitudeTextInput->setDrawOutline(true);
    this->longitudeTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( longitudeTextInput );
    this->textInputs.push_back(this->longitudeTextInput);

    this->saveImageToggle = new ofxUIToggle(this->currentStrings["saveImage"], true, 16, 16);

    this->saveImageToggle->setDrawOutline(true);
    this->imagePanel->addWidgetDown(this->saveImageToggle);

    this->intervalToSaveImage = 15;

    this->intervalToSaveTextInput = new ofxUITextInput("A cada", "15", 60, 18);
    this->intervalToSaveTextInput->setOnlyNumericInput(true);
    this->intervalToSaveTextInput->setDrawOutline(true);
    this->intervalToSaveTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( this->intervalToSaveTextInput );
    this->textInputs.push_back(this->intervalToSaveTextInput);

    this->minutesLabel = minutesLabel = new ofxUILabel(90, this->currentStrings["minutes"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetRight( minutesLabel );

    this->clearButton = this->gui->addLabelButton(this->currentStrings["resetImage"], false, 150, 20);
    clearButton->setDrawFill(true);
    clearButton->setDrawOutline(true);
    this->imagePanel->addWidgetDown( clearButton );
    this->imagePanel->addSpacer();

    this->showAtStartupToggle = this->imagePanel->addToggle(this->currentStrings["showAtStartup"], true, 16, 16);
    this->showAtStartupToggle->setDrawOutline(true);
    this->imagePanel->addSpacer();

    this->fullScreenToggle = this->imagePanel->addToggle(this->currentStrings["fullScreen"], true, 16, 16);
    this->fullScreenToggle->setDrawOutline(true);

    this->gui->addWidgetRight(this->imagePanel);

    this->gui->addSpacer();

    this->saveButton = this->gui->addLabelButton(this->currentStrings["save"], false, 100, 20);
    saveButton->setDrawFill(true);
    saveButton->setDrawOutline(true);

    this->cancelButton = new ofxUILabelButton(this->currentStrings["cancel"], false, 100, 20);
    cancelButton->setDrawFill(true);
    cancelButton->setDrawOutline(true);
    this->gui->addWidgetRight(cancelButton);
    this->gui->addSpacer();

    this->credits1Label = this->gui->addLabel(this->currentStrings["credits1"], OFX_UI_FONT_SMALL);
    this->credits2Label = this->gui->addLabel(this->currentStrings["credits2"], OFX_UI_FONT_SMALL);
    this->credits3Label = this->gui->addLabel(this->currentStrings["credits3"], OFX_UI_FONT_SMALL);
    this->credits4Label = this->gui->addLabel(this->currentStrings["credits4"], OFX_UI_FONT_SMALL);
    this->credits5Label = this->gui->addLabel(this->currentStrings["credits5"], OFX_UI_FONT_SMALL);
    this->gui->addSpacer();

    this->supportLabel = this->gui->addLabel(this->currentStrings["support"]);
    this->gui->addImageButton(ofApp::SUPPORT_BUTTON_NAME, "funarte.png", false, 509, 60);

    ofAddListener(this->gui->newGUIEvent, this, &ofApp::guiEvent);
    ofAddListener(this->cameraPanel->newGUIEvent, this, &ofApp::cameraPanelEvent);
    ofAddListener(this->imagePanel->newGUIEvent, this, &ofApp::imagePanelEvent);

    this->cameraPanel->loadSettings("camera.xml");
    this->imagePanel->loadSettings("image.xml");

    this->applyConfigurationChanges();

    if (this->showAtStartup) {
        this->showConfigurationPanel();
    }
    else {
        this->hideConfigurationPanel();
    }

    this->getSunTime();
    this->reset();

    char backgroundImageName[27];
    sprintf( backgroundImageName, "relogiodesol_%d_%d_%d.png", ofGetYear(), ofGetMonth(), ofGetDay() );

    ofImage initialBackground;
    if (initialBackground.loadImage(backgroundImageName)) {
        this->imageFbo.begin();
            initialBackground.draw( 0, 0 );
        this->imageFbo.end();
    }
}

void ofApp::reset() {

    if (this->grabber != NULL) {
        if (this->grabber->isInitialized()) {
            this->grabber->close();
            delete this->grabber;
            this->grabber = new ofVideoGrabber();
        }
    }

    // set camera
    vector<int> selectedCamera = this->cameraList->getSelectedIndeces();
    if (selectedCamera.size() > 0) {
        this->grabber->setDeviceID( selectedCamera[0] - 1 );
    }
    else {
        this->grabber->setDeviceID( 0 );
    }

    this->grabber->setDesiredFrameRate(30);
    this->grabber->initGrabber(this->cameraWidthTextInput->getIntValue(), this->cameraHeightTextInput->getIntValue());

    this->cameraPixelsLength = this->cameraWidthTextInput->getIntValue() * this->cameraHeightTextInput->getIntValue();
    ofLog() << "this->cameraPixelsLength: " << this->cameraPixelsLength;

    this->imageFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->shadowFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->finalFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->screenImage.allocate( this->cameraWidth, this->cameraHeight, OF_IMAGE_COLOR_ALPHA );

    float scaleV = (float) ofGetHeight() / (float) this->cameraHeight;
    float scaleH = (float) ofGetWidth() / (float) this->cameraWidth;
    this->scale = min(scaleV, scaleH);

    this->latitude = this->latitudeTextInput->getIntValue();
    this->longitude = this->longitudeTextInput->getIntValue();

    this->lastDrawnPixel = 0;

}


//--------------------------------------------------------------
void ofApp::update(){
    this->grabber->update();
    if (!grabber->isFrameNew())
        return;

    ofPixels shadowPixels;
    ofPixels cameraPixels;
    ofxCvColorImage colorShadow;
    ofxCvGrayscaleImage grayShadow;
    ofTexture grayShadowTexture;

    ofTexture videoTexture = this->grabber->getTextureReference();
    videoTexture.readToPixels(cameraPixels);

    float shadowWidth = getShadowWidth();
    float positionAndOffset = getShadowPositionAndOffset();

    cameraPixels.rotate90( this->rotations );

    colorShadow.setFromPixels(cameraPixels);

    grayShadow = colorShadow;
    shadowPixels = grayShadow.getPixelsRef();

    grayShadowTexture.allocate( colorShadow.getPixelsRef() );
    grayShadowTexture.setRGToRGBASwizzles(true);
    grayShadowTexture.loadData( shadowPixels );

    this->shadowFbo.begin();
        ofSetColor( 255, 255, 255, 255 );
        ofClear( 255, 255, 255 );
        for (int i=0; i<shadowWidth; i++) {
            ofSetColor( 255, 255, 255, getShadowAlpha(i, shadowWidth) );
            grayShadowTexture.drawSubsection( (positionAndOffset+i), 0, 1, this->cameraHeight, (positionAndOffset+i), 0 );
        }

    this->shadowFbo.end();

    struct timeval inicio;
    gettimeofday(&inicio, NULL);
    this->currentTime = ((ofGetHours())*3600 + ofGetMinutes()*60 + ofGetSeconds()) * 1000 + (long) inicio.tv_usec/1000;

    int currentPixel = ofMap(currentTime, 0, (24*MILLISECONDS_PER_HOUR)-1, 0, (cameraPixelsLength-1), true);
    if (currentPixel < this->lastDrawnPixel) {
        if (ofGetHours() > 0) {
            return;
        }
    }

    if (this->lastDrawnPixel == -1 || currentPixel < this->lastDrawnPixel) {
        this->clearImage();
        this->getSunTime();
        this->lastDrawnPixel = -1;
    }

    this->imageFbo.begin();
        while (this->lastDrawnPixel < currentPixel) {

            int column = this->lastDrawnPixel / this->cameraHeight;
            int line = this->lastDrawnPixel % this->cameraHeight;

            color = cameraPixels.getColor( column, line );
            ofSetColor( color );
            ofRect( column, line, 1, 1);

            ++this->lastDrawnPixel;
        }
    this->imageFbo.end();

    this->finalFbo.begin();
        ofClear( this->getBackgroundColor() );
        ofSetColor( 255, 255, 255 );
        this->imageFbo.draw( 0, 0 );
        ofSetColor( 255, 255, 255);
        this->shadowFbo.draw( 0, 0 );
    this->finalFbo.end();

    if (ofGetElapsedTimef() >= this->lastSavedImageTime+(this->intervalToSaveImage*60)) {
        this->saveCurrentImage();
    }
}

//--------------------------------------------------------------
void ofApp::getSunTime() {

    time_t now = time(0);
    tm *ltm = localtime(&now);
    int dayInYear = (ltm->tm_yday) + 1;

    float earthInclination = 23.45 * sin( ((360.0/365.0) * (float) (284 + dayInYear)) * PI/180 );
    float Td = (2.0/15.0) * (acos( -tan(this->latitude * PI/180) * tan( earthInclination * PI/180 ) ) / (PI/180));

    float correction = (this->longitude - 45.0) / 15.0;

    sunrise = (12 - (Td/2.0)) * MILLISECONDS_PER_HOUR;
    sunset = (12 + (Td/2.0)) * MILLISECONDS_PER_HOUR;

}

//--------------------------------------------------------------
float ofApp::getShadowWidth() {
    float aux;

    if (currentTime <= sunrise or currentTime >= sunset)
        return 0;
    else {
        aux = ofMap(currentTime, sunrise, sunset, 0, this->cameraWidth-1);

        if (aux < (this->cameraWidth/2))
            return ( (float) this->cameraWidth/2.0) - aux;
        else
            return aux - ( (float) this->cameraWidth/2.0);
    }
}

//--------------------------------------------------------------
float ofApp::getShadowPositionAndOffset() {
    if (currentTime >= 12*MILLISECONDS_PER_HOUR)
        return this->cameraWidth/2;
    else
        return ofMap(currentTime, sunrise, (12*MILLISECONDS_PER_HOUR), 0, this->cameraWidth/2);
}

//--------------------------------------------------------------
float ofApp::getShadowAlpha(int index, float shadowWidth) {
    if (currentTime > 12*MILLISECONDS_PER_HOUR)
        return ofMap(index, 0, shadowWidth, 255, 0);
    else
        return ofMap(index, 0, shadowWidth, 0, 255);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor( 255, 255, 255, 255 );
    ofBackground( 0, 0, 0 );
    this->finalFbo.setAnchorPercent( 0.5, 0.5 );

    ofPushMatrix();
    ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
    ofScale( this->scale, this->scale );
    this->finalFbo.draw( 0, 0 );
    ofPopMatrix();

}

//--------------------------------------------------------------
void ofApp::clearImage() {
    this->imageFbo.begin();
        ofClear( 255, 255, 255 );
    this->imageFbo.end();

}

//--------------------------------------------------------------
ofColor ofApp::getBackgroundColor() {
    float RGBColor;
    if (currentTime > 12*MILLISECONDS_PER_HOUR)
        RGBColor = ofMap(ofGetHours(), 12, 24, 255, 0);
    else
        RGBColor = ofMap(ofGetHours(), 0, 12, 0, 255);

    ofColor color(RGBColor, RGBColor, RGBColor, 255);
    return color;
}

//--------------------------------------------------------------
void ofApp::saveCurrentImage() {

    char name[31];
    sprintf( name, "relogiodesol_%04d_%02d_%02d_%02d_%02d_%02d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds() );

    ofPixels screenPixels;
    this->finalFbo.readToPixels(screenPixels);
    ofSaveImage(screenPixels, name, OF_IMAGE_QUALITY_BEST);

    //char backgroundImageName[27];
    //sprintf( backgroundImageName, "relogiodesol_%d_%d_%d.png", ofGetYear(), ofGetMonth(), ofGetDay() );

    //ofPixels backgroundPixels;
    //this->imageFbo.readToPixels(backgroundPixels);
    //ofSaveImage(backgroundPixels, backgroundImageName, OF_IMAGE_QUALITY_BEST);*/

    lastSavedImageTime = ofGetElapsedTimef();
}

//------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        saveCurrentImage();
    } else if (key == OF_KEY_TAB) {

        if (this->gui->isVisible()) {
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
        }
        else {
            this->showConfigurationPanel();
        }

    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    float scaleV = (float) ofGetHeight() / (float) this->cameraHeight;
    float scaleH = (float) ofGetWidth() / (float) this->cameraWidth;
    this->scale = min(scaleV, scaleH);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofApp::exit() {
    //delete gui;
}

//--------------------------------------------------------------

void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->currentStrings["save"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            
            this->cameraPanel->saveSettings("camera.xml");
            this->imagePanel->saveSettings("image.xml");
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();
            
        }
    }
    else if (e.getName() == this->currentStrings["cancel"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
            
        }
    }
    else if (e.getName() == this->currentStrings["support"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            ofLaunchBrowser("http://www.funarte.gov.br/");
        }
    }
    else if (e.getName() == ofApp::CHANGE_LOCALE_BUTTON_NAME) {
        // catches the click when mouse is released, not pressed
        if(!e.getButton()->getValue()) {
            if (this->currentLocale == LOCALE_ENGLISH) {
                this->currentLocale = LOCALE_PORTUGUESE;
                this->currentStrings = this->ptStrings;
            }
            else {
                this->currentLocale = LOCALE_ENGLISH;
                this->currentStrings = this->enStrings;
            }
            if (this->currentLocale == LOCALE_ENGLISH) {
                this->changeLocaleButton->getLabelWidget()->setLabel(PORTUGUESE_LABEL);
            }
            else {
                this->changeLocaleButton->getLabelWidget()->setLabel(ENGLISH_LABEL);
            }
            
            this->titleLabel->setLabel( this->currentStrings["sundial"] );
            
            this->pickCameraLabel->setLabelText( this->currentStrings["pickCamera"] );
            
            this->cameraWidthLabel->setLabel( this->currentStrings["cameraWidth"] );
            this->cameraHeightLabel->setLabel( this->currentStrings["cameraHeight"] );
            
            this->imageRotationLabel->setLabel( this->currentStrings["imageRotation"] );
            
            this->zeroRotationToggle->getLabelWidget()->setLabel( this->currentStrings["zeroDegress"] );
            this->ninetyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["ninetyDegress"] );
            this->oneHundredEightyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["oneHundredEightyDegress"] );
            this->twoHundredSeventyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["twoHundredSeventyDegress"] );
            
            this->latitudeLabel->setLabel( this->currentStrings["latitude"] );
            this->longitudeLabel->setLabel( this->currentStrings["longitude"] );
            
            this->saveImageToggle->getLabelWidget()->setLabel( this->currentStrings["saveImage"] );
            
            this->minutesLabel->setLabel( this->currentStrings["minutes"] );
            
            this->clearButton->setLabelText( this->currentStrings["resetImage"] );
            
            this->showAtStartupToggle->getLabelWidget()->setLabel( this->currentStrings["showAtStartup"] );
            
            this->fullScreenToggle->getLabelWidget()->setLabel( this->currentStrings["fullScreen"] );
            
            this->saveButton->setLabelText( this->currentStrings["save"] );
            this->cancelButton->setLabelText( this->currentStrings["cancel"] );
            
            this->credits1Label->setLabel( this->currentStrings["credits1"] );
            this->credits2Label->setLabel( this->currentStrings["credits2"] );
            this->credits3Label->setLabel( this->currentStrings["credits3"] );
            this->credits4Label->setLabel( this->currentStrings["credits4"] );
            this->credits5Label->setLabel( this->currentStrings["credits5"] );
            
            this->supportLabel->setLabel( this->currentStrings["support"] );
            
        }
    }
}

void ofApp::checkTextInputFocus (ofxUIEventArgs &e) {
    if (e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            this->unfocusAllTextInputs(ti);
        }
    }
}

void ofApp::cameraPanelEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->currentStrings["zeroDegress"] && e.getToggle()->getValue()) {
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->currentStrings["ninetyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->currentStrings["oneHundredEightyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->currentStrings["twoHundredSeventyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
    }
    else {
        this->checkTextInputFocus(e);
    }
}

void ofApp::imagePanelEvent(ofxUIEventArgs &e) {
    ofLog() << "e.getName(): " << e.getName();
    if (e.getName() == this->currentStrings["resetImage"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            this->clearImage();
            this->saveCurrentImage();

        }
    }

    if (e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            this->unfocusAllTextInputs(ti);
        }
    }
}

void ofApp::hideConfigurationPanel() {
    this->gui->setVisible(false);
    this->cameraPanel->setVisible(false);
    this->imagePanel->setVisible(false);
    ofHideCursor();
}

void ofApp::showConfigurationPanel() {
    this->gui->setVisible(true);
    this->gui->disableKeyEventCallbacks();

    this->cameraPanel->setVisible(true);
    this->imagePanel->setVisible(true);
    ofShowCursor();
}

void ofApp::cancelConfigurationChanges() {
    this->zeroRotationToggle->setValue(this->rotations == 0);
    this->ninetyRotationToggle->setValue(this->rotations == 1);
    this->oneHundredEightyRotationToggle->setValue(this->rotations == 2);
    this->twoHundredSeventyRotationToggle->setValue(this->rotations == 3);

    if (this->rotations % 2 == 0) {
        this->cameraWidthTextInput->setTextString( ofToString(this->cameraWidth) );
        this->cameraHeightTextInput->setTextString( ofToString(this->cameraHeight) );
    }
    else {
        this->cameraWidthTextInput->setTextString( ofToString(this->cameraHeight) );
        this->cameraHeightTextInput->setTextString( ofToString(this->cameraWidth) );
    }

    this->intervalToSaveTextInput->setTextString( ofToString(this->intervalToSaveImage) );
    this->showAtStartupToggle->setValue( this->showAtStartup );
    this->fullScreenToggle->setValue( this->fullScreen );
    this->saveImageToggle->setValue( this->saveImage );
}

void ofApp::applyConfigurationChanges() {
    if (this->zeroRotationToggle->getValue()) {
        this->rotations = 0;
    }
    if (this->ninetyRotationToggle->getValue()) {
        this->rotations = 1;
    }
    else if (this->oneHundredEightyRotationToggle->getValue()) {
        this->rotations = 2;
    }
    else if (this->twoHundredSeventyRotationToggle->getValue()) {
        this->rotations = 3;
    }

    this->zeroRotationToggle->setValue(this->rotations == 0);
    this->ninetyRotationToggle->setValue(this->rotations == 1);
    this->oneHundredEightyRotationToggle->setValue(this->rotations == 2);
    this->twoHundredSeventyRotationToggle->setValue(this->rotations == 3);

    if (this->rotations % 2 == 0) {
        this->cameraWidth = this->cameraWidthTextInput->getIntValue();
        this->cameraHeight = this->cameraHeightTextInput->getIntValue();
    }
    else {
        this->cameraWidth = this->cameraHeightTextInput->getIntValue();
        this->cameraHeight = this->cameraWidthTextInput->getIntValue();
    }

    this->intervalToSaveImage = this->intervalToSaveTextInput->getIntValue();

    this->showAtStartup = this->showAtStartupToggle->getValue();
    this->fullScreen = this->fullScreenToggle->getValue();
    this->saveImage = this->saveImageToggle->getValue();

    if (this->fullScreen) {
        ofSetFullscreen(true);
    }
    else {
        ofSetFullscreen(false);
        ofSetWindowShape(this->cameraWidth, this->cameraHeight);
    }
}

void ofApp::unfocusAllTextInputs(ofxUITextInput* except){
    for (int i = 0; i < this->textInputs.size(); i ++){
        if (except != this->textInputs[i]){
            this->textInputs[i]->setFocus(false);
        }
    }
}
