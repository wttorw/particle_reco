#include "MassMean.h"
#include "ParticleReco.h"

#include "../AnalysisFramework/Event.h"

#include <cmath>
#include <iostream>


// constructor
MassMean::MassMean( float min, float max ) : min(min), max(max){
	if ( max <= min ){//So it catch max==min
		std::cerr <<  "Error: in MassMean(min,max) max must be greater than min"  << std ::endl;
		return;
	};
}
// destructor
MassMean::~MassMean() {}

// add data from a new event
bool MassMean::add() {

  static ParticleReco* reco_particle = ParticleReco::instance();
    
  bool state = false;
  // calculate the mass of the particle
  double par_mass = reco_particle->par_mass();
  
  if ( par_mass >= min && par_mass <= max ) {
  	state = true;
    // update number of events and sums
    mass_sum += par_mass;
    mass_sqr += par_mass * par_mass;
    n_events++;
  }
  ready = false; // if add() it's called a second time
  return state;
}

// compute mean and rms
void MassMean::compute() const{ 
	if ( n_events == 0 ){
	std::cerr << "Error: zero events in range ( " << min << "," << max << " ) " << std::endl;
	return;
	}
	else{
  		mass_mean = mass_sum / n_events;
  		mass_rms = sqrt( (mass_sqr / n_events) - (mass_mean * mass_mean) );
  		ready = true;
	}
	return;
}


// return number of selected events
int MassMean::nEvents() const{
if( !ready  ){ compute(); } 
return n_events; 
}

// return mean and rms
double MassMean::mMean() const{ 
if( !ready  ){ compute(); } 
return mass_mean; 
}

double MassMean::mRMS() const{ 
if( !ready  ){ compute(); } 
return mass_rms; 
}
