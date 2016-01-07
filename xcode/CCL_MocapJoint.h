#pragma once

#include "cinder/Json.h"
#include "cinder/Vector.h"

using namespace std;
using namespace ci;
class CCL_MocapJoint {
public:
    CCL_MocapJoint( string jointName, int skip);
    int skip;
    string jointName;
    
    vector<vec3> jointPositions;
    
    void addUUID(string title, string uuid);
    void loadPositions();
    
    string UUID[3];
    
    vector<float> xPositions;
    vector<float> yPositions;
    vector<float> zPositions;
    
private:
    
    
    bool xUpdated;
    bool yUpdated;
    bool zUpdated;
    
};