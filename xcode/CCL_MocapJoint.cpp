//
//  CCL_MocapJoint.cpp
//  CCL_JSON
//
//  Created by Gene Han on 8/28/15.
//
//

#include "CCL_MocapJoint.h"
#define NULL_POINT -123456.f

CCL_MocapJoint::CCL_MocapJoint( string jName, int skip){
    jointName = jName;
    this->skip = skip;
    xUpdated = false;
    yUpdated = false;
    zUpdated = false;
};

void CCL_MocapJoint::addUUID(string title,const string uuid){
    if( title.compare("X") == 0){
        string url = "http://api.piecemeta.com/streams/" + uuid+".json?skip=10";
     //   std::cout << "[Stream URL]"<< url<< std::endl;
        const JsonTree stream( loadUrl(url) );
        int frameCount = stream["frameCount"].getValue<int>();
        for (int i = 0 ; i < stream["frames"].getNumChildren() ; i++){
            string temp = stream["frames"][i].getValue();
            if( temp == "null" || temp.size() == 0){
   //             cout << "[FrameAt:"<<i<< ","<<temp<<endl;
                xPositions.push_back(NULL_POINT);

            }else{
  //              cout << "[FrameAt:"<<i<< ","<<temp<<endl;
                xPositions.push_back(stof(temp));
            }
        }
    //    cout << jointName << "X FrameCount:" << frameCount <<" FramesNum:" << xPositions.size() << " Skip:10" << endl;
        xUpdated = true;
    }else if( title.compare("Y") == 0 ){
        string url = "http://api.piecemeta.com/streams/" + uuid+".json?skip=10";
     //   std::cout << "[Stream URL]"<< url<< std::endl;
        const JsonTree stream( loadUrl(url) );
        int frameCount = stream["frameCount"].getValue<int>();
        for (int i = 0 ; i < stream["frames"].getNumChildren() ; i++){
            string temp = stream["frames"][i].getValue();
            if( temp == "null" || temp.size() == 0){
                //             cout << "[FrameAt:"<<i<< ","<<temp<<endl;
                yPositions.push_back(NULL_POINT);
                
            }else{
                //              cout << "[FrameAt:"<<i<< ","<<temp<<endl;
                yPositions.push_back(stof(temp));
            }
        }
      //  cout << jointName << "Y FrameCount:" << frameCount <<" FramesNum:" << yPositions.size() << " Skip:10" << endl;
        yUpdated = true;
    }else if( title.compare("Z") == 0){
        string url = "http://api.piecemeta.com/streams/" + uuid+".json?skip=10";
    //    std::cout << "[Stream URL]"<< url<< std::endl;
        const JsonTree stream( loadUrl(url) );
        int frameCount = stream["frameCount"].getValue<int>();
        for (int i = 0 ; i < stream["frames"].getNumChildren() ; i++){
            string temp = stream["frames"][i].getValue();
            if( temp == "null" || temp.size() == 0){
                //             cout << "[FrameAt:"<<i<< ","<<temp<<endl;
                zPositions.push_back(NULL_POINT);
                
            }else{
                //              cout << "[FrameAt:"<<i<< ","<<temp<<endl;
                zPositions.push_back(stof(temp));
            }
        }
   //     cout << jointName <<  "Z FrameCount:" << frameCount <<" FramesNum:" << zPositions.size() << " Skip:10" << endl;
        zUpdated = true;
    }
    
    if( xUpdated && yUpdated && zUpdated){
        
        loadPositions();
    }
    

};

void CCL_MocapJoint::loadPositions(){
    for( int i = 0 ; i < xPositions.size() ; i++){
        vec3 vecter = vec3(xPositions[i], yPositions[i], zPositions[i]);
        jointPositions.push_back(vecter);
        //      cout << "["<<jointName<< " at " << i<< "] " << vecter << endl;
    }
    std::cout << "added joint " << jointName << ", count:" << jointPositions.size() << std::endl;
};