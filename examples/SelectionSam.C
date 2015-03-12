/*
root -l examples/SelectionSam.C'("test.root","selected.root")'
*/

//------------------------------------------------------------------------------

#include <vector>
using namespace std;

void HistStyle(TH1 *hist, Int_t color, Int_t style)
{
  hist->SetLineColor(color);
  hist->SetFillColor(color);
  hist->SetLineWidth(2);
  hist->SetFillStyle(style);
}

void SelectionSam(const char *inputFileName, const char *outputFileName, int cutNumber)
{
  gSystem->Load("libILCAnalysis");

  // Create chain of root trees
  TChain chain("ILC");
  chain.Add(inputFileName);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  
  // Get pointers to branches used in this analysis
  TClonesArray *branchEvent = treeReader->UseBranch("Event");
  TClonesArray *branchCaloHit = treeReader->UseBranch("CaloHit");

  // Create output file
  TDirectory *dir = gDirectory;
  TFile *outputFile = TFile::Open(outputFileName, "UPDATE");
  dir->cd();

  const Int_t NLAYERS = 52;
  Int_t entry, i, j, k, bin, nbins, N;
  Long64_t counter = 0;
  TDirectory *hstDir;

  // Create object of class ExRootTreeWriter
  char name[512];
  ExRootTreeWriter *treeWriter;
  ExRootTreeBranch *outputEvent;
  ExRootTreeBranch *outputCaloHit;
  ExRootTreeBranch *outputResult;

  sprintf(name, "ILC_%d", cutNumber);

  if(outputFile->Get(name))
  {
    cout << "File " << outputFileName << " already contains ROOT tree " << name << endl;
    return;
  }

  treeWriter = new ExRootTreeWriter(outputFile, name);
  
  // Create output branches
  outputEvent = treeWriter->NewBranch("Event", Event::Class());
  outputCaloHit = treeWriter->NewBranch("CaloHit", CaloHit::Class());
  outputResult = treeWriter->NewBranch("Result", MyResult::Class());

  // Create histogram folders
  sprintf(name, "HST_%d", cutNumber);
  hstDir = outputFile->mkdir(name);    

  Event *event;
  CaloHit *hit;
  MyResult *result;
  Double_t frac, mult, E[NLAYERS], Mx[NLAYERS], My[NLAYERS], Mz[NLAYERS];
  Double_t radius, energy, x, y, z, r[NLAYERS], R[NLAYERS], Rrms[NLAYERS], R2Moy[NLAYERS], SigmaR[NLAYERS];
  Int_t Start, Hits[NLAYERS];

  TH1F *hist, *hNR[NLAYERS], *hNRS[NLAYERS];
  for(k=0; k<NLAYERS; ++k)
  {
    sprintf(name, "NR_%d", k);
    hist = new TH1F(name, "Nb of particles vs R", 50, 0., 50.);
    HistStyle(hist, 8, 3004);
    hNR[k] = hist;

    sprintf(name, "NRS_%d", k);
    hist = new TH1F(name, "Frac of Nb of Particles_vs_R", 50, 0., 50.);
    HistStyle(hist, 8, 3004);
    hNRS[k] = hist;
  }

  TH1F *KminusStart = new TH1F("KminusStart", "Hit K from the start", 2*NLAYERS, -NLAYERS, NLAYERS);
  HistStyle(KminusStart, 9, 3017);

  TH1F *Nlayer95 = new TH1F("Nlayer95","R 95% energy contained", NLAYERS, 0., NLAYERS);
  HistStyle(Nlayer95, 9, 3017);

  TH1F *Nlayer99 = new TH1F("Nlayer99","R 99% energy contained", NLAYERS, 0., NLAYERS);
  HistStyle(Nlayer99, 8, 3017);

  ExRootProgressBar progressBar(numberOfEntries - 1);
  // Loop over all events
  for(entry = 0; entry < numberOfEntries; ++entry)
  {
    progressBar.Update(entry);
    mult = 0.0;  
    Start = 0;  
    for(k = 0; k < NLAYERS; ++k)
    {
      E[k] = 0.0;
      Mx[k] = 0.0;
      My[k] = 0.0;
      Mz[k] = 0.0;
      R[k] = 0.0;
      R2Moy[k] = 0.0;
      Rrms[k] = 0.0;
      SigmaR[k] = 0.0;
      Hits[k] = 0;
    }
     // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);
  
    // Skip event if it contains less than 25 hits
  //  if(branchCaloHit->GetEntries() < 25) continue;

    // Prepare tree writer for new event
    treeWriter->Clear();

    // Copy event information
    event = (Event *) outputEvent->NewEntry();
    *event = *(Event *) branchEvent->At(0);

    N = branchCaloHit->GetEntriesFast();
    // Copy all hits in event
    for(i = 0; i < N; ++i)
    {
      hit = (CaloHit *) outputCaloHit->NewEntry();
      *hit = *(CaloHit *) branchCaloHit->At(i);
    }

    // First loop over all hits
    for(i = 0; i < N; ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);

      energy = hit->E;
      x = hit->X;
      y = hit->Y;
      z = hit->Z;
      k = hit->K;

      Hits[0]++;
      Hits[k]++;
      E[0] = E[0] + energy;
      E[k] = E[k] + energy;
      Mx[0] = Mx[0] + energy*x;
      Mx[k] = Mx[k] + energy*x;
      My[0] = My[0] + energy*y;
      My[k] = My[k] + energy*y;
      Mz[0] = Mz[0] + energy*z;
      Mz[k] = Mz[k] + energy*z;
    }

    for(k = 1; k < NLAYERS - 3; ++k)
    {
      if(Hits[k] > 4 && Hits[k+1] > 4 && Hits[k+2] > 4 && Hits[k+3] > 4)
      {
        Start = k;
        break;
      }
    }

    // Second loop over all hits
    for(i = 0; i < N; ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);

      energy = hit->E;
      x = hit->X;
      y = hit->Y;
      k = hit->K;

      r[0] = TMath::Hypot((x-Mx[0]/E[0]), (y-My[0]/E[0]));
      r[k] = TMath::Hypot((x-Mx[k]/E[k]), (y-My[k]/E[k]));
      Rrms[0] = Rrms[0] + r[0]*r[0];
      Rrms[0] = Rrms[0] + r[0]*r[0];
      R[0] = R[0] + energy*r[0];
      R[k] = R[k] + energy*r[k];
      R2Moy[0] = R2Moy[0] + energy*r[0]*r[0];
      R2Moy[k] = R2Moy[k] + energy*r[k]*r[k];
    }

    result = (MyResult *) outputResult->NewEntry();
    for(k = 0; k < NLAYERS; ++k)
    {
      result->Mx[k] = Mx[k]/E[k];
      result->My[k] = My[k]/E[k];
      result->Mz[k] = Mz[k]/E[k];
      result->R[k] = R[k]/E[k];
      result->R2Moy[k] = R2Moy[k]/E[k];
      result->Rrms[k] = TMath::Sqrt(Rrms[k]/N);
      result->SigmaR[k] = TMath::Sqrt((R2Moy[k]/E[k])-((R[k]/E[k])*(R[k]/E[k])));
      result->Hits[k] = Hits[k];
    }
    result->mult = N/48;
    result->Start = Start;

    Bool_t cutValue[3];
    cutValue[0] = kTRUE;
    cutValue[1] = N > 25 && N/48 > 2.2;
    cutValue[2] = N > 25 && N/48 < 2.2 && Hits[1] < 4;

    // Check cut
    if(!cutValue[cutNumber]) continue;

    // Write event
    treeWriter->Fill();
    counter++;

    // Fill histograms

    for(i = 0; i < N; ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);

      energy = hit->E;
      x = hit->X;
      y = hit->Y;
      z = hit->Z;
      k = hit->K;

      KminusStart->Fill(k - Start);
    }

    for(k = 0; k < NLAYERS; ++k)
    {
      hNR[k]->Fill(r[k]);
    }

    for(k = 0; k < NLAYERS; ++k)
    {
      hNRS[k]->SetBinContent(0,(hNR[k]->GetBinContent(0)));
      nbins = hNR[k]->GetNbinsX();
      for(bin = 1; bin < nbins; ++bin)
      {
        hNRS[k]->SetBinContent(bin,hNRS[k]->GetBinContent(bin-1)+hNR[k]->GetBinContent(bin));
      }
      hNRS[k]->Scale(1./(hNRS[k]->GetBinContent(nbins-1)));
      frac = 1.0;
      bin = nbins;
      while(frac > 0.99 && bin > 0)
      {
        frac=hNRS[k]->GetBinContent(--bin);
      }
      radius = bin*(hNRS[k]->GetXaxis()->GetBinUpEdge(nbins-1))/nbins;
      Nlayer99->Fill(k, radius);
      while(frac > 0.95 && bin > 0)
      {
        frac=hNRS[k]->GetBinContent(bin--);
      }
      radius = bin*(hNRS[k]->GetXaxis()->GetBinUpEdge(nbins-1))/nbins;
      Nlayer95->Fill(k, radius);
    }
  }
  progressBar.Finish();

  cout << "Total: " << numberOfEntries << endl;
  cout << "Cut " << cutNumber << ": " << counter << endl;

  // Write histograms
  dir = gDirectory;
  hstDir->cd();

  for(k = 0; k < NLAYERS; ++k)
  {
    hNR[k]->Write();
    hNRS[k]->Write();
  }
  KminusStart->Write();
  Nlayer95->Write();
  Nlayer99->Write();

  dir->cd();

  // Close output file
  treeWriter->Write();
  delete treeWriter;
  delete outputFile;
}
