#ifndef Event_h
#define Event_h
#include <vector>


class Event {

public:
    // nested struct for particle data
    struct Particle {
        int charge;
        float px;  // momenta coordinates
        float py;
        float pz;
    };
    
    // constructor
    Event(int n, float x, float y, float z);
    
    // destructor
    ~Event();

    // copy and assignment costructors deleted
    Event( const Event &ev ) = delete;  
    Event &operator = ( const Event &ev ) = delete;  
    
    // add a particle to the event
    void add(int charge, float px, float py, float pz);
    
    // get event data
    int eventNumber() const;
    float x() const;
    float y() const;
    float z() const;
    
    // get number of particles
    int nParticles() const;
    
    // get particle pointer (returns nullptr if out of bounds)
    const Particle* particle( unsigned int i ) const;

private:
    // event-specific information
    int id;      // event id
    float dx;    // decay point x
    float dy;    // decay point y
    float dz;    // decay point z
    
    // particles
    std::vector<Particle*> par;         // vector of particle pointers
};

#endif
