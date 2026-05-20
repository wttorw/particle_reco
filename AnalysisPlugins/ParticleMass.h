#ifndef ParticleMass_h
#define ParticleMass_h

#include "../util/include/ActiveObserver.h"

#include "../AnalysisFramework/AnalysisSteering.h"


#include <vector>
#include <string>

class AnalysisInfo;
class Event;
class MassMean;
class TH1F;

class ParticleMass : public AnalysisSteering,
					 public ActiveObserver<Event>{

	public:
	ParticleMass( const AnalysisInfo* info );
	// deleted copy constructor and assignment to prevent unadvertent copy
  	ParticleMass           ( const ParticleMass& x ) = delete;
 	ParticleMass& operator=( const ParticleMass& x ) = delete;

	~ParticleMass() override;

	
	// function to be called at execution start
 	void beginJob() override;
 	// function to be called at execution end
 	void endJob() override;
 	// function to be called for each event
  	void update( const Event& /*ev*/  ) override;
	
 	private:

	// Particle curve information
	struct Particle{
		std::string name; // element name
		MassMean *pMean; // statistic object
		TH1F *hMean;	 // graph	
	};
		
 	// set of point for different particles
 	std::vector<Particle*> pList;

 	void readFile( const std::string& name );
 	
 	void pCreate( const std::string& name, float min, float max );
};

#endif

