#include "ProperTime.h"
#include "ParticleReco.h"

#include "../AnalysisFramework/Event.h"

#include "../AnalysisUtilities/Utilities.h"
#include "../AnalysisUtilities/Constants.h"


#include <iostream>
#include <cmath>

using namespace std;


// constructor
ProperTime::ProperTime() {
}


// destructor
ProperTime::~ProperTime() {
}



void ProperTime::update( const Event& ev ) {

    static ParticleReco* reco_particle = ParticleReco::instance();

	//recall recostructed particle energy and mass 
	double mass = reco_particle->par_mass();
	float energy  = reco_particle->par_energy();
	float p = sqrt( energy * energy - mass * mass );

	//take decay coordinates from event
	float x = ev.x();
	float y = ev.y();
	float z = ev.z();
	
	float distance = sqrt( x * x + y * y + z * z );

	time = ( distance * mass ) /( p * Constants::lightVelocity );

	return;

}

// return particle mass
float ProperTime::decayTime() {
// check for new event and return result
	check();
	return time;
}