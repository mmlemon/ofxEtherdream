//
//  ofxEtherdream.h
//  ofxILDA
//
//  Created by Daito Manabe + Yusuke Tomoto (Rhizomatiks)
//  Mods by Memo Akten
//
//


#include "ofMain.h"
#include "etherdream.h"
#include "ofxIldaFrame.h"
#include "ofxEtherdreamInfo.h"
#include <sstream>

class ofxEtherdream : public ofThread {
public:
    
    // constructor / destructor
    ofxEtherdream():state(ETHERDREAM_NOTFOUND), bAutoConnect(false) {}
    ~ofxEtherdream() {
        kill();
    }
    
    // static functions -------------------------------
    static bool startlib(){
        int res = etherdream_lib_start();
        usleep(1500000);
        if(res==0){
            cout << ofxEtherdream::listDevicesStr() << endl;
        }
        return (res == 0);
    }
    
    static vector<ofxEtherdreamInfo> listDevices(){
        int num_devices = etherdream_dac_count();
        vector<ofxEtherdreamInfo> res;
        for(int i = 0; i < num_devices; i++)
        {
            ofxEtherdreamInfo info;
            etherdream* dac = etherdream_get(i);
            unsigned long dac_id = etherdream_get_id(dac);
            const in_addr* addr = etherdream_get_in_addr(dac);
            std::string ip = inet_ntoa(*addr);
            info.ip = ip;
            info.dac_id = dac_id;
            
            res.push_back(info);
        }
        return res;
    }
    
    static string listDevicesStr(){
        vector<ofxEtherdreamInfo> info = listDevices();
        stringstream ss;
        for(int i = 0; i < info.size();i++){
            ss << "============" << endl;
            ss << "device[" << i << "] dac_id:" << info[i].dac_id << "  ip:" << info[i].ip << endl;
            ss << "============" << endl;
        }
        cout << ss.str() << endl;
        return ss.str();
    }
    
    bool stateIsFound();
    void kill();
    void setup();
    void setDacRef(unsigned long dac_id);
    virtual void threadedFunction();
    bool connect();
    void disconnect();
    // check if the device has shutdown (weird bug in etherdream driver) and reconnect if nessecary
    bool checkConnection(bool bForceReconnect = true);
    
    void clear();
    void addPoints(const vector<ofxIlda::Point>& _points);
    void addPoints(const ofxIlda::Frame &ildaFrame);
    void setPoints(const vector<ofxIlda::Point>& _points);
    void setPoints(const ofxIlda::Frame &ildaFrame);
    
    void send();
    void setPPS(int i);
    int getPPS() const;
    void setWaitBeforeSend(bool b);
    bool getWaitBeforeSend() const;
    bool enableEmit();
    void stopEmit();
    ofxEtherdreamInfo* getDeviceInfo();
    
private:
    etherdream* getDevice();
    void start();
    void stop();
    
private:

    enum {
        ETHERDREAM_NOTFOUND = 0,
        ETHERDREAM_FOUND
    } state;
    
    int pps;
    bool bWaitBeforeSend;
    bool bAutoConnect;
    // etherdream info
    ofxEtherdreamInfo _deviceInfo;
    
    etherdream *device;
    vector<ofxIlda::Point> points;
    
    int idEtherdreamConnection;
};