#ifndef LifetimeFit_h
#define LifetimeFit_h

#include <vector>

class Event;

class LifetimeFit {

public:

  // constructor
  LifetimeFit( float min, float max, double time_min, double time_max, double scan_min, double scan_max, double scan_step ); 

  // destructor
  ~LifetimeFit();

  // deleted copy and assignment operator
  LifetimeFit( const LifetimeFit& ) = delete;
  LifetimeFit& operator=( const LifetimeFit& ) = delete;


  bool add();      // add data from a new event
  void compute() const;             // compute mean and rms
  //it's const so it can be called from getters


  double lifeTime() const;         // return mean lifetime
  double lifeTimeError() const;	   // return rms lifetime

private:


  float min; // min mass
  float max; // max mass

  double time_min;
  double time_max;
  
  double scan_min;
  double scan_max;

  double scan_step;

  mutable bool ready = false;// flag for lazy evaluation
  mutable double time_mean = 0.;// mean time
  mutable double time_rms = 0.;// rms  time
  //mutable it's needed to mantain the getters const  

  std::vector<double> decay_times;//container 
 
  //lazy MassMean

};
	
#endif

