#include "LifetimeFit.h"
#include "ParticleReco.h"
#include "ProperTime.h"

#include "../AnalysisFramework/Event.h"

#include "../AnalysisUtilities/Utilities.h"
#include "../AnalysisUtilities/QuadraticFitter.h"






#include <cmath>
#include <iostream>
#include <vector>


using namespace std;

// constructor
LifetimeFit::LifetimeFit( float min, float max, double time_min, double time_max, double scan_min, double scan_max, double scan_step ):
	min(min), max(max), time_min(time_min), time_max(time_max), scan_min(scan_min), scan_max(scan_max), scan_step(scan_step){
	if ( max <= min ){//So it catch max==min
		std::cerr <<  "Error: in LifetimeFit  max must be greater than min"  << std ::endl;
		return;
	}
	if ( time_max <= time_min ){//So it catch max==min
		std::cerr <<  "Error: in LifetimeFit time_max must be greater than time_min"  << std ::endl;
		return;
	}
	if ( scan_max <= scan_min ){//So it catch max==min
		std::cerr <<  "Error: in LifetimeFit scan_max must be greater than scan_min"  << std ::endl;
		return;
	}
	if ( scan_step <= 0){
		std::cerr <<  "Error: in LifetimeFit scan_step must be positive"  << std ::endl;			
	}
	decay_times.reserve(1000);
};
// destructor
LifetimeFit::~LifetimeFit() {}

// add data from a new event
bool LifetimeFit::add() {

	//using the Dispacher instance
	static ParticleReco* reco_particle = ParticleReco::instance();
	static ProperTime* reco_time = ProperTime::instance();
    
	bool state = false;
  	// calculate the mass of the particle
  	double par_mass = reco_particle->par_mass();
  	double par_time = reco_time->decayTime();

  	//check if the particle it's in the correct time and mass range
  	if ( par_mass >= min && par_mass <= max && par_time >= time_min && par_time <= time_max ) {
  		state = true;
   		//save in the container
   		decay_times.push_back( par_time );
  	}
  	ready = false; // if add() it's called a second time
  	return state;
}




/*The mean lifetime is estimated as the value "t" that minimizes the function:
L(t) = S_i=1^N ( (t_i/t) + log(t) + log( exp(-t_min/t) - exp(-t_max/t) ) )
where t_min and t_max are the min. and max. decay times accepted in the
analysis.
*/



// compute mean and rms
void LifetimeFit::compute() const{ 

	auto L = [this]( double t ) -> double {
	    double constant_log = log( exp(-time_min/t) - exp(-time_max/t) );     //costant term
	    double sum = 0.0;
	    for( double ti : decay_times ){
	        sum += ( ti/t ) + log(t) + constant_log;
	    }
	    return sum;
	};
	
	if ( decay_times.empty() ){
		std::cerr << "Error: zero events in range ( " << time_min << "," << time_max << " ) " << std::endl;
		return;
	}

	else{
		// L=a+bt+ct^2 
		QuadraticFitter *fit = new QuadraticFitter();
		for ( double t = scan_min ; t <= scan_max ; t += scan_step ){
			fit->add( t, L(t) );
		}
		time_mean = - fit->b() / ( 2 * fit->c() );
		time_rms =  1/sqrt( 2 * fit->c() );
		
		ready = true;
  		delete fit;
	}	
	return;
}


// return mean and rms
double LifetimeFit::lifeTime() const{ 
if( !ready  ){ compute(); } 
return time_mean; 
}

double LifetimeFit::lifeTimeError() const{ 
if( !ready  ){ compute(); } 
return time_rms; 
}





