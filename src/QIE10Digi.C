#include "QIE10Digi.h"

QIE10Digi::QIE10Digi():
  HcalDigi(){}

QIE10Digi::QIE10Digi(Collection& c, unsigned short i, short j ):
  HcalDigi(c,i){}

float QIE10Digi::energy       () {return -1;}
float QIE10Digi::recHitTime   () {return -1;}
int   QIE10Digi::size         () {return -1;}
int   QIE10Digi::presamples   () {return -1;}
int   QIE10Digi::dv      (int i) {return -1;}
int   QIE10Digi::er      (int i) {return -1;}

int   QIE10Digi::ieta         () {return m_collection -> GetData() -> QIE10DigiIEta          -> at(m_raw_index);}
int   QIE10Digi::iphi         () {return m_collection -> GetData() -> QIE10DigiIPhi          -> at(m_raw_index);}
int   QIE10Digi::depth        () {return m_collection -> GetData() -> QIE10DigiDepth         -> at(m_raw_index);}
int   QIE10Digi::rawId        () {return -1;} 
int   QIE10Digi::electronicsId() {return -1;}
float QIE10Digi::fc      (int i) {return m_collection -> GetData() -> QIE10DigiFC            -> at(m_raw_index)[i];}
int   QIE10Digi::adc     (int i) {return m_collection -> GetData() -> QIE10DigiADC           -> at(m_raw_index)[i];}
int   QIE10Digi::tdc     (int i) {return m_collection -> GetData() -> QIE10DigiLETDC         -> at(m_raw_index)[i];}
int   QIE10Digi::capid   (int i) {return m_collection -> GetData() -> QIE10DigiCapID         -> at(m_raw_index)[i];}
