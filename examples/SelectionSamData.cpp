#include <iostream>
#include <utility>
#include <vector>

#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"

#include "TString.h"

#include "TH2.h"
#include "THStack.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include "classes/ILCClasses.h"

#include "ExRootAnalysis/ExRootTreeReader.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"
#include "ExRootAnalysis/ExRootResult.h"
#include "ExRootAnalysis/ExRootUtilities.h"
#include "ExRootAnalysis/ExRootProgressBar.h"

using namespace std;

//------------------------------------------------------------------------------

// Here you can put your analysis macro

#include "examples/SelectionSamData.C"

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  char *appName = "SelectionSamData";

  if(argc != 4)
  {
    cout << " Usage: " << appName << " input_file"  << " output_file" << " cut_number" << endl;
    cout << " input_file - input file in ROOT format ('ILC' tree)," << endl;
    cout << " output_file - output file in ROOT format," << endl;
    cout << " cut_number - cut number." << endl;
    return 1;
  }

  gROOT->SetBatch();

  int appargc = 1;
  char *appargv[] = {appName};
  TApplication app(appName, &appargc, appargv);

//------------------------------------------------------------------------------

// Here you call your macro's main function 

  SelectionSamData(argv[1], argv[2], atoi(argv[3]));

//------------------------------------------------------------------------------

}



