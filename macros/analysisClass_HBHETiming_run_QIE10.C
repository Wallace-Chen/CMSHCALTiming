#include "analysisClass.h"
#include "HcalTupleTree.h"
//#include "HFDigi.h"
#include "QIE10Digi.h"
//#include "HBHEDigi.h"
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
 
//HE Branches
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiFC"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiDepth"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiFlags"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiIEta"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiIPhi"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiLinkError"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiRawID"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiSubdet"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiADC"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiCapID"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiLETDC"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiOK"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiSOI"         , kTRUE);
  tuple_tree -> fChain -> SetBranchStatus("QIE10DigiTETDC"         , kTRUE);
  
  reader.parse(in,root);
  bReader.parse(bin,bRoot);

  //--------------------------------------------------------------------------------
  // Prepare the histograms and variables
  //--------------------------------------------------------------------------------

//  int runStart = 317850;
//  int runStop  = 318000;
  int runStart = 315000;
  //int runStop  = 317280;
  int runStop  = 321000;
//  int runStop  = 318900;

  int YnBin[2] = {180,100};
  float YminBin[2] = {0,-10};
  float YmaxBin[2] = {90,10};
 
  TH1F *QIE10_run = makeTH1F("QIE10_run","QIE10_run",runStop-runStart+1,runStart-0.5,runStop+0.5);
  std::map<int,TH1F*> QIE10_TDC_Timing_0; //map timing distribution with run#
  std::map<int,TH1F*> QIE10_TDC_Timing_30; //map timing distribution with run#
  std::map<int,TH1F*> QIE10_TDC_Timing_40; //map timing distribution with run#
  std::map<std::vector<int>,TH2F*> QIE10_TDC_vs_IEta; // map timing vs ieta vs run#, depth and TS
  std::map<std::vector<int>,TH3F*> QIE10_TDC_vs_IPhi_IEta; // map timing vs iphi,ieta vs run#, depth and TS
  std::map<int,TH2F*> QIE10_tdc_vs_run_IEta; //map tdc vs run with ieta

  std::map<std::vector<int>,TH1F*> QIE10_TDC; //map tdc distribution with run#,ts
  std::map<std::vector<int>,TH1F*> QIE10_ADC; //map adc distribution with run#,ts
  std::map<std::vector<int>,TH1F*> QIE10_Charge; //map charge distribution with run#,ts

  TH2F *QIE10_Timing_vs_run = makeTH2F("QIE10_Timing_vs_run","HF charge Averaged Timing vs Run",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]); 
  TH2F *QIE10_Timing_vs_run_P = makeTH2F("QIE10_Timing_vs_run_plus","HF charge Averaged Timing vs Run Plus Side",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]); 
  TH2F *QIE10_Timing_vs_run_M = makeTH2F("QIE10_Timing_vs_run_minus","HF charge Averaged Timing vs Run Minus Side",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]); 
  TH2F *QIE10_Timing_vs_run_a = makeTH2F("QIE10_Timing_vs_run_a","HF charge Averaged Timing vs Run A Partition",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]); 
  TH2F *QIE10_Timing_vs_run_b = makeTH2F("QIE10_Timing_vs_run_b","HF charge Averaged Timing vs Run B Partition",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]); 
  TH2F *QIE10_Timing_vs_run_c = makeTH2F("QIE10_Timing_vs_run_c","HF charge Averaged Timing vs Run C Partition",runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]); 
  TH2F *QIE10_Timing_vs_run_Depth[4];
  std::map<int,TH2F*> QIE10_Timing_vs_run_IEta; //map timing vs run with ieta
  
  TH2F *QIE10_ratio_vs_run = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run","HF TS2_TS1_ratio Ratio vs Run",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_P = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_plus","HF TS2_TS1_ratio Ratio vs Run Plus Side",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_M = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_minus","HF TS2_TS1_ratio Ratio vs Run Minus Side",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_a = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_a","HF TS2_TS1_ratio Ratio vs Run A Partition",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_b = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_b","HF TS2_TS1_ratio Ratio vs Run B Partition",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_c = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_c","HF TS2_TS1_ratio Ratio vs Run C Partition",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_Depth[4];
  std::map<int,TH2F*> QIE10_ratio_vs_run_IEta; //map ratio vs run with ieta
  std::map<std::vector<int>,TH3F*> QIE10_ratio_vs_IPhi_IEta; //map tdc vs iphi, ieta vs run# and depth
  
  TH2F *QIE10_ratio_vs_run_H = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_H","HF TS2_TS1_ratio Ratio vs Run High threshold",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_P_H = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_plus_H","HF TS2_TS1_ratio Ratio vs Run Plus Side High threshold",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_M_H = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_minus_H","HF TS2_TS1_ratio Ratio vs Run Minus Side High threshold",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_a_H = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_a_H","HF TS2_TS1_ratio Ratio vs Run A Partition High threshold",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_b_H = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_b_H","HF TS2_TS1_ratio Ratio vs Run B Partition High threshold",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_c_H = makeTH2F("QIE10_TS2_TS1_Ratio_vs_Run_c_H","HF TS2_TS1_ratio Ratio vs Run C Partition High threshold",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  TH2F *QIE10_ratio_vs_run_Depth_H[4];
  std::map<int,TH2F*> QIE10_ratio_vs_run_IEta_H; //map ratio vs run with ieta
  std::map<std::vector<int>,TH3F*> QIE10_ratio_vs_IPhi_IEta_H; //map tdc vs iphi, ieta vs run# and depth
  
  QIE10_ratio_vs_run_IEta[0] = makeTH2F("QIE10_ratio_vs_run_IEta","QIE10_ratio_vs_run_IEta",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  QIE10_ratio_vs_run_IEta_H[0] = makeTH2F("QIE10_ratio_vs_run_IEta_H","QIE10_ratio_vs_run_IEta High threshold",runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
  QIE10_tdc_vs_run_IEta[0] = makeTH2F("QIE10_tdc_vs_run_IEta","QIE10_tdc_vs_run_IEta",runStop-runStart+1,runStart-0.5,runStop+0.5,150,0,75);
  QIE10_TDC_Timing_0[0] = makeTH1F("QIE10_TDC_Timing_0_Run","QIE10_TDC_Timing_0_ALL_Runs",150,0,75);
  QIE10_TDC_Timing_30[0] = makeTH1F("QIE10_TDC_Timing_30_Run","QIE10_TDC_Timing_30_ALL_Runs",150,0,75);
  QIE10_TDC_Timing_40[0] = makeTH1F("QIE10_TDC_Timing_40_Run","QIE10_TDC_Timing_40_ALL_Runs",150,0,75);
  
  for(int ts=0;ts<3;ts++){
    std::vector<int> run_ts = {0,ts};
    QIE10_TDC[run_ts] = makeTH1F(Form("QIE10_TDC_TS%d",ts),Form("QIE10_TDC_ALL_Runs_TS%d",ts),80,0,80);
    QIE10_TDC[run_ts]->Sumw2();

    QIE10_ADC[run_ts] = makeTH1F(Form("QIE10_ADC_TS%d",ts),Form("QIE10_ADC_ALL_Runs_TS%d",ts),300,0,300);
    QIE10_ADC[run_ts]->Sumw2();
    
//    QIE10_Charge[run_ts] = makeTH1F(Form("QIE10_Charge_TS%d",ts),Form("QIE10_Charge__ALL_Runs_TS%d",ts),300,0,3000);
    QIE10_Charge[run_ts] = makeTH1F(Form("QIE10_Charge_TS%d",ts),Form("QIE10_Charge__ALL_Runs_TS%d",ts),30000,0,3000000);
    QIE10_Charge[run_ts]->Sumw2();
  }

  for(int depth=0;depth<4;depth++){
    std::vector<int> run_depth = {0,depth};
    QIE10_ratio_vs_IPhi_IEta[run_depth] = makeTH3F(Form("QIE10_TS2_TS1_Ratio_vs_IPhi_IEta_Depth%d",depth+1),Form("QIE10_ratio_vs_IPhi_IEta_Depth%d",depth+1),85,-42.5,42.5,72,0.5,72.5,500,0,5);
    QIE10_ratio_vs_IPhi_IEta[run_depth]->Sumw2();
    QIE10_ratio_vs_IPhi_IEta_H[run_depth] = makeTH3F(Form("QIE10_TS2_TS1_Ratio_vs_IPhi_IEta_Depth%d_H",depth+1),Form("QIE10_ratio_vs_IPhi_IEta_Depth%d High threshold",depth+1),85,-42.5,42.5,72,0.5,72.5,500,0,5);
    QIE10_ratio_vs_IPhi_IEta_H[run_depth]->Sumw2();
   
    for(int ts=0;ts<3;ts++){ 
      run_depth = {0,depth,ts};
      QIE10_TDC_vs_IEta[run_depth] = makeTH2F(Form("QIE10_TDC_vs_IEta_Depth%d_TS%d",depth+1,ts),Form("QIE10_TDC_vs_IEta_Depth%d_TS%d",depth+1,ts),85,-42.5,42.5,80,0,80);
      QIE10_TDC_vs_IEta[run_depth]->Sumw2();

      QIE10_TDC_vs_IPhi_IEta[run_depth] = makeTH3F(Form("QIE10_TDC_vs_IPhi_IEta_Depth%d_TS%d",depth+1,ts),Form("QIE10_TDC_vs_IPhi_IEta_Depth%d_TS%d",depth+1,ts),85,-42.5,42.5,72,0.5,72.5,80,0,80);
      QIE10_TDC_vs_IPhi_IEta[run_depth]->Sumw2();
    }
    
    QIE10_Timing_vs_run_Depth[depth] = makeTH2F(Form("QIE10_Timing_vs_run_Depth%d",depth+1),Form("QIE10_Timing_vs_run_Depth%d",depth+1),runStop-runStart+1,runStart-0.5,runStop+0.5,YnBin[0],YminBin[0],YmaxBin[0]);
    QIE10_Timing_vs_run_Depth[depth]->Sumw2();

    QIE10_ratio_vs_run_Depth[depth] = makeTH2F(Form("QIE10_ratio_vs_run_Depth%d",depth+1),Form("QIE10_ratio_vs_run_Depth%d",depth+1),runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
    QIE10_ratio_vs_run_Depth[depth]->Sumw2();
    QIE10_ratio_vs_run_Depth_H[depth] = makeTH2F(Form("QIE10_ratio_vs_run_Depth%d_H",depth+1),Form("QIE10_ratio_vs_run_Depth%d High threshold",depth+1),runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
    QIE10_ratio_vs_run_Depth_H[depth]->Sumw2();

  }

  int   run[3] = {0};
  int   index = 0;
  int   nQIE10Digis = 0;
  float QIE10Timing = 0, QIE10ALLFC = 0;
  float TS[3]={0}, TS_sub_ped[3]={0}, TDC[3]={0}, ADC[3]={0};
  int   qie10_depth=0, qie10_ieta=0, qie10_iphi=0, qie10_run=0, qie10_ls=0, qie10_bx=0;
  int   eventSkip = 0;
  //--------------------------------------------------------------------------------
  // Loop
  //--------------------------------------------------------------------------------
  for (int i = 0; i < n_events; ++i){
    
    tuple_tree -> GetEntry(i);
    if ( (i + 1) % 100 == 0 ) std::cout << "Processing event " << i + 1 << "/" << n_events << std::endl;
    if ( (i + 1) % 500000 == 0 ) {write();std::cout << "Saved at the event  " << i + 1 << "/" << n_events << std::endl;}
// check is ls is ok
    qie10_bx = tuple_tree -> bx;
    qie10_ls = tuple_tree -> ls;
    qie10_run = tuple_tree -> run;
    bool  lsValid = false;
    if( root[std::to_string(qie10_run)].empty() ) {std::cout<<"The run is:"<<qie10_run<<" The event has been skipped\n";eventSkip++;continue;} //skip if run number is not in the json file
    for( int i=0;i<root[std::to_string(qie10_run)].size();i++ )
    {
      if(  qie10_ls >= root[std::to_string(qie10_run)][i][0].asInt() && qie10_ls <= root[std::to_string(qie10_run)][i][1].asInt())
      {
        lsValid = true;
       break;   
      }
    }
    if( !lsValid ) {std::cout<<"The run is: "<<qie10_run<<" The ls is: "<<qie10_ls<<" ,ls is out of range, has been skipped.\n";eventSkip++;continue;} //skip if ls is not in the range
// check is bx is ok
    bool  bxValid = false;
    if( bRoot[std::to_string(qie10_run)].empty() ) {std::cout<<"bx error, cannot find the run! skipped\n";eventSkip++;continue;} //skip if run number is not in the json file
    for( int i=0;i<bRoot[std::to_string(qie10_run)].size();i++ )    {
      if( bRoot[std::to_string(qie10_run)][i]==qie10_bx )
      {
        bxValid = true;
        break;   
      }
    }
    if( !bxValid ) {std::cout<<"bx warning, bx range is not within the range\n";eventSkip++;continue;} //skip if ls is not in the range

    CollectionPtr qie10Digis (new Collection(*tuple_tree, tuple_tree -> QIE10DigiIEta -> size()));
    // nObj = total # of QIE10 pulses in the current event
    nQIE10Digis = qie10Digis->GetSize();  
    // Loop over all QIE10 pulses in the current event to fill a TS Entry
    for (int iQIE10Digi = 0; iQIE10Digi < nQIE10Digis; ++iQIE10Digi){
      QIE10Digi qie10Digi = qie10Digis -> GetConstituent<QIE10Digi>(iQIE10Digi);

// Get Infos
      qie10_depth        = qie10Digi.depth();
      qie10_ieta         = qie10Digi.ieta();
      qie10_iphi         = qie10Digi.iphi();

      QIE10_run->Fill(qie10_run);
      for(int i=0;i<3;i++)
      {
        TS[i] = qie10Digi.fc(i) < 0 ? 0 : qie10Digi.fc(i);
        TS_sub_ped[i] = (i == 0) ? TS[i] : ((TS[i]-TS[0]<0) ? 0 : TS[i]-TS[0]);
        TDC[i] = qie10Digi.tdc(i);
        ADC[i] = qie10Digi.adc(i);

        std::vector<int> run_ts = {qie10_run,i};
        if(QIE10_TDC.find(run_ts) == QIE10_TDC.end()) QIE10_TDC[run_ts] = makeTH1F(Form("QIE10_TDC_Run%d_TS%d",qie10_run,i),Form("QIE10_TDC_Run%d_TS%d",qie10_run,i),80,0,80);
        QIE10_TDC[run_ts]->Fill(TDC[i]);
        if(QIE10_ADC.find(run_ts) == QIE10_ADC.end()) QIE10_ADC[run_ts] = makeTH1F(Form("QIE10_ADC_Run%d_TS%d",qie10_run,i),Form("QIE10_ADC_Run%d_TS%d",qie10_run,i),300,0,300);
        QIE10_ADC[run_ts]->Fill(ADC[i]);
//        if(QIE10_Charge.find(run_ts) == QIE10_Charge.end()) QIE10_Charge[run_ts] = makeTH1F(Form("QIE10_Charge_Run%d_TS%d",qie10_run,i),Form("QIE10_Charge_Run%d_TS%d",qie10_run,i),300,0,3000);
        if(QIE10_Charge.find(run_ts) == QIE10_Charge.end()) QIE10_Charge[run_ts] = makeTH1F(Form("QIE10_Charge_Run%d_TS%d",qie10_run,i),Form("QIE10_Charge_Run%d_TS%d",qie10_run,i),30000,0,3000000);
//        QIE10_Charge[run_ts]->Fill(TS[i]);
        QIE10_Charge[run_ts]->Fill(TS[i]/1000);
        run_ts = {0,i};
        QIE10_TDC[run_ts]->Fill(TDC[i]);
        QIE10_ADC[run_ts]->Fill(ADC[i]);
//        QIE10_Charge[run_ts]->Fill(TS[i]);
        QIE10_Charge[run_ts]->Fill(TS[i]/1000);

        if(TDC[i]<60){
          double timing = TDC[i]/2. + 25.*i;
          if(QIE10_TDC_Timing_0.find(qie10_run) == QIE10_TDC_Timing_0.end()) QIE10_TDC_Timing_0[qie10_run] = makeTH1F(Form("QIE10_TDC_Timing_0_Run%d",qie10_run),Form("QIE10_TDC_Timing_0_Run%d",qie10_run),150,0,75); 
          QIE10_TDC_Timing_0[qie10_run]->Fill(timing);
          QIE10_TDC_Timing_0[0]->Fill(timing);
          if(QIE10_TDC_Timing_30.find(qie10_run) == QIE10_TDC_Timing_30.end()) QIE10_TDC_Timing_30[qie10_run] = makeTH1F(Form("QIE10_TDC_Timing_30_Run%d",qie10_run),Form("QIE10_TDC_Timing_30_Run%d",qie10_run),150,0,75); 
          if(ADC[i]>30) {
            QIE10_TDC_Timing_30[qie10_run]->Fill(timing); 
            QIE10_TDC_Timing_30[0]->Fill(timing); 
          }
          if(QIE10_TDC_Timing_40.find(qie10_run) == QIE10_TDC_Timing_40.end()) QIE10_TDC_Timing_40[qie10_run] = makeTH1F(Form("QIE10_TDC_Timing_40_Run%d",qie10_run),Form("QIE10_TDC_Timing_40_Run%d",qie10_run),150,0,75); 
          if(ADC[i]>40) {
            QIE10_TDC_Timing_40[qie10_run]->Fill(timing); 
            QIE10_TDC_Timing_40[0]->Fill(timing); 
          }
        }
      }
      std::cout << "Charge for 3 TSs is: " << TS[0] << " " << TS[1] << " " << TS[2] << std::endl;
      //if(TDC[0]<60 || TDC[2]<60)  std::cout << iQIE10Digi << " hits, TDC value are: " << TDC[0] << " " << TDC[1] << " " << TDC[2] << std::endl;
      float TS2_TS1_ratio = TS_sub_ped[1] == 0 ? 0 : TS_sub_ped[2]/TS_sub_ped[1];


      for(int i=0;i<3;i++)
      {
        if(TDC[i]<60){
          double Timing = TDC[i]/2. + 25.*i;
          QIE10_Timing_vs_run->Fill(qie10_run,Timing);
          if(qie10_ieta>0){
            QIE10_Timing_vs_run_P->Fill(qie10_run,Timing);
          }
          if(qie10_ieta<0){
            QIE10_Timing_vs_run_M->Fill(qie10_run,Timing);
          }
          if (qie10_iphi>=3 && qie10_iphi<=26){
            QIE10_Timing_vs_run_a->Fill(qie10_run,Timing);
          }
          if (qie10_iphi>=27 && qie10_iphi<=50){
            QIE10_Timing_vs_run_b->Fill(qie10_run,Timing);
          }
          if (qie10_iphi>=51 && qie10_iphi<=72 || qie10_iphi==1 || qie10_iphi==2){
            QIE10_Timing_vs_run_c->Fill(qie10_run,Timing);
          }
          if(QIE10_Timing_vs_run_IEta.find(qie10_ieta) == QIE10_Timing_vs_run_IEta.end()){
            QIE10_Timing_vs_run_IEta[qie10_ieta] = makeTH2F(Form("QIE10_Timing_vs_run_IEta%d",qie10_ieta),Form("QIE10_Timing_vs_run_IEta%d",qie10_ieta),runStop-runStart+1,runStart-0.5,runStop+0.5,150,0,75);
            QIE10_Timing_vs_run_IEta[qie10_ieta]->Sumw2();
          }
          QIE10_Timing_vs_run_IEta[qie10_ieta]->Fill(qie10_run,Timing);
          QIE10_Timing_vs_run_Depth[qie10_depth-1]->Fill(qie10_run,Timing);
          
        }

      }
      
        if(TS[1] > 90){
          QIE10_tdc_vs_run_IEta[0]->Fill(qie10_run,TDC[1]);
          if(QIE10_tdc_vs_run_IEta.find(qie10_ieta) == QIE10_tdc_vs_run_IEta.end()){
            QIE10_tdc_vs_run_IEta[qie10_ieta] = makeTH2F(Form("QIE10_tdc_vs_run_IEta%d",qie10_ieta),Form("QIE10_tdc_vs_run_IEta%d",qie10_ieta),runStop-runStart+1,runStart-0.5,runStop+0.5,150,0,75);
            QIE10_tdc_vs_run_IEta[qie10_ieta]->Sumw2();
          }
          QIE10_tdc_vs_run_IEta[qie10_ieta]->Fill(qie10_run,TDC[1]);

          QIE10_ratio_vs_run->Fill(qie10_run,TS2_TS1_ratio);
          if(qie10_ieta>0){ 
            QIE10_ratio_vs_run_P->Fill(qie10_run,TS2_TS1_ratio);
          }
          if(qie10_ieta<0){ 
            QIE10_ratio_vs_run_M->Fill(qie10_run,TS2_TS1_ratio);
          }
          if(qie10_iphi>=3 && qie10_iphi<=26){ 
            QIE10_ratio_vs_run_a->Fill(qie10_run,TS2_TS1_ratio);
          }
          if(qie10_iphi>=27 && qie10_iphi<=50){
            QIE10_ratio_vs_run_b->Fill(qie10_run,TS2_TS1_ratio);
          }  
          if(qie10_iphi>=51 && qie10_iphi<=72 || qie10_iphi==1 || qie10_iphi==2){
            QIE10_ratio_vs_run_c->Fill(qie10_run,TS2_TS1_ratio);
          }
          QIE10_ratio_vs_run_IEta[0]->Fill(qie10_run,TS2_TS1_ratio);
          if(QIE10_ratio_vs_run_IEta.find(qie10_ieta) == QIE10_ratio_vs_run_IEta.end()){
            QIE10_ratio_vs_run_IEta[qie10_ieta] = makeTH2F(Form("QIE10_ratio_vs_run_IEta%d",qie10_ieta),Form("QIE10_ratio_vs_run_IEta%d",qie10_ieta),runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
            QIE10_ratio_vs_run_IEta[qie10_ieta]->Sumw2();
          }
          QIE10_ratio_vs_run_IEta[qie10_ieta]->Fill(qie10_run,TS2_TS1_ratio);
          QIE10_ratio_vs_run_Depth[qie10_depth-1]->Fill(qie10_run,TS2_TS1_ratio);

          for(int ts=0;ts<3;ts++){
            std::vector<int> run_depth_ts = {qie10_run,qie10_depth-1,ts};
            if(QIE10_TDC_vs_IEta.find(run_depth_ts) == QIE10_TDC_vs_IEta.end()){
              QIE10_TDC_vs_IEta[run_depth_ts] = makeTH2F(Form("QIE10_TDC_vs_IEta_Run%d_Depth%d_TS%d",qie10_run,qie10_depth,ts),Form("QIE10_TDC_vs_IEta_Run%d_Depth%d_TS%d",qie10_run,qie10_depth,ts),85,-42.5,42.5,80,0,80);
              QIE10_TDC_vs_IEta[run_depth_ts]->Sumw2();
            }
            QIE10_TDC_vs_IEta[run_depth_ts]->Fill(qie10_ieta,TDC[ts]);
            if(TDC[ts]<60){
              if(QIE10_TDC_vs_IPhi_IEta.find(run_depth_ts) == QIE10_TDC_vs_IPhi_IEta.end()){
                QIE10_TDC_vs_IPhi_IEta[run_depth_ts] =makeTH3F(Form("QIE10_TDC_vs_IPhi_IEta_Run%d_Depth%d_TS%d",qie10_run,qie10_depth,ts),Form("QIE10_TDC_vs_IPhi_IEta_Run%d_Depth%d_TS%d",qie10_run,qie10_depth,ts),85,-42.5,42.5,72,0.5,72.5,80,0,80);
                QIE10_TDC_vs_IPhi_IEta[run_depth_ts]->Sumw2();
              }
              if(TDC[1]<60) QIE10_TDC_vs_IPhi_IEta[run_depth_ts]->Fill(qie10_ieta,qie10_iphi,TDC[ts]);

              run_depth_ts = {0,qie10_depth-1,ts};
              if(TDC[1]<60) QIE10_TDC_vs_IPhi_IEta[run_depth_ts]->Fill(qie10_ieta,qie10_iphi,TDC[ts]);
            }
            run_depth_ts = {0,qie10_depth-1,ts};
            QIE10_TDC_vs_IEta[run_depth_ts]->Fill(qie10_ieta,TDC[ts]);
          }

          std::vector<int> run_depth = {qie10_run,qie10_depth-1};
          if(QIE10_ratio_vs_IPhi_IEta.find(run_depth) == QIE10_ratio_vs_IPhi_IEta.end()){
            QIE10_ratio_vs_IPhi_IEta[run_depth] = makeTH3F(Form("QIE10_ratio_vs_IPhi_IEta_Run%d_Depth%d",qie10_run,qie10_depth),Form("QIE10_ratio_vs_IPhi_IEta_Run%d_Depth%d",qie10_run,qie10_depth),85,-42.5,42.5,72,0.5,72.5,500,0,5);
          }
          QIE10_ratio_vs_IPhi_IEta[run_depth]->Fill(qie10_ieta,qie10_iphi,TS2_TS1_ratio);

          run_depth = {0,qie10_depth-1};
          QIE10_ratio_vs_IPhi_IEta[run_depth]->Fill(qie10_ieta,qie10_iphi,TS2_TS1_ratio);
        }
        if(TS[1] > 200){

          QIE10_ratio_vs_run_H->Fill(qie10_run,TS2_TS1_ratio);
          if(qie10_ieta>0){ 
            QIE10_ratio_vs_run_P_H->Fill(qie10_run,TS2_TS1_ratio);
          }
          if(qie10_ieta<0){ 
            QIE10_ratio_vs_run_M_H->Fill(qie10_run,TS2_TS1_ratio);
          }
          if(qie10_iphi>=3 && qie10_iphi<=26){ 
            QIE10_ratio_vs_run_a_H->Fill(qie10_run,TS2_TS1_ratio);
          }
          if(qie10_iphi>=27 && qie10_iphi<=50){
            QIE10_ratio_vs_run_b_H->Fill(qie10_run,TS2_TS1_ratio);
          }  
          if(qie10_iphi>=51 && qie10_iphi<=72 || qie10_iphi==1 || qie10_iphi==2){
            QIE10_ratio_vs_run_c_H->Fill(qie10_run,TS2_TS1_ratio);
          }
          QIE10_ratio_vs_run_IEta_H[0]->Fill(qie10_run,TS2_TS1_ratio);
          if(QIE10_ratio_vs_run_IEta_H.find(qie10_ieta) == QIE10_ratio_vs_run_IEta_H.end()){
            QIE10_ratio_vs_run_IEta_H[qie10_ieta] = makeTH2F(Form("QIE10_ratio_vs_run_IEta%d_H",qie10_ieta),Form("QIE10_ratio_vs_run_IEta%d High threshold",qie10_ieta),runStop-runStart+1,runStart-0.5,runStop+0.5,500,0,5);
            QIE10_ratio_vs_run_IEta_H[qie10_ieta]->Sumw2();
          }
          QIE10_ratio_vs_run_IEta_H[qie10_ieta]->Fill(qie10_run,TS2_TS1_ratio);
          QIE10_ratio_vs_run_Depth_H[qie10_depth-1]->Fill(qie10_run,TS2_TS1_ratio);

          std::vector<int> run_depth = {qie10_run,qie10_depth-1};
          if(QIE10_ratio_vs_IPhi_IEta_H.find(run_depth) == QIE10_ratio_vs_IPhi_IEta_H.end()){
            QIE10_ratio_vs_IPhi_IEta_H[run_depth] = makeTH3F(Form("QIE10_ratio_vs_IPhi_IEta_Run%d_Depth%d_H",qie10_run,qie10_depth),Form("QIE10_ratio_vs_IPhi_IEta_Run%d_Depth%d High threshold",qie10_run,qie10_depth),85,-42.5,42.5,72,0.5,72.5,500,0,5);
          }
          QIE10_ratio_vs_IPhi_IEta_H[run_depth]->Fill(qie10_ieta,qie10_iphi,TS2_TS1_ratio);

          run_depth = {0,qie10_depth-1};
          QIE10_ratio_vs_IPhi_IEta_H[run_depth]->Fill(qie10_ieta,qie10_iphi,TS2_TS1_ratio);
        }

//       if(qie10_ieta != 0){
//          QIE10_ratio_vs_run_IEta[qie10_ieta+mini_ieta]->Fill(qie10_run,TS2_TS1_ratio);
//        }

    }

  }
  std::cout << "Has skipped: " << eventSkip << " events\n";
//  std::cout << "Run number is: " << run[0] << std::endl << run[1] << std::endl << run[2] << std::endl;

}
