#include "../util/include/ActiveObserver.h"
#include "../util/include/TFileProxy.h"


#include "ParticleLifetime.h"

#include "../AnalysisFramework/Event.h"
#include "../AnalysisFramework/AnalysisInfo.h"
#include "../AnalysisFramework/AnalysisFactory.h"

#include "../AnalysisObjects/LifetimeFit.h"
#include "../AnalysisObjects/ParticleReco.h"
#include "../AnalysisObjects/ProperTime.h"

#include "TH1F.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>


using namespace std;

// concrete factory to create an ParticleLifetime analyzer
class ParticleLifetimeFactory: public AnalysisFactory::AbsFactory {
 public:
  // assign "lifetime" as name for this analyzer and factory
  ParticleLifetimeFactory(): AnalysisFactory::AbsFactory( "lifetime" ) {}
  // create an ParticleLifetime when builder is run
  AnalysisSteering* create( const AnalysisInfo* info ) override {
    return new ParticleLifetime( info );
  }
};
// create a global ParticleLifetimeFactory, so that it is created and registered 
// before main execution start:
// when the AnalysisFactory::create function is run,
// an ParticleLifetimeFactory will be available with name "lifetime".
static  ParticleLifetimeFactory pm;//actual object that "this" point


ParticleLifetime::ParticleLifetime(const AnalysisInfo* info ):
 AnalysisSteering( info ) {
}


ParticleLifetime::~ParticleLifetime() {
    for (Particle* pParticle : pList) {
        delete pParticle->pMean;  
       	delete pParticle->hMean;  
       	delete pParticle;        
	}
	pList.clear();
}

void ParticleLifetime::beginJob(){
	pList.reserve(10);
	ParticleLifetime::readFile( aInfo->value( "lifetime" ) );

	return;
} 

void ParticleLifetime::endJob(){

	
	// save current working area
	TDirectory* currentDir = gDirectory;


	string root_filename = aInfo->value("output") ==  "" ? "data" : aInfo->value("output");
	TFileProxy* file = new TFileProxy(  root_filename+".root" , "UPDATE" );
	

	cout << "\nParticle: Lifetime ( sec )" << endl; //results description
	// loop over elements
	for ( Particle *pParticle : pList ){
		// loop over points
		cout << pParticle->name  <<": "<< pParticle->pMean->lifeTime() << " +/- " <<  pParticle->pMean->lifeTimeError() << endl;
		pParticle->hMean->Write();
	}

	// close file
	file->Close();
	delete file;
	// restore working area
	currentDir->cd();
	return;
}

// data comes from ParticleReco singleton, not directly from ev
void ParticleLifetime::update( const Event& /*ev*/ ){
	
  	static ProperTime* reco_time = ProperTime::instance();
  	
	for ( Particle *pParticle : pList ){ 
		if( pParticle->pMean->add() ){ //calling LifetimeFit::add() 
			pParticle->hMean->Fill( reco_time->decayTime() );
		}
	}
 	return;
}

void ParticleLifetime::pCreate( const string& name, float min, float max , double t_min , double t_max, double scan_min, double scan_max, double scan_step ) {
   // create energy distribution for events with total energy in given range

  // create name for TH1F object
  string temp_name = "time" + name;
  const char* hName =  temp_name.c_str();
  //Trying to set the number of bins dynamically
  float range = t_max-t_min;
  float wBin = 10;//it's the same magnitude of the time_rms() but it does not change dynamically
  int nBins = static_cast<int>( range / wBin );
  // create TH1F and statistic objects and store their pointers
  Particle* pc = new Particle;
  pc-> name = name;
  pc->pMean = new LifetimeFit( min, max, t_min, t_max, scan_min, scan_max, scan_step );
  pc->hMean = new TH1F( hName, hName, nBins, t_min-wBin , t_max+wBin );
  pList.push_back( pc );

  return;
}

void ParticleLifetime::readFile( const string& name ){
	ifstream* file = new ifstream(name);

	if ( !file->is_open() ) {
	    std::cerr << "Error: Unable to open file " << name << std::endl;
	    return;
	}

	string par_name;
	float min;
	float max;
	double t_min;
	double t_max;
	double scan_min;
	double scan_max;
	double scan_step;

	// Read the particle information
	while ( *file >> par_name >> min >> max >> t_min >> t_max >> scan_min >> scan_max >> scan_step ){
		pCreate( par_name, min, max, t_min, t_max, scan_min, scan_max, scan_step ); 
	}
	file->close();
	delete file;
}


