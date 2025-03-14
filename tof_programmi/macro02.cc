#include "readWD.hh"
#include "readWD.cc"
#include "TApplication.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <math.h>

// COSA SCRIVERE SU TERMINALE PER AVVIARE IL PROGRAMMA!!
//root macro02.cc

float GetTimeError (vector<float> vec){
    float x = 0.0;
    x = 0.195;
    float diff=0,error=0;
    for (int i=0; i<vec.size();i++){
        diff=vec[i]-i*x;
        error+=diff;
    }
    
    error=error/vec.size();
    return error;
}

void macro02()
{
    DAQFile file("long_data_10000.dat");
    DRSEvent event;

    auto h0 = new TH1F( "Position", "h0", 100, -20, 200);
    auto h1 = new TH1F( "Angle", "h1", 100, -1.57, 1.57);
    
    // dati provvisori
    float v = 13.98; // è in cm/ns perchè i tempi li lasciamo in ns
    float dv = 0.07;

    float q = -20.0; // è in ns
    float dq = 0.1;


    float x_true = 0.0;
    float dx_true = 0.0;

    float origin = 10;
    float dorigin = 1;
    float y = 190;
    float dy = 1;

    float dt0 = 0.0;
    float dt1 = 0.0;

    while (file >> event)
    {
        if (!event.GetChannel(0,0).IsSaturated() && !event.GetChannel(0, 1).IsSaturated () )
        {
            float t0 = event.GetChannel(0, 0).GetTime(-0.03);
            float t1 = event.GetChannel(0, 1).GetTime(-0.03);

            float x =  v*(t0-t1-q)/2;

            x_true = x - origin; 
            dx_true = sqrt(pow((dv*(t0-t1-q)/2), 2) + pow((v*(dt0+dt1+dq)/2), 2));

            float angle = atan(x_true/y);
            float dangle = y/(pow(y, 2) + pow(x_true, 2)) * sqrt(pow(dx_true, 2) + pow((x_true*dy)/y, 2));
            
            h1->Fill(angle);
            h0->Fill(x);
        }
    }

    TCanvas* c0 = new TCanvas("c0");
    h0->Draw("COLZ0");
    h0->SetTitle("Position");

    TCanvas* c1 = new TCanvas("c1");
    h1->Draw("COLZ0");
    h1->SetTitle("Angle distribution");

    return;
}