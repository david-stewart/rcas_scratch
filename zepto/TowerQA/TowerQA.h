#ifndef TowerQA_h
#define TowerQA_h

#include "StRoot/StPicoEvent/StPicoDst.h"
#include "StRoot/StPicoEvent/StPicoTrack.h"
#include "StRoot/StPicoEvent/StPicoEvent.h"
#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "StRoot/StPicoEvent/StPicoEmcTrigger.h"
#include "StRoot/StPicoEvent/StPicoBTowHit.h"
#include "StPicoEvent/StPicoBEmcPidTraits.h"
#include "StMaker.h"
#include "../ZeptoGrams/bad_runs.h"
#include "../ZeptoGrams/bad_towers.h"
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
class TowerQA : public StMaker {
private: 
    // histograms
    // 1. histograms of results for all towers
    // 2. histograms & profiles of results 
    private:
        StPicoDstMaker *mPicoDstMaker;
        int mtriggerid;
        StPicoDst      *mPicoDst;
        TString    	     mOutName;
        std::vector<int> bad_run_list;
        std::vector<int> bad_tower_list;
        const char*      mProfile;
        TFile*           fout;

    // from StPicoEmcTrigger 
    int   fnbins_EmcTowers;
    float fbinlo_EmcTowers;
    float fbinhi_EmcTowers;

    TH1F* fHgram_EmcTriggerId_all; // 0-4800 -- how many called. --> read out < 0

    TH1F* fHgram_EmcTriggerId_HT1; // 0-4800 -- how many called. --> read out < 0
    TH1F* fHgram_EmcTriggerId_HT2; // 0-4800 -- how many called. --> read out < 0
    TH1F* fHgram_EmcTriggerId_HT3; // 0-4800 -- how many called. --> read out < 0

    TH2F* fHgram2_EmcTriggerId_HTbreakout;   // 0-4800 x 8 (each combination of trigger that can fire:
   // int   fnbinsX_EmcTriggerId_all;   // 0: (none) -- get this by (trigger->flag()/2)%8
    int   fnbinsY_EmcTriggerId_HTbreakout;   // 1: HT1
    float fbinloY_EmcTriggerId_HTbreakout;   // 2:       HT2
    float fbinhiY_EmcTriggerId_HTbreakout;   // 3: HT1 & HT2
    //float fbinloX_EmcTriggerId_all; // 4:             HT3
    //float fbinhiX_EmcTriggerId_all; // 5: HT1       & HT3
                                      // 6:       HT2 & HT3
                                      // 7: HT1 & HT2 & HT3

    TProfile* fProfE_EmcTriggerId_alladc; // Profile with error on mean adc values
    TProfile* fProfS_EmcTriggerId_alladc; // Profile with std. on distribution

    // from StPicoBTowHit
    TProfile* fProfE_BTowHitE_all; // Profile of hit energy for all fired towers

    TProfile* fProfE_BTowHitE_HT1; // 
    TProfile* fProfE_BTowHitAdc_HT1; // 

    TProfile* fProfE_BTowHitE_HT2; // 
    TProfile* fProfE_BTowHitAdc_HT2; // 

    TProfile* fProfE_BTowHitE_HT3; // 
    TProfile* fProfE_BTowHitAdc_HT3; // 

    int       fnbinsY_BTowHitE;
    float     fbinloY_BTowHitE;
    float     fbinhiY_BTowHitE;

    TH2F*     fHgram2_BTowHitE_All; // -10.5 to 4800.5 x -10 to 35
    TH2F*     fHgram2_BTowHitE_HT1; // -5.5 to 4800.5 x -10 to 35

    TH1F*     fProfE_EmcTrigger_adcall; // profile of adc values -- mean and standard error mean
    TH1F*     fProfE_EmcTrigger_adcHT1; // profile of adc values -- mean and error on mean
    TH1F*     fProfE_EmcTrigger_adcHT2; // profile of adc values -- mean and error on mean
    TH1F*     fProfE_EmcTrigger_adcHT3; // profile of adc values -- mean and error on mean

    int       fnbins_BTowHitE;
    TH1F*     fHgram_BTowHitE_All;
    TH1F*     fHgram_BTowHitE_HT1;
    TH1F*     fHgram_BTowHitE_HT2;
    TH1F*     fHgram_BTowHitE_HT3;

    TH1F*     fHgram_nEmcTriggers;
    int       fnbins_nEmcTriggers;

    TH1F*  ev_cuts;
	TH1F*  n_events;
    TH1F*  vz;
    TH1F*  ln_ranking;

  public:
    TowerQA(const char *name,
        StPicoDstMaker *picoMaker,
        const char     *outName,
        int            triggerid
    );
    virtual ~TowerQA();

    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();
    

  private:
    ClassDef(TowerQA, 1)
};

#endif
