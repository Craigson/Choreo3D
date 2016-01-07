#include "Skeleton.h"

Skeleton::Skeleton(){}

Skeleton::Skeleton(const std::vector<glm::vec3>& jointPos)
{
    boneJoints = jointPos;
    
    head = boneJoints[0];
    r_shoulder = boneJoints[6];
    r_elbow = boneJoints[8];
    r_hand = boneJoints[9];
    l_shoulder = boneJoints[13];
    l_elbow = boneJoints[16];
    l_hand = boneJoints[17];
    spine_base = boneJoints[27];
    spine_top = boneJoints[21];
    r_hip = boneJoints[28];
    r_knee = boneJoints[34];
    r_foot = boneJoints[35];
    l_hip = boneJoints[29];
    l_knee = boneJoints[43];
    l_foot = boneJoints[44];
    
    
    physicsJoints.push_back(Joint(boneJoints[0],"head"));
    physicsJoints.push_back(Joint(boneJoints[21],"spine_top"));
    physicsJoints.push_back(Joint(l_shoulder,"l_shoulder"));
    physicsJoints.push_back(Joint(l_elbow,"l_elbow"));
    physicsJoints.push_back(Joint(l_hand,"l_hand"));
    physicsJoints.push_back(Joint(boneJoints[27],"spine_base"));
    physicsJoints.push_back(Joint(boneJoints[29],"l_hip"));
    physicsJoints.push_back(Joint(boneJoints[43],"l_knee"));
    physicsJoints.push_back(Joint(boneJoints[44],"l_foot"));
    physicsJoints.push_back(Joint(r_hip,"r_hip"));
    physicsJoints.push_back(Joint(r_knee,"r_knee"));
    physicsJoints.push_back(Joint(r_foot,"r_foot"));
    physicsJoints.push_back(Joint(r_shoulder,"r_shoulder"));
    physicsJoints.push_back(Joint(r_elbow,"r_elbow"));
    physicsJoints.push_back(Joint(r_hand,"r_hand"));
    
    
    physicsBones.push_back(Bone(&physicsJoints[0], &physicsJoints[1], distance(physicsJoints[0].location, physicsJoints[1].location)));
    physicsBones.push_back(Bone(&physicsJoints[1], &physicsJoints[2], distance(physicsJoints[1].location, physicsJoints[2].location)));
    physicsBones.push_back(Bone(&physicsJoints[2], &physicsJoints[3], distance(physicsJoints[2].location, physicsJoints[3].location)));
    physicsBones.push_back(Bone(&physicsJoints[3], &physicsJoints[4], distance(physicsJoints[3].location, physicsJoints[4].location)));
    physicsBones.push_back(Bone(&physicsJoints[1], &physicsJoints[5], distance(physicsJoints[1].location, physicsJoints[5].location)));
    
    
    physicsBones.push_back(Bone(&physicsJoints[5], &physicsJoints[6], distance(physicsJoints[5].location, physicsJoints[6].location)));
    physicsBones.push_back(Bone(&physicsJoints[6], &physicsJoints[7], distance(physicsJoints[6].location, physicsJoints[7].location)));
    physicsBones.push_back(Bone(&physicsJoints[7], &physicsJoints[8], distance(physicsJoints[7].location, physicsJoints[8].location)));
    
    physicsBones.push_back(Bone(&physicsJoints[5], &physicsJoints[9], distance(physicsJoints[5].location, physicsJoints[9].location)));
    physicsBones.push_back(Bone(&physicsJoints[9], &physicsJoints[10], distance(physicsJoints[9].location, physicsJoints[10].location)));
    physicsBones.push_back(Bone(&physicsJoints[10], &physicsJoints[11], distance(physicsJoints[10].location, physicsJoints[11].location)));
    physicsBones.push_back(Bone(&physicsJoints[1], &physicsJoints[12], distance(physicsJoints[1].location, physicsJoints[12].location)));
    physicsBones.push_back(Bone(&physicsJoints[12], &physicsJoints[13], distance(physicsJoints[12].location, physicsJoints[13].location)));
    physicsBones.push_back(Bone(&physicsJoints[13], &physicsJoints[14], distance(physicsJoints[13].location, physicsJoints[14].location)));
    
    
}

void Skeleton::renderStatic()
{
    
        ci::gl::color(1.,1.,1.);
        ci::gl::lineWidth(5);
        
        //TORSO
        ci::gl::drawLine(r_shoulder, l_shoulder);
        ci::gl::drawLine(spine_top, spine_base);
        ci::gl::drawLine(spine_base, r_hip);
        ci::gl::drawLine(spine_base, l_hip);
        
        //RIGHT ARM
        ci::gl::drawLine(r_shoulder, r_elbow);
        ci::gl::drawLine(r_elbow, r_hand);
        
        //LEFT ARM
        ci::gl::drawLine(l_shoulder, l_elbow);
        ci::gl::drawLine(l_elbow, l_hand);
        
        //RIGHT LEG
        ci::gl::drawLine(r_hip, r_knee);
        ci::gl::drawLine(r_knee, r_foot);
        
        //LEFT LEG
        ci::gl::drawLine(l_hip, l_knee);
        ci::gl::drawLine(l_knee, l_foot);
}
    
void Skeleton::renderPhysics(bool originOn)
{
    for (auto &j : physicsJoints){
        j.display();
    }
    
    for (auto &b : physicsBones){
        b.display();
    }

}



void Skeleton::update(const std::vector<glm::vec3>& pos){
    
    boneJoints = pos;
    
    head = boneJoints[0];
    r_shoulder = boneJoints[6];
    r_elbow = boneJoints[8];
    r_hand = boneJoints[9];
    l_shoulder = boneJoints[13];
    l_elbow = boneJoints[16];
    l_hand = boneJoints[17];
    spine_top = boneJoints[21];
    spine_base = boneJoints[27];
    r_hip = boneJoints[28];
    r_knee = boneJoints[34];
    r_foot = boneJoints[35];
    l_hip = boneJoints[29];
    l_knee = boneJoints[43];
    l_foot = boneJoints[44];
    
    physicsJoints[0].update1( head );
    physicsJoints[1].update1( spine_top);
    physicsJoints[2].update1( l_shoulder );
    physicsJoints[3].update1( l_elbow );
    physicsJoints[4].update1( l_hand );
    physicsJoints[5].update1( spine_base);
    physicsJoints[6].update1( l_hip );
    physicsJoints[7].update1( l_knee );
    physicsJoints[8].update1( l_foot );
    physicsJoints[9].update1( r_hip );
    physicsJoints[10].update1( r_knee );
    physicsJoints[11].update1( r_foot );
    physicsJoints[12].update1( r_shoulder );
    physicsJoints[13].update1( r_elbow );
    physicsJoints[14].update1( r_hand );
    
    for (auto &b : physicsBones){
        b.update();
    }
}


void Skeleton::pushone(glm::vec3 f){
    
    physicsJoints[1].applyForce( f );
}

void Skeleton::drawRibbon()
{
    
}

