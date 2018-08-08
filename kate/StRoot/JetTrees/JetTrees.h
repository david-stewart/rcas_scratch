#ifndef StMyAnalysisMaker_h
#define StMyAnalysisMaker_h

#include "TTree.h"
#include "StRoot/StPicoDstMaker/StPicoDst.h"
#include "StRoot/StPicoEvent/StPicoTrack.h"
#include "StRoot/StPicoEvent/StPicoEvent.h"
#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "StRoot/StPicoEvent/StPicoEmcTrigger.h"
#include "StRoot/StPicoEvent/StPicoBTowHit.h"
#include "StPicoEvent/StPicoBEmcPidTraits.h"
#include "StMaker.h"
/* #include "bad_runs.h" */
/* #include "bad_towers.h" */
#include "StThreeVectorF.hh"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH1I.h"
#include "TProfile.h"
#include "TFile.h"
#include <iostream>
#include "StThreeVectorF.hh"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/projection/StEmcPosition.h"
#include "jet_tree.h"

#include "TRandom3.h"


/* #include "StPicoEvent/StPicoBTowHit.h" */

class StPicoDst;
class StPicoDstMaker;
class StPicoEvent;
class TString;
class TH1F;
class TH2F;
class TH3F;
class TH1I;
class TProfile;
class MyJetDefinitionTerms ;

class JetTrees : public StMaker {
    private:
    StPicoDstMaker* fPicoDstMaker;
    TString         fOutName;
    int             ftriggerid;
    const char*     fbad_tower_file;
    const char*     fbad_run_file;
    bool            fdebug;
    bool            fmakePreCuts;

    float fTT_threshold;
    float f_jetR;
    float f_jet_etaMax;
    TClonesArray b_jets;
    long long int fEventsProcessed;

    StPicoDst*       fPicoDst;
    std::vector<int> fbad_run_list;
    std::vector<int> fbad_tower_list;
    TFile* fout;
    FILE*  flog;
    FILE*  dlog;

    TTree*       ftree;
    TTree*       fpc_tree; // a tree which will hold the pre-cut BBCsig, ZDCsig, Zvert, max_TT_Et, 
                               // and NTracks prior to requiring a minimum TT_Et
    PreCut       fpc;
    JtEvent      fevent;

    MyJetDefinitionTerms* f_bge;
    TRandom3* frand;
    TH1F*  fHgram_ev_cuts;
    TH1F*  fHgram_tr_cuts;    // not currently implemented

    /* TH1F*  fHgram_bbcES_percentile; */
    /* TH1F*  fHgram_zdcE_percentile; */
    TH1F*      fHgram_ev_prun;
    TProfile*  fProfE_vz_prun;
    TProfile*  fProfE_zdcSumAdcEast_prun;
    TProfile*  fProfE_bbcAdcES_prun;
    TProfile*  fProfE_zdcX_prun;


    TH1F*  fHgram_vz;
    TH1F*  fHgram_zdcX;
    TH1F*  fHgram_ln_ranking;
    TH1F*  fHgram_track_pt;
    TH1F*  fHgram_BTowHit_pt; // all hits
    TH2F*  fHgram_BTowHit_phieta; // 
    TH2F*  fHgram_TT_phieta; // 

  public:
    JetTrees(
        const char*     name,
        StPicoDstMaker* picoMaker,
        const char*     outName,
        int             triggerid,
        const char*     i_bad_tower_file,
        const char*     i_bad_run_file,
        bool            i_debug,
        bool            i_makePreCuts 
    );
    virtual ~JetTrees();
    
    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();

    ClassDef(JetTrees, 1)
};





#endif
