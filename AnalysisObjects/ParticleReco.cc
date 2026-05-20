#include "ParticleReco.h"

#include "../AnalysisFramework/Event.h"

#include "../AnalysisUtilities/Utilities.h"
#include "../AnalysisUtilities/Constants.h"

#include <iostream>
#include <cmath>

using namespace std;


// constructor
ParticleReco::ParticleReco() {
}


// destructor
ParticleReco::~ParticleReco() {
}


// recompute tag informations for new event

//compute total energy and invariant mass for the two
// mass hypotheses for the decay products
void ParticleReco::update( const Event &ev ) {

	// set default quantities
	type = ParticleType::unknown;
	total_energy = -1.0;
	invariant_mass   = -1.0;

	// valid events only 
  	if ( ev.nParticles() != 2 ){
  		return;
  	}
  
  
  	// positive / negative track counters
   	int charge_tot = 0;
  
  	// variables or momentum sums
  	float px_sum = 0.;
  	float py_sum = 0.;
  	float pz_sum = 0.;
  	
  	// variables for energy sums, for K0 and Lambda0
    float energy_K0 = 0.;
  	float energy_L0 = 0.;
  
  
  
  	
  	
  	// loop over particles - momenta
    for( int i=0;i < ev.nParticles();i++ ) {
    	// update momentum sums
      	px_sum += ev.particle(i)->px;
      	py_sum += ev.particle(i)->py;
      	pz_sum += ev.particle(i)->pz;
      	// update charge sums
  		charge_tot += ev.particle(i)->charge;
  
  		
  		//L0->proton + pion-
  		//k0->pion+ + pion-
  		if( ev.particle(i)->charge == 1 ){
  			energy_K0 += Utilities::energy( ev.particle(i)->px, ev.particle(i)->py, ev.particle(i)->pz, Constants::massPion );
  			energy_L0 += Utilities::energy( ev.particle(i)->px, ev.particle(i)->py, ev.particle(i)->pz, Constants::massProton );
  		}
  		else if( ev.particle(i)->charge == -1){
  			energy_K0 += Utilities::energy( ev.particle(i)->px, ev.particle(i)->py, ev.particle(i)->pz, Constants::massPion );
  			energy_L0 += Utilities::energy( ev.particle(i)->px, ev.particle(i)->py, ev.particle(i)->pz, Constants::massPion );   
  		}
  	}	
  	
  	// only valid events 
    	if( charge_tot != 0 ) {
    		return;
    	}

  	
 	// invariant masses for different decay product mass hypotheses
  	double	mass_hyp_K0 = Utilities::mass_inv( px_sum, py_sum, pz_sum, energy_K0 );
   	double 	mass_hyp_L0 = Utilities::mass_inv( px_sum, py_sum, pz_sum, energy_L0 );

	// compare invariant masses with known values and set final values
  	// ( type, energy and mass )
  	if ( fabs( mass_hyp_K0-Constants::massK0 ) <= fabs( mass_hyp_L0-Constants::massLambda0 ) ){
  		invariant_mass = mass_hyp_K0;
  		total_energy = energy_K0;
  		type = ParticleType::K0;
  	}
  	else {
  		invariant_mass = mass_hyp_L0;
  		total_energy = energy_L0;
  		type = ParticleType::Lambda0;
  	}

	return;

}


// return particle type
ParticleReco::ParticleType ParticleReco::par_type() {
// check for new event and return result
	check();
	return type;	
}

// return particle energy
float ParticleReco::par_energy() {
// check for new event and return result
	check();
	return total_energy;	
}

// return particle mass
double ParticleReco::par_mass() {
// check for new event and return result
	check();
	return invariant_mass;
}
