#include <TH1F.h>
#include <TRandom.h>
#include <math.h>
#include <ToyMc.cc>


void test(void)
{
  TH1F Distribution("Distribution","Distribution of random numbers", 1000,0,.1);
  TRandom MyRandom;

  /*  for(int i=0; i<100000; i++)
    Distribution.Fill( -log(MyRandom.Uniform()) );
  */
  
  Detector MyDetector (.9,100,50 * ns_u);
  
  double t = 0. , next_t;
  while (t <  10000)
  { 
    next_t = MyDetector.NextFakeHitTime(t);
    Distribution.Fill( next_t - t);
    t = next_t;      
  };

  Distribution.DrawCopy();
}
