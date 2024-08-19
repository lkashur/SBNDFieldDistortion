// C++ Includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <cmath>

// ROOT Includes
#include "TROOT.h"
#include "ROOT/TThreadedObject.hxx"
#include <ROOT/RDataFrame.hxx>
#include "TTree.h"
#include "TFile.h"
#include "TSystemFile.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TVirtualFFT.h"
#include "TFile.h"
#include "TChain.h"
#include "TSystemDirectory.h"

using namespace std;

// Function Declaration
void AddFiles(TChain *ch, const char *fileList);
int get_tpc_ew(float a, float b);
ROOT::RVec<float> get_track_hits(ROOT::RVec<bool> htraj, ROOT::RVec<float> hits, int tpc, ROOT::RVec<unsigned short> tpcs);

//////////////////////////////////////////////////
/// Main Analysis
//////////////////////////////////////////////////
int main(int argc, char** argv)
{
  
  // Constants
  float top = 203.7325;
  float bottom = -203.7325;
  float upstream = 0.0;
  float downstream = 509.4;

  // Load data
  Char_t *inputfilename = (Char_t*)"";
  inputfilename = argv[1];

  char* treename;
  treename = (char*) "caloskim/TrackCaloSkim";
     
  TChain* inputfiles = new TChain(treename);
  AddFiles(inputfiles, inputfilename);

  // Create rdataframe
  //ROOT::EnableImplicitMT();
  ROOT::RDataFrame rdf(*inputfiles);  

  // TPC info
  auto rdf_tpc = rdf.Define("tpc", [&] (float hit_max_time_p2_tpcE, float hit_min_time_p2_tpcE, float hit_max_time_p2_tpcW, float hit_min_time_p2_tpcW)->int{return get_tpc_ew(hit_max_time_p2_tpcE - hit_min_time_p2_tpcE, hit_max_time_p2_tpcW - hit_min_time_p2_tpcW);}, {"hit_max_time_p2_tpcE", "hit_min_time_p2_tpcE", "hit_max_time_p2_tpcW", "hit_min_time_p2_tpcW"});
  
  // Selected tracks
  int sel = stoi(argv[2]);
  auto rdf_sel = rdf_tpc.Filter([&](int selected){ return selected == (signed char) sel; }, {"selected"} )
                        .Filter("tpc != -1");

  // SpacePoints
  auto rdf_hits = rdf_sel.Define("hit_xs", [&] (ROOT::RVec<bool> htraj, ROOT::RVec<float> hits, int tpc, ROOT::RVec<unsigned short> tpcs)->ROOT::RVec<float>{return get_track_hits(htraj, hits, tpc, tpcs);}, {"hits2.ontraj", "hits2.h.sp.x", "tpc", "hits2.h.tpc"})
    .Define("hit_ys", [&] (ROOT::RVec<bool> htraj, ROOT::RVec<float> hits, int tpc, ROOT::RVec<unsigned short> tpcs)->ROOT::RVec<float>{return get_track_hits(htraj, hits, tpc, tpcs);}, {"hits2.ontraj", "hits2.h.sp.y", "tpc", "hits2.h.tpc"})
    .Define("hit_zs", [&] (ROOT::RVec<bool> htraj, ROOT::RVec<float> hits, int tpc, ROOT::RVec<unsigned short> tpcs)->ROOT::RVec<float>{return get_track_hits(htraj, hits, tpc, tpcs);}, {"hits2.ontraj", "hits2.h.sp.z", "tpc", "hits2.h.tpc"});
  
  // Save new output tree
  string output_file_text = string("sps.root");
  rdf_hits.Snapshot("sptree", output_file_text.c_str(), {"tpc", "hit_xs", "hit_ys", "hit_zs"});

  return 0;

} // end main

/////////////////////////////////////////////////
/// Functions
/////////////////////////////////////////////////

// Add intput files to TChain
void AddFiles(TChain *ch, const char *fileList)
{
  ifstream InFile(fileList);
  vector<string> fileVec;
  string line;
  int n_files = 0;
  while(getline(InFile, line))
    {
      fileVec.push_back(line);
      n_files++;
    }

  for(unsigned int iFile=0; iFile<fileVec.size(); ++iFile)
    {
      //cout << fileVec[iFile].c_str() << endl;
      ch->AddFile(fileVec[iFile].c_str());
    }
  return;
}

int get_tpc_ew(float a, float b)
{
  if(a > b)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

ROOT::RVec<float> get_track_hits(ROOT::RVec<bool> htraj, ROOT::RVec<float> hits, int tpc, ROOT::RVec<unsigned short> tpcs)
{
  ROOT::RVec<float> sel_hits;
  for(int i=0; i < hits.size(); i++)
    {
      if((htraj[i] == true) && (((tpc == 0) && (tpcs[i] == 0)) || ((tpc == 1) && (tpcs[i] == 1))))
        {
          sel_hits.push_back(hits[i]);
        }
    }
  return sel_hits;
}
