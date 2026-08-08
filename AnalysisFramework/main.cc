#include <iostream>
#include <sstream>

#include <dlfcn.h>

#include "SourceFactory.h"
#include "AnalysisFactory.h"
#include "AnalysisInfo.h"


#include "EventSim.h"
#include "EventReadFromFile.h"

#include "../AnalysisPlugins/EventDump.h"
#include "../AnalysisPlugins/ParticleMass.h"
#include "../AnalysisPlugins/ParticleLifetime.h"


using namespace std;





int main( int argc,char *argv[] ){

	
	if (argc < 2){
    	cerr << "Error: Invalid keyword, type \"input filename mode\" or \"sim N [seed] mode\". " << endl;
    	return -1;
    }
    
  	// store command line parameters
  	AnalysisInfo* info = new AnalysisInfo( argc, argv );

  	// create data source, build source with real data or simulation
  	EventSource* es = SourceFactory::create( info );
  	if ( !(es) ){ //if es it's nullptr return
		cerr << "Error: Invalid keyword, type \"input filename mode\" or \"sim N [seed] mode\". " << endl;
		return -1;
  	}

	// I created a private function AnalysisFactory::valid( const AnalysisInfo* info ) that check if info it's valid
	
	// create a list of analyzers
  	AnalysisFactory::create( info );

  	// initialize all analyzers
  	Dispatcher<AnalysisInfo::AnalysisStatus>::notify( AnalysisInfo::begin );

  	// loop over events
  	es->run();

  	// finalize all analyzers
  	Dispatcher<AnalysisInfo::AnalysisStatus>::notify( AnalysisInfo::end );

  	delete es;
  	delete info;
  	return 0;

}
