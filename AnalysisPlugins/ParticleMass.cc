#include "../util/include/ActiveObserver.h"
#include "../util/include/TFileProxy.h"

#include "ParticleMass.h"

#include "../AnalysisFramework/Event.h"
#include "../AnalysisFramework/AnalysisInfo.h"
#include "../AnalysisFramework/AnalysisFactory.h"


#include "../AnalysisObjects/MassMean.h"
#include "../AnalysisObjects/ParticleReco.h"

#include "TH1F.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>


using namespace std;

// concrete factory to create an ParticleMass analyzer
class ParticleMassFactory: public AnalysisFactory::AbsFactory {
 public:
  // assign "plot" as name for this analyzer and factory
  ParticleMassFactory(): AnalysisFactory::AbsFactory( "plot" ) {}
  // create an ParticleMassFactory when builder is run
  AnalysisSteering* create( const AnalysisInfo* info ) override {
    return new ParticleMass( info );
  }
};
// create a global ParticleMassFactory, so that it is created and registered 
// before main execution start:
// when the AnalysisFactory::create function is run,
// an ParticleMassFactory will be available with name "plot".
static  ParticleMassFactory pm;//actual object that "this" point


ParticleMass::ParticleMass(const AnalysisInfo* info ):
 AnalysisSteering( info ) {
}


ParticleMass::~ParticleMass() {
    for (Particle* pParticle : pList) {
        delete pParticle->pMean;  
       	delete pParticle->hMean;  
       	delete pParticle;        
	}
	pList.clear();
}

void ParticleMass::beginJob(){
	pList.reserve(10);
	
	ParticleMass::readFile( aInfo->value("plot") );

	return;
} 


void ParticleMass::endJob(){
	
	// save current working area
	TDirectory* currentDir = gDirectory;
	//if the option "output" it's not set it's return data anyway
	string root_filename = aInfo->value("output") ==  "" ? "data" : aInfo->value("output"); //if the option "output" it's not set it's return data anyway
	TFileProxy* file = new TFileProxy(  root_filename+".root" , "UPDATE" );

	cout << "\nParticle: Mass ( GeV/c^2 )" << endl; //results description
	// loop over elements
	for ( Particle *pParticle : pList ){ 
		cout << pParticle->name  <<": "<< pParticle->pMean->mMean() << " +/- " <<  pParticle->pMean->mRMS () << endl;
		pParticle->hMean->Write();
	}

	// close file
	file->Close();
	delete file;
	// restore working area
	currentDir->cd();
	return;
}


void ParticleMass::update( const Event& /*ev*/ ){
	
  	static ParticleReco* reco_particle = ParticleReco::instance();
  	
	for ( Particle *pParticle : pList ){ 
		if( pParticle->pMean->add() ){ //calling MassMean::add() that use ParticleReco()::instance()
			pParticle->hMean->Fill( reco_particle->par_mass() );
		}
	}
 	return;
}

void ParticleMass::pCreate( const string& name, float min, float max ) {
   // create energy distribution for events with total energy in given range

  // create name for TH1F object
  string temp_name = "mass" + name;
  const char* hName =  temp_name.c_str();
  //Trying to set the number of bins dynamically
  float range = max-min;
  float wBin = 0.00005;//it's the same magnitude of the mass_rms() but it does not change dynamically
  int nBins = static_cast<int>( range / wBin );
  // create TH1F and statistic objects and store their pointers
  Particle* pc = new Particle;
  pc-> name = name;
  pc->pMean = new MassMean( min, max );
  pc->hMean = new TH1F( hName, hName, nBins, min-wBin , max+wBin );
  pList.push_back( pc );

  return;

}


void ParticleMass::readFile( const string& name ){
	ifstream* file = new ifstream(name);

	if ( !file->is_open() ) {
	    std::cerr << "Error: Unable to open file " << name << std::endl;
	    return;
	}

	string par_name;
	float min;
	float max;

	// Read the particle information
	while ( *file >> par_name >> min >> max ){
		pCreate( par_name, min , max ); 
	}
	file->close();
	delete file;
}
