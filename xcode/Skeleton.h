//
//  Skeleton.h
//  CCL
//
//  Created by Craig Pickard on 8/29/15.
//
//

#pragma once

#include <stdio.h>
#include "Joint.h"
#include "Bone.h"

class Skeleton{
public:
    
    Skeleton();
    Skeleton(const std::vector<glm::vec3>& jointPos);

    
    void update();
    void draw();

    
    std::vector<glm::vec3>boneJoints;
    
    glm::vec3 head, r_shoulder, r_elbow, r_hand, l_shoulder, l_elbow, l_hand, spine_base, spine_top, r_hip, r_knee, r_foot, l_hip, l_knee, l_foot;
    
    
    std::vector<Joint> physicsJoints;
    
    std::vector<Bone> physicsBones;
    
    void renderStatic();

    void renderPhysics(bool originOn);
    void update(const std::vector<glm::vec3>& positions);
    void drawRibbon();
    
    void pushone( glm::vec3 f);
    
};


