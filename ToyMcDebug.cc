#include <math.h>
#include "Detector.hh"
#include "Framework.hh"

bool
Detector::CosmicDetected(void) const {
  return Framework::get().Random.Uniform() <= Efficiency;
}

double
Detector::UpdateNextFakeHitTime( void ) // s
{
  return NextFakeHitTime += - log(Framework::get().Random.Uniform())/ FakeHitRate ;     
}

tuple<double,Trigger::triggerType>
Trigger::NextTrigger(void){  
  if( NextCosmicTriggerTime < NextAccidentalTriggerTime ){
    double t = NextCosmicTriggerTime ;
    UpdateNextCosmicTrigger();
    return {t,cosmic};
  }
  double t = NextAccidentalTriggerTime;
  UpdateNextAccidentalTrigger();
  return {t,accidental};
}


double
Trigger::UpdateNextCosmicTrigger(void){
  bool cosmicDetected(false);
  
  while (! cosmicDetected ){
    cosmicDetected = true;
    NextCosmicTriggerTime += - log(Framework::get().Random.Uniform())/CosmicRate; 
    for( unsigned long i =0; i< Detectors.size() ;i++)
      cosmicDetected = cosmicDetected && Detectors[i]->CosmicDetected();
  }
  
  return NextCosmicTriggerTime;
}

double
Trigger::UpdateNextAccidentalTrigger(void){
  // Let us assume that the two detectors are not in an accidental coincidence status
  // Let us assume that Detectors[0] fake hit is before Detectors[1] fake hit

  // Detectors[1] ticks to reach Detectors[0]
  while( Detectors[0]->UpdateNextFakeHitTime()
	 + Detectors[0]->GetDiscriminatorWidth() <
	 Detectors[1]->GetNextFakeHitTime() + MinSuperposition){
  }

  // now Detectors[0] is close before Detectors[1] or it becomes the latest one
  sort( Detectors.begin(), Detectors.end(), [] (const Detector *d1, const Detector *d2){ return d1->GetNextFakeHitTime() < d2->GetNextFakeHitTime();});

  if( Detectors[0]->GetNextFakeHitTime() <
      Detectors[1]->GetNextFakeHitTime() + Detectors[1]->GetDiscriminatorWidth() -
      MinSuperposition &&
      Detectors[0]->GetNextFakeHitTime() + Detectors[0]->GetDiscriminatorWidth() >
      Detectors[1]->GetNextFakeHitTime() + MinSuperposition
      ){

    Detectors[0]->UpdateNextFakeHitTime();
    
    NextAccidentalTriggerTime = Detectors[1]->GetNextFakeHitTime();
    sort( Detectors.begin(), Detectors.end(), [] (const Detector *d1, const Detector *d2){ return d1->GetNextFakeHitTime() < d2->GetNextFakeHitTime();});
    return NextAccidentalTriggerTime;

    
  }

  return UpdateNextAccidentalTrigger();
}





