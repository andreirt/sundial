#include "ofApp.h"
#include <sys/time.h>
#include <math.h>

const string ofApp::ENGLISH_LABEL = "English";
const string ofApp::PORTUGUESE_LABEL = "Português";
const string ofApp::CHANGE_LOCALE_BUTTON_NAME = "changeLocale";

const float ofApp::MAX_STRENGTH_AROUND_PIXEL = .15;
const long ofApp::MILLISECONDS_PER_HOUR = 60*60*1000;

float ofApp::latitude = -23.55149;
float ofApp::longitude = 46.63051;
int ofApp::intervalToSaveImage = 15;
int ofApp::degreesButtonValue = 0;
int ofApp::currentResolution = 2;
bool ofApp::saveImageToggle = false;
bool ofApp::showAtStartup = true;
bool ofApp::fullscreen = true;
bool ofApp::configurationPanelShow = true;

//----------------------------------------------------------------------------------------------------------------------------
void ofApp::setup(){
    gui = new ofxImGui;
    gui->setup();
    imageButtonID = gui->loadImage("funarte.png");
    this->lastTimeImageWasSaved = 0;
    this->rotations = 0;
    this->selectedCameraIndex = 0;
    this->lastDrawnPixel = -1;
    this->grabber = new ofVideoGrabber();

    this->currentLocale = LOCALE_PORTUGUESE;
    this->changeLocaleLabel = PORTUGUESE_LABEL;
    
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
    
    this->loadXmlSettings();
    this->selectResolution();
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
    if (initialBackground.load(backgroundImageName)) {
        this->imageFbo.begin();
        initialBackground.draw( 0, 0 );
        this->imageFbo.end();
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::reset(){
    if (this->grabber != nullptr) {
        if (this->grabber->isInitialized()) {
            this->grabber->close();
            delete this->grabber;
            this->grabber = new ofVideoGrabber();
        }
    }
    
    // set camera
    if (selectedCameraIndex > 0) {
        this->grabber->setDeviceID( selectedCameraIndex );
    }
    else {
        this->grabber->setDeviceID( 0 );
    }
    
    this->grabber->initGrabber(this->cameraWidth, this->cameraHeight);
    this->cameraPixelsLength = this->cameraWidth * this->cameraHeight;
    ofLog() << "this->cameraPixelsLength: " << this->cameraPixelsLength;

    this->imageFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->shadowFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->finalFbo.allocate( this->cameraWidth, this->cameraHeight );
    this->screenImage.allocate( this->cameraWidth, this->cameraHeight, OF_IMAGE_COLOR_ALPHA );
    
    float scaleV = (float) ofGetHeight() / (float) this->cameraHeight;
    float scaleH = (float) ofGetWidth() / (float) this->cameraWidth;
    this->scale = min(scaleV, scaleH);
    
    this->lastDrawnPixel = 0;
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::update(){
    this->grabber->update();
    
    if (this->configurationPanelShow == true) {
        ofShowCursor();
        return;
    }
    
    if(!grabber->isFrameNew())
        return;
    
    ofPixels shadowPixels;
    ofPixels cameraPixels;
    ofxCvColorImage colorShadow;
    ofxCvGrayscaleImage grayShadow;
    ofTexture grayShadowTexture;
    
    ofTexture videoTexture = this->grabber->getTexture();
    videoTexture.readToPixels(cameraPixels);
    
    float shadowWidth = getShadowWidth();
    float positionAndOffset = getShadowPositionAndOffset();
    
    cameraPixels.rotate90( this->rotations );
    
    colorShadow.setFromPixels(cameraPixels);
    
    grayShadow = colorShadow;
    shadowPixels = grayShadow.getPixels();
    
    grayShadowTexture.allocate( colorShadow.getPixels() );
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
        ofRectangle( column, line, 1, 1);
        
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
//----------------------------------------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------------------------------
float ofApp::getShadowPositionAndOffset() {
    if (currentTime >= 12*MILLISECONDS_PER_HOUR)
        return this->cameraWidth/2;
    else
        return ofMap(currentTime, sunrise, (12*MILLISECONDS_PER_HOUR), 0, this->cameraWidth/2);
}
//----------------------------------------------------------------------------------------------------------------------------
float ofApp::getShadowAlpha(int index, float shadowWidth) {
    if (currentTime > 12*MILLISECONDS_PER_HOUR)
        return ofMap(index, 0, shadowWidth, 255, 0);
    else
        return ofMap(index, 0, shadowWidth, 0, 255);
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::draw(){
    ofSetColor( 255, 255, 255, 255 );
    ofBackground( 0, 0, 0 );
    this->finalFbo.setAnchorPercent( 0.5, 0.5 );
    
    ofPushMatrix();
    ofTranslate( ofGetWidth()/2, ofGetHeight()/2 );
    ofScale( this->scale, this->scale );
    this->finalFbo.draw( 0, 0 );
    ofPopMatrix();
    
    //Interface
    if(configurationPanelShow == true){
        gui->begin();
        ImGui::SetNextWindowSize(ofVec2f(800,500));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin(this->currentStrings["sundial"].c_str());
        ImGui::Text(this->currentStrings["sundial"].c_str());
        if(ImGui::Button(this->changeLocaleLabel.c_str())){
            this->changeLocale();
        }
        
        vector< ofVideoDevice > devices = this->grabber->listDevices();
        vector<string> *cameras = new vector<string>();
        vector<ofVideoDevice>::iterator it;
        
        for (it = devices.begin(); it != devices.end(); ++it) {
            ofVideoDevice device = *it;
            cameras->push_back(device.deviceName);
        }
        
        char* nameDevices = (char*) malloc(512 * cameras->size());
        char* current = nameDevices;
        for (int i = 0; i < cameras->size(); i++) {
            strcpy(current, cameras->at(i).c_str());
            current += strlen(cameras->at(i).c_str());
            *current = '\0';
            ++current;
        }
        ImGui::PushItemWidth(200);
        ImGui::Combo(this->currentStrings["pickCamera"].c_str(), &selectedCameraIndex, nameDevices, cameras->size());
        free(nameDevices);
        ImGui::PushItemWidth(100);
        ImGui::Combo(this->currentStrings["deviceResolution"].c_str(), &currentResolution, deviceResolution, 3);
        ImGui::Text(this->currentStrings["imageRotation"].c_str());
        ImGui::RadioButton(this->currentStrings["zeroDegress"].c_str(), &degreesButtonValue, 0); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["ninetyDegress"].c_str(), &degreesButtonValue, 1); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["oneHundredEightyDegress"].c_str(), &degreesButtonValue, 2); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["twoHundredSeventyDegress"].c_str(), &degreesButtonValue, 3);
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat(this->currentStrings["latitude"].c_str(), &latitude, -360, 360);
        ImGui::SliderFloat(this->currentStrings["longitude"].c_str(), &longitude, -360, 360);
        ImGui::Checkbox(this->currentStrings["saveImage"].c_str(), &saveImageToggle); ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderInt(this->currentStrings["minutes"].c_str(), &intervalToSaveImage, 1, 60);
        
        if(ImGui::Button(this->currentStrings["resetImage"].c_str())){
            this->clearImage();
        }
        
        ImGui::Checkbox(this->currentStrings["showAtStartup"].c_str(), &showAtStartup);
        ImGui::Checkbox(this->currentStrings["fullScreen"].c_str(), &fullscreen);
        
        if(ImGui::Button(this->currentStrings["save"].c_str())){
            this->saveXmlSettings();
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();
        }
        ImGui::SameLine();
        if(ImGui::Button(this->currentStrings["cancel"].c_str())){
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
        }
        
        ImGui::Text(this->currentStrings["credits1"].c_str());
        ImGui::Text(this->currentStrings["credits2"].c_str());
        ImGui::Text(this->currentStrings["credits3"].c_str());
        ImGui::Text(this->currentStrings["credits4"].c_str());
        ImGui::Text(this->currentStrings["credits5"].c_str());
        ImGui::Text(this->currentStrings["support"].c_str());
        if(ImGui::ImageButton((ImTextureID)(uintptr_t)imageButtonID, ImVec2(509, 60))){
            ofLaunchBrowser("http://www.funarte.gov.br/");
        }
        
        setFullscreen();
        selectResolution();
        
        ImGui::End();
        gui->end();
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::clearImage() {
    this->imageFbo.begin();
    ofClear( 255, 255, 255 );
    this->imageFbo.end();
    
}
//----------------------------------------------------------------------------------------------------------------------------
ofColor ofApp::getBackgroundColor() {
    float RGBColor;
    if (currentTime > 12*MILLISECONDS_PER_HOUR)
        RGBColor = ofMap(ofGetHours(), 12, 24, 255, 0);
    else
        RGBColor = ofMap(ofGetHours(), 0, 12, 0, 255);
    
    ofColor color(RGBColor, RGBColor, RGBColor, 255);
    return color;
}
//----------------------------------------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        saveCurrentImage();
    } else if (key == 'c') {
        
        if (this->configurationPanelShow == true) {
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
        }
        else {
            this->showConfigurationPanel();
        }
        
    }

}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    float scaleV = (float) ofGetHeight() / (float) this->cameraHeight;
    float scaleH = (float) ofGetWidth() / (float) this->cameraWidth;
    this->scale = min(scaleV, scaleH);
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::hideConfigurationPanel(){
    this->configurationPanelShow = false;
    ofHideCursor();
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::showConfigurationPanel(){
    this->configurationPanelShow = true;
    ofShowCursor();
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::cancelConfigurationChanges(){
    this->latitude = -23.55149;
    this->longitude = 46.63051;
    intervalToSaveImage= 15;
    degreesButtonValue = 0;
    saveImageToggle = false;
    showAtStartup = true;
    fullscreen = true;
    currentResolution = 2;
    selectedCameraIndex = 0;
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::applyConfigurationChanges(){
    if (degreesButtonValue == 0) {
        this->rotations = 0;
    }
    if (degreesButtonValue == 1) {
        this->rotations = 1;
    }
    else if (degreesButtonValue == 2) {
        this->rotations = 2;
    }
    else if (degreesButtonValue == 3) {
        this->rotations = 3;
    }
    
    if (this->rotations % 2 != 0) {
        this->cameraWidth = this->cameraHeight;
        this->cameraHeight = this->cameraWidth;
    }
    
    this->selectResolution();
    this->setFullscreen();
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::setFullscreen(){
    if (fullscreen == true){
        ofSetFullscreen(true);
    } else {
        ofSetFullscreen(false);
        ofSetWindowShape(800, 600);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::selectResolution(){
    this->deviceResolution[0] = "800x600";
    this->deviceResolution[1] = "1280x720";
    this->deviceResolution[2] = "1920x1080";
    
    if(currentResolution == 0){
        cameraWidth = 800;
        cameraHeight = 600;
    } else if (currentResolution == 1){
        cameraWidth = 1280;
        cameraHeight = 720;
    } else if (currentResolution == 2){
        cameraWidth = 1920;
        cameraHeight = 1080;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::changeLocale(){
    //    string.c_str()
    if (this->currentLocale == LOCALE_ENGLISH) {
        this->currentLocale = LOCALE_PORTUGUESE;
        this->currentStrings = this->ptStrings;
    }
    else {
        this->currentLocale = LOCALE_ENGLISH;
        this->currentStrings = this->enStrings;
    }
    
    if (this->currentLocale == LOCALE_ENGLISH) {
        this->changeLocaleLabel = PORTUGUESE_LABEL;
    }
    else {
        this->changeLocaleLabel = ENGLISH_LABEL;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::saveXmlSettings(){
    settings.clear();
    settings.addChild("MAIN");
    settings.setTo("MAIN");
    settings.addValue("LATITUDE", ofToString(latitude));
    settings.addValue("LONGITUTE", ofToString(longitude));
    settings.addValue("INTERVAL_TO_SAVE", ofToString(intervalToSaveImage));
    settings.addValue("DEGREES_BUTTON_VAL", ofToString(degreesButtonValue));
    settings.addValue("CURRENT_RESOLUTION", ofToString(currentResolution));
    settings.addValue("SAVE_IMAGE_TOGGLE", ofToString(saveImageToggle));
    settings.addValue("SHOW_AT_STARTUP", ofToString(showAtStartup));
    settings.addValue("FULLSCREEN", ofToString(fullscreen));
    settings.addValue("SHOW_CONFIGURATION_PANEL", ofToString(configurationPanelShow));
    settings.save("settings.xml");
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::loadXmlSettings(){
    this->settings.load("settings.xml");
    
    if(settings.exists("//LATITUDE")){
        this->latitude = settings.getValue<float>("//LATITUDE");
    } else {
        latitude = -23.55149;;
    }
    
    if(settings.exists("//LONGITUDE")){
        this->longitude = settings.getValue<float>("//LONGITUDE");
    } else {
        longitude = 46.63051;
    }
    
    if(settings.exists("//INTERVAL_TO_SAVE")){
        this->intervalToSaveImage = settings.getValue<int>("//INTERVAL_TO_SAVE");
    } else {
        intervalToSaveImage = 15;
    }
    
    if(settings.exists("//DEGREES_BUTTON_VAL")){
        this->degreesButtonValue = settings.getValue<int>("//DEGREES_BUTTON_VAL");
    } else {
        degreesButtonValue = 0;
    }
    
    if(settings.exists("//CURRENT_RESOLUTION")){
        this->currentResolution = settings.getValue<int>("//CURRENT_RESOLUTION");
    } else {
        this->currentResolution = 2;
    }
    
    if(settings.exists("//SAVE_IMAGE_TOGGLE")){
        this->saveImageToggle = settings.getValue<bool>("//SAVE_IMAGE_TOGGLE");
    } else {
        this->saveImageToggle = false;
    }
    
    if(settings.exists("//SHOW_AT_STARTUP")){
        this->showAtStartup = settings.getValue<bool>("//SHOW_AT_STARTUP");
    } else {
        this->showAtStartup = true;
    }
    
    if(settings.exists("//FULLSCREEN")){
        this->fullscreen = settings.getValue<bool>("//FULLSCREEN");
    } else {
        this->fullscreen = true;
    }
    
    if(settings.exists("//SHOW_CONFIGURATION_PANEL")){
        this->configurationPanelShow = settings.getValue<bool>("//SHOW_CONFIGURATION_PANEL");
    } else {
        this->configurationPanelShow = true;
    }
}