#include "EventReadFromFile.h"

#include "Event.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// read data from file "name"
EventReadFromFile::EventReadFromFile( const string& name ) {
  	file = new ifstream(name);
  	
	if ( !file->is_open() ) {
	    std::cerr << "Error: Unable to open file " << name << std::endl;
	    return;
	}
}


EventReadFromFile::~EventReadFromFile() {
  file->close();
  delete file;
}


// get an event
const Event* EventReadFromFile::get() {
  return readFile();
}


// read an event
const Event* EventReadFromFile::readFile() {

  int id;
  float dx, dy, dz;
  int nParticles;
  int charge;
  float px, py, pz;

  // Read the event information
  if ( !( *file >> id >> dx >> dy >> dz >> nParticles ) ) {
    return nullptr;
  }

  // Create a new Event object using the read data
  Event *ev = new Event( id, dx, dy, dz );

  // Read the particle data and fill the event
  for ( int i = 0; i < nParticles; i++ ) {
	if ( !( *file >> charge >> px >> py >> pz ) ) {
      delete ev;
      ev = nullptr;
      return nullptr;
      }
   ev->add( charge, px, py, pz ); 
   }
   return ev;
}

