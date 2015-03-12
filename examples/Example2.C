/*
root -l examples/Example2.C'("test.root")'
*/

//------------------------------------------------------------------------------

void Example2(const char *inputFile)
{
  gSystem->Load("libILCAnalysis");

  // Create chain of root trees
  TChain chain("ILC");
  chain.Add(inputFile);
  
  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  
  // Get pointers to branches used in this analysis
  TClonesArray *branchEvent = treeReader->UseBranch("Event");
  TClonesArray *branchCaloHit = treeReader->UseBranch("CaloHit");
  
  // Book histograms
  TH1 *histE = new TH1F("hit_E", "hit E", 100, 0.0, 100.0);
  TH2 *histIvsK = new TH2F("hit_IK", "hit I vs K", 100, 0.0, 100.0, 100, 0.0, 100.0);

  Int_t entry, i;
  CaloHit *hit;

  // Loop over all events
  for(entry = 0; entry < numberOfEntries; ++entry)
  {
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);
  
    // Skip event if it contains less than 25 hits
    if(branchCaloHit->GetEntries() < 25) continue;

    // Loop over all hits in event
    for(i = 0; i < branchCaloHit->GetEntriesFast(); ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);
      histE->Fill(hit->E);
      histIvsK->Fill(hit->K, hit->I);
    }
    
  }

  // Show resulting histograms
  histE->Draw();
  histIvsK->Draw();
}
