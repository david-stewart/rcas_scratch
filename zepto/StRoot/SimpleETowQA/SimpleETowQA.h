#ifndef SimpleETowQA_h
#define SimpleETowQA_h

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
class SimpleETowQA : public StMaker {

    // histograms
    // 1. histograms of results for all towers
    // 2. histograms & profiles of results 
    private:
    FILE* flog;
    StPicoDstMaker  *mPicoDstMaker;
    int              mtriggerid;
    StPicoDst       *mPicoDst;
    TString    	     mOutName;
    std::vector<int> bad_run_list;
    std::vector<int> bad_tower_list;
    const char*      mProfile;
    TFile*           fout;

    TH1F*  ev_cuts;
    TH1F*  vz;
    TH1F*  ln_ranking;

    // from StPicoEmcTrigger 
    int   fnbins_EmcTriggers;
    float fbinlo_EmcTriggers;
    float fbinhi_EmcTriggers;

    TH2F* fHgram2_EmcTriggerId_hits;        // 0-4800 x 8 (each combination of trigger that can fire:
    int   fnbinsY_EmcTriggerId_HTbreakout;  // 1: HT1
    float fbinloY_EmcTriggerId_HTbreakout;  // 2:       HT2
    float fbinhiY_EmcTriggerId_HTbreakout;  // 3: HT1 & HT2
    //float fbinloX_EmcTriggerId_all;       // 4:             HT3
    //float fbinhiX_EmcTriggerId_all;       // 5: HT1       & HT3
                                            // 6:       HT2 & HT3
                                            // 7: HT1 & HT2 & HT3
    TProfile* fHgram_EmcTriggerId_adcAll;
    TProfile* fHgram_EmcTriggerId_adcHT1;
    TProfile* fHgram_EmcTriggerId_adcHT2;
    TProfile* fHgram_EmcTriggerId_adcHT3;
    TProfile* fHgram_EmcTriggerId_adcHT3_only;

    int       fnbins_EmcTrigger_adcSpectra;
    float     fbinlo_EmcTrigger_adcSpectra;
    float     fbinhi_EmcTrigger_adcSpectra;

    TH1F*     fHgram_EmcTrigger_adcSpectra_all;
    TH1F*     fHgram_EmcTrigger_adcSpectra_HT1;
    TH1F*     fHgram_EmcTrigger_adcSpectra_HT2;
    TH1F*     fHgram_EmcTrigger_adcSpectra_HT3;
    TH1F*     fHgram_EmcTrigger_adcSpectra_HT3_only;

    // from StPicoBTowHit
    TProfile* fProfE_BTowHitId_eAll; // Profile of hit energy for all fired towers
    TProfile* fProfE_BTowHitId_eHT1; // Profile of hit energy for all fired towers
    TProfile* fProfE_BTowHitId_eHT2; // energy of all non-fired towers
    TProfile* fProfE_BTowHitId_eHT3; // energy of all non-fired towers
    TProfile* fProfE_BTowHitId_eHT3_only; // energy of all non-fired towers
    TProfile* fProfE_BTowHitId_e_noTrig; // energy of all non-fired towers
    TProfile* fProfE_BTowHitId_adc_noTrig; // energy of all non-fired towers


    TProfile* fProfE_BTowHitId_adcAll; // Profile of hit energy for all fired towers
    TProfile* fProfE_BTowHitId_adcHT1; // Profile of hit energy for all fired towers

    int       fnbins_BTowHitId_eSpectra;
    float     fbinlo_BTowHitId_eSpectra;
    float     fbinhi_BTowHitId_eSpectra;


    TH1F*     fHgram_BTowHit_eSpectra_noTrig;
    TH1F*     fHgram_BTowHit_eSpectra_all;
    TH1F*     fHgram_BTowHit_eSpectra_HT1;
    TH1F*     fHgram_BTowHit_eSpectra_HT2;
    TH1F*     fHgram_BTowHit_eSpectra_HT3;
    TH1F*     fHgram_BTowHit_eSpectra_HT3_only;

    int       fnbins_BTowHit_adcSpectra;
    float     fbinlo_BTowHit_adcSpectra;
    float     fbinhi_BTowHit_adcSpectra;
    TH1F*     fHgram_BTowHit_adcSpectra_noTrig;
    TH1F*     fHgram_BTowHit_adcSpectra_HT1;
    TH1F*     fHgram_BTowHit_adcSpectra_HT2;
    TH1F*     fHgram_BTowHit_adcSpectra_HT3;
    TH1F*     fHgram_BTowHit_adcSpectra_HT3_only;

    int       fnbinsY_BTowHit_energy;
    float     fbinloY_BTowHit_energy;
    float     fbinhiY_BTowHit_energy;
    TH2F*     fHgram_BTowHitId_eAll; // TH2 histogram (too big?)

    int       fnbins_nEmcTriggers;
    TH1F*     fHgram_nEmcTriggers;

    int       fnbins_maxE;
    float     fbinlo_maxE;
    float     fbinhi_maxE;
    TH1F*     fHgram_maxE_HT1;
    TH1F*     fHgram_maxE_HT2;
    TH1F*     fHgram_maxE_HT3;
    TH1F*     fHgram_maxE_HT3_only;
    TH1F*     fHgram_maxE_all;

    int       fnbins_maxA;
    float     fbinlo_maxA;
    float     fbinhi_maxA;
    TH1F*     fHgram_maxA_HT1;
    TH1F*     fHgram_maxA_HT2;
    TH1F*     fHgram_maxA_HT3;
    TH1F*     fHgram_maxA_HT3_only;

  public:
    SimpleETowQA(const char *name,
        StPicoDstMaker *picoMaker,
        const char     *outName,
        int            triggerid
    );
    virtual ~SimpleETowQA();

    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();
    

  private:
    ClassDef(SimpleETowQA, 1)
};

#endif
