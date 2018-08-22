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
  int N_TS4_cut[4]={0,0,0,0} ;
  int N_HF_TS=0 ;
  int nHBHEDigis = 0, nHFDigis =0 ; 		//# of pulses in each event
  int IEta=0,  IPhi=0;
  int nieta=33,minieta=16,niphi=1,miniphi=1,ndepth=2,minidepth=1;
  int nmethod=3;
//  int nmethod=2;
  int _iphi[] = {0,14,32,50}; //Input for iphis to check, the first elements is always 0 for all iphis
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

// Booking histograms for the plot of Timing vs Energy
// There three types:
// 1. Charge-Averaged Timing  2. MAHI 3. Method 0
  TH2F *HBHE_Timing_vs_Energy_check[nmethod];
  TH2F *HBHE_Timing_vs_Energy_compare1[nmethod];
  TH2F *HBHE_Timing_vs_Energy_compare2[nmethod];

  TString methodName[3] = {"HBHE Charge-Averaged Timing","HBHE DigiRec Timing","HBHE RecHit Timing"};
  TString energyName[3] = {"Digi Energy","DigiRec Energy","RecHit Energy"};
  TString methodNo[3] = {"0","1","2"};
  // Set Y axis limits
//TEST histograms are for RecHit Timing M0 vs Energy M0, somehow HBHE_Timing_vs_Energy_All[2] for M0 didn't work
  TH2F *HBHE_SecondPeakMap_nEvt          = makeTH2F("HBHE_SecondPeakMap_nEvt"      ,"HBHE Second Peak Distribution ", 60, -29.5, 30.5, 74, 0, 74);
  TH2F *HBHE_NormalMap_nEvt          = makeTH2F("HBHE_NormalMap_nEvt"      ,"HBHE Normal Events Distribution ", 60, -29.5, 30.5, 74, 0, 74);
  
  TH1F *HBHE_bxDis_SecondPeak = makeTH1F("HBHE_bxDis_SecondPeak","HBHE_bxDis_SecondPeak",3564,0,3564);
  TH1F *HBHE_bxDis_Normal = makeTH1F("HBHE_bxDis_Normal","HBHE_bxDis_Normal",3564,0,3564);
  TH1F *HBHE_lumiDis_SecondPeak = makeTH1F("HBHE_lumiDis_SecondPeak","HBHE_lumiDis_SecondPeak",528,1,529);
  TH1F *HBHE_lumiDis_Normal = makeTH1F("HBHE_lumiDis_Normal","HBHE_lumiDis_Normal",528,1,529);
//  TH1F *HBHE_timingDis_SecondPeak = makeTH1F("HBHE_timingDis_SecondPeak","HBHE_timingDis_SecondPeak",100,-50,50);
//  TH1F *HBHE_timingDis_Normal = makeTH1F("HBHE_timingDis_Normal","HBHE_timingDis_Normal",100,-50,50);

  for(int method=1;method<nmethod;method++){
    TString method_c=methodNo[method];
    HBHE_Timing_vs_Energy_check[method] = makeTH2F("HBHE_Timing_vs_Energy check channels "+method_c,methodName[method]+" vs "+energyName[method]+" TS3>30 fC"+" for check channels",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_Timing_vs_Energy_compare1[method] = makeTH2F("HBHE_Timing_vs_Energy compare channels 1 "+method_c,methodName[method]+" vs "+energyName[method]+" TS3>30 fC"+" for compare channels 1",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    HBHE_Timing_vs_Energy_compare2[method] = makeTH2F("HBHE_Timing_vs_Energy compare channels 2 "+method_c,methodName[method]+" vs "+energyName[method]+" TS3>30 fC"+" for compare channels 2",3000,0,3000,YnBin[method],YminBin[method],YmaxBin[method]);
    
  }



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
  //--------------------------------------------------------------------------------
  // Start Filling Histograms
  //--------------------------------------------------------------------------------
//	 if(TS[3]>30){
	 if(TS[3]>10){
// Fill histograms for healthy and unhealthy regions
    int method = 0;//special naming here, 0 for method0 while 2 for MAHI
    float Energy = 0;
    float Time = 0;
    if(method == 0){Time=hbhe_recHitTimeMethod0;Energy=hbhe_recHitEnergyMethod0;}
    else if(method == 2){Time=hbhe_recTime;Energy=hbhe_energy;}
    if(Energy>30){
      if(Time < (3.28325*log10(Energy)-12.5)){
        HBHE_SecondPeakMap_nEvt->Fill(hbhe_ieta,hbhe_iphi);
        HBHE_bxDis_SecondPeak->Fill(hbhe_bx);
        HBHE_lumiDis_SecondPeak->Fill(hbhe_ls);
      }
      else{
        HBHE_NormalMap_nEvt->Fill(hbhe_ieta,hbhe_iphi);
        HBHE_bxDis_Normal->Fill(hbhe_bx);
        HBHE_lumiDis_Normal->Fill(hbhe_ls);
      }
    }
  //--------------------------------------------------------------------------------
  // Fill Histograms for Timing vs Energy
  //--------------------------------------------------------------------------------
    for(int method=1;method<nmethod;method++){
      // skip if ieta is out of the range abs(ieta)<16 and abs(ieta)=16 && depth<=2
      if( (abs(hbhe_recHitIeta)>16) || (abs(hbhe_recHitIeta)==16 && hbhe_recHitDepth>2 ) ) continue;

      float Energy =  ( (method==1) ?  hbhe_energy : hbhe_recHitEnergyMethod0);
      float Time   = ( (method==1) ?  hbhe_recTime: hbhe_recHitTimeMethod0);

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
	}
//General END, next for every depth

      // Fill the histograms of HBHE TS ratios:  
    }

    }// end of event loop

  //--------------------------------------------------------------------------------
  std::cout << "Read " << n_events << " events" << std::endl;

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
}
