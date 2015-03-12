/*
root -l examples/Selection.C'("test.root","selected.root")'
*/

//------------------------------------------------------------------------------

void Selection(const char *inputFileName, const char *outputFileName)
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
  TFile *outputFile = TFile::Open(outputFileName, "RECREATE");

  const Int_t NLAYERS = 52;
  const Int_t NCUTS = 3;
  Int_t entry, i, j, k, N;
  Long64_t counter[NCUTS];

  // Create object of class ExRootTreeWriter
  char buffer[512];
  ExRootTreeWriter *treeWriter[NCUTS];
  ExRootTreeBranch *outputEvent[NCUTS];
  ExRootTreeBranch *outputCaloHit[NCUTS];
  ExRootTreeBranch *outputResult[NCUTS];

  for(i = 0; i < NCUTS; ++i)
  {
    sprintf(buffer, "ILC_%d", i);
    treeWriter[i] = new ExRootTreeWriter(outputFile, buffer);
  
    // Create output branches
    outputEvent[i] = treeWriter[i]->NewBranch("Event", Event::Class());
    outputCaloHit[i] = treeWriter[i]->NewBranch("CaloHit", CaloHit::Class());
    outputResult[i] = treeWriter[i]->NewBranch("Result", MyResult::Class());
    
    counter[i] = 0;
  }

  Event *event;
  CaloHit *hit;
  MyResult *result;
  Double_t mult, E[NLAYERS], Mx[NLAYERS], My[NLAYERS], Mz[NLAYERS];
  Double_t energy, x, y, z, r, R[NLAYERS], Rrms[NLAYERS];

  // Loop over all events
  for(entry = 0; entry < numberOfEntries; ++entry)
  {
    mult = 0.0;  
    for(k = 0; k < NLAYERS; ++k)
    {
      E[k] = 0.0;
      Mx[k] = 0.0;
      My[k] = 0.0;
      Mz[k] = 0.0;
      R[k] = 0.0;
      Rrms[k] = 0.0;
    }
     // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);
  
    // Skip event if it contains less than 25 hits
  //  if(branchCaloHit->GetEntries() < 25) continue;

    // Prepare tree writer for new event
    for(j = 0; j < NCUTS; ++j)
    {
      treeWriter[j]->Clear();

      // Copy event information
      event = (Event *) outputEvent[j]->NewEntry();
      *event = *(Event *) branchEvent->At(0);
    }

    N = branchCaloHit->GetEntriesFast();
    // Copy all hits in event
    for(i = 0; i < N; ++i)
    {
      for(j = 0; j < NCUTS; ++j)
      {
        hit = (CaloHit *) outputCaloHit[j]->NewEntry();
        *hit = *(CaloHit *) branchCaloHit->At(i);

        energy = hit->E;
        x = hit->X;
        y = hit->Y;
        z = hit->Z;
        k = hit->K;
        E[0] = E[0] + energy;
        E[k] = E[k] + energy;
        Mx[0] = Mx[0] + energy*x;
        Mx[k] = Mx[k] + energy*x;
        My[0] = My[0] + energy*y;
        My[k] = My[k] + energy*y;
        Mz[0] = Mz[0] + energy*z;
        Mz[k] = Mz[k] + energy*z;
      }
    }

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
      Rrms[k] = Rrms[k] + r[k]*r[k];
      R[0] = R[0] + energy*r[0];
      R[k] = R[k] + energy*r[k];
    }

 
    for(j = 0; j < NCUTS; ++j)
    {
      result = (MyResult *) outputResult[j]->NewEntry();
      for(k = 0; k < NLAYERS; ++k)
      {
        result->Mx[k] = Mx[k]/E[k];
        result->My[k] = My[k]/E[k];
        result->Mz[k] = Mz[k]/E[k];
        result->R[k] = R[k]/E[k];
        result->Rrms[k] = TMath::Sqrt(Rrms[k]/N);
      }
      result->Mult = N/48;
    }

    // Write new event
    treeWriter[0]->Fill();
    counter[0]++;
 
    if(Mz[0]/E[0]>30)
    {
      treeWriter[1]->Fill();
      counter[1]++;
    }
    if(N>25&&N/48>2.2)
    {
      treeWriter[2]->Fill();
      counter[2]++;
    }
  }

  cout << "Total: " << counter[0] << endl;
  cout << "Cut 1: " << counter[1] << endl;
  cout << "Cut 2: " << counter[2] << endl;

  // Close output file
  for(j = 0; j < NCUTS; ++j)
  {
    treeWriter[j]->Write();
    delete treeWriter[j];
  }
  delete outputFile;
}
