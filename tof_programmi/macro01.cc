#include "readWD.hh"
#include "readWD.cc"
#include "TApplication.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <math.h>

// COSA SCRIVERE SU TERMINALE PER AVVIARE IL PROGRAMMA!!
//root macro01.cc

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

float GetVoltError (vector<float> vec){
    float app = 0.0;
    float diff=0,error=0;
    for (int i=0; i<vec.size();i++){
        if(i>1){
            diff = abs((vec[i] - app)/2);
        }
        error += diff;
        app = vec[i];
    }
    error=error/vec.size();
    return error;
}

float ErrMolt(float a, float da, float b, float db)
{
    float err = sqrt(pow((a*db), 2) + pow((da*b), 2)) ; 
    return err;
}

float ErrDiv(float a, float da, float b, float db)
{
    float err = sqrt(pow((a*db/(pow(b,2))), 2) + pow((da/b), 2)) ; 
    return err;
}

void macro01()
{
    DAQFile file("data_9000_220.dat");
    DRSEvent event;

    auto h0 = new TH1F( "time CH0", "h0", 100, 0, 200);
    auto h1 = new TH1F("time CH1", "h1", 100, 0, 200);
    auto h2 = new TH1F( "volt CH0", "h2", 100, -0.5, 0.5);
    auto h3 = new TH1F("volt CH1", "h3", 100, -0.5, 0.5);
    int i = 0; // conto i non saturati
    float sum0 = 0;
    float sum_amp0 = 0;
    float sum1 = 0;
    float sum_amp1 = 0;

    int number_of_event = 0; // conta gli eventi effettivamente presi (togliamo gli impulsi che non arrivano a 0.03)
    float err_t0 = 0.0;
    float err_t1 = 0.0;
    float t0 = 0.0;
    float t1 = 0.0;

    float err_v0 = 0.0;
    float err_v1 = 0.0;

    float err_sum0 = 0.0;
    float err_sum1 = 0.0;
    float err_sum_amp0 = 0.0;
    float err_sum_amp1 = 0.0;

    int j = 0; //Conto tutti

    while (file >> event)
    {
        if (!event.GetChannel(0,0).IsSaturated() && !event.GetChannel(0, 1).IsSaturated ())
        {
            t0 = event.GetChannel(0, 0).GetTime(-0.03);
            t1 = event.GetChannel(0, 1).GetTime(-0.03);

            err_t0 = abs(GetTimeError(event.GetChannel(0, 0).GetTimes()));
            err_t1 = abs(GetTimeError(event.GetChannel(0, 1).GetTimes()));

            const pair<float, float> ped0 = event.GetChannel(0, 0).GetPedestal();
            const pair<float, float> ped1 = event.GetChannel(0, 1).GetPedestal();
            

            if(t0 < 0 || t0 > 200 || t1 < 0 || t1 > 200){ // || j == 459
                cout<< "Error in position: "<< j <<endl;
                cout << "t0 = " << t0 << " and t1 = " << t1 << endl;
            }else{
                cout<< j <<endl;
                auto v0 = event.GetChannel(0, 0).GetVolt(t0);
                auto v1 = event.GetChannel(0, 1).GetVolt(t1);

                v0-=ped0.first; //The first element is the average and the second the std deviation
                v1-=ped1.first;

                err_v0 = GetVoltError(event.GetChannel(0, 0).GetVolts());
                err_v1 = GetVoltError(event.GetChannel(0, 1).GetVolts());

                err_v0 = sqrt(pow(err_v0, 2) + pow(ped0.second, 2));
                err_v1 = sqrt(pow(err_v1, 2) + pow(ped1.second, 2));

                h0->Fill(t0);
                h1->Fill(t1);
                h2->Fill(v0);
                h3->Fill(v1);

                sum0 += t0*v0;
                err_sum0 += ErrMolt(t0, err_t0, v0, err_v0);
                sum_amp0 += v0;
                err_sum_amp0 += err_v0;

                sum1 += t1*v1;
                err_sum1 += ErrMolt(t1, err_t1, v1, err_v1);
                sum_amp1 += v1;
                err_sum_amp1 += err_v1;

                number_of_event++;
            }
            i++;
        }
        j++;
    };
    cout<<i<<endl;
    cout<<j<<endl;

    // Calcoliamo il tempo da associare ad x facendo una sorta di media pesata
    err_sum0 = err_sum0/number_of_event;
    err_sum1 = err_sum1/number_of_event;
    err_sum_amp0 = err_sum_amp0/number_of_event;
    err_sum_amp1 = err_sum_amp1/number_of_event;

    auto time0 = sum0/sum_amp0;
    auto time1 = sum1/sum_amp1;

    // errore che ci associamo
    err_t0 = ErrDiv(sum0, err_sum0, sum_amp0, err_sum_amp0);
    err_t1 = ErrDiv(sum1, err_sum1, sum_amp1, err_sum_amp1);

    cout << "Time0=" << time0 << "+-" << err_t0 << endl;
    cout << "Time1=" << time1 << "+-" << err_t1 << endl;

    TCanvas* c0 = new TCanvas("c0");

    h1->SetTitle("Signal arrival time");
    h1->SetLineColor(4);

    h0->SetLineColor(2);
    h0->SetLineStyle(2);

    h1->Draw();
    h0->Draw("SAMES");

    TCanvas* c1 = new TCanvas("c1");

    h3->SetTitle("Voltage");
    h3->SetLineColor(4);

    h2->SetLineColor(2);
    h2->SetLineStyle(2);

    h3->Draw();
    h2->Draw("SAMES");

    return;
}