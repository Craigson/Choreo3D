#include "Joint.h"

using namespace ci;

Joint::Joint(){
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
}


Joint::Joint(glm::vec3 loc){
    
    location = loc;
    
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
    
    isDataDriven = true;
}

Joint::Joint(glm::vec3 loc, std::string n){
    name =n;
    location = loc;
    
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
    
    isDataDriven = true;
}


void Joint::display(){
    gl::color(0.68, 0.68, 0.68);
    gl::drawColorCube(location, vec3(10*m,10*m,10*m));
}

void Joint::update(){
    velocity+=acceleration;
    velocity*=damp*0.1;
    //location+=velocity;
    
    offsetLoc += velocity;
    location+=offsetLoc;
  
    acceleration*=0;
}


void Joint::applyForce(ci::vec3 f){
    vec3 force =f;
    force/=m;
    acceleration+=force;
}


void Joint::update1( ci::vec3 newloc){
    if( distance(location, newloc)<1000 && distance(location, newloc)>-1000)
        location = newloc;
    
    velocity+=acceleration;
    velocity*=damp;
    //location+=velocity;
    
    offsetLoc+=velocity;
    acceleration*=0;
}


/*
void Joint::click(ci::vec3 po){
    
    float d = distance(po, location);
    if (d < m) {   //if less than the radius
        drag = true;
        cinder::app::console()<<"set drag true"<<drag<<std::endl;
        dragOffset.x = location.x-po.x;
        dragOffset.y = location.y-po.y;
        dragOffset.z = location.z-po.z;
    }
}


void Joint::dragged(ci::vec3 po){
    if(drag){
        location = po+ dragOffset;
    }
} 
 */










