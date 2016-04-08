//
//  ofxEtherdreamInfo.h
//  LaserAppOrg
//
//  Created by Mitsuru Takeuchi on 2016/04/08.
//
//

#ifndef ofxEtherdreamInfo_h
#define ofxEtherdreamInfo_h

#include <iostream>

class ofxEtherdreamInfo {
    
public:
    unsigned long dac_id;
    string ip;
    ofxEtherdreamInfo(){
        dac_id = 0;
        ip = "";
    }
};


#endif /* ofxEtherdreamInfo_h */
