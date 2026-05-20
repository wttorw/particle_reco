#ifndef ParticleLifetime_h
#define ParticleLifetime_h

#include "../util/include/ActiveObserver.h"

#include "../AnalysisFramework/AnalysisSteering.h"



#include <vector>
#include <string>

class AnalysisInfo;
class Event;
class LifetimeFit;
class TH1F;

class ParticleLifetime : public AnalysisSteering,
					 public ActiveObserver<Event>{

	public:
	ParticleLifetime( const AnalysisInfo* info );
	// deleted copy constructor and assignment to prevent unadvertent copy
  	ParticleLifetime           ( const ParticleLifetime& x ) = delete;
 	ParticleLifetime& operator=( const ParticleLifetime& x ) = delete;

	~ParticleLifetime() override;

	
	// function to be called at execution start
 	void beginJob() override;
 	// function to be called at execution end
 	void endJob() override;
 	// function to be called for each event
  	void update( const Event& ev ) override;
	
 	private:

	// Particle curve information
	struct Particle{
		std::string name; // element name
		LifetimeFit *pMean; // statistic object
		float timeMin;  //decay proper time
		float timeMax;
		TH1F *hMean;	 // graph	
	};
		
 	// set of point for different particles
 	std::vector<Particle*> pList;

	void readFile( const std::string& name );
	
 	void pCreate( const std::string& name, float min, float max , double t_min , double t_max, double scan_min, double scan_max, double scan_step  );
};

#endif

