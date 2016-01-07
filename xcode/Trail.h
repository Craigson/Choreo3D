//
//  Trail.h
//  CCLfixed
//
//  Created by Craig Pickard on 8/31/15.
//

#pragma once

#include <stdio.h>

class Trail {
public:
    Trail();
    Trail(const glm::vec3& origin);
    
    ci::gl::VertBatch trail;
    
    std::vector<glm::vec3> positions;
    
    ci::ColorA defaultColor;
    
    void update(const glm::vec3& pos);
    
    void render();
    
    ci::gl::BatchRef trailBatch;
    
    glm::vec3 prevPos;
    
    float threshold;
};
