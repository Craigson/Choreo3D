//
//  Trail.cpp
//  CCLfixed
//
//  Created by Craig Pickard on 8/31/15.
//
//

#include "Trail.h"

Trail::Trail(){
    
}

Trail::Trail(const glm::vec3& origin)
{
//    ci::gl::VertBatch trail( GL_LINE_STRIP );
//    
//    
//    ci::ColorA defaultColor( 0.9f, 0.9f, 0.9f,0.9f);
//    
//    trail.color( defaultColor );
    
    positions.push_back(origin);
    
    prevPos = origin;
    
    threshold = 10000;
}


void Trail::update(const glm::vec3& pos)
{
    if(pos.x > threshold || pos.x < threshold || pos.z > threshold || pos.z < threshold || pos.y > threshold || pos.y < threshold){
    positions.push_back(prevPos);
    } else {
    positions.push_back(pos);
    }

    prevPos = pos;
  
    trail.color( ci::ColorAf(0.9f, 0.9f, 0.9f,0.9f) );
    
    trail.vertex(pos);
    
    trailBatch = ci::gl::Batch::create (trail, ci::gl::getStockShader( ci::gl::ShaderDef().color() ) );
//
//    std::cout << positions.size() << std::endl;
}

void Trail::render(){
    {
        ci::gl::enableAlphaBlending();
        ci::ColorA moop(1.0,1.0,1.,0.4);
        ci::gl::begin(GL_LINE_STRIP);
        for (auto &p : positions)
        {
            ci::gl::vertex(p);
        }
        ci::gl::end();
    }
    
//    ci::gl::lineWidth(5);
//   trail.draw();
//    trail.clear();
//    //std::cout << "i'm drawing" << std::endl;

}