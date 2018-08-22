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
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitEnergy"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitTime"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitIEta"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitIPhi"         , kTRUE);
//  tuple_tree -> fChain -> SetBranchStatus("HBHERecHitDepth"         , kTRUE);
 
  reader.parse(in,root);
  bReader.parse(bin,bRoot);

  //--------------------------------------------------------------------------------
  // Prepare the histograms and variables
  //--------------------------------------------------------------------------------

//  int runStart = 317850;
//  int runStop  = 318000;
  int runStart = 315322;
  //int runStop  = 317280;
  int runStop  = 321000;
//  int runStop  = 318900;

  int YnBin[2] = {100,100};
  float YminBin[2] = {1,-10};
  float YmaxBin[2] = {5,10};
 
  int mini_ieta = 16;
  
  TH1F *HBHE_run = makeTH1F("HBHE_run","HBHE_run",runStop-runStart+1,runStart-0.5,runStop+0.5);
  TH2F *HBHE_chargeAveragedTiming_vs_run = makeTH2F("HBHE_chargeAveragedTiming_vs_run","HB charge Averaged Timing vs Run",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]); 
  TH2F *HBHE_RecTiming_vs_run = makeTH2F("HBHE_RecTiming_vs_run","HB RecHit Timing vs Run",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[1],YminBin[1],YmaxBin[1]); 
  TH2F *HBHE_chargeAveragedTiming_vs_run_IEta[33];
  TH2F *HBHE_RecTiming_vs_run_IEta[33];

  for(int eta=0;eta<33;eta++){
    TString name = Form("IEta %i",eta-mini_ieta);
    HBHE_chargeAveragedTiming_vs_run_IEta[eta] = makeTH2F("HBHE_chargeAveragedTiming_vs_run "+name,"HBHE charge Averaged Timing vs Run "+name,runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]);
    HBHE_chargeAveragedTiming_vs_run_IEta[eta]->Sumw2();
    
    HBHE_RecTiming_vs_run_IEta[eta] = makeTH2F("HBHE_RecTiming_vs_run "+name,"HB RecHit Timing vs Run "+name,runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[1],YminBin[1],YmaxBin[1]);
    HBHE_RecTiming_vs_run_IEta[eta]->Sumw2();  
  }

  int   run[3] = {0};
  int   index = 0;
  int   nHBHEDigis = 0;
  float HBHETiming = 0, HBHEALLFC = 0;
  float TS[8]={0},hbhe_energy=0, hbhe_recTime=0, hbhe_recHitEnergy=0,hbhe_recHitTime=0;
  int   hbhe_recHitIeta=0,hbhe_recHitIphi=0,hbhe_recHitDepth=0;
  int   hbhe_depth=0, hbhe_ieta=0, hbhe_iphi=0, hbhe_run=0, hbhe_ls=0, hbhe_bx=0;
  int   eventSkip = 0;
  //--------------------------------------------------------------------------------
  // Loop
  //--------------------------------------------------------------------------------
  for (int i = 0; i < n_events; ++i){
    
    tuple_tree -> GetEntry(i);
    if ( (i + 1) % 100 == 0 ) std::cout << "Processing event " << i + 1 << "/" << n_events << std::endl;
    if ( (i + 1) % 500000 == 0 ) {write();std::cout << "Saved at the event  " << i + 1 << "/" << n_events << std::endl;}
// check is ls is ok
    hbhe_bx = tuple_tree -> bx;
    hbhe_ls = tuple_tree -> ls;
    hbhe_run = tuple_tree -> run;
    bool  lsValid = false;
    if( root[std::to_string(hbhe_run)].empty() ) {std::cout<<"The run is:"<<hbhe_run<<" The event has been skipped\n";eventSkip++;continue;} //skip if run number is not in the json file
    for( int i=0;i<root[std::to_string(hbhe_run)].size();i++ )
    {
      if(  hbhe_ls >= root[std::to_string(hbhe_run)][i][0].asInt() && hbhe_ls <= root[std::to_string(hbhe_run)][i][1].asInt())
      {
        lsValid = true;
       break;   
      }
    }
//    if( !lsValid ) {std::cout<<"The run is: "<<hbhe_run<<" The ls is: "<<hbhe_ls<<" ,ls is out of range, has been skipped.\n";eventSkip++;continue;} //skip if ls is not in the range
// check is bx is ok
//    bool  bxValid = false;
//    if( bRoot[std::to_string(hbhe_run)].empty() ) {eventSkip++;continue;} //skip if run number is not in the json file
//    for( int i=0;i<bRoot[std::to_string(hbhe_run)].size();i++ )
//    {
//      if( bRoot[std::to_string(hbhe_run)][i]==hbhe_bx )
//      {
//        bxValid = true;
//        break;   
//      }
//    }
//    if( !bxValid ) {eventSkip++;continue;} //skip if ls is not in the range

    CollectionPtr hbheDigis (new Collection(*tuple_tree, tuple_tree -> HBHEDigiIEta -> size()));
    // nObj = total # of HBHE pulses in the current event
    nHBHEDigis = hbheDigis->GetSize();  
    // Loop over all HBHE pulses in the current event to fill a TS Entry
    for (int iHBHEDigi = 0; iHBHEDigi < nHBHEDigis; ++iHBHEDigi){
      HBHEDigi hbheDigi = hbheDigis -> GetConstituent<HBHEDigi>(iHBHEDigi);

// Get Infos
      hbhe_depth        = hbheDigi.depth();
//      hbhe_energy       = hbheDigi.energy();
//      hbhe_recTime      = hbheDigi.recHitTime();
      hbhe_ieta         = hbheDigi.ieta();
      hbhe_iphi         = hbheDigi.iphi();
//      hbhe_recHitTime   = hbheDigi.RecHitTime(); 
//      hbhe_recHitEnergy = hbheDigi.RecHitEnergy(); 
//      hbhe_recHitIeta   = hbheDigi.RecHitIeta(); 
//      hbhe_recHitIphi   = hbheDigi.RecHitIphi(); 
//      hbhe_recHitDepth  = hbheDigi.RecHitDepth(); 

      HBHE_run->Fill(hbhe_run);
      for(int i=0;i<8;i++)
      {
        TS[i] = hbheDigi.fc(i) < 0 ? 0 : hbheDigi.fc(i);
      }
//      if(run[0]==0 || run[1]==0 || run[2]==0){
//        for(int i=0;i<3;i++){
//          if(run[i]==hbhe_run){
//            break;
//          }
//        }
//        if(index == 3){
//          run[index] = hbhe_run;
//          index++;
 //       }
 //     }

      HBHEALLFC = TS[2] + TS[3] + TS[4];
      HBHETiming = (TS[2]*2+TS[3]*3+TS[4]*4)/abs(HBHEALLFC);
      if(TS[3]>30){
        HBHE_chargeAveragedTiming_vs_run->Fill(hbhe_run,HBHETiming);
//        HBHE_RecTiming_vs_run           ->Fill(hbhe_run,hbhe_recTime);

        if(hbhe_ieta != 0){
          HBHE_chargeAveragedTiming_vs_run_IEta[hbhe_ieta+mini_ieta]->Fill(hbhe_run,HBHETiming);
//          HBHE_RecTiming_vs_run_IEta[abs(hbhe_ieta)-1]->Fill(hbhe_run,hbhe_recTime);
        }
      }

    }

  }
  std::cout << "Has skipped: " << eventSkip << " events\n";
//  std::cout << "Run number is: " << run[0] << std::endl << run[1] << std::endl << run[2] << std::endl;

}
