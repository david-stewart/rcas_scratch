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
    TString          fbad_tower_file;
    TString          fbad_run_file;
    std::vector<int> fbad_run_list;
    std::vector<int> fbad_tower_list;
    TFile*           fout;

    TH1F*  fHgram_ev_cuts;
    TH1F*  fHgram_vz;
    TH1F*  fHgram_ln_ranking;

    //---   EmcTrigger
    int   fnbins_EmcTriggerId;
    float fbinlo_EmcTriggerId;
    float fbinhi_EmcTriggerId;

    TH1F*     fHgram_EmcTriggerId_nHT1;
    TH1F*     fHgram_EmcTriggerId_nHT2;

    TProfile* fProfE_EmcTriggerId_adcHT1;
    TProfile* fProfE_badT_EmcTriggerId_adcHT1;
    TProfile* fProfE_EmcTriggerId_adcHT2;


    // - EmcTrigger Spectra
    int       fnbins_EmcTrigger_adcSpectra;
    float     fbinlo_EmcTrigger_adcSpectra;
    float     fbinhi_EmcTrigger_adcSpectra;

    TH1F*     fHgram_EmcTrigger_adcSpectra_HT1;
    TH1F*     fHgram_badT_EmcTrigger_adcSpectra_HT1;
    TH1F*     fHgram_EmcTrigger_adcSpectra_HT2;


    // - Adc-Adc TH2F histograms
    int       fnbinsY_TriggerBTow_adc;
    float     fbinloY_TriggerBTow_adc;
    float     fbinhiY_TriggerBTow_adc;

    int       fnbinsY_BTowHit_EtSpectra;

    TH2F*     fHgram_EmcTrighits_phieta;
    TH2F*     fHgram_badT_EmcTrighits_phieta;

    TH2F*     fHgram_EmcTrigBTow_AdcAdc_all;   // 
    TH2F*     fHgram_badT_EmcTrigBTow_AdcAdc;   // 
    TH2F*     fHgram_EmcTrigBTow_AdcAdc_482;   // hi
    TH2F*     fHgram_EmcTrigBTow_AdcAdc_236;   // lo
    TH2F*     fHgram_EmcTrigBTow_AdcAdc_799;   // normal
    TH2F*     fHgram_EmcTrigBTow_AdcAdc_3836;  // normal

    TH2F*     fHgram_BTowHitId_Et;
    TH2F*     fHgram_badT_BTowHitId_Et;

    TH2F*     fHgram_BTowHit_adcEt_all;
    TH2F*     fHgram_badT_BTowHit_adcEt_all;
    TH2F*     fHgram_BTowHit_adcEt_482;
    TH2F*     fHgram_BTowHit_adcEt_236;
    TH2F*     fHgram_BTowHit_adcEt_799;
    TH2F*     fHgram_BTowHit_adcEt_3836;

    //---   BTowHit
    int       fnbins_BTowHitId;
    float     fbinlo_BTowHitId;
    float     fbinhi_BTowHitId;

    // - BTowHit Id
    TProfile* fProfE_BTowHitId_eHT1;     // Profile of hit energy for all fired towers
    TProfile* fProfE_BTowHitId_eHT2;     // energy of all non-fired towers

    TProfile* fProfE_BTowHitId_EtHT1;    // "transverse energy" (like pT)
    TProfile* fProfE_badT_BTowHitId_EtHT1;    // "transverse energy" (like pT)
    TProfile* fProfE_BTowHitId_EtHT2;    //  = energy/(float) TMath::CosH(eta)

    TProfile* fProfE_BTowHitId_adcHT1;   // Profile of hit energy for all fired towers
    TProfile* fProfE_badT_BTowHitId_adcHT1;   // Profile of hit energy for all fired towers
    TProfile* fProfE_BTowHitId_adcHT2;   // Profile of hit energy for all fired towers

    // - BTowHit Spectra
    int       fnbins_BTowHit_eSpectra;
    float     fbinlo_BTowHit_eSpectra;
    float     fbinhi_BTowHit_eSpectra;

    TH1F*     fHgram_BTowHit_eSpectra_HT1;
    TH1F*     fHgram_BTowHit_eSpectra_HT2;

    int       fnbins_BTowHit_EtSpectra;
    float     fbinlo_BTowHit_EtSpectra;
    float     fbinhi_BTowHit_EtSpectra;
    
    TH1F*     fHgram_BTowHit_EtSpectra_HT1;
    TH1F*     fHgram_badT_BTowHit_EtSpectra_HT1;
    TH1F*     fHgram_BTowHit_EtSpectra_HT2;

    int       fnbins_BTowHit_adcSpectra;
    float     fbinlo_BTowHit_adcSpectra;
    float     fbinhi_BTowHit_adcSpectra;

    TH1F*     fHgram_BTowHit_adcSpectra_HT1;
    TH1F*     fHgram_badT_BTowHit_adcSpectra_HT1;
    TH1F*     fHgram_BTowHit_adcSpectra_HT2;

    int       fnbins_BTowHit_EtMax;
    float     fbinlo_BTowHit_EtMax;
    float     fbinhi_BTowHit_EtMax;

    TH1F*     fHgram_BTowHit_EtMaxHT1;
    TH1F*     fHgram_badT_BTowHit_EtMaxHT1;
    TH1F*     fHgram_BTowHit_EtMaxHT2;

    int       fnbins_BTowHit_eMax;
    float     fbinlo_BTowHit_eMax;
    float     fbinhi_BTowHit_eMax;

    TH1F*     fHgram_BTowHit_eMaxHT1;
    TH1F*     fHgram_BTowHit_eMaxHT2;

    int       fnbins_BTowHit_adcMax;
    float     fbinlo_BTowHit_adcMax;
    float     fbinhi_BTowHit_adcMax;

    TH1F*     fHgram_BTowHit_adcMaxHT1;
    TH1F*     fHgram_badT_BTowHit_adcMaxHT1;
    TH1F*     fHgram_BTowHit_adcMaxHT2;

    TH1F*     fHgram_goodbadtower;

  public:
    SimpleETowQA(
        const char     *name,
        StPicoDstMaker *picoMaker,
        const char     *outName,
        int            triggerid,
        const char     *i_bad_tower_file,
        const char     *i_bad_run_file
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
