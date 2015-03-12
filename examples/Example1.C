/*
root -l examples/Example1.C'("test.root","selected.root")'
*/

//------------------------------------------------------------------------------

void Example1(const char *inputFileName, const char *outputFileName)
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

  // Create object of class ExRootTreeWriter
  ExRootTreeWriter *treeWriter = new ExRootTreeWriter(outputFile, "ILC");

  // Create output branches
  ExRootTreeBranch *outputEvent = treeWriter->NewBranch("Event", Event::Class());
  ExRootTreeBranch *outputCaloHit = treeWriter->NewBranch("CaloHit", CaloHit::Class());
  // ExRootTreeBranch *outputResult = treeWriter->NewBranch("Result", MyResult::Class());

  Int_t entry, i, j, N;
  Event *event;
  CaloHit *hit;
  MyResult *result;
  Double_t mult, E, Mx, My, Mz, energy, x, y, z, R, Rrms, r;

  // Loop over all events
  for(entry = 0; entry < numberOfEntries; ++entry)
  {
    mult=0.0;  
    E = 0.0;
    Mx = 0.0;
    My = 0.0;
    Mz = 0.0;

    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);
  
    // Skip event if it contains less than 55 hits
    if(branchCaloHit->GetEntries() < 500) continue;

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

      energy = hit->E;
      x = hit->X;
      y = hit->Y;
      z = hit->Z;
      E = E + energy;
      Mx = Mx + energy*x;
      My = My + energy*y;
      Mz = Mz + energy*z;
    }
/*
    for(i = 0; i < N; ++i)
    {
      energy = hit->E;
      x = hit->X;
      y = hit->Y;
      r = TMath::Hypot((x-Mx/E), (y-My/E));
      Rrms = Rrms + r;
      R = R + energy*r;
    }

    result = (MyResult *) outputResult->NewEntry();
    result->Mx = Mx/E;
    result->My = My/E;
    result->Mz = Mz/E;
    result->R = R/E;
    result->Rrms = TMath::Sqrt(Rrms/N);
    result->Mult = N/48;
*/

    // Write new event
    // if(Mz/E>20&&Mz/E<80&&R/E>30&&N>50&&N<1000) 
    // if(Mz/E>50 && Mz/E<90 && N>50)
    treeWriter->Fill();
  }

  // Close output file
  treeWriter->Write();
  delete treeWriter;
  delete outputFile;
}
