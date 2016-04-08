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
    ofxEtherdream():state(ETHERDREAM_NOTFOUND), bAutoConnect(false) {}
    
    ~ofxEtherdream() {
        kill();
    }
    
    static bool lib_initialized;
    static bool startlib(){
        int res = etherdream_lib_start();
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
    
    void kill() {
        clear();
        stop();
        if(stateIsFound()) {
            etherdream_stop(device);
            etherdream_disconnect(device);
        }
    }
    
//    void setup(bool bStartThread = true, int idEtherdream = 0);
    void setup();
    virtual void threadedFunction();
    bool connect();
    void disconnect();
    
    // check if the device has shutdown (weird bug in etherdream driver) and reconnect if nessecary
    bool checkConnection(bool bForceReconnect = true);
    
    void clear();
    void start();
    void stop();

    void addPoints(const vector<ofxIlda::Point>& _points);
    void addPoints(const ofxIlda::Frame &ildaFrame);
    
    void setPoints(const vector<ofxIlda::Point>& _points);
    void setPoints(const ofxIlda::Frame &ildaFrame);
    
    void send();
    
    void setPPS(int i);
    int getPPS() const;
    
    void setWaitBeforeSend(bool b);
    bool getWaitBeforeSend() const;
    
private:
    void init();
    etherdream* getDevice();
    
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