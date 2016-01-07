//
//  CCL_MocapData.cpp
//  CCL_JSON
//
//  Created by Gene Han on 8/28/15.
//
//

#include "CCL_MocapData.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

namespace ccl
{

std::vector<CCL_MocapJoint> loadMotionCaptureFromJson(const ci::fs::path &file)
{
  vector<CCL_MocapJoint> mJoints;

  Jzon::Object rootNode;
  Jzon::FileReader::ReadFile(file.string(), rootNode);

  const Jzon::Array &joints = rootNode.Get("joints").AsArray();
  for( Jzon::Array::const_iterator it = joints.begin() ; it != joints.end();++it){
      Jzon::Object joint = (*it).AsObject();
      // name
      string name = joint.Get("name").ToString();

      // xPos
      Jzon::Array xPosList = joint.Get("xPos").AsArray();
      vector<float> xPosVec;
      for( Jzon::Array::iterator itt = xPosList.begin() ; itt != xPosList.end();++itt){
          float pos = stof((*itt).ToString());
          xPosVec.push_back(pos);
      }
      // yPos
      Jzon::Array yPosList = joint.Get("yPos").AsArray();
      vector<float>yPosVec;
      for( Jzon::Array::iterator itt = yPosList.begin() ; itt != yPosList.end();++itt){
          float pos = stof((*itt).ToString());
          yPosVec.push_back(pos);
      }
      // zPos
      Jzon::Array zPosList = joint.Get("zPos").AsArray();
      vector<float> zPosVec;
      for( Jzon::Array::iterator itt = zPosList.begin() ; itt != zPosList.end();++itt){
          float pos = stof((*itt).ToString());
          zPosVec.push_back(pos);
      }


    //  cout << "JOINT NAME:"<<name<<endl;
      CCL_MocapJoint cclJoint = CCL_MocapJoint(name,10);
      cclJoint.xPositions = xPosVec;
      cclJoint.yPositions = yPosVec;
      cclJoint.zPositions = zPosVec;
      cclJoint.loadPositions();

      mJoints.push_back(cclJoint);

  }

  for( int i  = 0 ; i < 10 ; i++) {
      CCL_MocapJoint jointt = mJoints[i];
      for( int j = 0 ; j < 3 ; j++){
          cout << "[" << jointt.jointName << "] (" << jointt.jointPositions[j].x << "," <<jointt.jointPositions[j].y << ","<< jointt.jointPositions[j].z <<")"<< endl;
      }
  }

  return mJoints;
};


std::vector<CCL_MocapJoint> loadMotionCaptureFromSite(const ci::Url &url, int skip)
{
  vector<CCL_MocapJoint> mJoints;
  try{
      //LOAD JSON FILE CONTAINING UUIDs of each stream under specified channel
      const JsonTree json( loadUrl(url) );

      //LOOP THROUGH STREAMS
      for( auto &stream : json.getChildren() ){
          const string &uuid = stream["uuid"].getValue();
          string title = stream["title"].getValue();
          if( stream.hasChild("group")){
              string group = stream["group"].getValue();
              //      std::cout << "group:"<< group << " title:" << title << " uuid[" << i << "] = " << uuid << std::endl;
              addUUIDtoJoint(group, title, uuid, skip, mJoints);
          }
      }

      // create json file
      Jzon::Object root;
      root.Add("URL", url.str());
      Jzon::Array joints;

      for( int i = 0 ; i < mJoints.size() ; i++){
          CCL_MocapJoint theJoint = mJoints[i];
          Jzon::Object joint;
          joint.Add("name", theJoint.jointName );
          Jzon::Array xPoslist;
          Jzon::Array yPoslist;
          Jzon::Array zPoslist;

          for( int j = 0 ; j < theJoint.jointPositions.size(); j++){
              xPoslist.Add(theJoint.jointPositions[j].x);
              yPoslist.Add(theJoint.jointPositions[j].y);
              zPoslist.Add(theJoint.jointPositions[j].z);
          }

          joint.Add("xPos", xPoslist);
          joint.Add("yPos", yPoslist);
          joint.Add("zPos", zPoslist);

          joints.Add(joint);
      }
      root.Add("joints", joints);
      cout << "Start writing file as CCL_JOINT.json" <<endl;
      Jzon::FileWriter::WriteFile("CCL_JOINT.json", root, Jzon::StandardFormat);
      cout << "Ended writing file" <<endl;


  }
  catch( ci::Exception &exc ) {
      std::cout << "Failed to parse json, what: " << exc.what() << std::endl;
  }

  return mJoints;
};



void addUUIDtoJoint(const std::string &grp, const std::string &title, const std::string &uuid, int skip, std::vector<CCL_MocapJoint>& mJoints)
{
 //   std::cout << "add - grp:"<<grp<< " title:"<<title<<" uuid:"<<uuid<<std::endl;

    bool hasFound = false;
    for(int i = 0; i < mJoints.size(); i++) {
        CCL_MocapJoint joint = mJoints[i];
        if( joint.jointName == grp  ){
            hasFound = true;
            mJoints[i].addUUID(title, uuid);
            i = mJoints.size();
        }
    }

    if( !hasFound ){
        CCL_MocapJoint joint = CCL_MocapJoint(grp, skip);
        std::cout << "total joints: " << mJoints.size() + 1 << std::endl;
        joint.addUUID(title, uuid);
        mJoints.push_back(joint);
//        cout<<"[JOINT NUM]"<<mJoints.size()<<endl <<endl;

    }
}

} // namespace ccl