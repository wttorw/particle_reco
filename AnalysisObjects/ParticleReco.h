#ifndef ParticleReco_h
#define ParticleReco_h

#include "../AnalysisFramework/Event.h"

#include "../util/include/Singleton.h"
#include "../util/include/LazyObserver.h"

class ParticleReco: public Singleton<ParticleReco>, //singleton to create one and only static istance of ParticleReco
                    public LazyObserver<Event> {

  friend class Singleton<ParticleReco>;

 public:

  enum ParticleType { K0, Lambda0, unknown };
  // recompute informations for new event
  void update(const Event &ev) override;

  // return particle type
  ParticleType par_type();
  // return particle energy
  float par_energy();
  // return particle mass
  double par_mass();

 private:

  // private constructor and destructor for singleton
  ParticleReco();
  // deleted copy constructor and assignment to prevent unadvertent copy
  ParticleReco           ( const ParticleReco& x ) = delete;
  ParticleReco& operator=( const ParticleReco& x ) = delete;

  // destructor
  ~ParticleReco() override;

  // particle type
  ParticleType type;
  // particle energy
  float total_energy;
  // particle mass
  double invariant_mass;

};

#endif

