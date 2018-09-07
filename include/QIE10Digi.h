#ifndef QIE10_DIGI_H
#define QIE10_DIGI_H

#include "HcalDigi.h"
#include "HFSample.h"

class QIE10Digi : public HcalDigi {

 public:
  QIE10Digi();
  QIE10Digi(Collection& c, unsigned short i, short j = 0);
 
// energy(), recHitTime(), size(), presamples(), electronicsId(), rawId(), dv(), er() are pure vitual functions defined in HcalDigi.h
// Have to be realized in this class, so are all set to -1 since we don't have such infos in the QIE10Digi  --added by Yuan
  float energy();
  float recHitTime();
  int   size();
  int   presamples();
  int   dv(int i);
  int   er(int i);
   
  int   ieta  () ;
  int   iphi  () ;
  int   depth () ;
  int   electronicsId();
  int   rawId();
  float fc (int i);
  int   adc(int i);
  int   tdc(int i);
  int   capid (int i);
  
  int getRawIndex () { return m_raw_index; }

  HFSample operator[] ( int i ){
    return HFSample( *m_collection, m_raw_index, i );
  }
  
};

#endif 
