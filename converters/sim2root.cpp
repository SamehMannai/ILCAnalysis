#include <stdexcept>
#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include "TROOT.h"
#include "TApplication.h"

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

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <EVENT/CalorimeterHit.h>
#include <EVENT/SimCalorimeterHit.h>
#include <EVENT/RawCalorimeterHit.h>

#include <EVENT/LCFloatVec.h>
#include <EVENT/LCParameters.h>

#include <UTIL/CellIDDecoder.h>

using namespace std;

using namespace IO;
using namespace IOIMPL;

using namespace EVENT;

//------------------------------------------------------------------------------

void ProcessEvent(LCEvent *readerEvent, ExRootTreeBranch *branchEvent,
  ExRootTreeBranch *branchCaloHit)
{
  LCCollection *collection = 0;
  CalorimeterHit *hit = 0;
  Int_t i = 0, numberOfElements = 0;

  Event *event = 0;
  CaloHit *caloHit = 0;  

  event = static_cast<Event *>(branchEvent->NewEntry());
  event->EventNumber = readerEvent->getEventNumber();
  event->RunNumber = readerEvent->getRunNumber() ;

  collection = readerEvent->getCollection("HCALEndcap");
  CellIDDecoder<CalorimeterHit> decoder(collection);

  numberOfElements = collection->getNumberOfElements();
  for(i = 0; i < numberOfElements; ++i)
  {
    hit = static_cast<CalorimeterHit *>(collection->getElementAt(i));

    caloHit = static_cast<CaloHit*>(branchCaloHit->NewEntry());
    caloHit->I = decoder(hit)["I"];
    caloHit->J = decoder(hit)["J"];
    caloHit->K = 0; // decoder(hit)["K"];
    caloHit->Dif_id = 0; // decoder(hit)["Dif_id"];
    caloHit->Asic_id = 0; // decoder(hit)["Asic_id"];
    caloHit->Chan_id = 0; // decoder(hit)["Chan_id"];
    caloHit->X = hit->getPosition()[0]/10.0;
    caloHit->Y = hit->getPosition()[1]/10.0;
    caloHit->Z = hit->getPosition()[2]/10.0;
    caloHit->T = hit->getTime();
    caloHit->E = hit->getEnergy(); // the threshold
  }
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
  char appName[] = "sim2root";
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
    cout << " Usage: " << appName << " output_file" << " input_file(s)" << endl;
    cout << " output_file - output file in ROOT format," << endl;
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

    i = 2;
    do
    {
      if(interrupted) break;

      cout << "** Reading " << argv[i] << endl;

      reader->open(argv[i]);

      // Loop over all events
      while((readerEvent = reader->readNextEvent()) && !interrupted)
      {
        treeWriter->Clear();
        ProcessEvent(readerEvent, branchEvent, branchCaloHit);
        treeWriter->Fill();
      }

      reader->close();

      ++i;
    }
    while(i < argc);

    treeWriter->Write();

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

