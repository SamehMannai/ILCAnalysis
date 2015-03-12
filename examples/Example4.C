void Example4(const char *inputFile, int entry)
{
  gSystem->Load("libILCAnalysis");

  // Create chain of root trees
  TChain chain("ILC");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  // Get pointers to branches used in this analysis
  TClonesArray *branchCaloHit = treeReader->UseBranch("CaloHit");

  // Delete histograms if they exist
  TObject *object = 0;
  if(object = gDirectory->FindObject("hit_IK")) delete object;
  if(object = gDirectory->FindObject("hit_IJ")) delete object;
  if(object = gDirectory->FindObject("hit_KJI_0")) delete object;
  if(object = gDirectory->FindObject("hit_KJI_1")) delete object;
  if(object = gDirectory->FindObject("hit_KJI_2")) delete object;

  // Book histograms
  TH2 *histIvsK = new TH2F("hit_IK", "hit I vs K", 100, 0.0, 100.0, 100, 0.0, 100.0);
  TH2 *histIvsJ = new TH2F("hit_IJ", "hit I vs J", 100, 0.0, 100.0, 100, 0.0, 100.0);

  Int_t i;
  CaloHit *hit;


  // Load selected branches with data from specified event
  treeReader->ReadEntry(entry);

  TGraph2D *graphKJI[3];
  graphKJI[0] = new TGraph2D(branchCaloHit->GetEntriesFast());
  graphKJI[0]->SetNameTitle("hit_KJI_0", "hit 3D");
  graphKJI[1] = new TGraph2D(branchCaloHit->GetEntriesFast());
  graphKJI[1]->SetNameTitle("hit_KJI_1", "hit 3D");
  graphKJI[2] = new TGraph2D(branchCaloHit->GetEntriesFast());
  graphKJI[2]->SetNameTitle("hit_KJI_2", "hit 3D");

  Int_t n[3] = {0, 0, 0};

  // Loop over all hits in event
  for(i = 0; i < branchCaloHit->GetEntriesFast(); ++i)
  {
    hit = (CaloHit *) branchCaloHit->At(i);
    histIvsK->Fill(hit->K, hit->I);
    histIvsJ->Fill(hit->J, hit->I);
    cout << hit->K << "\t" << hit->J << "\t" << hit->I << "\t" << endl;
    if(hit->E == 1) graphKJI[0]->SetPoint(n[0]++, hit->K, hit->J, hit->I);
    if(hit->E == 2) graphKJI[1]->SetPoint(n[1]++, hit->K, hit->J, hit->I);
    if(hit->E == 3) graphKJI[2]->SetPoint(n[2]++, hit->K, hit->J, hit->I);
  }
  cout << n[0] << " " << n[1] << " " << n[2] << endl;

  // Show resulting histograms
  histIvsK->Draw("colz");
  histIvsJ->Draw("colz");
  for(i = 0; i < 3; ++i)
  {
    if(n[i] == 0) continue;
    graphKJI[i]->SetMarkerStyle(kOpenSquare);
    graphKJI[i]->SetMarkerSize(0.2);
    graphKJI[i]->Draw("p");
    graphKJI[i]->GetXaxis()->SetTitle("K");
    graphKJI[i]->GetXaxis()->SetLimits(0.0, 100.0);
    graphKJI[i]->GetXaxis()->SetTitleOffset(1.8);
    graphKJI[i]->GetYaxis()->SetTitle("J");
    graphKJI[i]->GetYaxis()->SetLimits(0.0, 100.0);
    graphKJI[i]->GetYaxis()->SetTitleOffset(1.8);
    graphKJI[i]->GetZaxis()->SetTitle("I");
    graphKJI[i]->GetZaxis()->SetLimits(0.0, 100.0);
  }
  if(n[0] > 0) graphKJI[0]->SetMarkerColor(kGreen);
  if(n[1] > 0) graphKJI[1]->SetMarkerColor(kBlue);
  if(n[2] > 0) graphKJI[2]->SetMarkerColor(kRed);
  Option_t *option = "p";
  if(n[0] > 0) {graphKJI[0]->Draw(option); option = "p same";}
  if(n[1] > 0) {graphKJI[1]->Draw(option); option = "p same";}
  if(n[2] > 0) {graphKJI[2]->Draw(option); option = "p same";}
}
