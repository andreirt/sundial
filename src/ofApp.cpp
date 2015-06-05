#include "ofApp.h"
#include <sys/time.h>
#include <math.h>
#define PI 3.14159265

const string ofApp::CAMERA_WIDTH_LABEL = "Largura da câmera";
const string ofApp::CAMERA_HEIGHT_LABEL = "Altura da câmera";
const string ofApp::LATITUDE_LABEL = "Latitude";
const string ofApp::LONGITUDE_LABEL = "Longitude";
const string ofApp::SAVE_IMAGE_LABEL = "Salvar imagem a cada";
const string ofApp::RESET_IMAGE_LABEL = "Limpar imagem";
const string ofApp::SUPPORT_BUTTON_NAME = "support";
const string ofApp::SAVE_LABEL = "Salvar";
const string ofApp::CANCEL_LABEL = "Cancelar";

const string ofApp::ZERO_DEGREES_LABEL = "0 graus";
const string ofApp::NINETY_DEGREES_LABEL = "90 graus";
const string ofApp::ONE_HUNDRED_EIGHTY_DEGREES_LABEL = "180 graus";
const string ofApp::TWO_HUNDRED_SEVENTY_DEGREES_LABEL = "270 graus";



//--------------------------------------------------------------
void ofApp::setup(){

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

    this->gui->addLabel("title", "Relógio de Sol", OFX_UI_FONT_LARGE);
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

    this->cameraList = this->cameraPanel->addDropDownList("Escolha a câmera", *cameras, 300, 10);
    this->cameraList->setAllowMultiple(false);
    this->cameraList->setDrawOutline(true);

    this->cameraPanel->addWidgetDown( new ofxUILabel(170, ofApp::CAMERA_WIDTH_LABEL, OFX_UI_FONT_SMALL) );
    this->cameraWidthTextInput = new ofxUITextInput("CameraWidth", "1920", 80, 18) ;
    this->cameraWidthTextInput->setOnlyNumericInput(true);
    this->cameraWidthTextInput->setDrawOutline(true);
    this->cameraWidthTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( cameraWidthTextInput );
    this->textInputs.push_back(this->cameraWidthTextInput);

    this->cameraPanel->addWidgetDown( new ofxUILabel(170, ofApp::CAMERA_HEIGHT_LABEL, OFX_UI_FONT_SMALL) );
    this->cameraHeightTextInput = new ofxUITextInput("CameraHeight", "1080", 80, 18);
    this->cameraHeightTextInput->setOnlyNumericInput(true);
    this->cameraHeightTextInput->setDrawOutline(true);
    this->cameraHeightTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( this->cameraHeightTextInput );
    this->textInputs.push_back(this->cameraHeightTextInput);

    this->rotations = 0;

    this->cameraPanel->addLabel("Rotação da imagem", OFX_UI_FONT_SMALL);

    this->zeroRotationToggle = new ofxUIToggle(ofApp::ZERO_DEGREES_LABEL, true, 16, 16);
    this->zeroRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->zeroRotationToggle);

    this->ninetyRotationToggle = new ofxUIToggle(ofApp::NINETY_DEGREES_LABEL, true, 16, 16);
    this->ninetyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->ninetyRotationToggle);

    this->oneHundredEightyRotationToggle = new ofxUIToggle(ofApp::ONE_HUNDRED_EIGHTY_DEGREES_LABEL, true, 16, 16);
    this->oneHundredEightyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->oneHundredEightyRotationToggle);

    this->twoHundredSeventyRotationToggle = new ofxUIToggle(ofApp::TWO_HUNDRED_SEVENTY_DEGREES_LABEL, true, 16, 16);
    this->twoHundredSeventyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->twoHundredSeventyRotationToggle);

    this->imagePanel = new ofxUICanvas(320, 0, 430, 260);
    this->imagePanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->imagePanel->setWidgetSpacing(10);
    this->imagePanel->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );

    this->imagePanel->addWidgetDown( new ofxUILabel(170, ofApp::LATITUDE_LABEL, OFX_UI_FONT_SMALL) );
    this->latitudeTextInput = new ofxUITextInput("Latitude", "-23.55149", 80, 18) ;
    this->latitudeTextInput->setOnlyNumericInput(true);
    this->latitudeTextInput->setDrawOutline(true);
    this->latitudeTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( latitudeTextInput );
    this->textInputs.push_back(this->latitudeTextInput);

    this->imagePanel->addWidgetDown( new ofxUILabel(170, ofApp::LONGITUDE_LABEL, OFX_UI_FONT_SMALL) );
    this->longitudeTextInput = new ofxUITextInput("Longitude", "46.63051", 80, 18) ;
    this->longitudeTextInput->setOnlyNumericInput(true);
    this->longitudeTextInput->setDrawOutline(true);
    this->longitudeTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( longitudeTextInput );
    this->textInputs.push_back(this->longitudeTextInput);

    this->saveImageToggle = new ofxUIToggle(ofApp::SAVE_IMAGE_LABEL, true, 16, 16);
    this->saveImageToggle->setDrawOutline(true);
    this->imagePanel->addWidgetDown(this->saveImageToggle);

    this->intervalToSaveImage = 15;

    this->intervalToSaveTextInput = new ofxUITextInput("A cada", "15", 60, 18);
    this->intervalToSaveTextInput->setOnlyNumericInput(true);
    this->intervalToSaveTextInput->setDrawOutline(true);
    this->intervalToSaveTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( this->intervalToSaveTextInput );
    this->textInputs.push_back(this->intervalToSaveTextInput);

    ofxUILabel* minutesLabel = new ofxUILabel(170, "minutos", OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetRight( minutesLabel );

    ofxUILabelButton* clearButton = this->gui->addLabelButton(ofApp::RESET_IMAGE_LABEL, false, 150, 20);
    clearButton->setDrawFill(true);
    clearButton->setDrawOutline(true);
    this->imagePanel->addWidgetDown( clearButton );
    this->imagePanel->addSpacer();

    this->showAtStartupToggle = this->imagePanel->addToggle("Exibir esta tela ao iniciar", true, 16, 16);
    this->showAtStartupToggle->setDrawOutline(true);
    this->imagePanel->addSpacer();

    this->fullScreenToggle = this->imagePanel->addToggle("Tela cheia", true, 16, 16);
    this->fullScreenToggle->setDrawOutline(true);

    this->gui->addWidgetRight(this->imagePanel);

    this->gui->addSpacer();

    ofxUILabelButton* saveButton = this->gui->addLabelButton(ofApp::SAVE_LABEL, false, 100, 20);
    saveButton->setDrawFill(true);
    saveButton->setDrawOutline(true);

    ofxUILabelButton* cancelButton = new ofxUILabelButton(ofApp::CANCEL_LABEL, false, 100, 20);
    cancelButton->setDrawFill(true);
    cancelButton->setDrawOutline(true);
    this->gui->addWidgetRight(cancelButton);
    this->gui->addSpacer();

    this->gui->addLabel("Relógio de Sol - Andrei Thomaz, 2015");
    this->gui->addLabel("Integrante do projeto Máquinas do Tempo, desenvolvido com apoio da Bolsa de Artes Visual da Funarte 2014");
    this->gui->addLabel("Desenvolvido em C++ / OpenFrameworks e distribução sob a licença MPL");
    this->gui->addLabel("Programação por Andrei Thomaz e Vitor Andrioli");
    this->gui->addSpacer();

    this->gui->addLabel("Realização");
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

    this->imageFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->shadowFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->finalFbo.allocate( this->cameraWidth, this->cameraHeight );

    float scaleV = (float) ofGetHeight() / (float) this->cameraHeight;
    float scaleH = (float) ofGetWidth() / (float) this->cameraWidth;
    this->scale = min(scaleV, scaleH);

    this->getCurrentDay();

    this->latitude = this->latitudeTextInput->getIntValue();
    this->longitude = this->longitudeTextInput->getIntValue();
}


//--------------------------------------------------------------
void ofApp::update(){
    ofSetColor( 255, 255, 255, 255 );
    ofSetColor( 255, 255, 255, 255 );
    this->getSunTime();

    ofPixels shadowPixels;
    ofPixels cameraPixels;
    ofxCvColorImage colorShadow;
    ofxCvGrayscaleImage grayShadow;
    ofTexture grayShadowTexture;

    this->grabber->update();
    ofTexture videoTexture = this->grabber->getTextureReference();
    videoTexture.readToPixels(cameraPixels);

    this->shadowFbo.begin();
        ofClear( 255, 255, 255 );

        float shadowWidth = getShadowWidth();
        float positionAndOffset = getShadowPositionAndOffset();


        cameraPixels.rotate90( this->rotations );

        colorShadow.setFromPixels(cameraPixels);

        grayShadow = colorShadow;
        shadowPixels = grayShadow.getPixelsRef();

        grayShadowTexture.allocate( colorShadow.getPixelsRef() );
        grayShadowTexture.setRGToRGBASwizzles(true);
        grayShadowTexture.loadData( shadowPixels );

        for (int i=0; i<shadowWidth; i++) {
            ofSetColor( 255, 255, 255, getShadowAlpha(i, shadowWidth) );
            grayShadowTexture.drawSubsection( (positionAndOffset+i), 0, 1, this->cameraHeight, (positionAndOffset+i), 0 );
        }

    this->shadowFbo.end();



    struct timeval inicio;
    gettimeofday(&inicio, NULL);
    this->currentTime = ((ofGetHours()-4)*3600 + ofGetMinutes()*60 + ofGetSeconds()) * 1000 + (int) inicio.tv_usec/1000;

    this->currentPixel = ofMap(currentTime, 0, (24*MILLISECONDS_PER_HOUR)-1, 0, (cameraPixelsLength-1));

    if (this->previousPixel == -1)
        this->previousPixel = this->currentPixel;

    if (this->currentPixel >= this->cameraPixelsLength)
			this->currentPixel = this->cameraPixelsLength-1;

    if (this->currentPixel != this->previousPixel) {

        int cont = this->previousPixel;

        while (cont != this->currentPixel) {

            column = cont / this->cameraHeight;
            line = cont % this->cameraHeight;

            this->imageFbo.begin();
                color = cameraPixels.getColor( column, line );
                ofSetColor( color );
                ofRect( column, line, 1, 1);
            this->imageFbo.end();

            cont++;

            if (cont >= this->cameraPixelsLength) {
                cont=0;
                this->clearImage();
            }
        }

        if (this->currentPixel < this->previousPixel) {
            this->getCurrentDay();
        }

        this->previousPixel = this->currentPixel;

         this->finalFbo.begin();
            ofClear( this->getBackgroundColor() );
            ofSetColor( 255, 255, 255 );
            this->imageFbo.draw( 0, 0 );
            ofSetColor( 255, 255, 255);
            this->shadowFbo.draw( 0, 0 );
         this->finalFbo.end();
    }

    if (ofGetElapsedTimef() >= this->lastSavedImageTime+(this->intervalToSaveImage*60)) {
        this->saveCurrentImage();
    }

}

//--------------------------------------------------------------
void ofApp::getSunTime() {

    float earthInclination = 23.45 * sin( ((360.0/365.0) * (float) (284 + dayInYear)) * PI/180 );
    float Td = (2.0/15.0) * (acos( -tan(this->latitude * PI/180) * tan( earthInclination * PI/180 ) ) / (PI/180));

    float correction = (this->longitude - 45.0) / 15.0;

    sunrise = (12 - (Td/2.0)) * MILLISECONDS_PER_HOUR;
    sunset = (12 + (Td/2.0)) * MILLISECONDS_PER_HOUR;

}

//--------------------------------------------------------------
void ofApp::getCurrentDay() {

    time_t now = time(0);
    tm *ltm = localtime(&now);
    dayInYear = (ltm->tm_yday) + 1;

    printf("Novo dia %d\n", dayInYear);

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
    sprintf( name, "relogiodesol_%d_%d_%d_%d_%d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes() );

    ofPixels screenPixels;
    this->finalFbo.readToPixels(screenPixels);
    ofSaveImage(screenPixels, name, OF_IMAGE_QUALITY_BEST);

    char backgroundImageName[27];
    sprintf( backgroundImageName, "relogiodesol_%d_%d_%d.png", ofGetYear(), ofGetMonth(), ofGetDay() );

    ofPixels backgroundPixels;
    this->imageFbo.readToPixels(backgroundPixels);
    ofSaveImage(backgroundPixels, backgroundImageName, OF_IMAGE_QUALITY_BEST);

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

void ofApp::guiEvent(ofxUIEventArgs &e) {
    ofLog() << "e.getName(): " << e.getName();

    if (e.getName() == ofApp::SAVE_LABEL) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {

            this->cameraPanel->saveSettings("camera.xml");
            this->imagePanel->saveSettings("image.xml");
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();

        }
    }
    else if (e.getName() == ofApp::CANCEL_LABEL) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {

            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();

        }
    }
    else if (e.getName() == ofApp::SUPPORT_BUTTON_NAME) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            ofLaunchBrowser("http://www.funarte.gov.br/");
        }
    }

    if (e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            this->unfocusAllTextInputs(ti);
        }
    }
}

void ofApp::cameraPanelEvent(ofxUIEventArgs &e) {
    if (e.getName() == ofApp::ZERO_DEGREES_LABEL) {
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == ofApp::NINETY_DEGREES_LABEL) {
        this->zeroRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == ofApp::ONE_HUNDRED_EIGHTY_DEGREES_LABEL) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == ofApp::TWO_HUNDRED_SEVENTY_DEGREES_LABEL) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
    }

    if (e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            this->unfocusAllTextInputs(ti);
        }
    }
}

void ofApp::imagePanelEvent(ofxUIEventArgs &e) {
    ofLog() << "e.getName(): " << e.getName();
    if (e.getName() == ofApp::RESET_IMAGE_LABEL) {
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
