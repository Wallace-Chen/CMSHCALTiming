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
//  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiRecEnergy"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiRecTime"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHEDigiEnergy"         , kTRUE);
// Add RecHit Info for HBHE
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitEnergy"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitTime"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitIEta"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitIPhi"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitDepth"         , kTRUE);
  
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
//  int nmethod=3;
  int nmethod=2;
//  int _iphi[] = {0,14,32,50}; //Input for iphis to check, the first elements is always 0 for all iphis
//  int _iphi[] = {0,2,7,8,10,30,50,63,70}; //Input for iphis to check, the first elements is always 0 for all iphis
  int _iphi[] = {0,7,23,25,52,53,66}; //Input for iphis to check, the first elements is always 0 for all iphis
//  int _iphi[] = {0}; //Input for iphis to check, the first elements is always 0 for all iphis
  int _length = (sizeof(_iphi)/sizeof(*_iphi));
  int ieta_check_min=-7, ieta_check_max=-1, iphi_check_min=32, iphi_check_max=32, idepth_check_min=1, idepth_check_max=1; // channels to be checked, inclusive
  int ieta_compare1_min=-7, ieta_compare1_max=-1, iphi_compare1_min=1, iphi_compare1_max=72, idepth_compare1_min=1, idepth_compare1_max=1; // channels to be compared, inclusive
  int ieta_compare2_min=1, ieta_compare2_max=7, iphi_compare2_min=1, iphi_compare2_max=72, idepth_compare2_min=1, idepth_compare2_max=1; // channels to be checked, inclusive

  int YnBin[3] = {100,60,60};
  float YminBin[3] = {1,-6,-6};
  float YmaxBin[3] = {5,6,6};
// Maps of HBHE uniformity in TS5/TS4, binned by energy
  float HBHE_EnerWeiTiming_D1_sum[60][74],HBHE_EnerWeiTiming_D2_sum[60][74];
  float HBHE_TSRatioMap_D1_sum[60][74],HBHE_TSRatioMap_fC1_D1_sum[60][74],HBHE_TSRatioMap_fC2_D1_sum[60][74],HBHE_TSRatioMap_fC3_D1_sum[60][74],HBHE_TSRatioMap_fC4_D1_sum[60][74];	
  float HBHE_TSRatioMap_D2_sum[60][74],HBHE_TSRatioMap_fC1_D2_sum[60][74],HBHE_TSRatioMap_fC2_D2_sum[60][74],HBHE_TSRatioMap_fC3_D2_sum[60][74],HBHE_TSRatioMap_fC4_D2_sum[60][74];	
  float HBHE_TSRatioMap_D3_sum[60][74],HBHE_TSRatioMap_fC1_D3_sum[60][74],HBHE_TSRatioMap_fC2_D3_sum[60][74],HBHE_TSRatioMap_fC3_D3_sum[60][74],HBHE_TSRatioMap_fC4_D3_sum[60][74];	

  float HBHE_TSRatioMap_D1_nEvt[60][74],HBHE_TSRatioMap_fC1_D1_nEvt[60][74],HBHE_TSRatioMap_fC2_D1_nEvt[60][74],HBHE_TSRatioMap_fC3_D1_nEvt[60][74],HBHE_TSRatioMap_fC4_D1_nEvt[60][74];	
  float HBHE_TSRatioMap_D2_nEvt[60][74],HBHE_TSRatioMap_fC1_D2_nEvt[60][74],HBHE_TSRatioMap_fC2_D2_nEvt[60][74],HBHE_TSRatioMap_fC3_D2_nEvt[60][74],HBHE_TSRatioMap_fC4_D2_nEvt[60][74];	
  float HBHE_TSRatioMap_D3_nEvt[60][74],HBHE_TSRatioMap_fC1_D3_nEvt[60][74],HBHE_TSRatioMap_fC2_D3_nEvt[60][74],HBHE_TSRatioMap_fC3_D3_nEvt[60][74],HBHE_TSRatioMap_fC4_D3_nEvt[60][74];	

  float HBHE_TSRatioMap_D1_sum2[60][74];	
  float HBHE_TSRatioMap_D2_sum2[60][74];	
  float HBHE_TSRatioMap_D3_sum2[60][74];


  for(int i=0;i<60;i++){
	for(int j=0;j<74;j++){
    HBHE_EnerWeiTiming_D1_sum[i][j]=0;HBHE_EnerWeiTiming_D2_sum[i][j]=0;

  	HBHE_TSRatioMap_D1_sum[i][j]=0;HBHE_TSRatioMap_fC1_D1_sum[i][j]=0;HBHE_TSRatioMap_fC2_D1_sum[i][j]=0;HBHE_TSRatioMap_fC3_D1_sum[i][j]=0;HBHE_TSRatioMap_fC4_D1_sum[i][j]=0;	
  	HBHE_TSRatioMap_D2_sum[i][j]=0;HBHE_TSRatioMap_fC1_D2_sum[i][j]=0;HBHE_TSRatioMap_fC2_D2_sum[i][j]=0;HBHE_TSRatioMap_fC3_D2_sum[i][j]=0;HBHE_TSRatioMap_fC4_D2_sum[i][j]=0;	
  	HBHE_TSRatioMap_D3_sum[i][j]=0;HBHE_TSRatioMap_fC1_D3_sum[i][j]=0;HBHE_TSRatioMap_fC2_D3_sum[i][j]=0;HBHE_TSRatioMap_fC3_D3_sum[i][j]=0;HBHE_TSRatioMap_fC4_D3_sum[i][j]=0;	

    HBHE_TSRatioMap_D1_nEvt[i][j]=0;HBHE_TSRatioMap_fC1_D1_nEvt[i][j]=0;HBHE_TSRatioMap_fC2_D1_nEvt[i][j]=0;HBHE_TSRatioMap_fC3_D1_nEvt[i][j]=0;HBHE_TSRatioMap_fC4_D1_nEvt[i][j]=0;	
    HBHE_TSRatioMap_D2_nEvt[i][j]=0;HBHE_TSRatioMap_fC1_D2_nEvt[i][j]=0;HBHE_TSRatioMap_fC2_D2_nEvt[i][j]=0;HBHE_TSRatioMap_fC3_D2_nEvt[i][j]=0;HBHE_TSRatioMap_fC4_D2_nEvt[i][j]=0;
    HBHE_TSRatioMap_D3_nEvt[i][j]=0;HBHE_TSRatioMap_fC1_D3_nEvt[i][j]=0;HBHE_TSRatioMap_fC2_D3_nEvt[i][j]=0;HBHE_TSRatioMap_fC3_D3_nEvt[i][j]=0;HBHE_TSRatioMap_fC4_D3_nEvt[i][j]=0;
	
	  HBHE_TSRatioMap_D1_sum2[i][j]=0;
	  HBHE_TSRatioMap_D2_sum2[i][j]=0;
	  HBHE_TSRatioMap_D3_sum2[i][j]=0;
	}
  }	

//  TH1F *HBHE_Energy_D1		      = makeTH1F("HBHE_Energy_D1" ,"HBHE: E>5GeV Depth 1", 50, 0, 0);
  TH1F *HBHE_TSRatio_D1		      = makeTH1F("HBHE_TSRatio_D1","HBHE: TS4>30fC  Depth 1", 30, 0,0);
  TH1F *HBHE_TSRatio_D2		      = makeTH1F("HBHE_TSRatio_D2","HBHE: TS4>30fC  Depth 2", 30, 0,0);
  TH1F *HBHE_TSRatio_D3		      = makeTH1F("HBHE_TSRatio_D3","HBHE: TS4>30fC  Depth 3", 30, 0,0);
  TH1F *HBHE_TSRatio_a		      = makeTH1F("HBHE_TSRatio_a ","HBHEa: TS4>30fC", 30, 0,0);
  TH1F *HBHE_TSRatio_b		      = makeTH1F("HBHE_TSRatio_b ","HBHEb: TS4>30fC", 30, 0,0);
  TH1F *HBHE_TSRatio_c		      = makeTH1F("HBHE_TSRatio_c ","HBHEc: TS4>30fC", 30, 0,0);

  TH1F *HBHE_TSRatio_fC1_D1	      = makeTH1F("HBHE_TSRatio_fC1_D1","HBHE: 30fC<TS4<100fC    Depth 1", 30, 0,0);
  TH1F *HBHE_TSRatio_fC2_D1	      = makeTH1F("HBHE_TSRatio_fC2_D1","HBHE: 100fC<TS4<200fC   Depth 1", 30, 0,0);
  TH1F *HBHE_TSRatio_fC3_D1	      = makeTH1F("HBHE_TSRatio_fC3_D1","HBHE: 200fC<TS4<600fC   Depth 1", 30, 0,0);
  TH1F *HBHE_TSRatio_fC4_D1	      = makeTH1F("HBHE_TSRatio_fC4_D1","HBHE: 600fC<TS4<2000fC  Depth 1", 30, 0,0);

  TH1F *HBHE_TSRatio_fC1_D2	      = makeTH1F("HBHE_TSRatio_fC1_D2","HBHE: 30fC<TS4<100fC    Depth 2", 30, 0,0);
  TH1F *HBHE_TSRatio_fC2_D2	      = makeTH1F("HBHE_TSRatio_fC2_D2","HBHE: 100fC<TS4<200fC   Depth 2", 30, 0,0);
  TH1F *HBHE_TSRatio_fC3_D2	      = makeTH1F("HBHE_TSRatio_fC3_D2","HBHE: 200fC<TS4<600fC   Depth 2", 30, 0,0);
  TH1F *HBHE_TSRatio_fC4_D2	      = makeTH1F("HBHE_TSRatio_fC4_D2","HBHE: 600fC<TS4<2000fC  Depth 2", 30, 0,0);

  TH1F *HBHE_TSRatio_fC1_D3	      = makeTH1F("HBHE_TSRatio_fC1_D3","HBHE: 30fC<TS4<100fC    Depth 3", 30, 0,0);
  TH1F *HBHE_TSRatio_fC2_D3	      = makeTH1F("HBHE_TSRatio_fC2_D3","HBHE: 100fC<TS4<200fC   Depth 3", 30, 0,0);
  TH1F *HBHE_TSRatio_fC3_D3	      = makeTH1F("HBHE_TSRatio_fC3_D3","HBHE: 200fC<TS4<600fC   Depth 3", 30, 0,0);
  TH1F *HBHE_TSRatio_fC4_D3	      = makeTH1F("HBHE_TSRatio_fC4_D3","HBHE: 600fC<TS4<2000fC  Depth 3", 30, 0,0);
  
  TH2F *HBHE_TSRatioMap_Avg           = makeTH2F("HBHE_TSRatioMap_Avg"       ,"HBHE: TS4>30fC, Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Rms           = makeTH2F("HBHE_TSRatioMap_Rms"       ,"HBHE: TS4>30fC, Rms TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_nEvt          = makeTH2F("HBHE_TSRatioMap_nEvt"      ,"HBHE: TS4>30fC nEvt ", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Pull          = makeTH2F("HBHE_TSRatioMap_Pull"      ,"HBHE: TS4>30fC, Pull of ratio=|r-r#{_avg}}|/ (rms/sqrt{N})", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC1       = makeTH2F("HBHE_TSRatioMap_Avg_fC1"   ,"HBHE: 30fC<TS4<100fC   , Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC2       = makeTH2F("HBHE_TSRatioMap_Avg_fC2"   ,"HBHE: 100fC<TS4<200fC  , Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC3       = makeTH2F("HBHE_TSRatioMap_Avg_fC3"   ,"HBHE: 200fC<TS4<600fC  , Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC4       = makeTH2F("HBHE_TSRatioMap_Avg_fC4"   ,"HBHE: 600fC<TS4<2000fC , Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);

  TH2F *HBHE_TSRatioMap_Avg_D1           = makeTH2F("HBHE_TSRatioMap_Avg_D1"       ,"HBHE: TS4>30fC Depth 1, Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Rms_D1           = makeTH2F("HBHE_TSRatioMap_Rms_D1"       ,"HBHE: TS4>30fC Depth 1, Rms TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_nEvt_D1          = makeTH2F("HBHE_TSRatioMap_nEvt_D1"      ,"HBHE: TS4>30fC Depth 1 nEvt ", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Pull_D1          = makeTH2F("HBHE_TSRatioMap_Pull_D1"      ,"HBHE: TS4>30fC Depth 1, Pull of ratio=|r-r#{_avg}}|/ (rms/sqrt{N})", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC1_D1       = makeTH2F("HBHE_TSRatioMap_Avg_fC1_D1"   ,"HBHE: 30fC<TS4<100fC   , Avg TS5/TS4 Depth 1", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC2_D1       = makeTH2F("HBHE_TSRatioMap_Avg_fC2_D1"   ,"HBHE: 100fC<TS4<200fC  , Avg TS5/TS4 Depth 1", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC3_D1       = makeTH2F("HBHE_TSRatioMap_Avg_fC3_D1"   ,"HBHE: 200fC<TS4<600fC  , Avg TS5/TS4 Depth 1", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC4_D1       = makeTH2F("HBHE_TSRatioMap_Avg_fC4_D1"   ,"HBHE: 600fC<TS4<2000fC , Avg TS5/TS4 Depth 1", 60, -29.5, 30.5, 74, 0, 74);

  TH2F *HBHE_TSRatioMap_Avg_D2         = makeTH2F("HBHE_TSRatioMap_Avg_D2"       ,"HBHE: TS4>30fC Depth 2, Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Rms_D2         = makeTH2F("HBHE_TSRatioMap_Rms_D2"       ,"HBHE: TS4>30fC Depth 2, Rms TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_nEvt_D2         = makeTH2F("HBHE_TSRatioMap_nEvt_D2"   ,"HBHE: TS4>30fC Depth 2 nEvt ", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Pull_D2         = makeTH2F("HBHE_TSRatioMap_Pull_D2"   ,"HBHE: TS4>30fC Depth 2, Pull of ratio=|r-r#{_avg}}|/ (rms/sqrt{N})", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC1_D2      = makeTH2F("HBHE_TSRatioMap_Avg_fC1_D2"  ,"HBHE: 30fC<TS4<100fC    , Avg TS5/TS4 Depth 2", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC2_D2      = makeTH2F("HBHE_TSRatioMap_Avg_fC2_D2"  ,"HBHE: 100fC<TS4<200fC   , Avg TS5/TS4 Depth 2", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC3_D2      = makeTH2F("HBHE_TSRatioMap_Avg_fC3_D2"  ,"HBHE: 200fC<TS4<600fC   , Avg TS5/TS4 Depth 2", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC4_D2      = makeTH2F("HBHE_TSRatioMap_Avg_fC4_D2"  ,"HBHE: 600fC<TS4<2000fC  , Avg TS5/TS4 Depth 2", 60, -29.5, 30.5, 74, 0, 74);

  TH2F *HBHE_TSRatioMap_Avg_D3         = makeTH2F("HBHE_TSRatioMap_Avg_D3"     ,"HBHE: TS4>30fC Depth 3, Avg TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Rms_D3         = makeTH2F("HBHE_TSRatioMap_Rms_D3"     ,"HBHE: TS4>30fC Depth 3, Rms TS5/TS4", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_nEvt_D3           = makeTH2F("HBHE_TSRatioMap_nEvt_D3"   ,"HBHE: TS4>30fC Depth 3 nEvt ", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Pull_D3         = makeTH2F("HBHE_TSRatioMap_Pull_D3"   ,"HBHE: TS4>30fC Depth 3, Pull of ratio=|r-r#{_avg}}|/ (rms/sqrt{N})", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC1_D3      = makeTH2F("HBHE_TSRatioMap_Avg_fC1_D3"  ,"HBHE:  30fC<TS4<100fC   , Avg TS5/TS4 Depth 3", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC2_D3      = makeTH2F("HBHE_TSRatioMap_Avg_fC2_D3"  ,"HBHE:  100fC<TS4<200fC  , Avg TS5/TS4 Depth 3", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC3_D3      = makeTH2F("HBHE_TSRatioMap_Avg_fC3_D3"  ,"HBHE:  200fC<TS4<600fC  , Avg TS5/TS4 Depth 3", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_TSRatioMap_Avg_fC4_D3      = makeTH2F("HBHE_TSRatioMap_Avg_fC4_D3"  ,"HBHE:  600fC<TS4<2000fC , Avg TS5/TS4 Depth 3", 60, -29.5, 30.5, 74, 0, 74);

  float TS[8]={0},hbhe_energy=0, hbhe_recTime=0, hbhe_recHitEnergy=0,hbhe_recHitTime=0;
  int hbhe_recHitIeta=0,hbhe_recHitIphi=0,hbhe_recHitDepth=0;
  int   hbhe_depth=0, hbhe_ieta=0, hbhe_iphi=0,hbhe_run=0, hbhe_ls=0, hbhe_bx=0;

  int  nTS_fired[3] ={0,0,0};

  
  //--------------------------------------------------------------------------------
  // Loop
  //--------------------------------------------------------------------------------
  float HBHETiming=0,HBHEALLFC=0;
  float HBHEDigiEnergy=0;
  int   eventSkip = 0;
  for (int i = 0; i < n_events; ++i){
    
    tuple_tree -> GetEntry(i);
//    if ( (i + 1) % 1000 == 0 ) std::cout << "Processing event " << i + 1 << "/" << n_events << std::endl;
    if ( (i + 1) % 100 == 0 ) std::cout << "Processing event " << i + 1 << "/" << n_events << std::endl;
    if ( (i + 1) % 100000 == 0 ) {write();std::cout << "Saved at the event  " << i + 1 << "/" << n_events << std::endl;}
// check is ls is ok
    hbhe_bx = tuple_tree -> bx;
    hbhe_ls = tuple_tree -> ls;
    hbhe_run = tuple_tree -> run;
    bool  lsValid = false;
    if( root[std::to_string(hbhe_run)].empty() ) {eventSkip++;continue;} //skip if run number is not in the json file
    for( int i=0;i<root[std::to_string(hbhe_run)].size();i++ )
    {
      if(  hbhe_ls >= root[std::to_string(hbhe_run)][i][0].asInt() && hbhe_ls <= root[std::to_string(hbhe_run)][i][1].asInt())
      {
        lsValid = true;
        break;   
      }
    }
    if( !lsValid ) {eventSkip++;continue;} //skip if ls is not in the range
// check is bx is ok
    bool  bxValid = false;
    if( bRoot[std::to_string(hbhe_run)].empty() ) {eventSkip++;continue;} //skip if run number is not in the json file
    for( int i=0;i<bRoot[std::to_string(hbhe_run)].size();i++ )
    {
      if( bRoot[std::to_string(hbhe_run)][i]==hbhe_bx )
      {
        bxValid = true;
       break;   
      }
    }
    if( !bxValid ) {eventSkip++;continue;} //skip if ls is not in the range

    CollectionPtr hbheDigis (new Collection(*tuple_tree, tuple_tree -> HBHEDigiIEta -> size()));
    // CollectionPtr hbheDigis (new Collection(*tuple_tree, tuple_tree -> HBHERecHitIEta -> size()));

    // nObj = total # of HBHE pulses in the current event
    nHBHEDigis = hbheDigis->GetSize();  
    // Loop over all HBHE pulses in the current event to fill a TS Entry
    for (int iHBHEDigi = 0; iHBHEDigi < nHBHEDigis; ++iHBHEDigi){
      HBHEDigi hbheDigi = hbheDigis -> GetConstituent<HBHEDigi>(iHBHEDigi);
      HBHEDigiEnergy = 0;

	// Estimate the pedestal event by event
      for(int i=0;i<8;i++)
      {
        TS[i] = hbheDigi.fc(i) < 0 ? 0 : hbheDigi.fc(i);
      }
// Get Infos
      hbhe_depth  = hbheDigi.depth();
//      hbhe_energy = hbheDigi.energy();
//      hbhe_recTime= hbheDigi.recHitTime();
      hbhe_ieta =   hbheDigi.ieta();
      hbhe_iphi =   hbheDigi.iphi();
//      hbhe_recHitTime = hbheDigi.RecHitTime(); 
//      hbhe_recHitEnergy = hbheDigi.RecHitEnergy(); 
//      hbhe_recHitIeta = hbheDigi.RecHitIeta(); 
//      hbhe_recHitIphi = hbheDigi.RecHitIphi(); 
//      hbhe_recHitDepth = hbheDigi.RecHitDepth(); 

      // Fill the TS5/TS4 ratio map of HBHE, Binned by energies
//	if(hbhe_energy>5)		HBHE_Energy_D1->Fill( hbhe_energy );
	 if(TS[3]>30){

		if( hbhe_iphi >=3  && hbhe_iphi<=26) HBHE_TSRatio_a->Fill(TS[4]/TS[3]);
		if( hbhe_iphi >=27 && hbhe_iphi<=50) HBHE_TSRatio_b->Fill(TS[4]/TS[3]);
		if( (hbhe_iphi >=51 && hbhe_iphi<=72) || hbhe_iphi==1 || hbhe_iphi==2) HBHE_TSRatio_c->Fill(TS[4]/TS[3]);
	}
//General END, next for every depth
	 if(hbhe_depth==1){
	 	if(TS[3]>30){
	 		HBHE_TSRatio_D1 ->Fill(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D1_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D1_sum2[hbhe_ieta+30][hbhe_iphi]+= ((TS[4]/TS[3])*(TS[4]/TS[3]));
		        HBHE_TSRatioMap_D1_nEvt[hbhe_ieta+30][hbhe_iphi]++;
		}
		if(TS[3]>30 && TS[3]<100)  {
			HBHE_TSRatioMap_fC1_D1_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC1_D1_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC1_D1->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>100 && TS[3]<200)  {
			HBHE_TSRatioMap_fC2_D1_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC2_D1_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC2_D1->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>200 && TS[3]<600)  {
			HBHE_TSRatioMap_fC3_D1_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC3_D1_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC3_D1->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>600 && TS[3]<2000)  {
			HBHE_TSRatioMap_fC4_D1_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC4_D1_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC4_D1->Fill(TS[4]/TS[3]);
		}
	 }
	 if(hbhe_depth==2){ 
		if(TS[3]>30){
	 		HBHE_TSRatio_D2 ->Fill(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D2_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D2_sum2[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3])*(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D2_nEvt[hbhe_ieta+30][hbhe_iphi]++;
		}
		if(TS[3]>30 && TS[3]<100)  {
			HBHE_TSRatioMap_fC1_D2_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC1_D2_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC1_D2->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>100 && TS[3]<200)  {
			HBHE_TSRatioMap_fC2_D2_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC2_D2_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC2_D2->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>200 && TS[3]<600)  {
			HBHE_TSRatioMap_fC3_D2_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC3_D2_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC3_D2->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>600 && TS[3]<2000)  {
			HBHE_TSRatioMap_fC4_D2_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC4_D2_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC4_D2->Fill(TS[4]/TS[3]);
		}
	 }
	 if(hbhe_depth==3){ 
		if(TS[3]>30){
	 		HBHE_TSRatio_D3 ->Fill(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D3_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D3_sum2[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3])*(TS[4]/TS[3]);
		        HBHE_TSRatioMap_D3_nEvt[hbhe_ieta+30][hbhe_iphi]++;
		}
		if(TS[3]>30 && TS[3]<100)  {
			HBHE_TSRatioMap_fC1_D3_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC1_D3_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC1_D3->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>100 && TS[3]<200)  {
			HBHE_TSRatioMap_fC2_D3_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC2_D3_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC2_D3->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>200 && TS[3]<600)  {
			HBHE_TSRatioMap_fC3_D3_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC3_D3_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC3_D3->Fill(TS[4]/TS[3]);
		}
		if(TS[3]>600 && TS[3]<2000)  {
			HBHE_TSRatioMap_fC4_D3_sum[hbhe_ieta+30][hbhe_iphi]+=(TS[4]/TS[3]);
			HBHE_TSRatioMap_fC4_D3_nEvt[hbhe_ieta+30][hbhe_iphi]++;
			HBHE_TSRatio_fC4_D3->Fill(TS[4]/TS[3]);
		}

	 }

      // Fill the histograms of HBHE TS ratios:  
    }
    }// end of event loop

  //--------------------------------------------------------------------------------
  // Analysis
  //--------------------------------------------------------------------------------
  std::cout << "Read " << n_events << " events" << std::endl;


  const int n_HBHE_fault=1000;
  int n_HBHE_D1=0, n_HBHE_D2=0, n_HBHE_D3=0;
  char hname[100],htitle[100];

  // Filling the HBHE 2D Ratio map: 
  std::cout<<"List of HBHE D1 channels:"<<std::endl; 
  float hbhe_avg[5]={0,0,0,0,0}, hbhe_rms=0;
  float hbhe_D1_avg= HBHE_TSRatio_D1->GetMean();
  float hbhe_D2_avg= HBHE_TSRatio_D2->GetMean();
  float hbhe_D3_avg= HBHE_TSRatio_D3->GetMean();
  float hbhe_D1_timing=0,hbhe_D2_timing=0;
  for (int i=0; i<60; i++) {
  	for (int j=0; j<74; j++) {
		if( HBHE_TSRatioMap_D1_nEvt[i][j] !=0){
			hbhe_avg[0] = HBHE_TSRatioMap_D1_sum[i][j] / HBHE_TSRatioMap_D1_nEvt[i][j];
			hbhe_rms = sqrt( (HBHE_TSRatioMap_D1_sum2[i][j] / HBHE_TSRatioMap_D1_nEvt[i][j])   -  (hbhe_avg[0])*(hbhe_avg[0]));  //RMS = sqrt(<X^2> - <X>^2)

			hbhe_avg[1] = HBHE_TSRatioMap_fC1_D1_sum[i][j] / HBHE_TSRatioMap_fC1_D1_nEvt[i][j];
			hbhe_avg[2] = HBHE_TSRatioMap_fC2_D1_sum[i][j] / HBHE_TSRatioMap_fC2_D1_nEvt[i][j];
			hbhe_avg[3] = HBHE_TSRatioMap_fC3_D1_sum[i][j] / HBHE_TSRatioMap_fC3_D1_nEvt[i][j];
			hbhe_avg[4] = HBHE_TSRatioMap_fC4_D1_sum[i][j] / HBHE_TSRatioMap_fC4_D1_nEvt[i][j];
			if(  hbhe_avg[0] >2 || hbhe_rms> 10 ){
				std::cout<<"HBHE Depth =1 IEta= "<<i-43<<" IPhi= "<<j<<"      avg(TS1/TS2) = "<<hbhe_avg[0];
										cout<<"     rms(TS1/TS2) = "<<hbhe_rms;
										cout<<"     nEvt = "<< HBHE_TSRatioMap_D1_nEvt[i][j] <<endl;	
				n_HBHE_D1++;
			}
			HBHE_TSRatioMap_nEvt_D1  ->SetBinContent(i,j,HBHE_TSRatioMap_D1_nEvt[i][j] );
			HBHE_TSRatioMap_Avg_D1   ->SetBinContent(i,j,hbhe_avg[0]);
			HBHE_TSRatioMap_Rms_D1   ->SetBinContent(i,j,hbhe_rms);
			HBHE_TSRatioMap_Pull_D1  ->SetBinContent(i,j,abs(hbhe_avg[0]-hbhe_D1_avg)/ (hbhe_rms/ sqrt(HBHE_TSRatioMap_D1_nEvt[i][j])));
			HBHE_TSRatioMap_Avg_fC1_D1->SetBinContent(i,j,hbhe_avg[1]);
			HBHE_TSRatioMap_Avg_fC2_D1->SetBinContent(i,j,hbhe_avg[2]);
			HBHE_TSRatioMap_Avg_fC3_D1->SetBinContent(i,j,hbhe_avg[3]);
			HBHE_TSRatioMap_Avg_fC4_D1->SetBinContent(i,j,hbhe_avg[4]);

		}
	}
   }
  std::cout<<"# of fault HBHE D1 channels:  "<<n_HBHE_D1 <<std::endl; 
  std::cout<<"List of HBHE D2 channels:"<<std::endl; 
   for (int i=0; i<60; i++) {
  	for (int j=0; j<74; j++) {
		if( HBHE_TSRatioMap_D2_nEvt[i][j] !=0){
			hbhe_avg[0] = HBHE_TSRatioMap_D2_sum[i][j] / HBHE_TSRatioMap_D2_nEvt[i][j];
			hbhe_rms = sqrt( (HBHE_TSRatioMap_D2_sum2[i][j] / HBHE_TSRatioMap_D2_nEvt[i][j])   -  (hbhe_avg[0])*(hbhe_avg[0]));  //RMS = sqrt(<X^2> - <X>^2)

			hbhe_avg[1] = HBHE_TSRatioMap_fC1_D2_sum[i][j] / HBHE_TSRatioMap_fC1_D2_nEvt[i][j];
			hbhe_avg[2] = HBHE_TSRatioMap_fC2_D2_sum[i][j] / HBHE_TSRatioMap_fC2_D2_nEvt[i][j];
			hbhe_avg[3] = HBHE_TSRatioMap_fC3_D2_sum[i][j] / HBHE_TSRatioMap_fC3_D2_nEvt[i][j];
			hbhe_avg[4] = HBHE_TSRatioMap_fC4_D2_sum[i][j] / HBHE_TSRatioMap_fC4_D2_nEvt[i][j];
			if(  hbhe_avg[0] >2 || hbhe_rms> 10 ){
				std::cout<<"HBHE Depth =1 IEta= "<<i-43<<" IPhi= "<<j<<"      avg(TS1/TS2) = "<<hbhe_avg[0];
										cout<<"     rms(TS1/TS2) = "<<hbhe_rms;
										cout<<"     nEvt = "<< HBHE_TSRatioMap_D2_nEvt[i][j] <<endl;	
				n_HBHE_D2++;
			}
			HBHE_TSRatioMap_nEvt_D2  ->SetBinContent(i,j,HBHE_TSRatioMap_D2_nEvt[i][j] );
			HBHE_TSRatioMap_Avg_D2   ->SetBinContent(i,j,hbhe_avg[0]);
			HBHE_TSRatioMap_Rms_D2   ->SetBinContent(i,j,hbhe_rms);
			HBHE_TSRatioMap_Pull_D2  ->SetBinContent(i,j,abs(hbhe_avg[0]-hbhe_D2_avg)/ (hbhe_rms/ sqrt(HBHE_TSRatioMap_D2_nEvt[i][j])));
			HBHE_TSRatioMap_Avg_fC1_D2->SetBinContent(i,j,hbhe_avg[1]);
			HBHE_TSRatioMap_Avg_fC2_D2->SetBinContent(i,j,hbhe_avg[2]);
			HBHE_TSRatioMap_Avg_fC3_D2->SetBinContent(i,j,hbhe_avg[3]);
			HBHE_TSRatioMap_Avg_fC4_D2->SetBinContent(i,j,hbhe_avg[4]);

		}
	}
   }

  std::cout<<"# of fault HBHE D2 channels:  "<<n_HBHE_D2 <<std::endl; 
  std::cout<<"List of HBHE D3 channels:"<<std::endl; 
  for (int i=0; i<60; i++) {
  	for (int j=0; j<74; j++) {
		if( HBHE_TSRatioMap_D3_nEvt[i][j] !=0){
			hbhe_avg[0] = HBHE_TSRatioMap_D3_sum[i][j] / HBHE_TSRatioMap_D3_nEvt[i][j];
			hbhe_rms = sqrt( (HBHE_TSRatioMap_D3_sum2[i][j] / HBHE_TSRatioMap_D3_nEvt[i][j])   -  (hbhe_avg[0])*(hbhe_avg[0]));  //RMS = sqrt(<X^2> - <X>^2)

			hbhe_avg[1] = HBHE_TSRatioMap_fC1_D3_sum[i][j] / HBHE_TSRatioMap_fC1_D3_nEvt[i][j];
			hbhe_avg[2] = HBHE_TSRatioMap_fC2_D3_sum[i][j] / HBHE_TSRatioMap_fC2_D3_nEvt[i][j];
			hbhe_avg[3] = HBHE_TSRatioMap_fC3_D3_sum[i][j] / HBHE_TSRatioMap_fC3_D3_nEvt[i][j];
			hbhe_avg[4] = HBHE_TSRatioMap_fC4_D3_sum[i][j] / HBHE_TSRatioMap_fC4_D3_nEvt[i][j];
			if(  hbhe_avg[0] >2 || hbhe_rms> 10 ){
				std::cout<<"HBHE Depth =1 IEta= "<<i-43<<" IPhi= "<<j<<"      avg(TS1/TS2) = "<<hbhe_avg[0];
										cout<<"     rms(TS1/TS2) = "<<hbhe_rms;
										cout<<"     nEvt = "<< HBHE_TSRatioMap_D3_nEvt[i][j] <<endl;	
				n_HBHE_D3++;
			}
			HBHE_TSRatioMap_nEvt_D3  ->SetBinContent(i,j,HBHE_TSRatioMap_D3_nEvt[i][j] );
			HBHE_TSRatioMap_Avg_D3   ->SetBinContent(i,j,hbhe_avg[0]);
			HBHE_TSRatioMap_Rms_D3   ->SetBinContent(i,j,hbhe_rms);
			HBHE_TSRatioMap_Pull_D3  ->SetBinContent(i,j,abs(hbhe_avg[0]-hbhe_D3_avg)/ (hbhe_rms/ sqrt(HBHE_TSRatioMap_D3_nEvt[i][j])));
			HBHE_TSRatioMap_Avg_fC1_D2->SetBinContent(i,j,hbhe_avg[1]);
			HBHE_TSRatioMap_Avg_fC1_D3->SetBinContent(i,j,hbhe_avg[1]);
			HBHE_TSRatioMap_Avg_fC2_D3->SetBinContent(i,j,hbhe_avg[2]);
			HBHE_TSRatioMap_Avg_fC3_D3->SetBinContent(i,j,hbhe_avg[3]);
			HBHE_TSRatioMap_Avg_fC4_D3->SetBinContent(i,j,hbhe_avg[4]);
		}
	}
   }
//sun over deths
  for (int i=0; i<60; i++) {
  	for (int j=0; j<74; j++) {
			HBHE_TSRatioMap_nEvt  ->SetBinContent(i,j,HBHE_TSRatioMap_nEvt_D1->GetBinContent(i,j)+HBHE_TSRatioMap_nEvt_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Avg   ->SetBinContent(i,j,HBHE_TSRatioMap_Avg_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Avg_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Rms   ->SetBinContent(i,j,HBHE_TSRatioMap_Rms_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Rms_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Pull  ->SetBinContent(i,j,HBHE_TSRatioMap_Pull_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Pull_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Avg_fC1->SetBinContent(i,j,HBHE_TSRatioMap_Avg_fC1_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Avg_fC1_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Avg_fC1->SetBinContent(i,j,HBHE_TSRatioMap_Avg_fC1_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Avg_fC1_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Avg_fC2->SetBinContent(i,j,HBHE_TSRatioMap_Avg_fC2_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Avg_fC2_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Avg_fC3->SetBinContent(i,j,HBHE_TSRatioMap_Avg_fC3_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Avg_fC3_D2->GetBinContent(i,j));
			HBHE_TSRatioMap_Avg_fC4->SetBinContent(i,j,HBHE_TSRatioMap_Avg_fC4_D1->GetBinContent(i,j)+HBHE_TSRatioMap_Avg_fC4_D2->GetBinContent(i,j));
	}
   }



  std::cout<<"# of fault HBHE D3 channels:  "<<n_HBHE_D3 <<std::endl; 


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

}
