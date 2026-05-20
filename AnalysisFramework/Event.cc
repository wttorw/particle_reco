#include "Event.h"
#include <vector>


// constructor
Event::Event( int n, float x, float y, float z ):
id(n), dx(x), dy(y), dz(z){
	par.reserve(10);
}


// destructor
Event::~Event(){
  	for (unsigned int i = 0; i < par.size(); ++i) {
    delete par[i];
  }
}
// copy and assignment costructors deleted



void Event::add( int charge, float px, float py, float pz ) {
	
  //create a new Particle
  Particle *p = new Particle;

  p->charge = charge;
  p->px = px;
  p->py = py;
  p->pz = pz;

  //add the particle to the vector
  par.push_back(p);

}


// get event id.
int Event::eventNumber() const {
	return id;
}


// get decay point coordinates
float Event::x() const {
	return dx; 
}
float Event::y() const {
	return dy; 
}
float Event::z() const {
	return dz; 
}

// get number of particles
int Event::nParticles() const {
	return par.size();
}

// get particle
const Event::Particle* Event::particle( unsigned int i ) const {
	if ( i >= par.size() ){
		return nullptr;
	}
	return par[i];
}




