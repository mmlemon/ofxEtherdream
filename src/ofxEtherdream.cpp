#include "ofxEtherdream.h"
#include "etherdream.h"

//--------------------------------------------------------------
void ofxEtherdream::setup()
{
    setPPS(30000);
    setWaitBeforeSend(false);
}
//--------------------------------------------------------------
void ofxEtherdream::setDacRef(unsigned long dac_id)
{
    for(int i = 0; i < listDevices().size(); i++)
    {
        etherdream* dac = etherdream_get(i);
        unsigned long _id = etherdream_get_id(dac);
        if(dac_id == _id){
            device = dac;
            _deviceInfo = listDevices()[i];
            break;
        }
    }
}
//--------------------------------------------------------------
etherdream* ofxEtherdream::getDevice()
{
    return device;
}
//--------------------------------------------------------------
bool ofxEtherdream::connect(){
    if(getDevice()==NULL){ return;}
    
    int connect = etherdream_connect(device);
    if(connect==0){
        state = ETHERDREAM_FOUND;
        cout << "connecting......" << endl;
        start();
    }else{
        state = ETHERDREAM_NOTFOUND;
        cout << "connecting failed." << endl;
    }
    return (connect==0);
}
//--------------------------------------------------------------
void ofxEtherdream::disconnect()
{
    if(getDevice()==NULL){ return;}
    kill();
    etherdream_disconnect(device);
    state = ETHERDREAM_NOTFOUND;
}

//--------------------------------------------------------------
bool ofxEtherdream::stateIsFound() {
    return state == ETHERDREAM_FOUND;
}
//--------------------------------------------------------------
void ofxEtherdream::kill() {
    clear();
    stop();
    if(stateIsFound()) {
        etherdream_stop(device);
        etherdream_disconnect(device);
    }
}
//--------------------------------------------------------------
bool ofxEtherdream::checkConnection(bool bForceReconnect) {
    if(device->state == ST_SHUTDOWN || device->state == ST_BROKEN || device->state == ST_DISCONNECTED)
    {
    
        if(bForceReconnect) {
            kill();
            connect();
        }
        
        return false;
    }
    return true;
}

//--------------------------------------------------------------
void ofxEtherdream::threadedFunction() {
    while (isThreadRunning() != 0) {
        
        switch (state) {
            case ETHERDREAM_NOTFOUND:
                if(bAutoConnect) connect();
                break;
                
            case ETHERDREAM_FOUND:
                if(lock()) {
                    send();
                    unlock();
                }
                break;
        }
    }
}

//--------------------------------------------------------------
void ofxEtherdream::start() {
    startThread(true, false);  // TODO: blocking or nonblocking?
}

//--------------------------------------------------------------
void ofxEtherdream::stop() {
    stopThread();
}
//--------------------------------------------------------------
void ofxEtherdream::stopEmit(){
    etherdream_stop(device);
}
//--------------------------------------------------------------
void ofxEtherdream::send() {
    if(!stateIsFound() || points.empty()) return;
    
    if(bWaitBeforeSend) etherdream_wait_for_ready(device);
    else if(!etherdream_is_ready(device)) return;
    
    // DODGY HACK: casting ofxIlda::Point* to etherdream_point*
    int res = etherdream_write(device, (etherdream_point*)points.data(), points.size(), pps, 1);
    if (res != 0) {
        ofLogVerbose() << "ofxEtherdream::write " << res;
    }
    points.clear();
}


//--------------------------------------------------------------
void ofxEtherdream::clear() {
    if(lock()) {
        points.clear();
        unlock();
    }
}

//--------------------------------------------------------------
void ofxEtherdream::addPoints(const vector<ofxIlda::Point>& _points) {
    if(lock()) {
        if(!_points.empty()) {
            points.insert(points.end(), _points.begin(), _points.end());
        }
        unlock();
    }
}


//--------------------------------------------------------------
void ofxEtherdream::addPoints(const ofxIlda::Frame &ildaFrame) {
    addPoints(ildaFrame.getPoints());
}


//--------------------------------------------------------------
void ofxEtherdream::setPoints(const vector<ofxIlda::Point>& _points) {
    if(lock()) {
        points = _points;
        unlock();
    }
}


//--------------------------------------------------------------
void ofxEtherdream::setPoints(const ofxIlda::Frame &ildaFrame) {
    setPoints(ildaFrame.getPoints());
}

//--------------------------------------------------------------
void ofxEtherdream::setWaitBeforeSend(bool b) {
    if(lock()) {
        bWaitBeforeSend = b;
        unlock();
    }
}

//--------------------------------------------------------------
bool ofxEtherdream::getWaitBeforeSend() const {
    return bWaitBeforeSend;
}


//--------------------------------------------------------------
void ofxEtherdream::setPPS(int i) {
    if(lock()) {
        pps = i;
        unlock();
    }
}

//--------------------------------------------------------------
int ofxEtherdream::getPPS() const {
    return pps;
}
//--------------------------------------------------------------
ofxEtherdreamInfo* ofxEtherdream::getDeviceInfo()
{
    return &_deviceInfo;
}
//--------------------------------------------------------------
bool ofxEtherdream::enableEmit()
{
    return etherdream_is_ready(device);
}