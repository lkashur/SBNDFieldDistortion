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
int get_face(float x, float y, float z);

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
  char *cryo;
  inputfilename = argv[1];
  cryo = (char*) argv[2];

  char* treename;
  treename = (char*) "caloskim/TrackCaloSkim";
     
  TChain* inputfiles = new TChain(treename);
  AddFiles(inputfiles, inputfilename);

  // Create rdataframe
  //ROOT::EnableImplicitMT();
  ROOT::RDataFrame rdf(*inputfiles);  

  // TPC info
  auto rdf_tpc = rdf.Define("tpc", [&] (float hit_max_time_p2_tpcE, float hit_min_time_p2_tpcE, float hit_max_time_p2_tpcW, float hit_min_time_p2_tpcW)->int{return get_tpc_ew(hit_max_time_p2_tpcE - hit_min_time_p2_tpcE, hit_max_time_p2_tpcW - hit_min_time_p2_tpcW);}, {"hit_max_time_p2_tpcE", "hit_min_time_p2_tpcE", "hit_max_time_p2_tpcW", "hit_min_time_p2_tpcW"});
  
  // Throughgoing tracks
  auto rdf_tg = rdf_tpc.Filter("selected == 2")
                       .Filter("tpc != -1");

  // Find faces that each end point 
  auto rdf_faces = rdf_tg.Define("start_face", [&] (float start_x, float start_y, float start_z)->int{return get_face(start_x, start_y, start_z);}, {"start.x", "start.y", "start.z"}).Define("end_face", [&] (float end_x, float end_y, float end_z)->int{return get_face(end_x, end_y, end_z);}, {"end.x", "end.y", "end.z"});

  // Save new output tree
  string output_file_text = string("endpoints.root");
  rdf_faces.Snapshot("endpointtree", output_file_text.c_str(), {"tpc", "start.x", "start.y", "start.z", "end.x", "end.y", "end.z", "start_face", "end_face"});
  //rdf_faces.Snapshot("offsettree", output_file_text.c_str(), {"tpc", "start.x", "start.y", "start.z", "end.x", "end.y", "end.z", "start_face", "end_face", "truth.p.start.x", "truth.p.start.y", "truth.p.start.z", "truth.p.end.x", "truth.p.end.y", "truth.p.end.z"});

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

int get_face(float x, float y, float z)
{
  // face coordinates
  float top = 203.7325;
  float bottom = -203.7325;
  float upstream = 0.0;
  float downstream = 509.4;
  
  // Get closest face
  float dist_to_top = fabs(y - top);
  float dist_to_bottom = fabs(y - bottom);
  float dist_to_upstream = fabs(z - upstream);
  float dist_to_downstream = fabs(z - downstream);
  float arr [] = {dist_to_top, dist_to_bottom, dist_to_upstream, dist_to_downstream};
  int n = sizeof(arr) / sizeof(arr[0]);
  sort(arr, arr + n);

  if(arr[0] == arr[1])
    {
      return -1;
    }
  else if(arr[0] == dist_to_bottom)
    {
      return 0;
    }
  else if(arr[0] == dist_to_top)
    {
      return 1;
    }
  else if(arr[0] == dist_to_upstream)
    {
      return 2;
    }
  else if(arr[0] == dist_to_downstream)
    {
      return 3;
    }
  else
    {
      return -1;
    }
}
