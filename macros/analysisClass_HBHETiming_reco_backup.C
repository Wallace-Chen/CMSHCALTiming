#include "analysisClass.h"
#include "HcalTupleTree.h"
#include "HFDigi.h"
#include "HBHEDigi.h"
#include "/afs/cern.ch/user/y/yuanc/jsoncpp/dist/jsoncpp.cpp"
#include "/afs/cern.ch/user/y/yuanc/jsoncpp/dist/json/json.h"
#include "/afs/cern.ch/user/y/yuanc/jsoncpp/dist/json/json-forwards.h"

#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <string>
#include <fstream>
#include <math.h>

void analysisClass::loop(){
  
  Json::Reader reader;
  Json::Value root;

  ifstream in("lumi.json", ios::binary);
  if( !in.is_open() ){
    std::cout << "Error opening file\t" << "lumi.json\n";
    return;
  }
  if( !reader.parse(in,root) )
  {
    std::cout << "Error reading file\t" << "lumi.json\n";
    return;
  }

  Json::Reader bReader;
  Json::Value  bRoot;
  ifstream bin("bx.json", ios::binary);
  if( !bin.is_open() ){
    std::cout << "Error opening file\t" << "bx.json\n";
    return;
  }
  if( !bReader.parse(bin,bRoot) )
  {
    std::cout << "Error reading file\t" << "bx.json\n";
    return;
  }

  //--------------------------------------------------------------------------------
  // Declare HCAL tree(s)
  //--------------------------------------------------------------------------------

  HcalTupleTree * tuple_tree = getTree<HcalTupleTree>("tuple_tree");
  int n_events = tuple_tree -> fChain -> GetEntries();
  std::cout << "n events = " << n_events << std::endl;
  
  //--------------------------------------------------------------------------------
  // Turn on/off branches
  //--------------------------------------------------------------------------------
  
  tuple_tree -> fChain -> SetBranchStatus("*"               , kFALSE);
  tuple_tree -> fChain -> SetBranchStatus("run"             , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("bx"             , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("event"           , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("ls"              , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiFC"                , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiIEta"              , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiIPhi"              , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiDepth"             , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiRecEnergy"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiRecTime"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiEnergy"         , kTRUE);
// Add RecHit Info for HBHE
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitEnergy"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitTime"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitIEta"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitIPhi"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitDepth"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitEnergyMethod0"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitTimeMethod0"         , kTRUE);
  
// END
 
  reader.parse(in,root);
  bReader.parse(bin,bRoot);
  //--------------------------------------------------------------------------------
  // Prepare the histograms and variables
  //--------------------------------------------------------------------------------
  std::string plot_file = "_plot.py";
  std::string FILENAME = "plot.py";
  int N_TS4_cut[4]={0,0,0,0} ;
  int N_HF_TS=0 ;
  int nHBHEDigis = 0, nHFDigis =0 ; 		//# of pulses in each event
  int IEta=0,  IPhi=0;
  int nieta=33,minieta=16,niphi=1,miniphi=1,ndepth=2,minidepth=1;
  int nmethod=3;
//  int nmethod=2;
//  int _iphi[] = {0,14,32,50}; //Input for iphis to check, the first elements is always 0 for all iphis
  int _iphi[] = {0,20,23,31,60}; //Input for iphis to check, the first elements is always 0 for all iphis
//  int _iphi[] = {0,2,7,8,10,30,50,63,70}; //Input for iphis to check, the first elements is always 0 for all iphis
//  int _iphi[] = {0,7,23,25,52,53,66}; //Input for iphis to check, the first elements is always 0 for all iphis
//  int _iphi[] = {0}; //Input for iphis to check, the first elements is always 0 for all iphis
  int _length = (sizeof(_iphi)/sizeof(*_iphi));
  int ieta_check_min=-7, ieta_check_max=-1, iphi_check_min=32, iphi_check_max=32, idepth_check_min=1, idepth_check_max=1; // channels to be checked, inclusive
  int ieta_compare1_min=-7, ieta_compare1_max=-1, iphi_compare1_min=1, iphi_compare1_max=72, idepth_compare1_min=1, idepth_compare1_max=1; // channels to be compared, inclusive
  int ieta_compare2_min=1, ieta_compare2_max=7, iphi_compare2_min=1, iphi_compare2_max=72, idepth_compare2_min=1, idepth_compare2_max=1; // channels to be checked, inclusive

  int YnBin[3] = {100,500,500};
  float YminBin[3] = {1,-50,-50};
  float YmaxBin[3] = {5,50,50};
  float energy_threshold = 30;
  float timing_threshold = -2;

  TH1F *_TS0                     = makeTH1F("TS0","TS0",121,-60,60);
  TH1F *_TS1                     = makeTH1F("TS1","TS1",121,-60,60);
  TH1F *_TS2                     = makeTH1F("TS2","TS2",121,-60,60);
  TH1F *_TS3                     = makeTH1F("TS3","TS3",121,-60,60);
  TH1F *_TS4                     = makeTH1F("TS4","TS4",121,-60,60);
  TH1F *_TS5                     = makeTH1F("TS5","TS5",121,-60,60);
  TH1F *_TS6                     = makeTH1F("TS6","TS6",121,-60,60);
  TH1F *_TS7                     = makeTH1F("TS7","TS7",121,-60,60);
    
  TH2F *HBHE_totalCharge_vs_iEta_ALL = makeTH2F("HBHE_totalCharge_vs_iEta_ALL","HBHE totalCharge vs IEta ALL Channel",61,-30.5,30.5,100,0,3000);
  TH1F *HBHE_totalCharge[nieta][_length][ndepth];
  TH1F *HBHE_RecHitTiming[nieta][_length][ndepth];
  TH1F *HBHE_PlaceHolder[nieta];//this line cannot be deleted!!!!!!!!!!!!!
  TH1F *HBHE_RecHitTiming_ALL = makeTH1F("HBHE_RecHitTiming","HBHE Distribution of RecHit Timing",YnBin[1],YminBin[1],YmaxBin[1]);
  //Note the above five lines cannot change order randomly, otherwise, there will be error: <TH2:FILL> INVALID SIGNATURE, DO NOTHING
  //Possibly its related to memory allocation, not very clear right now.
  TString name;
  for(int eta=0;eta<nieta;eta++){
    for(int phi=0;phi<_length;phi++){
      for(int depth=0;depth<ndepth;depth++){
        name = Form("_ieta%i_iphi%i_idepth%i",eta-minieta,_iphi[phi],depth+minidepth);
      //  std::cout << name << std::endl;
        HBHE_totalCharge[eta][phi][depth] = makeTH1F("HBHE_totalCharge"+name,"HBHE Distributio of Total Charge"+name,100,0,3000);
        HBHE_totalCharge[eta][phi][depth]->Sumw2();

        HBHE_RecHitTiming[eta][phi][depth] = makeTH1F("HBHE_RecHitTiming"+name,"HBHE Distribution of RecHit Timing"+name,YnBin[1],YminBin[1],YmaxBin[1]);
        HBHE_RecHitTiming[eta][phi][depth]->Sumw2();
      }
    } 
  }
// Booking histograms for the plot of Timing vs Energy
// There three types:
// 1. Charge-Averaged Timing  2. MAHI 3. Method 0
  TH2F *HBHE_Timing_vs_Energy_All[nmethod];
  TH2F *HBHE_Timing_vs_Energy[nieta][niphi][ndepth][nmethod];
  TH2F *HBHE_Timing_vs_Energy_check[nmethod];
  TH2F *HBHE_Timing_vs_Energy_compare1[nmethod];
  TH2F *HBHE_Timing_vs_Energy_compare2[nmethod];
// Booking histograms for Timing vs iEta
  TH2F *HBHE_Timing_vs_iEta_ALL[nmethod];
// Booking histograms for Timing Map
  TH3F *HBHE_TimingMap[nmethod];
  TH3F *HBHE_TimingMap_D1[nmethod];
  TH3F *HBHE_TimingMap_D2[nmethod];
// Booking histograms for the plot of Pulse Shape
  TProfile *HBHE_PulseShape_ALL   = makeTProfile("HBHE_PulseShape_ALL","HBHE: TS3>30fC, Pulse Shape All Channels",8,0,8,0,150);
//  TProfile *HBHE_PulseShape[nieta][niphi][ndepth];//note that /HBHE_PulseShapeiphi = 0 for all iphis
  TProfile *HBHE_PulseShape[nieta][_length][ndepth];//note that /HBHE_PulseShapeiphi = 0 for all iphis

  TString methodName[3] = {"HBHE Charge-Averaged Timing","HBHE DigiRec Timing","HBHE RecHit Timing"};
  TString energyName[3] = {"Digi Energy","DigiRec Energy","RecHit Energy"};
  TString methodNo[3] = {"0","1","2"};
  // Set Y axis limits
//TEST histograms are for RecHit Timing M0 vs Energy M0, somehow HBHE_Timing_vs_Energy_All[2] for M0 didn't work
//  TH2F *HBHE_TEST = makeTH2F("TEST","TEST",3000,0,3000,YnBin[1],YminBin[1],YmaxBin[1]);
//  TH2F *HBHE_TEST_D1 = makeTH2F("TEST_D1","TEST_D1",3000,0,3000,YnBin[1],YminBin[1],YmaxBin[1]);
//  TH2F *HBHE_TEST_D2 = makeTH2F("TEST_D2","TEST_D2",3000,0,3000,YnBin[1],YminBin[1],YmaxBin[1]);
//  TH2F *HBHE_TEST_Minus = makeTH2F("TEST_M","TEST_M",3000,0,3000,YnBin[1],YminBin[1],YmaxBin[1]);
//  TH2F *HBHE_TEST_Plus = makeTH2F("TEST_P","TEST_P",3000,0,3000,YnBin[1],YminBin[1],YmaxBin[1]);
//  TH2F *HBHE_SecondPeakMap_nEvt          = makeTH2F("HBHE_SecondPeakMap_nEvt"      ,"HBHE Second Peak Distribution ", 60, -29.5, 30.5, 74, 0, 74);
//  TH2F *HBHE_NormalMap_nEvt          = makeTH2F("HBHE_NormalMap_nEvt"      ,"HBHE Normal Events Distribution ", 60, -29.5, 30.5, 74, 0, 74);
  
  TH2F *HBHE_TEST[nmethod];
  TH2F *HBHE_TEST_D1[nmethod];
  TH2F *HBHE_TEST_D2[nmethod];
  TH2F *HBHE_TEST_Minus[nmethod];
  TH2F *HBHE_TEST_Plus[nmethod];
  TH2F *HBHE_D1_SecondPeakMap_nEvt[nmethod];
  TH2F *HBHE_D2_SecondPeakMap_nEvt[nmethod];
  TH2F *HBHE_D1_NormalMap_nEvt[nmethod];
  TH2F *HBHE_D2_NormalMap_nEvt[nmethod];
  TH2F *HBHE_SecondPeakMap_nEvt[nmethod];
  TH2F *HBHE_NormalMap_nEvt[nmethod];

//  TH1F *HBHE_bxDis_SecondPeak = makeTH1F("HBHE_bxDis_SecondPeak","HBHE_bxDis_SecondPeak",3564,0,3564);
//  TH1F *HBHE_bxDis_Normal = makeTH1F("HBHE_bxDis_Normal","HBHE_bxDis_Normal",3564,0,3564);
//  TH1F *HBHE_lumiDis_SecondPeak = makeTH1F("HBHE_lumiDis_SecondPeak","HBHE_lumiDis_SecondPeak",528,1,529);
//  TH1F *HBHE_lumiDis_Normal = makeTH1F("HBHE_lumiDis_Normal","HBHE_lumiDis_Normal",528,1,529);
//  TH1F *HBHE_timingDis_SecondPeak = makeTH1F("HBHE_timingDis_SecondPeak","HBHE_timingDis_SecondPeak",100,-50,50);
//  TH1F *HBHE_timingDis_Normal = makeTH1F("HBHE_timingDis_Normal","HBHE_timingDis_Normal",100,-50,50);

  for(int method=1;method<nmethod;method++){
    TString method_c=methodNo[method];
    
    HBHE_TEST[method] = makeTH2F("TEST_"+method_c,"TEST_"+method_c,3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_TEST_D1[method] = makeTH2F("TEST_D1_"+method_c,"TEST_D1_"+method_c,3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_TEST_D2[method] = makeTH2F("TEST_D2_"+method_c,"TEST_D2_"+method_c,3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_TEST_Minus[method] = makeTH2F("TEST_M_"+method_c,"TEST_M_"+method_c,3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_TEST_Plus[method] = makeTH2F("TEST_P_"+method_c,"TEST_P_"+method_c,3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_SecondPeakMap_nEvt[method] = makeTH2F("HBHE_SecondPeakMap_nEvt_"+method_c,"HBHE Second Peak Distribution_"+method_c, 60, -29.5, 30.5, 74, 0, 74);
    HBHE_NormalMap_nEvt[method] = makeTH2F("HBHE_NormalMap_nEvt_"+method_c,"HBHE Normal Events Distribution_"+method_c,60, -29.5, 30.5, 74, 0, 74);
    HBHE_D1_SecondPeakMap_nEvt[method] = makeTH2F("HBHE_D1_SecondPeakMap_nEvt_"+method_c,"HBHE Second Peak Distribution_D1_"+method_c, 60, -29.5, 30.5, 74, 0, 74);
    HBHE_D2_SecondPeakMap_nEvt[method] = makeTH2F("HBHE_D2_SecondPeakMap_nEvt_"+method_c,"HBHE Second Peak Distribution_D2_"+method_c, 60, -29.5, 30.5, 74, 0, 74);
    HBHE_D1_NormalMap_nEvt[method] = makeTH2F("HBHE_D1_NormalMap_nEvt_"+method_c,"HBHE Normal Events Distribution_D1_"+method_c,60, -29.5, 30.5, 74, 0, 74);
    HBHE_D2_NormalMap_nEvt[method] = makeTH2F("HBHE_D2_NormalMap_nEvt_"+method_c,"HBHE Normal Events Distribution_D2_"+method_c,60, -29.5, 30.5, 74, 0, 74);

    HBHE_Timing_vs_Energy_All[method] = makeTH2F("HBHE_Timing_vs_Energy_"+method_c,methodName[method]+" vs "+energyName[method]+" TS3>30 fC",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_Timing_vs_Energy_All[method]->Sumw2();
    HBHE_Timing_vs_Energy_check[method] = makeTH2F("HBHE_Timing_vs_Energy check channels "+method_c,methodName[method]+" vs "+energyName[method]+" TS3>30 fC"+" for check channels",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_Timing_vs_Energy_compare1[method] = makeTH2F("HBHE_Timing_vs_Energy compare channels 1 "+method_c,methodName[method]+" vs "+energyName[method]+" TS3>30 fC"+" for compare channels 1",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_Timing_vs_Energy_compare2[method] = makeTH2F("HBHE_Timing_vs_Energy compare channels 2 "+method_c,methodName[method]+" vs "+energyName[method]+" TS3>30 fC"+" for compare channels 2",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_Timing_vs_iEta_ALL[method] = makeTH2F("HBHE_Timing_vs_iEta_"+method_c,methodName[method]+" vs iEta",61,-30.5,30.5,YnBin[method],YminBin[method],YmaxBin[method]);
    
    HBHE_TimingMap[method]            = makeTH3F("HBHE_TimingMap_"+method_c,"HBHE Timing Map "+method_c, 60, -29.5, 30.5, 74, 0, 74, YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_TimingMap_D1[method]            = makeTH3F("HBHE_TimingMap_D1_"+method_c,"HBHE Timing Map Depth 1 "+method_c, 60, -29.5, 30.5, 74, 0, 74, YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_TimingMap_D2[method]            = makeTH3F("HBHE_TimingMap_D2_"+method_c,"HBHE Timing Map Depth 2 "+method_c, 60, -29.5, 30.5, 74, 0, 74, YnBin[method],YminBin[method],YmaxBin[method]);

  }

  for(int eta=0;eta<nieta;eta++){
    for(int phi=0;phi<_length;phi++){
      for(int depth=0;depth<ndepth;depth++){
        TString name = Form("_ieta%i_iphi%i_idepth%i_",eta-minieta,_iphi[phi],depth+minidepth);
        for(int method=1;method<nmethod;method++){          
          TString method_c=methodNo[method];
        
          HBHE_Timing_vs_Energy[eta][phi][depth][method] = makeTH2F("HBHE_Timing_vs_Energy"+name+method_c,methodName[method]+" vs "+energyName[method]+name+" TS3>30 fC",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
          HBHE_Timing_vs_Energy[eta][phi][depth][method]->Sumw2();
        }
        //name = Form("_ieta%i_idepth%i",eta-minieta,depth+minidepth);
        name = Form("_ieta%i_iphi%i_idepth%i",eta-minieta,_iphi[phi],depth+minidepth);
        HBHE_PulseShape[eta][phi][depth] = makeTProfile("HBHE_PulseShape"+name,"HBHE: TS3>30fC, Pulse Shape"+name+" TS3>30 fC",8,0,8,0,150);
        HBHE_PulseShape[eta][phi][depth]->Sumw2();
      }
    }
  }

  TH2F *HBHE_Timing_vs_Energy_M0_from_MAHI = makeTH2F("HBHE_Timing_vs_Energy_M0_from_MAHI","HBHE M0 Timing vs Energy from MAHI Second Peak",3000,0,3000,YnBin[1],YminBin[1],YmaxBin[1]);
  TH2F *HBHE_Timing_vs_Energy_MAHI_from_M0 = makeTH2F("HBHE_Timing_vs_Energy_MAHI_from_M0","HBHE MAHI Timing vs Energy from M0 Second Peak",3000,0,3000,YnBin[2],YminBin[2],YmaxBin[2]);

  TH1F *HBHE_TSRatio_D1		      = makeTH1F("HBHE_TSRatio_D1","HBHE: TS3>30fC  Depth 1", 30, 0,5);
  TH1F *HBHE_TSRatio_D2		      = makeTH1F("HBHE_TSRatio_D2","HBHE: TS3>30fC  Depth 2", 30, 0,5);

  TH1F *HBHE_TSRatio_fC1_D1	      = makeTH1F("HBHE_TSRatio_fC1_D1","HBHE: 30fC<TS3<100fC    Depth 1", 30, 0,5);
  TH1F *HBHE_TSRatio_fC2_D1	      = makeTH1F("HBHE_TSRatio_fC2_D1","HBHE: 100fC<TS3<200fC   Depth 1", 30, 0,5);
  TH1F *HBHE_TSRatio_fC3_D1	      = makeTH1F("HBHE_TSRatio_fC3_D1","HBHE: 200fC<TS3<600fC   Depth 1", 30, 0,5);
  TH1F *HBHE_TSRatio_fC4_D1	      = makeTH1F("HBHE_TSRatio_fC4_D1","HBHE: 600fC<TS3<2000fC  Depth 1", 30, 0,5);

  TH1F *HBHE_TSRatio_fC1_D2	      = makeTH1F("HBHE_TSRatio_fC1_D2","HBHE: 30fC<TS3<100fC    Depth 2", 30, 0,5);
  TH1F *HBHE_TSRatio_fC2_D2	      = makeTH1F("HBHE_TSRatio_fC2_D2","HBHE: 100fC<TS3<200fC   Depth 2", 30, 0,5);
  TH1F *HBHE_TSRatio_fC3_D2	      = makeTH1F("HBHE_TSRatio_fC3_D2","HBHE: 200fC<TS3<600fC   Depth 2", 30, 0,5);
  TH1F *HBHE_TSRatio_fC4_D2	      = makeTH1F("HBHE_TSRatio_fC4_D2","HBHE: 600fC<TS3<2000fC  Depth 2", 30, 0,5);

  TH2F *HBHE_TSRatioMap_nEvt          = makeTH2F("HBHE_TSRatioMap_nEvt"      ,"HBHE: TS3>30fC nEvt ", 60, -29.5, 30.5, 74, 0, 74);
  TH3F *HBHE_TSRatioMap_Ave       = makeTH3F("HBHE_TSRatioMap_Ave","HBHE TS4/TS3 ratio Map", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_fC1   = makeTH3F("HBHE_TSRatioMap_Ave_fC1","HBHE: 30fC<TS3<100fC TS4/TS3 ratio Map", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_fC2   = makeTH3F("HBHE_TSRatioMap_Ave_fC2","HBHE: 100fC<TS3<200fC TS4/TS3 ratio Map", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_fC3   = makeTH3F("HBHE_TSRatioMap_Ave_fC3","HBHE: 200fC<TS3<600fC TS4/TS3 ratio Map", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_fC4   = makeTH3F("HBHE_TSRatioMap_Ave_fC4","HBHE: 600fC<TS3<2000fC TS4/TS3 ratio Map", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);

  TH2F *HBHE_TSRatioMap_nEvt_D1          = makeTH2F("HBHE_TSRatioMap_nEvt_D1"      ,"HBHE: TS3>30fC Depth 1 nEvt ", 60, -29.5, 30.5, 74, 0, 74);
  TH3F *HBHE_TSRatioMap_Ave_D1    = makeTH3F("HBHE_TSRatioMap_Ave_D1","HBHE TS4/TS3 ratio Map Depth1", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D1_fC1   = makeTH3F("HBHE_TSRatioMap_Ave_D1_fC1","HBHE: 30fC<TS3<100fC TS4/TS3 ratio Map Depth1", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D1_fC2   = makeTH3F("HBHE_TSRatioMap_Ave_D1_fC2","HBHE: 100fC<TS3<200fC TS4/TS3 ratio Map Depth1", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D1_fC3   = makeTH3F("HBHE_TSRatioMap_Ave_D1_fC3","HBHE: 200fC<TS3<600fC TS4/TS3 ratio Map Depth1", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D1_fC4   = makeTH3F("HBHE_TSRatioMap_Ave_D1_fC4","HBHE: 600fC<TS3<2000fC TS4/TS3 ratio Map Depth1", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);

  TH2F *HBHE_TSRatioMap_nEvt_D2         = makeTH2F("HBHE_TSRatioMap_nEvt_D2"   ,"HBHE: TS3>30fC Depth 2 nEvt ", 60, -29.5, 30.5, 74, 0, 74);
  TH3F *HBHE_TSRatioMap_Ave_D2    = makeTH3F("HBHE_TSRatioMap_Ave_D2","HBHE TS4/TS3 ratio Map Depth2", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D2_fC1   = makeTH3F("HBHE_TSRatioMap_Ave_D2_fC1","HBHE: 30fC<TS3<100fC TS4/TS3 ratio Map Depth2", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D2_fC2   = makeTH3F("HBHE_TSRatioMap_Ave_D2_fC2","HBHE: 100fC<TS3<200fC TS4/TS3 ratio Map Depth2", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D2_fC3   = makeTH3F("HBHE_TSRatioMap_Ave_D2_fC3","HBHE: 200fC<TS3<600fC TS4/TS3 ratio Map Depth2", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);
  TH3F *HBHE_TSRatioMap_Ave_D2_fC4   = makeTH3F("HBHE_TSRatioMap_Ave_D2_fC4","HBHE: 600fC<TS3<2000fC TS4/TS3 ratio Map Depth2", 60, -29.5, 30.5, 74, 0, 74, 100, 0, 10);

  float TS[8]={0},hbhe_energy=0, hbhe_recTime=0, hbhe_recHitEnergyMethod0=0,hbhe_recHitTimeMethod0=0;
  int hbhe_recHitIeta=0,hbhe_recHitIphi=0,hbhe_recHitDepth=0;
  int   hbhe_depth=0, hbhe_ieta=0, hbhe_iphi=0,hbhe_run=0, hbhe_ls=0, hbhe_bx=0;

  int  nTS_fired[3] ={0,0,0};

  
  //--------------------------------------------------------------------------------
  // Loop
  //--------------------------------------------------------------------------------
  float HBHEALLFC=0;
  float HBHEDigiEnergy=0;
  int   eventSkip = 0;
  for (int i = 0; i < n_events; ++i){
    
    tuple_tree -> GetEntry(i);
//    if ( (i + 1) % 1000 == 0 ) std::cout << "Processing event " << i + 1 << "/" << n_events << std::endl;
    if ( (i + 1) % 100 == 0 ) std::cout << "Processing event " << i + 1 << "/" << n_events << std::endl;
    if ( (i + 1) % 500000 == 0 ) {write();std::cout << "Saved at the event  " << i + 1 << "/" << n_events << std::endl;}
// check is ls is ok
    hbhe_bx = tuple_tree -> bx;
    hbhe_ls = tuple_tree -> ls;
    hbhe_run = tuple_tree -> run;

  //--------------------------------------------------------------------------------
  // Select lumi and bx according to json file
  //--------------------------------------------------------------------------------
    bool  lsValid = false;
    if( root[std::to_string(hbhe_run)].empty() ) {eventSkip++;std::cout<<"Event skipped, cannot find the run! "<<hbhe_run<<std::endl;continue;} //skip if run number is not in the json file
    for( int i=0;i<root[std::to_string(hbhe_run)].size();i++ )
    {
      if(  hbhe_ls >= root[std::to_string(hbhe_run)][i][0].asInt() && hbhe_ls <= root[std::to_string(hbhe_run)][i][1].asInt())
      {
        lsValid = true;
        break;   
      }
    }
    if( !lsValid ) {eventSkip++;std::cout<<"Event skipped, cannot find the lumi "<<hbhe_ls<<std::endl;continue;} //skip if ls is not in the range
// check is bx is ok
//    bool  bxValid = false;
//    if( bRoot[std::to_string(hbhe_run)].empty() ) {eventSkip++;continue;} //skip if run number is not in the json file
//    for( int i=0;i<bRoot[std::to_string(hbhe_run)].size();i++ )
//    {
//      if( bRoot[std::to_string(hbhe_run)][i]==hbhe_bx )
//      {
//        bxValid = true;
//       break;   
//      }
//   }
//   if( !bxValid ) {eventSkip++;continue;} //skip if ls is not in the range

    CollectionPtr hbheDigis (new Collection(*tuple_tree, tuple_tree -> HBHEDigiIEta -> size()));
    // CollectionPtr hbheDigis (new Collection(*tuple_tree, tuple_tree -> HBHERecHitIEta -> size()));

    // nObj = total # of HBHE pulses in the current event
    nHBHEDigis = hbheDigis->GetSize();  
  //--------------------------------------------------------------------------------
    // Loop over all HBHE pulses in the current event to fill a TS Entry
  //--------------------------------------------------------------------------------
    for (int iHBHEDigi = 0; iHBHEDigi < nHBHEDigis; ++iHBHEDigi){
      HBHEDigi hbheDigi = hbheDigis -> GetConstituent<HBHEDigi>(iHBHEDigi);
      HBHEDigiEnergy = 0;
//      std::cout << "iHBHEDigi: " << iHBHEDigi << std::endl;
      for(int i=0;i<8;i++)
      {
        TS[i] = hbheDigi.fc(i) < 0 ? 0 : hbheDigi.fc(i);
      }
// Get Infos
      hbhe_depth  = hbheDigi.depth();
      hbhe_energy = hbheDigi.energy();
      hbhe_recTime= hbheDigi.recHitTime();
      hbhe_ieta =   hbheDigi.ieta();
      hbhe_iphi =   hbheDigi.iphi();
      hbhe_recHitTimeMethod0 = hbheDigi.RecHitTimeMethod0(); 
      hbhe_recHitEnergyMethod0 = hbheDigi.RecHitEnergyMethod0(); 
//      std::cout<<"RecHit " << " energy and time are: " << hbhe_energy << " " << hbhe_recTime << std::endl;
//      std::cout<<"Method0 " << " energy and time are: " << hbhe_recHitEnergyMethod0 << " " << hbhe_recHitTimeMethod0 << std::endl;
//      hbhe_recHitIeta = hbheDigi.RecHitIeta(); 
//      hbhe_recHitIphi = hbheDigi.RecHitIphi(); 
//      hbhe_recHitDepth = hbheDigi.RecHitDepth(); 

      _TS0->Fill(TS[0]);
      _TS1->Fill(TS[1]);
      _TS2->Fill(TS[2]);
      _TS3->Fill(TS[3]);
      _TS4->Fill(TS[4]);
      _TS5->Fill(TS[5]);
      _TS6->Fill(TS[6]);
      _TS7->Fill(TS[7]);

      // Fill the TS5/TS4 ratio map of HBHE, Binned by energies
//	if(hbhe_energy>5)		HBHE_Energy_D1->Fill( hbhe_energy );
  //--------------------------------------------------------------------------------
  // Start Filling Histograms
  //--------------------------------------------------------------------------------
//	 if(TS[3]>30){
	 if(TS[3]>10){
//	 if(hbhe_energy>5){
    HBHE_TSRatioMap_nEvt->Fill(hbhe_ieta,hbhe_iphi);
//    HBHEALLFC = TS0 + TS1 + TS2 + TS3 + TS4 + TS5 + TS6 + TS7;
    HBHEDigiEnergy = hbheDigi.digienergy(2) + hbheDigi.digienergy(3) + hbheDigi.digienergy(4);
    HBHEALLFC = TS[2] + TS[3] + TS[4];
//    std::cout << "Total Charge is: " << HBHEALLFC << std::endl; 
// Fill Histograms for total charge
    HBHE_TSRatioMap_Ave ->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
    HBHE_totalCharge_vs_iEta_ALL->Fill(hbhe_ieta,HBHEALLFC);
    HBHE_RecHitTiming_ALL->Fill(hbhe_recTime);
    HBHE_totalCharge[hbhe_ieta+minieta][0][hbhe_depth-minidepth]->Fill(HBHEALLFC);
    HBHE_RecHitTiming[hbhe_ieta+minieta][0][hbhe_depth-minidepth]->Fill(hbhe_recTime);
//    std::cout << "Before Filling pulse shape " << std::endl;
    for(int phi=1;phi<_length;phi++){
      if(_iphi[phi]==hbhe_iphi){
        HBHE_totalCharge[hbhe_ieta+minieta][phi][hbhe_depth-minidepth]->Fill(HBHEALLFC);
        HBHE_RecHitTiming[hbhe_ieta+minieta][phi][hbhe_depth-minidepth]->Fill(hbhe_recTime);
        for(int i=0;i<8;i++){
          HBHE_PulseShape[hbhe_ieta+minieta][phi][hbhe_depth-minidepth]->Fill(i,TS[i]);
        }
        break;
      }
    }
    
    //HBHE_TEST->Fill(hbhe_recHitEnergyMethod0,hbhe_recHitTimeMethod0);
    //if(hbhe_ieta<0) HBHE_TEST_Minus->Fill(hbhe_recHitEnergyMethod0,hbhe_recHitTimeMethod0);
    //if(hbhe_ieta>0) HBHE_TEST_Plus->Fill(hbhe_recHitEnergyMethod0,hbhe_recHitTimeMethod0);
    //if(hbhe_depth==1) HBHE_TEST_D1->Fill(hbhe_recHitEnergyMethod0,hbhe_recHitTimeMethod0);
    //if(hbhe_depth==2) HBHE_TEST_D2->Fill(hbhe_recHitEnergyMethod0,hbhe_recHitTimeMethod0);
  //--------------------------------------------------------------------------------
  // Fill Histograms for Timing vs Energy
  //--------------------------------------------------------------------------------
    for(int method=1;method<nmethod;method++){
      // skip if ieta is out of the range abs(ieta)<16 and abs(ieta)=16 && depth<=2
      if( (abs(hbhe_recHitIeta)>16) || (abs(hbhe_recHitIeta)==16 && hbhe_recHitDepth>2 ) ) continue;

      float Energy =  ( (method==1) ?  hbhe_energy : hbhe_recHitEnergyMethod0);
      float Time   = ( (method==1) ?  hbhe_recTime: hbhe_recHitTimeMethod0);

      HBHE_Timing_vs_Energy_All[method]->Fill(Energy,Time);
      HBHE_TEST[method]->Fill(Energy,Time);
      if(hbhe_ieta<0) HBHE_TEST_Minus[method]->Fill(Energy,Time);
      if(hbhe_ieta>0) HBHE_TEST_Plus[method]->Fill(Energy,Time);
      if(hbhe_depth==1) HBHE_TEST_D1[method]->Fill(Energy,Time);
      if(hbhe_depth==2) HBHE_TEST_D2[method]->Fill(Energy,Time);
//      std::cout<<"RecHit " << " energy and time are: " << hbhe_energy << " " << hbhe_recTime << std::endl;
//      std::cout<<"Method0 " << " energy and time are: " << hbhe_recHitEnergyMethod0 << " " << hbhe_recHitTimeMethod0 << std::endl;
//      std::cout<<"Method Num" << method << " energy and time are: " << Energy << " " << Time << std::endl;
      HBHE_Timing_vs_Energy[hbhe_ieta+minieta][0][hbhe_depth-minidepth][method]->Fill(Energy,Time);
      for(int phi=1;phi<_length;phi++){
        if(_iphi[phi]==hbhe_iphi){
          HBHE_Timing_vs_Energy[hbhe_ieta+minieta][phi][hbhe_depth-minidepth][method]->Fill(Energy,Time);
          break;
        }
      }
//      if(Time < timing_threshold && Energy > energy_threshold){ //fill histograms to see the distribution of second peak in eta-phi plane
//        HBHE_SecondPeakMap_nEvt->Fill(hbhe_ieta,hbhe_iphi);     
//        HBHE_bxDis_SecondPeak->Fill(hbhe_bx);
//        HBHE_lumiDis_SecondPeak->Fill(hbhe_ls);
//      }
//      else{
//        HBHE_NormalMap_nEvt->Fill(hbhe_ieta,hbhe_iphi);
//        HBHE_bxDis_Normal->Fill(hbhe_bx);
//        HBHE_lumiDis_Normal->Fill(hbhe_ls);
//      }
// Fill histograms for Bifurcation and Normal Events
      if(Energy>30){
        if(method==1){//MAHI
          if(Time < -2){
            HBHE_Timing_vs_Energy_M0_from_MAHI->Fill(hbhe_recHitEnergyMethod0,hbhe_recHitTimeMethod0); 
            if(hbhe_depth==1) HBHE_D1_SecondPeakMap_nEvt[1]->Fill(hbhe_ieta,hbhe_iphi);
            if(hbhe_depth==2) HBHE_D2_SecondPeakMap_nEvt[1]->Fill(hbhe_ieta,hbhe_iphi);
          }
          else{
            if(hbhe_depth==1) HBHE_D1_NormalMap_nEvt[1]->Fill(hbhe_ieta,hbhe_iphi);
            if(hbhe_depth==2) HBHE_D2_NormalMap_nEvt[1]->Fill(hbhe_ieta,hbhe_iphi);
          }
        }
        else if(method==2){//M0
          if(Time < (3.28325*log10(Energy)-12.5)){
            HBHE_Timing_vs_Energy_MAHI_from_M0->Fill(hbhe_energy, hbhe_recTime);
            if(hbhe_depth==1) HBHE_D1_SecondPeakMap_nEvt[2]->Fill(hbhe_ieta,hbhe_iphi);
            if(hbhe_depth==2) HBHE_D2_SecondPeakMap_nEvt[2]->Fill(hbhe_ieta,hbhe_iphi);
          }
          else{
            if(hbhe_depth==1) HBHE_D1_NormalMap_nEvt[2]->Fill(hbhe_ieta,hbhe_iphi);
            if(hbhe_depth==2) HBHE_D2_NormalMap_nEvt[2]->Fill(hbhe_ieta,hbhe_iphi);
          }
        }
      }

      HBHE_Timing_vs_iEta_ALL[method]->Fill(hbhe_ieta,Time);
      HBHE_TimingMap[method]->Fill(hbhe_ieta,hbhe_iphi,Time);
      if(hbhe_depth==1) {
        HBHE_TimingMap_D1[method]->Fill(hbhe_ieta,hbhe_iphi,Time);
//        HBHE_TEST_D1->Fill(Energy,Time);
      }
      if(hbhe_depth==2) {
        HBHE_TimingMap_D2[method]->Fill(hbhe_ieta,hbhe_iphi,Time);
//        HBHE_TEST_D2->Fill(Energy,Time);
      }
  //  Fill histogram for check channels
      if(ieta_check_min <= hbhe_ieta && hbhe_ieta <= ieta_check_max)
      {
        if(iphi_check_min <= hbhe_iphi && hbhe_iphi <= iphi_check_max)
        {
          if(idepth_check_min <= hbhe_depth && hbhe_depth <= idepth_check_max)
          {
            HBHE_Timing_vs_Energy_check[method]->Fill(Energy,Time);
          }
        }
      }
  // Fill histograns for compare channels
      if(ieta_compare1_min <= hbhe_ieta && hbhe_ieta <= ieta_compare1_max)
      {
        if(iphi_compare1_min <= hbhe_iphi && hbhe_iphi <= iphi_compare1_max)
        {
          if(idepth_compare1_min <= hbhe_depth && hbhe_depth <= idepth_compare1_max)
          {
            HBHE_Timing_vs_Energy_compare1[method]->Fill(Energy,Time);
          }
        }
      }
    
      if(ieta_compare2_min <= hbhe_ieta && hbhe_ieta <= ieta_compare2_max)
      {
        if(iphi_compare2_min <= hbhe_iphi && hbhe_iphi <= iphi_compare2_max)
        {
          if(idepth_compare2_min <= hbhe_depth && hbhe_depth <= idepth_compare2_max)
          {
            HBHE_Timing_vs_Energy_compare2[method]->Fill(Energy,Time);
          }
        }
      }

    }
//Fill histogram for pulse shape
  for(int i=0;i<8;i++)
  {
    HBHE_PulseShape_ALL->Fill(i,TS[i]);
    HBHE_PulseShape[hbhe_ieta+minieta][0][hbhe_depth-minidepth]->Fill(i,TS[i]);
  }

    if(TS[3]>30 && TS[3]<100)  {
      HBHE_TSRatioMap_Ave_fC1->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
    }
    if(TS[3]>100 && TS[3]<200)  {
      HBHE_TSRatioMap_Ave_fC2->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
    }
    if(TS[3]>200 && TS[3]<600)  {
      HBHE_TSRatioMap_Ave_fC3->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
    }
    if(TS[3]>600 && TS[3]<2000)  {
      HBHE_TSRatioMap_Ave_fC4->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
    }
	}
//General END, next for every depth
	 if(hbhe_depth==1){
//	 	if(TS[3]>30){
	 	if(TS[3]>10){
//	 	if(hbhe_energy>5){
      HBHE_TSRatioMap_Ave_D1 ->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
      HBHE_TSRatioMap_nEvt_D1->Fill(hbhe_ieta,hbhe_iphi);
	 		HBHE_TSRatio_D1 ->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>30 && TS[3]<100)  {
      HBHE_TSRatioMap_Ave_D1_fC1->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC1_D1->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>100 && TS[3]<200)  {
      HBHE_TSRatioMap_Ave_D1_fC2->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC2_D1->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>200 && TS[3]<600)  {
      HBHE_TSRatioMap_Ave_D1_fC4->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC3_D1->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>600 && TS[3]<2000)  {
      HBHE_TSRatioMap_Ave_D1_fC4->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC4_D1->Fill(TS[4]/TS[3]);
		}
	 }
	 if(hbhe_depth==2){ 
//		if(TS[3]>30){
		if(TS[3]>10){
//		if(hbhe_energy>5){
      HBHE_TSRatioMap_Ave_D2 ->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
      HBHE_TSRatioMap_nEvt_D2->Fill(hbhe_ieta,hbhe_iphi);
	 		HBHE_TSRatio_D2 ->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>30 && TS[3]<100)  {
      HBHE_TSRatioMap_Ave_D2_fC1->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC1_D2->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>100 && TS[3]<200)  {
      HBHE_TSRatioMap_Ave_D2_fC2->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC2_D2->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>200 && TS[3]<600)  {
      HBHE_TSRatioMap_Ave_D2_fC3->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC3_D2->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>600 && TS[3]<2000)  {
      HBHE_TSRatioMap_Ave_D2_fC4->Fill(hbhe_ieta,hbhe_iphi,TS[4]/TS[3]);
			HBHE_TSRatio_fC4_D2->Fill(TS[4]/TS[3]);
		}
	 }

      // Fill the histograms of HBHE TS ratios:  
    }

    }// end of event loop

  //--------------------------------------------------------------------------------
  std::cout << "Read " << n_events << " events" << std::endl;




//-----------------------------------------------------------------
// Write the histograms that are not initialized with make* Methods
//-----------------------------------------------------------------
//  for(int eta=0;eta<nieta;eta++){
//    for(int phi=0;phi<niphi;phi++){
//      for(int depth=0;depth<ndepth;depth++){
//        HBHE_Timing_vs_Energy_c[eta][phi][depth]->Write();
        
//        HBHE_PulseShape[eta][phi][depth]->Write();
//      }
//    }
//  }


  cout<< "HBHE Depth 1 "<<endl;
  std::cout << "Read " << HBHE_TSRatio_fC1_D1->GetEntries() << " events with 30fC<TS4<100fC" << std::endl;
  std::cout << "Read " << HBHE_TSRatio_fC2_D1->GetEntries() << " events with 100fC<TS4<200fC" << std::endl;
  std::cout << "Read " << HBHE_TSRatio_fC3_D1->GetEntries() << " events with 200fC<TS4<600fC" << std::endl;
  std::cout << "Read " << HBHE_TSRatio_fC4_D1->GetEntries() << " events with 600fC<TS4<2000fC" << std::endl;

  cout << "Check Channel is: " << endl;
  std::cout << "IEta from " << ieta_check_min << " to " << ieta_check_max << std::endl;
  std::cout << "IPhi from " << iphi_check_min << " to " << iphi_check_max << std::endl;
  std::cout << "IDepth from " << idepth_check_min << " to " << idepth_check_max << std::endl;
 
  cout << "Compare1 Channel is: " << endl;
  std::cout << "IEta from " << ieta_compare1_min << " to " << ieta_compare1_max << std::endl;
  std::cout << "IPhi from " << iphi_compare1_min << " to " << iphi_compare1_max << std::endl;
  std::cout << "IDepth from " << idepth_compare1_min << " to " << idepth_compare1_max << std::endl;

  cout << "Compare2 Channel is: " << endl;
  std::cout << "IEta from " << ieta_compare2_min << " to " << ieta_compare2_max << std::endl;
  std::cout << "IPhi from " << iphi_compare2_min << " to " << iphi_compare2_max << std::endl;
  std::cout << "IDepth from " << idepth_compare2_min << " to " << idepth_compare2_max << std::endl;

  std::cout << "Has skipped: " << eventSkip << " events\n";
  return;
//------------------------------------------------------------------
// Pipe some parameters to the script: plot.py to get better drawing
//------------------------------------------------------------------
// judge if our FILENAME exists
  fstream _file;
  _file.open(FILENAME,ios::in);
  if(!_file){
    std::cout << "Warning: cannot find: " << FILENAME << ", please check and run unpolished plot.py";
    return;
  }
  _file.close();

// judge if our goal file exists
  _file.open(plot_file.c_str(),ios::in);
  int _prefix = 0;
  std::string _plot_file;
  while(_file){
    _file.close();
    _plot_file = std::to_string(_prefix) + plot_file;
    _file.open(_plot_file.c_str(),ios::in);
    _prefix++;
  }
  if(_prefix != 0) plot_file = _plot_file;

// construct the command and run
  std::string phi_list = "_phi = [" + std::to_string(_iphi[0]);
  for(int i=1;i<_length;i++){
    phi_list  = phi_list + ", " + std::to_string(_iphi[i]);
  }
  phi_list += "]";
  std::string cmd = "cp " + FILENAME + " " + plot_file + " &&  sed -i '9i" + phi_list + "' " + plot_file;
  std::array<char, 128> buffer;
  std::string result;
  std::shared_ptr<FILE> pipe(popen(cmd.c_str(),"r"),pclose);
  if (!pipe) {std::cout<<"popen() failed!"<<std::endl;return;}
  while (!feof(pipe.get())){
    if (fgets(buffer.data(),128,pipe.get()) != nullptr) result += buffer.data();
  }
  std::cout << "Plot script is: " << plot_file << std::endl;
//  std::cout << "Executing the command: " << cmd << std::endl \
            << "The result is: " << result << std::endl;
}
