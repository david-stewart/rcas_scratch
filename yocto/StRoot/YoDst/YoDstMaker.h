// simple class to read a data set and make cuts to it

#ifndef YoDstMaker_def
#define YoDstMaker_def

/* #include <TSystem> */
#include "StMaker.h"
#include "TString.h"
#include "TProfile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TChain.h"
#include "bad_runs.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "yocto.h"
#include "TRandom3.h"

class StMuDstMaker ;
class TFile        ;
class TH1F         ;

class MyJetDefinitionTerms ;

class YoDstMaker : public StMaker
{
 private:
  StMuDstMaker* mMuDstMaker ;  //  Make MuDst pointer available to member functions
  TString file_out_name;
  int mTriggerId;
  float m_TT_min;

  TH1F*  ev_cuts;
  TH1F*  vz;
  TH1F*  ln_ranking;
  TH1F*  tr_cuts;
  TH1F*  bbcES_percentile;
  TH1F*  zdcE_percentile;

  TH1F* track_pt;
  TH1F* refMult;

  TProfile* zdc_Nch_TOF;
  TProfile* bbc_Nch_TOF;

  TProfile* zdc_refMult;
  TProfile* bbc_refMult;

  TProfile* vz_Nch_all;
  TProfile* vz_Nch_TOF;
  TProfile* vz_Nch_BEMC;
  TProfile* vz_Nch_TOFBEMC;

  FILE* dlog;
  MyJetDefinitionTerms* m_bge;

  std::vector<int> bad_run_list;
  UInt_t        mEventsProcessed ;     //  Number of Events read and processed

  TFile* file_out ;

  TTree* tree;
  TClonesArray b_jets;
  TClonesArray b_tracks;
  TClonesArray b_tracks_noTOF;
  TClonesArray rand_int;
  YoEvent        event;

  long long int bemc_hits;

  TRandom3* yrand;

  unsigned int max_njets;
  unsigned int max_ntracks;
  unsigned int max_ntracks_noTOF;
  bool mTestConditions;
  bool mAllTOF;


  int debug;
 public:

  YoDstMaker(StMuDstMaker* maker, const char* fout_name, int triggerId, float TT_min) ;  //  Constructor
  virtual  ~YoDstMaker( ) ;       //  Destructor
  void setTestConditions();
  void setAllTOF();

  Int_t Init    ( ) ;                              //  Initiliaze the analysis tools ... done once
  Int_t Make    ( ) ;                              //  The main analysis that is done on each event
  Int_t Finish  ( ) ;                              //  Finish the analysis, close files, and clean up.

  ClassDef(YoDstMaker,1)                  //  Macro for CINT compatability
};

#endif
