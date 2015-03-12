/*
root -l examples/SelectionSam.C'("test.root","selected.root",0)'
*/

//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <Riostream.h>
#include <iostream>
#include <fstream>

#include <vector>
using namespace std;

void HistStyle(TH1 *hist, Int_t color, Int_t style)
{
  hist->SetLineColor(color);
  hist->SetFillColor(color);
  hist->SetLineWidth(2);
  hist->SetFillStyle(style);
}

void SelectionSamData(const char *inputFileName, const char *outputFileName, int cutNumber)
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

  const Int_t NLAYERS = 51;
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

  //char fichier[1000];
       //ofstream calibration; calibration.open(fichier);
ofstream fichier("test.txt");
  Event *event;
  CaloHit *hit;
  MyResult *result;
  Double_t frac, mult, E[NLAYERS], Mx[NLAYERS], My[NLAYERS], Mz[NLAYERS];
  Double_t radius, energy, x, y, z, xj, yj, kj, r[NLAYERS], R[NLAYERS], Rrms[NLAYERS], R2Moy[NLAYERS], SigmaR[NLAYERS], Mz2Moy[NLAYERS], SigmaMz[NLAYERS], SigmaMz2[NLAYERS], SigmaMz22[NLAYERS], SigmaMzStart[NLAYERS], MzStart[NLAYERS], R2test[NLAYERS];
  Double_t tanPhix; 
  Double_t tanPhixKais;
  Int_t First, Last, Start, Hits[NLAYERS];

  TH1F *hist, *hNR[NLAYERS], *hNRS[NLAYERS];
  for(k=0; k<NLAYERS; ++k)
  {
    sprintf(name, "NR_%d", k);
    hist = new TH1F(name, "Nb of particles vs R", 50, 0., 100.);
    HistStyle(hist, 9, 3017);
    hNR[k] = hist;

    sprintf(name, "NRS_%d", k);
    hist = new TH1F(name, "Frac of Nb of Particles_vs_R", 50, 0., 100.);
    HistStyle(hist, 9, 3017);
    hNRS[k] = hist;
  }

  
  TH1F *KminusStart = new TH1F("KminusStart", "Hit K from the start", 2*NLAYERS, -NLAYERS, NLAYERS);
  HistStyle(KminusStart, 9, 3017);
 
  TH1F *KProfile = new TH1F("KProfile", "Hit K from the Front Face", 2*NLAYERS, -NLAYERS, NLAYERS);
  HistStyle(KProfile, 9, 3017); 

  TH1F *Nlayer95 = new TH1F("Nlayer95","R 95% energy contained", NLAYERS, 0., NLAYERS);
  HistStyle(Nlayer95, 9, 3017);

  TH1F *Nlayer99 = new TH1F("Nlayer99","R 99% energy contained", NLAYERS, 0., NLAYERS);
  HistStyle(Nlayer99, 9, 3017);

  ExRootProgressBar progressBar(numberOfEntries - 1);
  // Loop over all events
  for(entry = 0; entry < numberOfEntries; ++entry)
  {
    progressBar.Update(entry);
    mult = 0.0;  
    First = 0;  
    Last = 0;  
    Start = 0;  
    tanPhix = 0;
    tanPhixKais = 0;
    for(k = 0; k < NLAYERS; ++k)
    {
      E[k] = 0.0;
      Mx[k] = 0.0;
      My[k] = 0.0;
      Mz[k] = 0.0;
      R[k] = 0.0;
      R2test[k] = 0.0;      
      R2Moy[k] = 0.0;
      Rrms[k] = 0.0;
      SigmaR[k] = 0.0;
      Mz2Moy[k]=0.0;
      SigmaMz[k]=0.0;
      Hits[k] = 0;
      MzStart[k]=0.0;
     SigmaMz2[k]=0.0;
     SigmaMz22[k]=0.0;
     SigmaMzStart[k]=0.0;

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
   // if(energy == 2) energy = 1;
    //else if(energy == 1) energy = 2;

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
      //MzStart[k] = MzStart[k] + energy*(z-Start+65);
      //MzStart[0] = MzStart[0] + energy*(z-Start+65);
      Mz2Moy[0] = Mz2Moy[0] + energy*z*z;
      Mz2Moy[k] = Mz2Moy[k] + energy*z*z;

    }

    for(k = 1; k < NLAYERS - 3; ++k)
    {
      if(Hits[k] > 4 && Hits[k+1] > 4 && Hits[k+2] > 4 && Hits[k+3] > 4)
      {
        Start = k;
        break;
      }
    }

    for(k = 1; k < NLAYERS; ++k)
    {
      if(Hits[k] > 0)
      {
        First = k;
        break;
      }
    }

    for(k = NLAYERS - 1; k >= 1; --k)
    {
      if(Hits[k] > 0)
      {
        Last = k;
        break;
      }
    }

    if(Last - First > 0)
    {
       tanPhix = (Mx[Last] - Mx[First])/(Last - First);
    }


for(i = 0; i < N; ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);

      energy = hit->E;
      //if(energy == 2) energy = 1;
      //else if(energy == 1) energy = 2;
      x = hit->X;
      y = hit->Y;
      z = hit->Z;
      k = hit->K;
      if(k < Start) continue;
      MzStart[0] = MzStart[0] + energy*(z-(2.8*(Start-1)+2.3));
      MzStart[k] = MzStart[k] + energy*(z-(2.8*(Start-1)+2.3));
}


for(i = 0; i < N; ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);

      energy = hit->E;
     // if(energy == 2) energy = 1;
     // else if(energy == 1) energy = 2;
      x = hit->X;
      y = hit->Y;
      z = hit->Z;
      k = hit->K;
      if(k < Start) continue;
  
SigmaMzStart[k] = SigmaMzStart[k] + energy*(z-(2.8*(Start-1)+2.3)-(MzStart[k]/E[k]))*(z-(2.8*(Start-1)+2.3)-(MzStart[k]/E[k]));
SigmaMzStart[0] = SigmaMzStart[0] + energy*(z-(2.8*(Start-1)+2.3)-(MzStart[0]/E[0]))*(z-(2.8*(Start-1)+2.3)-(MzStart[0]/E[0]));

}

//Int_t n[3] = {0, 0, 0};
    // Second loop over all hits
    for(i = 0; i < N; ++i)
    {
      
     hit = (CaloHit *) branchCaloHit->At(i);

      energy = hit->E;
      //if(energy == 0) continue;
       //if(energy == 2) energy = 1;//, n[0]++;
       //else if(energy == 1) energy = 2;//, n[1]++;
//else if (energy==3) n[2]++;
  //    fichier << n[0] << " " << n[1] << " " << n[2] << endl;
      x = hit->X;
      y = hit->Y;
      k = hit->K;
      z = hit->Z;
      // MzStart[0] = MzStart[0] + energy*(z-Start+65);
      //MzStart[k] = MzStart[k] + energy*(z-Start+65);

      r[0] = (E[0] > 0) ? TMath::Hypot((x-Mx[0]/E[0]), (y-My[0]/E[0])) : 0.0;
      r[k] = (E[k] > 0) ? TMath::Hypot((x-Mx[k]/E[k]), (y-My[k]/E[k])) : 0.0;
      Rrms[0] = Rrms[0] + r[0]*r[0];
      Rrms[k] = Rrms[k] + r[k]*r[k];
      R[0] = R[0] + energy*r[0];
      R[k] = R[k] + energy*r[k];
      R2Moy[0] = R2Moy[0] + energy*r[0]*r[0];
      R2Moy[k] = R2Moy[k] + energy*r[k]*r[k];
//R2test[0] = R2test[0] + energy*(r[0]-(R[0]/E[0]))*(r[0]-(R[0]/E[0]));
//R2test[k] = R2test[k] + energy*(r[k]-(R[k]/E[k]))*(r[k]-(R[k]/E[k]));    
SigmaMz22[k] = SigmaMz22[k] + energy*(z-(Mz[k]/E[k]))*(z-(Mz[k]/E[k])); 
SigmaMz22[0] = SigmaMz22[0] + energy*(z-(Mz[0]/E[0]))*(z-(Mz[0]/E[0]));
//SigmaMzStart[k] = SigmaMzStart[k] + energy*(z-2.6*Start-(MzStart[k]/E[k]))*(z-2.6*Start-(MzStart[k]/E[k]));
//SigmaMzStart[0] = SigmaMzStart[0] + energy*(z-2.6*Start-(MzStart[0]/E[0]))*(z-2.6*Start-(MzStart[0]/E[0]));  // to check! because of start


////////////////////////////////////////////////////////////
for(j = 0; j < N-1; ++j)
    {
      hit = (CaloHit *) branchCaloHit->At(j);
      xj = hit->X;
      yj = hit->Y;
      kj = hit->K;

tanPhixKais=(xj-x)/(kj-k);
}
/////////////////////////////////////////////////////////////
    }

//////////////////test

for(i = 0; i < N; ++i)
    {
      hit = (CaloHit *) branchCaloHit->At(i);

      energy = hit->E;
      //if(energy == 0) continue;
     // if(energy == 2) energy = 1;
      //else if(energy == 1) energy = 2;
      x = hit->X;
      y = hit->Y;
      k = hit->K;
      r[0] = (E[0] > 0) ? TMath::Hypot((x-Mx[0]/E[0]), (y-My[0]/E[0])) : 0.0;
      r[k] = (E[k] > 0) ? TMath::Hypot((x-Mx[k]/E[k]), (y-My[k]/E[k])) : 0.0;

R2test[0] = R2test[0] + energy*(r[0]-(R[0]/E[0]))*(r[0]-(R[0]/E[0]));
R2test[k] = R2test[k] + energy*(r[k]-(R[k]/E[k]))*(r[k]-(R[k]/E[k]));
}

///////////////////////////////

    // Fill result branch
    result = (MyResult *) outputResult->NewEntry();
    for(k = 0; k < NLAYERS; ++k)
    {



      result->Mx[k] = (E[k] > 0) ? Mx[k]/E[k] : 0.0;
      result->My[k] = (E[k] > 0) ? My[k]/E[k] : 0.0;
      result->Mz[k] = (E[k] > 0) ? Mz[k]/E[k] : 0.0;
  result->MzStart[k] = (E[k] > 0) ? MzStart[k]/E[k] : 0.0;
       result->R[k] = (E[k] > 0) ? R[k]/E[k] : 0.0;
       result->R2Moy[k] = (E[k] > 0) ? R2Moy[k]/E[k] : 0.0;
       result->Mz2Moy[k] = (E[k] > 0) ? Mz2Moy[k]/E[k] : 0.0;
       result->Rrms[k] = TMath::Sqrt(Rrms[k]/N);
       result->SigmaMz[k] = (E[k] > 0) ? TMath::Sqrt((Mz2Moy[k]/E[k])-((Mz[k]/E[k])*(Mz[k]/E[k]))) : 0.0;
       result->SigmaMz2[k] = (E[k] > 0) ? TMath::Sqrt(SigmaMz22[k]/E[k]) : 0.0;
result->SigmaMzStart[k] = (E[k] > 0 ) ? TMath::Sqrt(SigmaMzStart[k]/E[k]) : 0.0;      
result->SigmaR[k] = (E[k] > 0) ? TMath::Sqrt((R2Moy[k]/E[k])-((R[k]/E[k])*(R[k]/E[k]))) : 0.0;

result->Hits[k] = Hits[k];
  
  }
/*
result = (MyResult *) outputResult->NewEntry();
    for(k = 0; k < NLAYERS; ++k)
    {

if(k < Start) continue;

result->MzStart[k] = (E[k] > 0) ? MzStart[k]/E[k] : 0.0;
result->SigmaMzStart[k] = (E[k] > 0) ? TMath::Sqrt(SigmaMzStart[k]/E[k]) : 0.0;
  }*/
    result->mult = N/48;
    result->Start = Start;
    result->Last = Last;
   result->tanPhix = tanPhix;
   result->tanPhixKais = tanPhixKais;
    Bool_t cutValue[5];
  //  var1 = N;
    //var2 = cos(Mx[0]);
//    Double_t resNN = mlp1(var1, var2, var3, ......);
    cutValue[0] = kTRUE;
   //cutValue[1] = N>25 && counter<1000;

//cutValue[1] = N>100 && result->Rrms[0]<5 && result->Start<3 && result->Mz[0]<30 && result->Mx[0]>20 && result->Mx[0]<80 && result->My[0]>20 && result->My[0]<80;
cutValue[1] = N>200 && result->Rrms[0]>5 && result->Start>2 && result->Start<30 && result->Mx[0]>20 && result->Mx[0]<80 && result->My[0]>20 && result->My[0]<80;
//cutValue[2] = N/48>2.2 && result->Rrms[0]>5 && Start>2 && result->Mz[0]<100 && result->Mx[0]>-20 && result->Mx[0]<20 && result->My[0]>-20 && result->My[0]<20;

cutValue[2] = N/48>2.2 && result->Rrms[0]>5 && result->R[0]>3.5 && result->Start>3 && result->Mz[0]<80 && result->Mx[0]>-20 && result->Mx[0]<20 && result->My[0]>-20 && result->My[0]<20;

//cutValue[2] = N/48>2.2 && result->Rrms[0]>5 && result->R[0]>3.5 && result->Start>3  && result->Mx[0]>20 && result->Mx[0]<80 && result->My[0]>20 && result->My[0]<80 && result->Mz[0]<145;   

// cutValue[1] = N/48>2.2 && Mz[0]/E[0] > 20 && Mz[0]/E[0] <80 && Start < 10 && Last < 45 && Hits[Last] < 10;
    //cutValue[1] = N > 25 && Mz[0]/E[0] > -50 && Mz[0]/E[0] < -20 && counter<1000;// && Start < 10 && Last < 47;// && Hits[Last]<10;    
    cutValue[3] = N > 25 && N/48 < 2.2;
//    cutValue[4] = resNN > 0.8;
    for(k = 1; k < NLAYERS; ++k)
    {
       //cutValue[2] = N > 25 && N/48 > 2.2 && Mz[k] > 30 && Mz[k] < 80;
       cutValue[3] = cutValue[3] && Hits[k] < 5;
    }
cutValue[4] = N<25;

/*for(k = 0; k < 1; ++k)
    {
       cutValue[2] = cutValue[2] &&  Mz[k] > 30 && Mz[k] < 80;
       //cutValue[4] = cutValue[4] && Hits[k] < 4;
    }
*/
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
/*
if(energy == 2) energy = 1, n[0]++;
       else if(energy == 1) energy = 2, n[1]++;
else if (energy==3) n[2]++;
      cout << n[0] << " " << n[1] << " " << n[2] << endl;
      */


      //if(energy == 2) energy = 1;
      //else if(energy == 1) energy = 2;
      x = hit->X;
      y = hit->Y;
      z = hit->Z;
      k = hit->K;

      KminusStart->Fill(k - Start);
      KProfile->Fill(k);

      r[0] = (E[0] > 0) ? TMath::Hypot((x-Mx[0]/E[0]), (y-My[0]/E[0])) : 0;
      r[k] = (E[k] > 0) ? TMath::Hypot((x-Mx[k]/E[k]), (y-My[k]/E[k])) : 0;
      if(E[0] > 0) hNR[0]->Fill(r[0]);
      if(E[k] > 0) hNR[k]->Fill(r[k]);
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
  KminusStart->Scale(1.0/counter);
  KminusStart->Write();
  KProfile->Write();
  Nlayer95->Write();
  Nlayer99->Write();

  dir->cd();

  // Close output file
//  while(counter<1001) 
//if (counter%1000 == 1){treeWriter->Write();}
treeWriter->Write();  
delete treeWriter;
  delete outputFile;
}
