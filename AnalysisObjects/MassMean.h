#ifndef MassMean_h
#define MassMean_h

class Event;

class MassMean {

public:

  // constructor
  MassMean( float min, float max ); // mass range

  // destructor
  ~MassMean();

  // deleted copy and assignment operator
  MassMean( const MassMean& ) = delete;
  MassMean& operator=( const MassMean& ) = delete;


  bool add();      // add data from a new event
  void compute() const;             // compute mean and rms
  //it's const so it can be called from getters

  int nEvents() const;          // return number of accepted events
  double mMean() const;         // return mean mass
  double mRMS() const;	        // return rms  mass

private:


  float min; // min mass
  float max; // max mass
 
  unsigned int n_events = 0; // number of accepted events
  double mass_sum = 0.; // sum of masses
  double mass_sqr = 0.; // sum of masses square

  mutable bool ready = false;// flag for lazy evaluation
  mutable double mass_mean = 0.;// mean mass
  mutable double mass_rms = 0.;// rms  mass
  //mutable it's needed to mantain the getters const
};

#endif

