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
#include "bad_runs.h"
#include "bad_towers.h"
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

class ZeptoGrams : public StMaker {
    private:
    StPicoDstMaker *mPicoDstMaker;
    StPicoDst      *mPicoDst;
    TString    	mOutName;
    int mtriggerid;
    std::vector<int> bad_run_list;
    std::vector<int> bad_tower_list;
    const char* mProfile;
    /* TFile*	profile; */
    float mTT_threshold;
    TFile* fout;

   TTree* tree;
   TClonesArray b_jets;
   TClonesArray b_tracks;
   /* TClonesArray b_tracks_noTOF; */
   ZeEvent      event;

   MyJetDefinitionTerms* m_bge;

   TRandom3* frand;

   bool fdebug;
   int  fmax_njets;
   int  fmax_ntracks;

    TH1F*  ev_cuts;
	TH1F*  n_events;

    TH1F* tower_hits;
    TH1F* tower_hits_TH1;

    TH1F* max_e_tow;
    TH1F* i_max_e_tow;
    TH1F* max_e_tow_under10;
    TH1F* i_max_e_tow_under10;
    TH1F* i_tow_HT1_hgram;
    TProfile* i_tow_HT1_profile;


    TH1F*  tr_cuts; // not currently implemented
    TH1F*  vz;
    TH1F*  vz_9TT;
    TH1F*  ln_ranking;

    TH1F*  zdcSig;
    TH1F*  bbcSig;

    TH2F*    vz_zdcSig_course;
    TH2F*    vz_zdcSig;
    TH2F*    vz_bbcSig_course;
    TH2F*    vz_bbcSig;

    TProfile* vz_zdcSig_e; // error on mean
    TProfile* vz_bbcSig_e; // error on spread
    TProfile* vz_zdcSig_s;
    TProfile* vz_bbcSig_s;

    TH1F* track_pt;
    TH1F* refMult;

    TProfile* zdc_nTOF;
    TProfile* bbc_nTOF;

    TProfile* zdc_refMult;
    TProfile* bbc_refMult;
    TProfile* vz_refMult;

    TProfile* vz_nAll;
    TProfile* vz_nTOF;
    TProfile* vz_nBEMC;
    TProfile* vz_nTOFBEMC;

    TProfile* adc_energy;

    /* TH1F* isHT0_e[5]; */
    TH1F* isHT1_e;
    TH1F* isHT2_e;
    TH1F* isHT3_e;

    /* TH1F* isHT0_a[5]; */
    TH1F* isHT1_a;
    TH1F* isHT2_a;
    TH1F* isHT3_a;

    TH1F* adc_0;
    TH1F* energy_0;

    TH1F* ntracks_TOF;
    TH1F* ntracks_noTOF;
    TH1F* ntracks_BEMC;
    TH1F* ntracks_noBEMC;

    TH1F* bemc_match[5];
    TH1F* bemc_matchTT[5];
    TH1F* bemc_hasTT;

    TH1F* nBTOFMatch;
    TH1F* nBEMCMatch;


  public:
    ZeptoGrams(const char *name,
        StPicoDstMaker *picoMaker,
/* const char* prof,*/ const char *outName,
        int triggerid,
        float TT_threshold,
        bool  debug
    );
    virtual ~ZeptoGrams();
    
    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();
    
    /* void    DeclareHistograms(); */
    /* void    WriteHistograms(); */
    /* int     PicoSelect(StPicoEvent* mevent,float zvert); */ 
    /* int     RunIdAlter(int runid); */
    /* int	    NumBin(int number,float min,float max,float var); */
    /* TProfile*	east; */
    /* TProfile*	west; */
    /* TH1F*      	histPt; */
    /* TH2F*      	histVxy; */                  
    /* TH2F*      	histVxz; */
    /* TH2F*      	histVyz; */
    /* TH2F*      	Z_bbcAdcEast; */
    /* TH2F*      	Z_bbcAdcWest; */
    /* TH2F*	runid_AdcEast; */
    /* TH2F*	runid_AdcWest; */
    /* TH2F*	runid_RateEast; */
    /* TH2F*	runid_RateWest; */
    /* TProfile*	Rate_AdcEast; */
    /* TProfile*	Rate_AdcWest; */
    /* TProfile*	bbceastprofile; */
    /* TProfile*	bbcwestprofile; */
    /* TH1I*      	histmult; */
    /* TH1I*      	histcut; */
    ClassDef(ZeptoGrams, 1)
};

#endif
