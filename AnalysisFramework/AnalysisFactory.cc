#include "AnalysisFactory.h"
#include "AnalysisInfo.h"

#include <iostream>

using namespace std;

AnalysisFactory::AnalysisFactory() {
}


AnalysisFactory::~AnalysisFactory() {
}


// create all requested analysis objects
vector<AnalysisSteering*> AnalysisFactory::create( const AnalysisInfo* info ) {

	vector<AnalysisSteering*> aList;

	if ( valid( info ) ){ 
		// loop over analysis object factories
 		static map<string,AbsFactory*>* fm = factoryMap();
 		for ( const auto& element: *fm ) {
    	// create analysis object if its name is listed in the command line
    	if ( info->contains( element.first ) )
        	aList.push_back( element.second->create( info ) );//create the DerivedClass
  		}
  	}
  	return aList;  
}


// validate user input mode before the creation 
bool AnalysisFactory::valid( const AnalysisInfo* info ){
	 map<string,AbsFactory*>* fm = factoryMap();

	string no_value = "dump";// this Factory does not need a value
	auto rule = [&](){
		cerr << "The avaible mode are: ";
		for ( const auto& el : *fm ){
			cerr << el.first << ", ";
		}
		cerr << "every mode need a filename value exept for "<< no_value << "." << endl;
		cerr << "It's possible to choose the root file name with the \"output filename\" option " <<endl;
	};
	// all the mode are a key exept for "output"
	// the value can never be a key
	// if the value it's "" the the error it's trow from file->is_open()
	if ( fm->count( info->value("output") ) ){
		cerr << "Error: \"output\" have invalid value " <<  info->value("output")  << endl;	
		rule();
		return false;
	}
	bool at_least_one = false;
	for ( const auto& el : *fm ){
		if ( fm->count( info->value( el.first ) ) && el.first != no_value ){ //
		 	cerr << "Error: mode \"" << el.first << "\" need a value"  << endl;
		 	rule();
		 	return false;
		}
		if ( info->contains( el.first ) ){
			at_least_one = true;
		}
				
	}
	if ( ! at_least_one ){
		cerr << "Error: no mode provided"  << endl;
		rule();
		return false;
	}
	return true;
}



// function to add analyzer concrete factories
void AnalysisFactory::registerFactory( const string& name, AbsFactory* b ) {
  static map<string,AbsFactory*>& fm = *factoryMap();
  fm[name] = b;
  return;
}


// map to associate analyzer names with corresponding factories
map<string,AnalysisFactory::AbsFactory*>*
           AnalysisFactory::factoryMap() {
  static map<string,AbsFactory*>* fm = new map<string,AbsFactory*>;
  return fm;
}



