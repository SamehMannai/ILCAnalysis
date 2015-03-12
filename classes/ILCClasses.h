#ifndef ILCClasses_h
#define ILCClasses_h

/**
 *
 *  Definition of classes to be stored in the root tree.
 *
 *  $Date: 2008-06-04 13:57:24 $
 *  $Revision: 1.1 $
 *
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

// Dependencies (#includes)

#include "TRef.h"
#include "TObject.h"
#include "TRefArray.h"
#include "TLorentzVector.h"

#include "classes/SortableObject.h"

//---------------------------------------------------------------------------

class Event: public TObject
{
public:

  ULong64_t RunNumber; // run number
  ULong64_t EventNumber; // event number
  ULong64_t BCID;
  ULong64_t TS;
  ULong64_t T;
  ULong64_t N;

  ClassDef(Event, 1)
};

//---------------------------------------------------------------------------

class CaloHit: public TObject
{
public:

  Double_t E;
  Double_t T;
  Double_t X;
  Double_t Y;
  Double_t Z;

  Int_t I;
  Int_t J;
  Int_t K;

  UInt_t Dif_id;
  Int_t Asic_id;
  Int_t Chan_id;

  static CompBase *fgCompare; //!
  const CompBase *GetCompare() const { return fgCompare; }

  ClassDef(CaloHit, 1)
};


//---------------------------------------------------------------------------

class MyResult: public TObject
{
public:

  Double_t Mx[50];
  Double_t My[50];
  Double_t Mz[50];
  Double_t R[50];
  Double_t Rrms[50];
  Double_t mult;
  Double_t R2Moy[50];
Double_t R2test[50];
  Double_t SigmaR[50];
Double_t SigmaMzStart[50];
Double_t MzStart[50];
Double_t Mz2Moy[50];
Double_t SigmaMz2[50];
  Double_t SigmaMz[50]; 
 Double_t clusterx;
  Int_t Hits[50];
  Int_t Start;
  Int_t Last;
 Double_t tanPhix;
Double_t tanPhixKais;
 ClassDef(MyResult, 1)
};



//---------------------------------------------------------------------------

#endif

