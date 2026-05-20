#include "Utilities.h"
#include "math.h"

// compute energy from momentum x,y,z components and invariant mass
double Utilities::energy( float px, float py, float pz, double M ) {
	double P = sqrt( px*px+py*py+pz*pz );
	return sqrt( P*P+M*M );
}

// compute invariant mass from momentum x,y,z components and energy
double Utilities::mass_inv( float px, float py, float pz, double energy ) {
	double P = sqrt( px*px+py*py+pz*pz );
	return sqrt( energy*energy-P*P );
}
 