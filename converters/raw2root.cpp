#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>

#include <bitset>
#include <vector>
#include <map>
#include <set>

#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>

#include "TROOT.h"
#include "TApplication.h"

#include "TH1F.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TStopwatch.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TLorentzVector.h"

#include "classes/ILCClasses.h"

#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"

#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCEventImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include "IMPL/RawCalorimeterHitImpl.h"
#include <IMPL/LCGenericObjectImpl.h>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/SimCalorimeterHit.h>
#include <EVENT/RawCalorimeterHit.h>
#include <EVENT/LCGenericObject.h>

#include <EVENT/LCFloatVec.h>
#include <EVENT/LCParameters.h>

#include <UTIL/CellIDDecoder.h>

using namespace std;

using namespace IO;
using namespace IOIMPL;

using namespace EVENT;

const Int_t timeWin = 1;
const Int_t noiseCut = 7;
const Int_t layerCut = 7;

const vector< Int_t >::size_type timeBins = 400000;
const vector< RawCalorimeterHitImpl * >::size_type electronicNoiseCut = 100000;

//#######################
//###### MAPPING ########
//#######################

// Upper view (ASICs SIDE)
// Asics mapping on large chambers with HARDROC II & IIB
// 0   5  10   15    20    25    30    35    40    45    50    55    60




const unsigned short MapJLargeHR2[64] =
  {1,1,2,2,3,3,4,4,5,5,6,6,7,7,4,3,
   2,0,0,1,0,5,6,7,7,6,5,4,3,2,1,0,
   0,1,2,3,4,5,6,7,7,6,0,0,1,1,0,2,
   3,4,5,7,7,6,6,5,5,4,4,3,3,2,2,1};
const unsigned short MapILargeHR2[64] =
  {1,0,1,0,1,0,1,0,0,1,0,1,0,1,2,2,
   2,0,1,2,2,2,2,2,3,3,3,3,3,3,3,3,
   4,4,4,4,4,4,4,4,5,5,5,6,5,6,7,5,
   5,5,5,6,7,6,7,6,7,6,7,7,6,7,6,7};



//      (J Axis)

// 7    |46|63|61|59|58|56|54|52|
// 6    |43|45|62|60|57|55|53|51|
// 5    |42|44|47|48|49|50|41|40|
// 4    |32|33|34|35|36|37|38|39|
// 3    |31|30|29|28|27|26|25|24|            TOP VIEW (ASICs SIDE)
// 2    |20|19|16|15|14|21|22|23|
// 1    |18|00|02|04|06|09|11|13|
// 0    |17|01|03|05|07|08|10|12|
//       0  1  2  3  4  5  6  7    (I Axis)  ----->
//                              |        |
//                              |DIFF|
//                              |____|

const unsigned short AsicShiftI[49]=
  {     0,      0,      0,      0,      0,      8,      8,
        8,      8,      16,     16,     16,     16,     24,
        24,     24,     24,     32,     32,     32,     32,
        40,     40,     40,     40,     48,     48,     48,
        48,     56,     56,     56,     56,     64,     64,
        64,     64,     72,     72,     72,     72,     80,
        80,     80,     80,     88,     88,     88,     88};
const unsigned short AsicShiftJ[49]=
  {     0,      0,      8,      16,     24,     24,     16,
        8,      0,      0,      8,      16,     24,     24,
        16,     8,      0,      0,      8,      16,     24,
        24,     16,     8,      0,      0,      8,      16,
        24,     24,     16,     8,      0,      0,      8,
        16,     24,     24,     16,     8,      0,      0,
        8,      16,     24,     24,     16,     8,      0};

#include "classes/TCalorimeterHit.h"
#include "classes/DIFUnpacker.h"

class LMGeneric: public LCGenericObjectImpl
{
public:
  LMGeneric(){;}
  std::vector<int>& getIntVector(){return _intVec;}
};

//------------------------------------------------------------------------------

struct LayerID
{
  int K;
  double DifX;
  double DifY;
  double IncX;
  double IncY;
};

//------------------------------------------------------------------------------

struct RawCaloHit
{
  UInt_t ID0;
  UInt_t E;
  UInt_t T;
  ULong64_t BCID;
};

//------------------------------------------------------------------------------

bool SortByT(RawCaloHit *hit1, RawCaloHit *hit2)
{ 
  return hit1->T < hit2->T;
}

//------------------------------------------------------------------------------


vector< RawCaloHit * > *gHits;
vector< RawCaloHit > *gPool;

vector< unsigned char * > *gFrames;
vector< unsigned char * > *gLines;
map< Int_t, LayerID > *gMapping;
Int_t *gTimeHist;
ULong64_t gFirstBCID = 0;

void ProcessEvent(LCEvent *readerEvent, ExRootTreeWriter *treeWriter, ExRootTreeBranch *branchEvent, ExRootTreeBranch *branchCaloHit)
{
  stringstream message;

  LCCollection *collection = 0;
  RawCaloHit *hit = 0;
  Int_t i = 0, j = 0, k = 0;
  Int_t numberOfElements = 0;
  Int_t numberOfFrames = 0;
  Int_t numberOfHits = 0;

  ULong64_t currentBCID = 0, absBCID = 0;
  UInt_t difBCID = 0, hitBCID = 0;
  Int_t time, timeMax;
  UInt_t ID0, BarrelEndcapModule;
  Int_t value, valueBefore, valueAfter;
  UInt_t Dif_id;
  Int_t Asic_id, Chan_id;
  Int_t I, J, K;
  Int_t energy;
  Double_t X, Y, Z;
  Double_t DifX = 0.0, DifY = 0.0, DifZ = 0.0;

  map< Int_t, LayerID >::const_iterator itMapping;

  Event *event = 0;
  CaloHit *caloHit = 0;  

  LMGeneric *go;
  int *buf;
  unsigned char *tcbuf;
  uint32_t rusize;
  uint32_t idstart;
  bitset<2> ThStatus;

  set< Int_t > setK;

  collection = readerEvent->getCollection("RU_XDAQ");

  numberOfElements = collection->getNumberOfElements();

  timeMax = 0;
  gHits->clear();
  for(i = 0; i < numberOfElements; ++i)
  {
    //if(i == 0) continue;
    go = (LMGeneric*) collection->getElementAt(i);
    buf = &(go->getIntVector()[0]);
    tcbuf = (unsigned char*) buf;
    rusize = go->getNInt()*sizeof(int32_t);
    idstart = DIFUnpacker::getStartOfDIF(tcbuf, rusize, 24);
    gFrames->clear();
    gLines->clear();
    DIFUnpacker::getFramePtr(*gFrames, *gLines, rusize - idstart + 1, &tcbuf[idstart]);
    difBCID = DIFUnpacker::getBCID(&tcbuf[idstart]);
    absBCID = DIFUnpacker::getAbsoluteBCID(&tcbuf[idstart]);

    numberOfFrames = gFrames->size();

    for(j = 0; j < numberOfFrames; ++j)
    {
      hitBCID = DIFUnpacker::getFrameBCID(gFrames->at(j));
      time = difBCID - hitBCID;
      if(time > timeMax) timeMax = time;

      for(k = 0; k < 64; ++k)
      {
        if(numberOfHits >= electronicNoiseCut) return;

        ThStatus[0] = DIFUnpacker::getFrameLevel(gFrames->at(j), k, 0);
        ThStatus[1] = DIFUnpacker::getFrameLevel(gFrames->at(j), k, 1);

        if(!(ThStatus[0] || ThStatus[1])) continue; // skip empty pads

        ID0 = (DIFUnpacker::getID(&tcbuf[idstart]))&0x000000FF; // 8 firsts bits: DIF Id
        ID0+= (DIFUnpacker::getFrameAsicHeader(gFrames->at(j))<<8)&0x0000FF00; // 8 next bits:   Asic Id
        ID0+= ((k<<16)&0x003F0000); // 6 next bits:   Asic's Channel
        BarrelEndcapModule = 0;  // (40 barrel + 24 endcap) modules to be coded here 0 for testbeam (over 6 bits)
        ID0+=(BarrelEndcapModule<<22)&0x00FC00000;

        if(time > 0 && time < timeBins) gTimeHist[time]++;

        hit = &gPool->at(numberOfHits++);
        gHits->push_back(hit);
        hit->ID0 = ID0;
        hit->E = ThStatus.to_ulong();
        hit->T = time;
        hit->BCID = absBCID;
      }
    }
  }

  if(timeMax >= timeBins)
  {
    message << "time is out of range: " << timeMax;
    throw runtime_error(message.str());
  }

  sort(gHits->begin(), gHits->end(), SortByT);

  i = timeWin;
  k = 0;
  while(i <= timeMax - timeWin)
  {
    value = gTimeHist[i];
    valueBefore = gTimeHist[i - 1];
    valueAfter = gTimeHist[i + 1];

    if(value < noiseCut || value < valueBefore || value < valueAfter)
    {
      ++i;
      continue;
    }

    setK.clear();
    treeWriter->Clear();

    event = static_cast<Event *>(branchEvent->NewEntry());
    event->EventNumber = readerEvent->getEventNumber();
    event->RunNumber = readerEvent->getRunNumber();
    event->BCID = 0;
    event->TS = 0;
    event->T = i;
    event->N = 0;

    while(k < numberOfHits)
    {
      hit = gHits->at(k);

      time = hit->T;

      if(time < i - timeWin)
      {
        ++k;
        continue;
      }
      else if(time > i + timeWin)
      {
        break;
      }
      else
      {
        ++k;
      }

      if(event->BCID == 0 && time == i)
      {
        event->BCID = hit->BCID;
        currentBCID = event->BCID + ULong64_t(time);
        if(gFirstBCID == 0) gFirstBCID = currentBCID;
        if(currentBCID - gFirstBCID > 7.5e7) gFirstBCID = currentBCID;
        else event->TS = currentBCID - gFirstBCID;
      }
      ++event->N;

      ID0 = hit->ID0;
/*
      TCalorimeterHit transformation(*hit);
      Dif_id = transformation.fDif_id;
      Asic_id = transformation.fAsic_id;
      Chan_id = transformation.fChan_id;

      I = 0;
      J = 0;
      K = 0;

      X = transformation.fX;
      Y = transformation.fY;
      Z = transformation.fZ;
*/

      Dif_id = ID0 & 0xFF; // Dif id
      Asic_id = (ID0 & 0xFF00)>>8; // Asic id
      Chan_id = (ID0 & 0x3F0000)>>16; // Channel id

      itMapping = gMapping->find(Dif_id);
      if(itMapping != gMapping->end())
      {
        DifZ = itMapping->second.K;
        DifY = itMapping->second.DifY;
      }
      else
      {
//        message << "can't find mapping for " << Dif_id;
//        throw runtime_error(message.str());
        continue;
      }

      I = (1 + MapILargeHR2[Chan_id] + AsicShiftI[Asic_id]);
      J = (32 - (MapJLargeHR2[Chan_id] + AsicShiftJ[Asic_id])) + Int_t(DifY);
      K = Int_t(DifZ);

      X = I*1.04125;
      Y = J*1.04125;
      Z = DifZ*2.8;

      setK.insert(K);

      energy = hit->E & 3;
      if(energy == 2) energy = 1;
      else if(energy == 1) energy = 2;

      caloHit = static_cast<CaloHit*>(branchCaloHit->NewEntry());
      caloHit->I = I;
      caloHit->J = J;
      caloHit->K = K;
      caloHit->Dif_id = Dif_id;
      caloHit->Asic_id = Asic_id;
      caloHit->Chan_id = Chan_id;
      caloHit->X = X;
      caloHit->Y = Y;
      caloHit->Z = Z;
      caloHit->T = time;
      caloHit->E = energy;
    }
    if(setK.size() >= layerCut) treeWriter->Fill();
    i += (timeWin<<1) + 1;
  }
  memset(gTimeHist, 0, (timeMax + 1)*sizeof(Int_t));
}

//---------------------------------------------------------------------------

static bool interrupted = false;

void SignalHandler(int sig)
{
  interrupted = true;
}

//---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  char appName[] = "slcio2root";
  stringstream message;
  TFile *outputFile = 0;
  ExRootTreeWriter *treeWriter = 0;
  ExRootTreeBranch *branchEvent = 0;
  ExRootTreeBranch *branchCaloHit = 0;
  LCReader *reader = 0;
  LCEvent *readerEvent = 0;
  Int_t i;

  if(argc < 3)
  {
    cout << " Usage: " << appName << " output_file" << " mapping_file" << " input_file(s)" << endl;
    cout << " output_file - output file in ROOT format," << endl;
    cout << " mapping_file - mapping file," << endl;
    cout << " input_file(s) - input file(s) in SLCIO format." << endl;
    return 1;
  }

  signal(SIGINT, SignalHandler);

  gROOT->SetBatch();

  int appargc = 1;
  char *appargv[] = {appName};
  TApplication app(appName, &appargc, appargv);

  try
  {
    gHits = new vector< RawCaloHit * >(electronicNoiseCut);
    gPool = new vector< RawCaloHit >(electronicNoiseCut);
    gFrames = new vector< unsigned char * >(1000);
    gLines = new vector< unsigned char * >(1000);
    gMapping = new map< Int_t, LayerID >;
    gTimeHist = new Int_t[timeBins];
    memset(gTimeHist, 0, timeBins*sizeof(Int_t));

    outputFile = TFile::Open(argv[1], "CREATE");

    if(outputFile == NULL)
    {
      message << "can't open " << argv[1];
      throw runtime_error(message.str());
    }

    treeWriter = new ExRootTreeWriter(outputFile, "ILC");

    branchEvent = treeWriter->NewBranch("Event", Event::Class());
    branchCaloHit = treeWriter->NewBranch("CaloHit", CaloHit::Class());

    reader = LCFactory::getInstance()->createLCReader();

    LayerID layer;

    ifstream mappingFile(argv[2], ios::in);
    if(!mappingFile)
    {
      message << "can't open " << argv[2];
      throw runtime_error(message.str());
    }

    while(!mappingFile.eof())
    {
      int Dif_id;
      char co;
      mappingFile 
        >> Dif_id >> co
        >> layer.K >> co
        >> layer.DifX >> co
        >> layer.DifY >> co
        >> layer.IncX >> co
        >> layer.IncY;
      (*gMapping)[Dif_id] = layer;
    }
    mappingFile.close();

    i = 3;
    do
    {
      if(interrupted) break;

      cout << "** Reading " << argv[i] << endl;

      reader->open(argv[i]);

      // Loop over all events
      while((readerEvent = reader->readNextEvent()) && !interrupted)
      {
        ProcessEvent(readerEvent, treeWriter, branchEvent, branchCaloHit);
      }

      reader->close();

      ++i;
    }
    while(i < argc);

    treeWriter->Write();

    delete gHits;
    delete gPool;
    delete gFrames;
    delete gLines;
    delete gMapping;
    delete[] gTimeHist;

    delete reader;
    delete treeWriter;
    delete outputFile;

    cout << "** Exiting..." << endl;

    return 0;
  }
  catch(runtime_error &e)
  {
    if(reader) delete reader;
    if(treeWriter) delete treeWriter;
    if(outputFile) delete outputFile;
    cerr << "** ERROR: " << e.what() << endl;
    return 1;
  }
}

