#ifndef ProperTime_h
#define ProperTime_h

#include "../AnalysisFramework/Event.h"

#include "../util/include/Singleton.h"
#include "../util/include/LazyObserver.h"

class ProperTime: public Singleton<ProperTime>, //singleton to create one and only static istance of ProperTime
                    public LazyObserver<Event> {

  friend class Singleton<ProperTime>;

 public:

  // recompute informations for new event
  void update( const Event& /*ev*/ ) override;

  
  // return particle time
  float decayTime();


 private:

  // private constructor and destructor for singleton
  ProperTime();
  // deleted copy constructor and assignment to prevent unadvertent copy
  ProperTime           ( const ProperTime& x ) = delete;
  ProperTime& operator=( const ProperTime& x ) = delete;

  // destructor
  ~ProperTime() override;

  // particle time
  float time;
 

};

#endif

