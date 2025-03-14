// COSA SCRIVERE SU ROOT PER AVVIARE IL PROGRAMMA!!
//
// .L waveform.cpp
// waveform()

#include <TLorentzVector.h>
#include <TH2.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TEfficiency.h>
#include <TLatex.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

const int numPoints = 1024;
const int numChannels = 3;

double* w_array[numChannels];  // Ora è un array di puntatori
double* t_array[numChannels];
double w_array_out[numChannels][numPoints];
double t_array_out[numChannels][numPoints];

void waveform(int n_samples, int pos) {
    int N = n_samples;
    string str_n_samples = to_string(n_samples);
    string str_pos = to_string(pos);

    string name_root = "data_" + str_n_samples + "_" + str_pos + ".root";
    const char *file_name_root = name_root.c_str();

    // Apri il file dati esistente in modalità di scrittura
    TFile *file = TFile::Open(file_name_root, "update");
    if (!file) {
        cout << "Error opening data file." << endl;
        return;
    }

    // Chiudi il tree "rec" se esiste
    TTree *tree = dynamic_cast<TTree*>(file->Get("rec"));
    if (tree) {
        file->Delete("rec;1");
    }

    // Crea un nuovo albero dove salvare le waveform non saturate
    TTree *out_tree = new TTree("not_sat", "not_sat");

    // Imposta i rami per l'albero esistente e quelli per il nuovo albero
    for (int i = 0; i < numChannels; i++) {
        string name_w = "w" + to_string(i);
        string name_t = "t" + to_string(i);

        // Alloca dinamicamente la memoria per w_array[i] e t_array[i]
        w_array[i] = new double[numPoints];
        t_array[i] = new double[numPoints];

        // Associa i rami dell'albero esistente
        tree->SetBranchAddress(name_w.c_str(), w_array[i]);
        tree->SetBranchAddress(name_t.c_str(), t_array[i]);

        // Crea nuovi rami per il nuovo albero
        out_tree->Branch((name_w + "_out").c_str(), w_array_out[i], (name_w + "_out[" + to_string(numPoints) + "]/D").c_str());
        out_tree->Branch((name_t + "_out").c_str(), t_array_out[i], (name_t + "_out[" + to_string(numPoints) + "]/D").c_str());
    }

    int countSingleMinima0 = 0, countSingleMinima1 = 0;
    vector<int> indicesSingleMinima0, indicesSingleMinima1;
    int n_samples_clear = 0;

    for (int i = 0; i < n_samples; i++) {
        tree->GetEntry(i);

        vector<double> minValues0, minValues1;
        vector<int> minPositions0, minPositions1;

        double minValue0 = w_array[0][0];
        double minValue1 = w_array[1][0];

        

        for (int j = 1; j < numPoints; ++j) {
            // Trova i minimi per entrambi i canali
            if (w_array[0][j] < minValue0) {
                minValues0.clear();
                minPositions0.clear();
                minValue0 = w_array[0][j];
            }

            if (w_array[0][j] == minValue0) {
                minValues0.push_back(minValue0);
                minPositions0.push_back(j);
            }

            if (w_array[1][j] < minValue1) {
                minValues1.clear();
                minPositions1.clear();
                minValue1 = w_array[1][j];
            }

            if (w_array[1][j] == minValue1) {
                minValues1.push_back(minValue1);
                minPositions1.push_back(j);
            }
        }

        // Condizione: C'è solo un minimo per il canale 0
        if (minValues0.size() == 1) {
            // Incrementa il contatore per il canale 0
            countSingleMinima0++;
        }

        // Condizione: C'è solo un minimo per il canale 1
        if (minValues1.size() == 1) {
            // Incrementa il contatore per il canale 1
            countSingleMinima1++;
        }

        // Condizione: C'è solo un minimo per entrambi i canali
        if (minValues0.size() ==1 && minValues1.size() == 1) {
            // Copia i dati nelle nuove variabili
            for (int j = 0; j < numPoints; j++) {
                w_array_out[0][j] = w_array[0][j];
                t_array_out[0][j] = t_array[0][j];

                w_array_out[1][j] = w_array[1][j];
                t_array_out[1][j] = t_array[1][j];

                w_array_out[2][j] = w_array[2][j];
                t_array_out[2][j] = t_array[2][j];
            }

            // Riempimento del nuovo albero
            out_tree->Fill();
            n_samples_clear++;
        }
    }

    // Sovrascrivi l'albero nel file root
    out_tree->Write("", TObject::kOverwrite);

    // Stampa delle informazioni
    cout << "Total number of charts with a single minimum for PMT1: " << countSingleMinima0 << endl;
    cout << "Total number of charts with a single minimum for PMT2: " << countSingleMinima1 << endl;
    cout << "Total number of final number of samples: " << n_samples_clear << endl;


    

    // Chiudi il file dopo aver finito
    file->Close();
}
/* 
    
    TCanvas *c1 = new TCanvas("c1"); 
    // Creazione di un oggetto TGraph 
    TGraph *graph = new TGraph(); 
 
    // Verifica se il TTree contiene dati 
    if (out_tree->GetEntries() == 0) { 
        // Nessun dato nel TTree, esce dal programma
        return; 
    } 
    // Abilita la lettura dei rami e ottieni il numero di dati 
    out_tree->SetBranchStatus("*", 1); 
    int nPoints = out_tree->GetEntries(); 
 
    // Alloca memoria per gli array di dati 
    double *xValues = new double[nPoints]; 
    double *yValues = new double[nPoints]; 
 
    // Leggi i dati dal TTree 
    out_tree->SetBranchAddress("t0_out", xValues); 
    out_tree->SetBranchAddress("w0_out", yValues); 
    out_tree->GetEntry(0);  // Leggi solo la prima entry per ora 
 
    // Riempi il TGraph 
    graph->Set(nPoints); 
    for (int i = 0; i < nPoints; i+=1024) { 
        for (int j = 0; i<1024; ++j){
            graph->SetPoint(i, xValues[i], yValues[i]); 
        }
        // Modifica dello stile di linea 
        graph->SetLineStyle(2);  // Imposta lo stile della linea 
 
        // Disegna il grafico sul canvas 
        graph->Draw("APL");  // A: Axis, P: Points, L: Line 
    } 

    // Pulisci la memoria allocata dinamicamente 
    delete[] xValues; 
    delete[] yValues; 

    */
