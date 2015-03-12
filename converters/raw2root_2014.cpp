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

bool SortByTimeStamp(RawCalorimeterHitImpl *hit1, RawCalorimeterHitImpl *hit2)
{ 
  return hit1->getTimeStamp() < hit2->getTimeStamp();
}

//------------------------------------------------------------------------------

void ProcessEvent(LCEvent *readerEvent, vector< RawCalorimeterHitImpl * > &sortedHits, map< Int_t, LayerID > &mapping, Int_t *histTime, ExRootTreeWriter *treeWriter, ExRootTreeBranch *branchEvent, ExRootTreeBranch *branchCaloHit)
{
  stringstream message;

  LCCollection *collection = 0;
  RawCalorimeterHitImpl *hit = 0;
  Int_t i = 0, j = 0, k = 0;
  Int_t numberOfElements = 0;
  Int_t numberOfFrames = 0;
  Int_t numberOfHits = 0;

  Int_t time, timeMax;
  UInt_t  ID0, ID1, BarrelEndcapModule;
  Int_t value, valueBefore, valueAfter;
  UInt_t Dif_id;
  Int_t Asic_id, Chan_id;
  Int_t I, J, K;
  Int_t energy;
  Double_t X, Y, Z;
  Double_t DifX = 0.0, DifY = 0.0, DifZ = 0.0;

  map< Int_t, LayerID >::const_iterator itMap;

  Event *event = 0;
  CaloHit *caloHit = 0;  

  LMGeneric *go;
  int *buf;
  unsigned char *tcbuf;
  uint32_t rusize;
  uint32_t idstart;
  bitset<2> ThStatus;

  vector< unsigned char* > vFrames;
  vector< unsigned char* > vLines;

  set< Int_t > setK;

  collection = readerEvent->getCollection("RU_XDAQ");

  numberOfElements = collection->getNumberOfElements();

  timeMax = 0;
  memset(histTime, 0, timeBins*sizeof(Int_t));
  sortedHits.clear();
  for(i = 0; i < numberOfElements; ++i)
  {
    //if(i == 0) continue;
    go = (LMGeneric*) collection->getElementAt(i);
    buf = &(go->getIntVector()[0]);
    tcbuf = (unsigned char*) buf;
    rusize = go->getNInt()*sizeof(int32_t);
    idstart = DIFUnpacker::getStartOfDIF(tcbuf, rusize, 24);
    vFrames.clear();
    vLines.clear();
    DIFUnpacker::getFramePtr(vFrames, vLines, rusize-idstart+1, &tcbuf[idstart]);

    numberOfFrames = vFrames.size();

    for(j = 0; j < numberOfFrames; ++j)
    {
      for(k = 0; k < 64; ++k)
      {
        if(sortedHits.size() >= electronicNoiseCut) return;

        ThStatus[0] = DIFUnpacker::getFrameLevel(vFrames[j], k, 0);
        ThStatus[1] = DIFUnpacker::getFrameLevel(vFrames[j], k, 1);

        if(!(ThStatus[0] || ThStatus[1])) continue; // skip empty pads

        ID0 = (DIFUnpacker::getID(&tcbuf[idstart]))&0x000000FF; //8 firsts bits: DIF Id
        ID0+= (DIFUnpacker::getFrameAsicHeader(vFrames[j])<<8)&0x0000FF00; //8 next bits:   Asic Id
        ID0+= ((k<<16)&0x003F0000); //6 next bits:   Asic's Channel
        BarrelEndcapModule = 0;  //(40 barrel + 24 endcap) modules to be coded here  0 for testbeam (over 6 bits)
        ID0+=(BarrelEndcapModule<<22)&0x00FC00000;

        ID1 = DIFUnpacker::getFrameBCID(vFrames[j]);

        time = DIFUnpacker::getBCID(&tcbuf[idstart]) - ID1;

        if(time > timeMax) timeMax = time;
        if(time > 0 && time < timeBins) histTime[time]++;

        hit = new RawCalorimeterHitImpl();
        hit->setCellID0(ID0);
        hit->setCellID1(ID1);
        hit->setAmplitude(ThStatus.to_ulong());
        hit->setTimeStamp(time);
        sortedHits.push_back(hit);
      }
    }
  }

  if(timeMax >= timeBins)
  {
    message << "time is out of range: " << timeMax;
    throw runtime_error(message.str());
  }

  sort(sortedHits.begin(), sortedHits.end(), SortByTimeStamp);

  numberOfHits = sortedHits.size();

  i = timeWin;
  k = 0;
  while(i <= timeMax - timeWin)
  {
    value = histTime[i];
    valueBefore = histTime[i - 1];
    valueAfter = histTime[i + 1];

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

    while(k < numberOfHits)
    {
      hit = sortedHits[k];

      time = hit->getTimeStamp();

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

      ID0 = hit->getCellID0();
      ID1 = hit->getCellID1();
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

      itMap = mapping.find(Dif_id);
      if(itMap != mapping.end())
      {
        DifZ = itMap->second.K;
        DifY = itMap->second.DifY;
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

      energy = hit->getAmplitude() & 3;
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
  for(i = 0; i < sortedHits.size(); ++i) delete sortedHits[i];
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
    vector< RawCalorimeterHitImpl * > sortedHits(electronicNoiseCut);
    Int_t *histTime = new Int_t[timeBins];

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
    map< Int_t, LayerID > mapping;

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
      mapping[Dif_id] = layer;
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
        ProcessEvent(readerEvent, sortedHits, mapping, histTime, treeWriter, branchEvent, branchCaloHit);
      }

      reader->close();

      ++i;
    }
    while(i < argc);

    treeWriter->Write();

    delete[] histTime;
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

