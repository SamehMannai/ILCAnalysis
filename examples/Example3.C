/*
root -l examples/Example3.C'("test.root")'
*/

#include "TH1.h"
#include "TSystem.h"

//------------------------------------------------------------------------------

struct MyPlots
{
  TH1 *fHitE[2];
  TH2 *fHitIvsK;
};

//------------------------------------------------------------------------------

class ExRootResult;
class ExRootTreeReader;

//------------------------------------------------------------------------------

void BookHistograms(ExRootResult *result, MyPlots *plots)
{
  THStack *stack;
  TLegend *legend;
  TPaveText *comment;

  // book histogram for E

  plots->fHitE[0] = result->AddHist1D(
    "hit_E_0", "hit E",
    "hit E", "number of hits",
    5, 0.0, 10.0);

  plots->fHitE[1] = result->AddHist1D(
    "hit_E_1", "hit E",
    "hit E", "number of hits",
    5, 0.0, 10.0);


  plots->fHitE[0]->SetLineColor(kRed);
  plots->fHitE[1]->SetLineColor(kBlue);

  // book 1 stack of 2 histograms

  stack = result->AddHistStack("hit_E_all", "1st and 2nd hits E");
  stack->Add(plots->fHitE[0]);
  stack->Add(plots->fHitE[1]);

  // book legend for stack of 2 histograms

  legend = result->AddLegend(0.72, 0.86, 0.98, 0.98);
  legend->AddEntry(plots->fHitE[0], "highest E hit", "l");
  legend->AddEntry(plots->fHitE[1], "second highest E hit", "l");

  // attach legend to stack (legend will be printed over stack in .eps file)

  result->Attach(stack, legend);

  // book more histograms

  plots->fHitIvsK = result->AddHist2D(
    "hit_I_vs_K", "hit I vs K",
    "hit K", "hit I",
    50, 0.0, 100.0,
    50, 0.0, 100.0);

  // book general comment

  comment = result->AddComment(0.64, 0.86, 0.98, 0.98);
  comment->AddText("demonstration plot");
  comment->AddText("produced by Example3.C");

  // attach comment to single histograms

  result->Attach(plots->fHitE[0], comment);
  result->Attach(plots->fHitE[1], comment);
  result->Attach(plots->fHitIvsK, comment);
}

//------------------------------------------------------------------------------

void AnalyseEvents(ExRootTreeReader *treeReader, MyPlots *plots)
{
  TClonesArray *branchEvent = treeReader->UseBranch("Event");
  TClonesArray *branchCaloHit = treeReader->UseBranch("CaloHit");

  Long64_t allEntries = treeReader->GetEntries();

  cout << "** Chain contains " << allEntries << " events" << endl;

  CaloHit *hitPair[2];
  CaloHit *hit;

  Long64_t entry;

  Int_t i;

  // Loop over all events
  for(entry = 0; entry < allEntries; ++entry)
  {
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);

    // Analyse first two hits
    if(branchCaloHit->GetEntriesFast() >= 2)
    {
      hitPair[0] = (CaloHit *) branchCaloHit->At(0);
      hitPair[1] = (CaloHit *) branchCaloHit->At(1);

      plots->fHitE[0]->Fill(hitPair[0]->E);
      plots->fHitE[1]->Fill(hitPair[1]->E);
    }

    // Loop over all hits in event
    for(i = 0; i < branchCaloHit->GetEntriesFast(); ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);
      plots->fHitIvsK->Fill(hit->K, hit->I);
    }
  }
}

//------------------------------------------------------------------------------

void PrintHistograms(ExRootResult *result, MyPlots *plots)
{
  result->Print("png");
}

//------------------------------------------------------------------------------

void Example3(const char *inputFile)
{
  gSystem->Load("libILCAnalysis");

  TChain *chain = new TChain("ILC");
  chain->Add(inputFile);

  ExRootTreeReader *treeReader = new ExRootTreeReader(chain);
  ExRootResult *result = new ExRootResult();

  MyPlots *plots = new MyPlots;

  BookHistograms(result, plots);

  AnalyseEvents(treeReader, plots);

  PrintHistograms(result, plots);

  result->Write("results.root");

  cout << "** Exiting..." << endl;

  delete plots;
  delete result;
  delete treeReader;
  delete chain;
}

//------------------------------------------------------------------------------
