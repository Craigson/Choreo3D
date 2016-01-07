#include "Bone.h"

using namespace:: ci;

Bone::Bone(){}

Bone::Bone(Joint *_a, Joint *_b, float l){
    a=_a;
    b=_b;
    len = l;
}

void Bone::display(){
    gl::color(0.68, 0.68, 0.68);
    gl::drawLine(a->location, b->location);
}


void Bone::update(){
    ci::vec3 force = a->location - b->location;
    float dis = distance(force, glm::vec3(0,0,0));
    float stretch = dis - len;
    
    stretch = std::abs(stretch)>20 ? stretch-20*(std::abs(stretch)/stretch) : 0;
    
    force = normalize(force);
    force *= -1*k*stretch;
    
    a->applyForce(force);
    force*=-0.1;
    b->applyForce(force);
    
    a->update();
    b->update();
}




