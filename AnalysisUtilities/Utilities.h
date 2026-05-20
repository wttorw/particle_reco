#ifndef Utilities_h
#define Utilities_h

class Utilities {
public:
	// compute energy from momentum x,y,z components and invariant mass
	static double energy( float px,float py,float pz,double M );
	// compute invariant mass from momentum x,y,z components and energy
	static double mass_inv( float px,float py,float pz,double energy );
};
#endif