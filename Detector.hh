#pragma once
#include "units.hh"
#include <string>

class Detector{

private:
  double Efficiency;
  double FakeHitRate; // cps
  double DiscriminatedSignalTimeWidth; // s
  double NextFakeHitTime; // s

public:
  string Name;
  Detector( const char * name ="", double efficiency=1., double fakeHitRate=0,
	    double discriminatedSignalTimeWidth =  50*ns_u):
    Efficiency(efficiency),
    FakeHitRate(fakeHitRate),
    DiscriminatedSignalTimeWidth(discriminatedSignalTimeWidth),
    NextFakeHitTime(0.),
    Name(name)
  {
    setEfficiency(efficiency);
    setFakeHitRate( fakeHitRate );
    setDiscriminatedSignalTimeWidth( discriminatedSignalTimeWidth );
    //UpdateNextFakeHitTime();
  };
  
  void setEfficiency( double efficiency ){
    if( efficiency < 0 || efficiency >1)
      throw ;
    Efficiency = efficiency;
  };
  
  void setFakeHitRate( double fakeHitRate){
    if( fakeHitRate < 0 )
      throw ;
    FakeHitRate = fakeHitRate;
  };

  void setDiscriminatedSignalTimeWidth( double width){
    if( width < 0)
      throw;
    DiscriminatedSignalTimeWidth = width;
  };
				      
  bool CosmicDetected( void ) const;

  double UpdateNextFakeHitTime( void );

  double GetNextFakeHitTime( void ) const {
    return NextFakeHitTime;
  }

  double GetDiscriminatorWidth(void) const{
    return DiscriminatedSignalTimeWidth;
  }
  
};
