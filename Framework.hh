#pragma once
#include <TRandom.h>
#include "Detector.hh"
#include "Trigger.hh"
#include "units.hh"
#include <iostream>

class Framework{
public:
  TRandom Random;
  vector<Detector> Detectors;
  Trigger trigger;

  
  static Framework & get(void){
    if (frame == nullptr ){
      frame = new Framework();
    }
    return * frame;
  };
  
  
  //private:
  Framework();
  // Framework( Framework &) = delete;
  // Framework( const Framework & ) = delete;
  // Framework & operator=(const Framework & ) = delete;
  static Framework * frame;
  
};

Framework * Framework::frame = nullptr;

Framework::Framework(){
  Detectors.resize(3);

  Detectors.at(0) = Detector( "top", .9, 100, 50*ns_u);
  Detectors.at(1) = Detector( "mid", .9, 100, 50*ns_u);
  Detectors.at(2) = Detector( "bot", .9, 100, 50*ns_u);

 
  trigger.AddDetector( & Detectors[0] );
  trigger.AddDetector( & Detectors[2] );

}
