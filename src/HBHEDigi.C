#include "HBHEDigi.h"

HBHEDigi::HBHEDigi():
  HcalDigi(){}

HBHEDigi::HBHEDigi(Collection& c, unsigned short i, short j ):
  HcalDigi(c,i){}

float HBHEDigi::energy       () {
//        std::cout << "m_raw_index is: " << m_raw_index << std::endl;
//        std::cout << "HBHEDigiRecEnergy is: " << m_collection -> GetData() -> HBHEDigiRecEnergy -> at(m_raw_index) << std::endl;

        return m_collection -> GetData() -> HBHEDigiRecEnergy     -> at(m_raw_index);}
float HBHEDigi::recHitTime   () {return m_collection -> GetData() -> HBHEDigiRecTime       -> at(m_raw_index);}
int   HBHEDigi::ieta         () {return m_collection -> GetData() -> HBHEDigiIEta          -> at(m_raw_index);}
int   HBHEDigi::iphi         () {return m_collection -> GetData() -> HBHEDigiIPhi          -> at(m_raw_index);}
float HBHEDigi::eta          () {return m_collection -> GetData() -> HBHEDigiEta           -> at(m_raw_index);}
float HBHEDigi::phi          () {return m_collection -> GetData() -> HBHEDigiPhi           -> at(m_raw_index);}
int   HBHEDigi::depth        () {return m_collection -> GetData() -> HBHEDigiDepth         -> at(m_raw_index);}
int   HBHEDigi::subdet       () {return m_collection -> GetData() -> HBHEDigiSubdet        -> at(m_raw_index);}
int   HBHEDigi::size         () {return m_collection -> GetData() -> HBHEDigiSize          -> at(m_raw_index);}
int   HBHEDigi::presamples   () {return m_collection -> GetData() -> HBHEDigiPresamples    -> at(m_raw_index);}
int   HBHEDigi::rawId        () {return m_collection -> GetData() -> HBHEDigiRawID         -> at(m_raw_index);}
int   HBHEDigi::flag         () {
//        std::cout << "HBHERecHitFlags is: " << m_collection -> GetData() -> HBHERecHitFlags -> at(m_raw_index) << std::endl;
        return m_collection -> GetData() -> HBHERecHitFlags       -> at(m_raw_index);}
int   HBHEDigi::electronicsId() {return -1; }
float HBHEDigi::fc      (int i) {return m_collection -> GetData() -> HBHEDigiFC            -> at(m_raw_index)[i];}
int   HBHEDigi::adc     (int i) {return m_collection -> GetData() -> HBHEDigiADC           -> at(m_raw_index)[i];}
int   HBHEDigi::dv      (int i) {return m_collection -> GetData() -> HBHEDigiDV            -> at(m_raw_index)[i];}
int   HBHEDigi::er      (int i) {return m_collection -> GetData() -> HBHEDigiER            -> at(m_raw_index)[i];}
int   HBHEDigi::capid   (int i) {return m_collection -> GetData() -> HBHEDigiCapID         -> at(m_raw_index)[i];}
float HBHEDigi::digienergy(int i) {return m_collection -> GetData() -> HBHEDigiEnergy      -> at(m_raw_index)[i];}
// Add RecHit Info for HBHE
float HBHEDigi::RecHitTime   () {return m_collection -> GetData() -> HBHERecHitTime        -> at(m_raw_index);}
float HBHEDigi::RecHitEnergy () {return m_collection -> GetData() -> HBHERecHitEnergy      -> at(m_raw_index);}
int   HBHEDigi::RecHitDepth  () {return m_collection -> GetData() -> HBHERecHitDepth       -> at(m_raw_index);}
int   HBHEDigi::RecHitIeta   () {return m_collection -> GetData() -> HBHERecHitIEta        -> at(m_raw_index);}
int   HBHEDigi::RecHitIphi   () {return m_collection -> GetData() -> HBHERecHitIPhi        -> at(m_raw_index);}
int   HBHEDigi::flagMethod0  () {return m_collection -> GetData() -> HBHERecHitFlagsMethod0-> at(m_raw_index);}
float HBHEDigi::RecHitTimeMethod0   () {
//        std::cout << "HBHERecHitTimeMethod0 is: " << m_collection -> GetData() -> HBHERecHitTimeMethod0        -> at(m_raw_index) << std::endl;
        return m_collection -> GetData() -> HBHERecHitTimeMethod0        -> at(m_raw_index);}
float HBHEDigi::RecHitEnergyMethod0 () {
//        std::cout << "HBHERecHitEnergyMethod0 is: " << m_collection -> GetData() -> HBHERecHitEnergyMethod0 -> at(m_raw_index) << std::endl;
        return m_collection -> GetData() -> HBHERecHitEnergyMethod0      -> at(m_raw_index);}
// END
int   HBHEDigi::run          () {return m_collection -> GetData() -> run;}
int   HBHEDigi::bx           () {return m_collection -> GetData() -> bx;}
int   HBHEDigi::ls           () {return m_collection -> GetData() -> ls;}
int   HBHEDigi::event        () {return m_collection -> GetData() -> event;}
