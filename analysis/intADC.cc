#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "TBevt.h"
#include "TButility.h"

#include "functions.cc"

#include "TStyle.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

// Run the script by
// ./TBanalysis_ex <run number> <max entry>
// For example, ./TBanalysis_ex 4234 -1
// <max entry> == -1 means run all the entries
// This example uses external pedestal

int main(int argc, char** argv) {
    gStyle->SetOptFit(1);

    int runNum = std::stoi(argv[1]);
    int maxEntry = std::stoi(argv[2]);

    // Load mapping, pedestal info is not available for now, will be updated
    auto map = getModuleConfigMap();

    std::vector<TBcid> CID_C_counter = {TBcid( map.at("C1").at(0), map.at("C1").at(1)),
                                        TBcid( map.at("C1").at(0), map.at("C2").at(1))};

    std::vector<TBcid> CID_3D_C = { TBcid( map.at("mid-Ceren").at(0),  map.at("mid-Ceren").at(1) ),
                                    TBcid( map.at("W1-Ceren").at(0),   map.at("W1-Ceren").at(1)  ),
                                    TBcid( map.at("W2-Ceren").at(0),   map.at("W2-Ceren").at(1)  ),
                                    TBcid( map.at("W3-Ceren").at(0),   map.at("W3-Ceren").at(1)  )};

    std::vector<TBcid> CID_3D_S = { TBcid( map.at("mid-Scint").at(0),  map.at("mid-Scint").at(1) ),
                                    TBcid( map.at("W1-Scint").at(0),   map.at("W1-Scint").at(1)  ),
                                    TBcid( map.at("W2-Scint").at(0),   map.at("W2-Scint").at(1)  ),
                                    TBcid( map.at("W3-Scint").at(0),   map.at("W3-Scint").at(1)  )};

    std::vector<TBcid> CID_HeatSink_C= {TBcid( map.at("HW-Ceren").at(0),  map.at("HW-Ceren").at(1) ),
                                        TBcid( map.at("H1-Ceren").at(0),  map.at("H1-Ceren").at(1) ),
                                        TBcid( map.at("H2-Ceren").at(0),  map.at("H2-Ceren").at(1) ),
                                        TBcid( map.at("H3-Ceren").at(0),  map.at("H3-Ceren").at(1) )};

    std::vector<TBcid> CID_HeatSink_S= {TBcid( map.at("HW-Scint").at(0),  map.at("HW-Scint").at(1) ),
                                        TBcid( map.at("H1-Scint").at(0),  map.at("H1-Scint").at(1) ),
                                        TBcid( map.at("H2-Scint").at(0),  map.at("H2-Scint").at(1) ),
                                        TBcid( map.at("H3-Scint").at(0),  map.at("H3-Scint").at(1) )};

    std::vector<TBcid> CID_Lego_C= {TBcid( map.at("L1-Ceren").at(0),  map.at("L1-Ceren").at(1) ),
                                    TBcid( map.at("L2-Ceren").at(0),  map.at("L2-Ceren").at(1) ),
                                    TBcid( map.at("L3-Ceren").at(0),  map.at("L3-Ceren").at(1) ),
                                    TBcid( map.at("L4-Ceren").at(0),  map.at("L4-Ceren").at(1) )};

    std::vector<TBcid> CID_Lego_S= {TBcid( map.at("L1-Scint").at(0),  map.at("L1-Scint").at(1) ),
                                    TBcid( map.at("L2-Scint").at(0),  map.at("L2-Scint").at(1) ),
                                    TBcid( map.at("L3-Scint").at(0),  map.at("L3-Scint").at(1) ),
                                    TBcid( map.at("L4-Scint").at(0),  map.at("L4-Scint").at(1) )};

    // Load pedestal
    TH2F* pedMap = (TH2F*) (TFile::Open(("/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2023_DRC_TB_analysis/ped/Run_" + std::to_string(runNum) +"_pedestal.root").c_str()) )->Get("pedestal"); // By using pedestal map directly

    // Preparing histograms for ADC
    TH1F* Hist_C_counter_1_PeakADC = new TH1F("C_counter_1_PeakADC", "C_counter_1_PeakADC;PeakADC;Evt", 4096, 0, 4096);
    TH1F* Hist_C_counter_2_PeakADC = new TH1F("C_counter_2_PeakADC", "C_counter_2_PeakADC;PeakADC;Evt", 4096, 0, 4096);

    TH1F* Hist_3D_mid_C_IntADC = new TH1F("MCP_sum_C_IntADC", "MCP_sum_C_IntADC;IntADC;Evt", 1000, -5000, 200000);
    TH1F* Hist_3D_W1_C_IntADC  = new TH1F("3D_W1_C_IntADC",   "3D_W1_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W2_C_IntADC  = new TH1F("3D_W2_C_IntADC",   "3D_W2_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W3_C_IntADC  = new TH1F("3D_W3_C_IntADC",   "3D_W3_C_IntADC;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_3D_mid_S_IntADC = new TH1F("MCP_sum_S_IntADC", "MCP_sum_S_IntADC;IntADC;Evt", 1000, -5000, 200000);
    TH1F* Hist_3D_W1_S_IntADC  = new TH1F("3D_W1_S_IntADC",   "3D_W1_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W2_S_IntADC  = new TH1F("3D_W2_S_IntADC",   "3D_W2_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W3_S_IntADC  = new TH1F("3D_W3_S_IntADC",   "3D_W3_S_IntADC;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_HS_W_C_IntADC  = new TH1F("HS_W_C_IntADC",   "HS_W_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_1_C_IntADC  = new TH1F("HS_1_C_IntADC",   "HS_1_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_2_C_IntADC  = new TH1F("HS_2_C_IntADC",   "HS_2_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_3_C_IntADC  = new TH1F("HS_3_C_IntADC",   "HS_3_C_IntADC;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_HS_W_S_IntADC  = new TH1F("HS_W_S_IntADC",   "HS_W_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_1_S_IntADC  = new TH1F("HS_1_S_IntADC",   "HS_1_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_2_S_IntADC  = new TH1F("HS_2_S_IntADC",   "HS_2_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_3_S_IntADC  = new TH1F("HS_3_S_IntADC",   "HS_3_S_IntADC;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_L_1_C_IntADC  = new TH1F("L_1_C_IntADC",   "L_1_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_2_C_IntADC  = new TH1F("L_2_C_IntADC",   "L_2_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_3_C_IntADC  = new TH1F("L_3_C_IntADC",   "L_3_C_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_4_C_IntADC  = new TH1F("L_4_C_IntADC",   "L_4_C_IntADC;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_L_1_S_IntADC  = new TH1F("L_1_S_IntADC",   "L_1_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_2_S_IntADC  = new TH1F("L_2_S_IntADC",   "L_2_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_3_S_IntADC  = new TH1F("L_3_S_IntADC",   "L_3_S_IntADC;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_4_S_IntADC  = new TH1F("L_4_S_IntADC",   "L_4_S_IntADC;IntADC;Evt",   1000, -5000, 200000);

    // After cut
    TH1F* Hist_C_counter_1_PeakADC_cut = new TH1F("C_counter_1_PeakADC_cut", "C_counter_1_PeakADC_cut;PeakADC;Evt", 4096, 0, 4096);
    TH1F* Hist_C_counter_2_PeakADC_cut = new TH1F("C_counter_2_PeakADC_cut", "C_counter_2_PeakADC_cut;PeakADC;Evt", 4096, 0, 4096);

    TH1F* Hist_3D_mid_C_IntADC_cut = new TH1F("MCP_sum_C_IntADC_cut", "MCP_sum_C_IntADC_cut;IntADC;Evt", 1000, -5000, 200000);
    TH1F* Hist_3D_W1_C_IntADC_cut  = new TH1F("3D_W1_C_IntADC_cut",   "3D_W1_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W2_C_IntADC_cut  = new TH1F("3D_W2_C_IntADC_cut",   "3D_W2_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W3_C_IntADC_cut  = new TH1F("3D_W3_C_IntADC_cut",   "3D_W3_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_3D_mid_S_IntADC_cut = new TH1F("MCP_sum_S_IntADC_cut", "MCP_sum_S_IntADC_cut;IntADC;Evt", 1000, -5000, 200000);
    TH1F* Hist_3D_W1_S_IntADC_cut  = new TH1F("3D_W1_S_IntADC_cut",   "3D_W1_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W2_S_IntADC_cut  = new TH1F("3D_W2_S_IntADC_cut",   "3D_W2_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_3D_W3_S_IntADC_cut  = new TH1F("3D_W3_S_IntADC_cut",   "3D_W3_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_HS_W_C_IntADC_cut  = new TH1F("HS_W_C_IntADC_cut",   "HS_W_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_1_C_IntADC_cut  = new TH1F("HS_1_C_IntADC_cut",   "HS_1_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_2_C_IntADC_cut  = new TH1F("HS_2_C_IntADC_cut",   "HS_2_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_3_C_IntADC_cut  = new TH1F("HS_3_C_IntADC_cut",   "HS_3_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_HS_W_S_IntADC_cut  = new TH1F("HS_W_S_IntADC_cut",   "HS_W_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_1_S_IntADC_cut  = new TH1F("HS_1_S_IntADC_cut",   "HS_1_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_2_S_IntADC_cut  = new TH1F("HS_2_S_IntADC_cut",   "HS_2_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_HS_3_S_IntADC_cut  = new TH1F("HS_3_S_IntADC_cut",   "HS_3_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_L_1_C_IntADC_cut  = new TH1F("L_1_C_IntADC_cut",   "L_1_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_2_C_IntADC_cut  = new TH1F("L_2_C_IntADC_cut",   "L_2_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_3_C_IntADC_cut  = new TH1F("L_3_C_IntADC_cut",   "L_3_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_4_C_IntADC_cut  = new TH1F("L_4_C_IntADC_cut",   "L_4_C_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);

    TH1F* Hist_L_1_S_IntADC_cut  = new TH1F("L_1_S_IntADC_cut",   "L_1_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_2_S_IntADC_cut  = new TH1F("L_2_S_IntADC_cut",   "L_2_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_3_S_IntADC_cut  = new TH1F("L_3_S_IntADC_cut",   "L_3_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);
    TH1F* Hist_L_4_S_IntADC_cut  = new TH1F("L_4_S_IntADC_cut",   "L_4_S_IntADC_cut;IntADC;Evt",   1000, -5000, 200000);

    // Load Ntuple using TChain
    TChain* evtChain = new TChain("event");
    for (int fn = 0; fn < 50; fn++) {
        std::string fileName = "ntuple_Run_" + std::to_string(runNum) + "_Wave_" + std::to_string(fn) + ".root";
        std::string filePath = "/pnfs/knu.ac.kr/data/cms/store/user/sungwon/2023_DRC_TB_ntuple/Run_"  + std::to_string(runNum) + "/Run_" + std::to_string(runNum) + "_Wave/"+ fileName;
        if ( !access(filePath.c_str(), F_OK) ){
            std::cout << fn << " Ntuple file added to TChain : " << filePath << std::endl;
            evtChain->Add(filePath.c_str());
        }
    }
    TBevt<TBwaveform>* anEvt = new TBevt<TBwaveform>(); // Will use waveform data
    evtChain->SetBranchAddress("TBevt", &anEvt);

    // Check total # of events, limit # of events to process if maxEntry is specified
    int totalEntry = evtChain->GetEntries();
    std::cout << "Total entries : " << totalEntry << std::endl;
    if ( (maxEntry > 0) && (maxEntry < totalEntry) ) {
        totalEntry = maxEntry;
        std::cout << "Will process maximum " << std::to_string(totalEntry) << " events" << std::endl;
    }

    std::vector<float> Ped_C_counter;
    for(TBcid cid : CID_C_counter) {
        Ped_C_counter.push_back( pedMap->GetBinContent(cid.mid(), cid.channel()) );
    }

    std::vector<float> Ped_3D_C;
    for(TBcid cid : CID_3D_C) {
        Ped_3D_C.push_back( pedMap->GetBinContent(cid.mid(), cid.channel()) );
    }
    std::vector<float> Ped_3D_S;
    for(TBcid cid : CID_3D_S) {
        Ped_3D_S.push_back( pedMap->GetBinContent(cid.mid(), cid.channel()) );
    }

    std::vector<float> Ped_HeatSink_C;
    for(TBcid cid : CID_HeatSink_C) {
        Ped_HeatSink_C.push_back( pedMap->GetBinContent(cid.mid(), cid.channel()) );
    }
    std::vector<float> Ped_HeatSink_S;
    for(TBcid cid : CID_HeatSink_S) {
        Ped_HeatSink_S.push_back( pedMap->GetBinContent(cid.mid(), cid.channel()) );
    }

    std::vector<float> Ped_Lego_C;
    for(TBcid cid : CID_Lego_C) {
        Ped_Lego_C.push_back( pedMap->GetBinContent(cid.mid(), cid.channel()) );
    }
    std::vector<float> Ped_Lego_S;
    for(TBcid cid : CID_Lego_S) {
        Ped_Lego_S.push_back( pedMap->GetBinContent(cid.mid(), cid.channel()) );
    }

    // C counter cut : 50 Peak ADC
    float Cut_C_counter_1 = 100.;
    float Cut_C_counter_2 = 80.;
    // Evt Loop start
    for (int iEvt = 0; iEvt < totalEntry; iEvt++) {
        printProgress(iEvt + 1, totalEntry); // Print progress bar
        evtChain->GetEntry(iEvt); // Get event

        // TBwaveform
        std::vector<TBwaveform> Data_C_counter;
        for(TBcid cid : CID_C_counter) {
            Data_C_counter.push_back(anEvt->GetData(cid));
        }
        
        std::vector<TBwaveform> Data_3D_C;
        for(TBcid cid : CID_3D_C) {
            Data_3D_C.push_back(anEvt->GetData(cid));
        }
        std::vector<TBwaveform> Data_3D_S;
        for(TBcid cid : CID_3D_S) {
            Data_3D_S.push_back(anEvt->GetData(cid));
        }

        std::vector<TBwaveform> Data_HeatSink_C;
        for(TBcid cid : CID_HeatSink_C) {
            Data_HeatSink_C.push_back(anEvt->GetData(cid));
        }
        std::vector<TBwaveform> Data_HeatSink_S;
        for(TBcid cid : CID_HeatSink_S) {
            Data_HeatSink_S.push_back(anEvt->GetData(cid));
        }

        std::vector<TBwaveform> Data_Lego_C;
        for(TBcid cid : CID_Lego_C) {
            Data_Lego_C.push_back(anEvt->GetData(cid));
        }
        std::vector<TBwaveform> Data_Lego_S;
        for(TBcid cid : CID_Lego_S) {
            Data_Lego_S.push_back(anEvt->GetData(cid));
        }

        // Waveform vector
        std::vector< std::vector<short> > Wave_C_counter;
        for(TBwaveform data : Data_C_counter) {
            Wave_C_counter.push_back(data.waveform());
        }

        std::vector< std::vector<short> > Wave_3D_C;
        for(TBwaveform data : Data_3D_C) {
            Wave_3D_C.push_back(data.waveform());
        }
        std::vector< std::vector<short> > Wave_3D_S;
        for(TBwaveform data : Data_3D_S) {
            Wave_3D_S.push_back(data.waveform());
        }

        std::vector< std::vector<short> > Wave_HeatSink_C;
        for(TBwaveform data : Data_HeatSink_C) {
            Wave_HeatSink_C.push_back(data.waveform());
        }
        std::vector< std::vector<short> > Wave_HeatSink_S;
        for(TBwaveform data : Data_HeatSink_S) {
            Wave_HeatSink_S.push_back(data.waveform());
        }

        std::vector< std::vector<short> > Wave_Lego_C;
        for(TBwaveform data : Data_Lego_C) {
            Wave_Lego_C.push_back(data.waveform());
        }
        std::vector< std::vector<short> > Wave_Lego_S;
        for(TBwaveform data : Data_Lego_S) {
            Wave_Lego_S.push_back(data.waveform());
        }
        
        // Fill Histo
        float PeakADC_C_counter_1 = GetPeakWithPed(Wave_C_counter.at(0), 390, 660, Ped_C_counter.at(0));
        float PeakADC_C_counter_2 = GetPeakWithPed(Wave_C_counter.at(1), 230, 450, Ped_C_counter.at(1));
        
        // Try 01
        // float IntADC_3D_mid_C = GetIntWithPed(Wave_3D_C.at(0), 120, 300, Ped_3D_C.at(0));
        // float IntADC_3D_W1_C  = GetIntWithPed(Wave_3D_C.at(1), 120, 300, Ped_3D_C.at(1));
        // float IntADC_3D_W2_C  = GetIntWithPed(Wave_3D_C.at(2), 140, 300, Ped_3D_C.at(2));
        // float IntADC_3D_W3_C  = GetIntWithPed(Wave_3D_C.at(3), 130, 260, Ped_3D_C.at(3));

        // float IntADC_3D_mid_S = GetIntWithPed(Wave_3D_S.at(0), 140, 340, Ped_3D_S.at(0));
        // float IntADC_3D_W1_S  = GetIntWithPed(Wave_3D_S.at(1), 140, 340, Ped_3D_S.at(1));
        // float IntADC_3D_W2_S  = GetIntWithPed(Wave_3D_S.at(2), 150, 330, Ped_3D_S.at(2));
        // float IntADC_3D_W3_S  = GetIntWithPed(Wave_3D_S.at(3), 140, 310, Ped_3D_S.at(3));

        // float IntADC_HS_W_C  = GetIntWithPed(Wave_HeatSink_C.at(0), 120, 350, Ped_HeatSink_C.at(0));
        // float IntADC_HS_1_C  = GetIntWithPed(Wave_HeatSink_C.at(1), 130, 350, Ped_HeatSink_C.at(1));
        // float IntADC_HS_2_C  = GetIntWithPed(Wave_HeatSink_C.at(2), 130, 350, Ped_HeatSink_C.at(2));
        // float IntADC_HS_3_C  = GetIntWithPed(Wave_HeatSink_C.at(3), 130, 320, Ped_HeatSink_C.at(3));

        // float IntADC_HS_W_S  = GetIntWithPed(Wave_HeatSink_S.at(0), 130, 370, Ped_HeatSink_S.at(0));
        // float IntADC_HS_1_S  = GetIntWithPed(Wave_HeatSink_S.at(1), 140, 360, Ped_HeatSink_S.at(1));
        // float IntADC_HS_2_S  = GetIntWithPed(Wave_HeatSink_S.at(2), 140, 350, Ped_HeatSink_S.at(2));
        // float IntADC_HS_3_S  = GetIntWithPed(Wave_HeatSink_S.at(3), 140, 350, Ped_HeatSink_S.at(3));

        // float IntADC_L_1_C  = GetIntWithPed(Wave_Lego_C.at(0), 140, 350, Ped_Lego_C.at(0));
        // float IntADC_L_2_C  = GetIntWithPed(Wave_Lego_C.at(1), 200, 360, Ped_Lego_C.at(1));
        // float IntADC_L_3_C  = GetIntWithPed(Wave_Lego_C.at(2), 130, 330, Ped_Lego_C.at(2));
        // float IntADC_L_4_C  = GetIntWithPed(Wave_Lego_C.at(3), 140, 330, Ped_Lego_C.at(3));

        // float IntADC_L_1_S  = GetIntWithPed(Wave_Lego_S.at(0), 155, 360, Ped_Lego_S.at(0));
        // float IntADC_L_2_S  = GetIntWithPed(Wave_Lego_S.at(1), 210, 450, Ped_Lego_S.at(1));
        // float IntADC_L_3_S  = GetIntWithPed(Wave_Lego_S.at(2), 155, 330, Ped_Lego_S.at(2));
        // float IntADC_L_4_S  = GetIntWithPed(Wave_Lego_S.at(3), 150, 355, Ped_Lego_S.at(3));
        
        // Try 02
        // float IntADC_3D_mid_C = GetIntWithPed(Wave_3D_C.at(0), 140, 270, Ped_3D_C.at(0));
        // float IntADC_3D_W1_C  = GetIntWithPed(Wave_3D_C.at(1), 130, 275, Ped_3D_C.at(1));
        // float IntADC_3D_W2_C  = GetIntWithPed(Wave_3D_C.at(2), 145, 260, Ped_3D_C.at(2));
        // float IntADC_3D_W3_C  = GetIntWithPed(Wave_3D_C.at(3), 140, 250, Ped_3D_C.at(3));

        // float IntADC_3D_mid_S = GetIntWithPed(Wave_3D_S.at(0), 150, 330, Ped_3D_S.at(0));
        // float IntADC_3D_W1_S  = GetIntWithPed(Wave_3D_S.at(1), 165, 325, Ped_3D_S.at(1));
        // float IntADC_3D_W2_S  = GetIntWithPed(Wave_3D_S.at(2), 160, 305, Ped_3D_S.at(2));
        // float IntADC_3D_W3_S  = GetIntWithPed(Wave_3D_S.at(3), 150, 295, Ped_3D_S.at(3));

        // float IntADC_HS_W_C  = GetIntWithPed(Wave_HeatSink_C.at(0), 135, 280, Ped_HeatSink_C.at(0));
        // float IntADC_HS_1_C  = GetIntWithPed(Wave_HeatSink_C.at(1), 150, 325, Ped_HeatSink_C.at(1));
        // float IntADC_HS_2_C  = GetIntWithPed(Wave_HeatSink_C.at(2), 145, 325, Ped_HeatSink_C.at(2));
        // float IntADC_HS_3_C  = GetIntWithPed(Wave_HeatSink_C.at(3), 145, 300, Ped_HeatSink_C.at(3));

        // float IntADC_HS_W_S  = GetIntWithPed(Wave_HeatSink_S.at(0), 140, 335, Ped_HeatSink_S.at(0));
        // float IntADC_HS_1_S  = GetIntWithPed(Wave_HeatSink_S.at(1), 160, 330, Ped_HeatSink_S.at(1));
        // float IntADC_HS_2_S  = GetIntWithPed(Wave_HeatSink_S.at(2), 155, 325, Ped_HeatSink_S.at(2));
        // float IntADC_HS_3_S  = GetIntWithPed(Wave_HeatSink_S.at(3), 170, 330, Ped_HeatSink_S.at(3));

        // float IntADC_L_1_C  = GetIntWithPed(Wave_Lego_C.at(0), 155, 330, Ped_Lego_C.at(0));
        // float IntADC_L_2_C  = GetIntWithPed(Wave_Lego_C.at(1), 215, 370, Ped_Lego_C.at(1));
        // float IntADC_L_3_C  = GetIntWithPed(Wave_Lego_C.at(2), 145, 300, Ped_Lego_C.at(2));
        // float IntADC_L_4_C  = GetIntWithPed(Wave_Lego_C.at(3), 150, 395, Ped_Lego_C.at(3));

        // float IntADC_L_1_S  = GetIntWithPed(Wave_Lego_S.at(0), 170, 340, Ped_Lego_S.at(0));
        // float IntADC_L_2_S  = GetIntWithPed(Wave_Lego_S.at(1), 220, 405, Ped_Lego_S.at(1));
        // float IntADC_L_3_S  = GetIntWithPed(Wave_Lego_S.at(2), 155, 330, Ped_Lego_S.at(2));
        // float IntADC_L_4_S  = GetIntWithPed(Wave_Lego_S.at(3), 160, 340, Ped_Lego_S.at(3));

        // // Try 03
        // float IntADC_3D_mid_C = GetIntWithPed(Wave_3D_C.at(0), 120, 340, Ped_3D_C.at(0));
        // float IntADC_3D_W1_C  = GetIntWithPed(Wave_3D_C.at(1), 110, 340, Ped_3D_C.at(1));
        // float IntADC_3D_W2_C  = GetIntWithPed(Wave_3D_C.at(2), 120, 310, Ped_3D_C.at(2));
        // float IntADC_3D_W3_C  = GetIntWithPed(Wave_3D_C.at(3), 140, 360, Ped_3D_C.at(3));

        // float IntADC_3D_mid_S = GetIntWithPed(Wave_3D_S.at(0), 120, 420, Ped_3D_S.at(0));
        // float IntADC_3D_W1_S  = GetIntWithPed(Wave_3D_S.at(1), 140, 380, Ped_3D_S.at(1));
        // float IntADC_3D_W2_S  = GetIntWithPed(Wave_3D_S.at(2), 130, 360, Ped_3D_S.at(2));
        // float IntADC_3D_W3_S  = GetIntWithPed(Wave_3D_S.at(3), 150, 420, Ped_3D_S.at(3));

        // float IntADC_HS_W_C  = GetIntWithPed(Wave_HeatSink_C.at(0), 110, 360, Ped_HeatSink_C.at(0));
        // float IntADC_HS_1_C  = GetIntWithPed(Wave_HeatSink_C.at(1), 120, 360, Ped_HeatSink_C.at(1));
        // float IntADC_HS_2_C  = GetIntWithPed(Wave_HeatSink_C.at(2), 130, 380, Ped_HeatSink_C.at(2));
        // float IntADC_HS_3_C  = GetIntWithPed(Wave_HeatSink_C.at(3), 130, 400, Ped_HeatSink_C.at(3));

        // float IntADC_HS_W_S  = GetIntWithPed(Wave_HeatSink_S.at(0), 120, 420, Ped_HeatSink_S.at(0));
        // float IntADC_HS_1_S  = GetIntWithPed(Wave_HeatSink_S.at(1), 130, 490, Ped_HeatSink_S.at(1));
        // float IntADC_HS_2_S  = GetIntWithPed(Wave_HeatSink_S.at(2), 130, 480, Ped_HeatSink_S.at(2));
        // float IntADC_HS_3_S  = GetIntWithPed(Wave_HeatSink_S.at(3), 130, 480, Ped_HeatSink_S.at(3));

        // float IntADC_L_1_C  = GetIntWithPed(Wave_Lego_C.at(0), 140, 400, Ped_Lego_C.at(0));
        // float IntADC_L_2_C  = GetIntWithPed(Wave_Lego_C.at(1), 200, 400, Ped_Lego_C.at(1));
        // float IntADC_L_3_C  = GetIntWithPed(Wave_Lego_C.at(2), 130, 380, Ped_Lego_C.at(2));
        // float IntADC_L_4_C  = GetIntWithPed(Wave_Lego_C.at(3), 130, 480, Ped_Lego_C.at(3));

        // float IntADC_L_1_S  = GetIntWithPed(Wave_Lego_S.at(0), 145, 500, Ped_Lego_S.at(0));
        // float IntADC_L_2_S  = GetIntWithPed(Wave_Lego_S.at(1), 200, 550, Ped_Lego_S.at(1));
        // float IntADC_L_3_S  = GetIntWithPed(Wave_Lego_S.at(2), 130, 420, Ped_Lego_S.at(2));
        // float IntADC_L_4_S  = GetIntWithPed(Wave_Lego_S.at(3), 130, 480, Ped_Lego_S.at(3));

        // 2024-Feb-06th Re-Calib integral result
        float IntADC_3D_mid_C = GetIntWithPed(Wave_3D_C.at(0), 140, 270, Ped_3D_C.at(0));
        float IntADC_3D_W1_C  = GetIntWithPed(Wave_3D_C.at(1), 130, 270, Ped_3D_C.at(1));
        float IntADC_3D_W2_C  = GetIntWithPed(Wave_3D_C.at(2), 150, 260, Ped_3D_C.at(2));
        float IntADC_3D_W3_C  = GetIntWithPed(Wave_3D_C.at(3), 140, 250, Ped_3D_C.at(3));

        float IntADC_3D_mid_S = GetIntWithPed(Wave_3D_S.at(0), 140, 320, Ped_3D_S.at(0));
        float IntADC_3D_W1_S  = GetIntWithPed(Wave_3D_S.at(1), 150, 320, Ped_3D_S.at(1));
        float IntADC_3D_W2_S  = GetIntWithPed(Wave_3D_S.at(2), 160, 300, Ped_3D_S.at(2));
        float IntADC_3D_W3_S  = GetIntWithPed(Wave_3D_S.at(3), 150, 300, Ped_3D_S.at(3));

        float IntADC_HS_W_C  = GetIntWithPed(Wave_HeatSink_C.at(0), 130, 280, Ped_HeatSink_C.at(0));
        float IntADC_HS_1_C  = GetIntWithPed(Wave_HeatSink_C.at(1), 150, 300, Ped_HeatSink_C.at(1));
        float IntADC_HS_2_C  = GetIntWithPed(Wave_HeatSink_C.at(2), 145, 300, Ped_HeatSink_C.at(2));
        float IntADC_HS_3_C  = GetIntWithPed(Wave_HeatSink_C.at(3), 140, 290, Ped_HeatSink_C.at(3));

        float IntADC_HS_W_S  = GetIntWithPed(Wave_HeatSink_S.at(0), 140, 330, Ped_HeatSink_S.at(0));
        float IntADC_HS_1_S  = GetIntWithPed(Wave_HeatSink_S.at(1), 160, 340, Ped_HeatSink_S.at(1));
        float IntADC_HS_2_S  = GetIntWithPed(Wave_HeatSink_S.at(2), 155, 335, Ped_HeatSink_S.at(2));
        float IntADC_HS_3_S  = GetIntWithPed(Wave_HeatSink_S.at(3), 155, 330, Ped_HeatSink_S.at(3));

        float IntADC_L_1_C  = GetIntWithPed(Wave_Lego_C.at(0), 150, 330, Ped_Lego_C.at(0));
        float IntADC_L_2_C  = GetIntWithPed(Wave_Lego_C.at(1), 210, 350, Ped_Lego_C.at(1));
        float IntADC_L_3_C  = GetIntWithPed(Wave_Lego_C.at(2), 140, 300, Ped_Lego_C.at(2));
        float IntADC_L_4_C  = GetIntWithPed(Wave_Lego_C.at(3), 150, 290, Ped_Lego_C.at(3));

        float IntADC_L_1_S  = GetIntWithPed(Wave_Lego_S.at(0), 170, 360, Ped_Lego_S.at(0));
        float IntADC_L_2_S  = GetIntWithPed(Wave_Lego_S.at(1), 215, 410, Ped_Lego_S.at(1));
        float IntADC_L_3_S  = GetIntWithPed(Wave_Lego_S.at(2), 160, 340, Ped_Lego_S.at(2));
        float IntADC_L_4_S  = GetIntWithPed(Wave_Lego_S.at(3), 155, 340, Ped_Lego_S.at(3));

        Hist_C_counter_1_PeakADC->Fill(PeakADC_C_counter_1);
        Hist_C_counter_2_PeakADC->Fill(PeakADC_C_counter_2);

        Hist_3D_mid_C_IntADC->Fill(IntADC_3D_mid_C);
        Hist_3D_W1_C_IntADC ->Fill(IntADC_3D_W1_C);
        Hist_3D_W2_C_IntADC ->Fill(IntADC_3D_W2_C);
        Hist_3D_W3_C_IntADC ->Fill(IntADC_3D_W3_C);

        Hist_3D_mid_S_IntADC->Fill(IntADC_3D_mid_S);
        Hist_3D_W1_S_IntADC ->Fill(IntADC_3D_W1_S);
        Hist_3D_W2_S_IntADC ->Fill(IntADC_3D_W2_S);
        Hist_3D_W3_S_IntADC ->Fill(IntADC_3D_W3_S);

        Hist_HS_W_C_IntADC->Fill(IntADC_HS_W_C);
        Hist_HS_1_C_IntADC->Fill(IntADC_HS_1_C);
        Hist_HS_2_C_IntADC->Fill(IntADC_HS_2_C);
        Hist_HS_3_C_IntADC->Fill(IntADC_HS_3_C);

        Hist_HS_W_S_IntADC->Fill(IntADC_HS_W_S);
        Hist_HS_1_S_IntADC->Fill(IntADC_HS_1_S);
        Hist_HS_2_S_IntADC->Fill(IntADC_HS_2_S);
        Hist_HS_3_S_IntADC->Fill(IntADC_HS_3_S);
    
        Hist_L_1_C_IntADC->Fill(IntADC_L_1_C);
        Hist_L_2_C_IntADC->Fill(IntADC_L_2_C);
        Hist_L_3_C_IntADC->Fill(IntADC_L_3_C);
        Hist_L_4_C_IntADC->Fill(IntADC_L_4_C);

        Hist_L_1_S_IntADC->Fill(IntADC_L_1_S);
        Hist_L_2_S_IntADC->Fill(IntADC_L_2_S);
        Hist_L_3_S_IntADC->Fill(IntADC_L_3_S);
        Hist_L_4_S_IntADC->Fill(IntADC_L_4_S);

        if( (PeakADC_C_counter_1 < Cut_C_counter_1) || (PeakADC_C_counter_2 < Cut_C_counter_2) ) continue;

        Hist_C_counter_1_PeakADC_cut->Fill(PeakADC_C_counter_1);
        Hist_C_counter_2_PeakADC_cut->Fill(PeakADC_C_counter_2);

        Hist_3D_mid_C_IntADC_cut->Fill(IntADC_3D_mid_C);
        Hist_3D_W1_C_IntADC_cut ->Fill(IntADC_3D_W1_C);
        Hist_3D_W2_C_IntADC_cut ->Fill(IntADC_3D_W2_C);
        Hist_3D_W3_C_IntADC_cut ->Fill(IntADC_3D_W3_C);

        Hist_3D_mid_S_IntADC_cut->Fill(IntADC_3D_mid_S);
        Hist_3D_W1_S_IntADC_cut ->Fill(IntADC_3D_W1_S);
        Hist_3D_W2_S_IntADC_cut ->Fill(IntADC_3D_W2_S);
        Hist_3D_W3_S_IntADC_cut ->Fill(IntADC_3D_W3_S);

        Hist_HS_W_C_IntADC_cut->Fill(IntADC_HS_W_C);
        Hist_HS_1_C_IntADC_cut->Fill(IntADC_HS_1_C);
        Hist_HS_2_C_IntADC_cut->Fill(IntADC_HS_2_C);
        Hist_HS_3_C_IntADC_cut->Fill(IntADC_HS_3_C);

        Hist_HS_W_S_IntADC_cut->Fill(IntADC_HS_W_S);
        Hist_HS_1_S_IntADC_cut->Fill(IntADC_HS_1_S);
        Hist_HS_2_S_IntADC_cut->Fill(IntADC_HS_2_S);
        Hist_HS_3_S_IntADC_cut->Fill(IntADC_HS_3_S);
    
        Hist_L_1_C_IntADC_cut->Fill(IntADC_L_1_C);
        Hist_L_2_C_IntADC_cut->Fill(IntADC_L_2_C);
        Hist_L_3_C_IntADC_cut->Fill(IntADC_L_3_C);
        Hist_L_4_C_IntADC_cut->Fill(IntADC_L_4_C);

        Hist_L_1_S_IntADC_cut->Fill(IntADC_L_1_S);
        Hist_L_2_S_IntADC_cut->Fill(IntADC_L_2_S);
        Hist_L_3_S_IntADC_cut->Fill(IntADC_L_3_S);
        Hist_L_4_S_IntADC_cut->Fill(IntADC_L_4_S);
    }

    // Save the outputs
    std::string outFile = "./IntADC/IntADC_Run_" + std::to_string(runNum) + "_24Feb06ReCalib.root";
    TFile* outputRoot = new TFile(outFile.c_str(), "RECREATE");
    outputRoot->cd();

    // Write histogram before cut
    Hist_C_counter_1_PeakADC->Write();
    Hist_C_counter_2_PeakADC->Write();

    Hist_3D_mid_C_IntADC->Write();
    Hist_3D_W1_C_IntADC ->Write();
    Hist_3D_W2_C_IntADC ->Write();
    Hist_3D_W3_C_IntADC ->Write();

    Hist_3D_mid_S_IntADC->Write();
    Hist_3D_W1_S_IntADC ->Write();
    Hist_3D_W2_S_IntADC ->Write();
    Hist_3D_W3_S_IntADC ->Write();

    Hist_HS_W_C_IntADC->Write();
    Hist_HS_1_C_IntADC->Write();
    Hist_HS_2_C_IntADC->Write();
    Hist_HS_3_C_IntADC->Write();

    Hist_HS_W_S_IntADC->Write();
    Hist_HS_1_S_IntADC->Write();
    Hist_HS_2_S_IntADC->Write();
    Hist_HS_3_S_IntADC->Write();

    Hist_L_1_C_IntADC->Write();
    Hist_L_2_C_IntADC->Write();
    Hist_L_3_C_IntADC->Write();
    Hist_L_4_C_IntADC->Write();

    Hist_L_1_S_IntADC->Write();
    Hist_L_2_S_IntADC->Write();
    Hist_L_3_S_IntADC->Write();
    Hist_L_4_S_IntADC->Write();

    // Write histogram after cut
    Hist_C_counter_1_PeakADC_cut->Write();
    Hist_C_counter_2_PeakADC_cut->Write();

    Hist_3D_mid_C_IntADC_cut->Write();
    Hist_3D_W1_C_IntADC_cut->Write();
    Hist_3D_W2_C_IntADC_cut->Write();
    Hist_3D_W3_C_IntADC_cut->Write();

    Hist_3D_mid_S_IntADC_cut->Write();
    Hist_3D_W1_S_IntADC_cut->Write();
    Hist_3D_W2_S_IntADC_cut->Write();
    Hist_3D_W3_S_IntADC_cut->Write();

    Hist_HS_W_C_IntADC_cut->Write();
    Hist_HS_1_C_IntADC_cut->Write();
    Hist_HS_2_C_IntADC_cut->Write();
    Hist_HS_3_C_IntADC_cut->Write();

    Hist_HS_W_S_IntADC_cut->Write();
    Hist_HS_1_S_IntADC_cut->Write();
    Hist_HS_2_S_IntADC_cut->Write();
    Hist_HS_3_S_IntADC_cut->Write();

    Hist_L_1_C_IntADC_cut->Write();
    Hist_L_2_C_IntADC_cut->Write();
    Hist_L_3_C_IntADC_cut->Write();
    Hist_L_4_C_IntADC_cut->Write();

    Hist_L_1_S_IntADC_cut->Write();
    Hist_L_2_S_IntADC_cut->Write();
    Hist_L_3_S_IntADC_cut->Write();
    Hist_L_4_S_IntADC_cut->Write();

    outputRoot->Close();
}
