//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "SimpleETowQA.h"
#include <iostream>
#define zmin -30
#define zmax 30


ClassImp(SimpleETowQA)

//-----------------------------------------------------------------------------
SimpleETowQA::SimpleETowQA(const char* name, StPicoDstMaker *picoMaker,
        /* const char* prof,*/ 
        const char* outName, 
        int triggerid)
  : StMaker(name), 
    mPicoDstMaker(picoMaker),
    mtriggerid(triggerid), 
    mOutName(outName),

    fnbins_EmcTriggers(4803),
    fbinlo_EmcTriggers(-1.5),
    fbinhi_EmcTriggers(4801.5),
     
    fnbinsY_EmcTriggerId_HTbreakout(9),   // 1: HT,
    fbinloY_EmcTriggerId_HTbreakout(-1.5),   // 1: HT1
    fbinhiY_EmcTriggerId_HTbreakout(7.5),   // 1: HT1

    fnbins_EmcTrigger_adcSpectra(310),
    fbinlo_EmcTrigger_adcSpectra(-10.5),
    fbinhi_EmcTrigger_adcSpectra(299.5),
    
    fnbins_BTowHitId_eSpectra(1000),
    fbinlo_BTowHitId_eSpectra(-5),
    fbinhi_BTowHitId_eSpectra(35),

    fnbins_BTowHit_adcSpectra(2000),
    fbinlo_BTowHit_adcSpectra(-10.5),

    fnbinsY_BTowHit_energy(40),
    fbinloY_BTowHit_energy(-5),
    fbinhiY_BTowHit_energy(35),

    fnbins_nEmcTriggers(100),
    fnbins_maxE(1000),
    fbinlo_maxE(-5),
    fbinhi_maxE(35),

    fnbins_maxA(2000),
    fbinlo_maxA(-0.5),
    fbinhi_maxA(1999.5)
{
    bad_run_list = get_bad_runs();
	bad_tower_list = get_bad_towers();
}

//----------------------------------------------------------------------------- 
SimpleETowQA::~SimpleETowQA()
{ /*  */ }

//----------------------------------------------------------------------------- 
Int_t SimpleETowQA::Init() {
    fout = new TFile(TString::Format("%s.root",mOutName.Data()).Data(),"RECREATE") ;
    flog = fopen(TString::Format("%s.log",mOutName.Data()).Data(),"w");

    ev_cuts = new TH1F("ev_cuts",
            ";0:db 1:!badrun 2:trig 3:rnk 4:zdc 5:vz10 6:(vz-vpd)<=3.0",
            7, -0.5, 6.5);

    vz = new TH1F("vz","primary vz distribution",200,-100,100);

    ln_ranking = new TH1F("ln_ranking",
            "log of best vertex ranking - perseving the negative sign",
            400,-20,20);

    fHgram2_EmcTriggerId_hits = new TH2F("EmcTriggerId_hits",
            TString::Format("%s%s",
            "n-times each trigger called#; (a/b/c) = (HT1/HT2/HT3);triggerId;",
            "-1:!HT&&!JP 0:bJP&&HT 1:a 2:b 3:ab 4:c 5:ac 6:bc 7:abc").Data(),
            fnbins_EmcTriggers, fbinlo_EmcTriggers, fbinhi_EmcTriggers,
            fnbinsY_EmcTriggerId_HTbreakout,
            fbinloY_EmcTriggerId_HTbreakout,
            fbinhiY_EmcTriggerId_HTbreakout);

    fHgram_EmcTriggerId_adcAll = new TProfile("EmcTriggerId_adcAll",
            "Emc Trigger Adcs, all triggers;TriggerId;adc",
            fnbins_EmcTriggers, fbinlo_EmcTriggers, fbinhi_EmcTriggers);

    fHgram_EmcTriggerId_adcHT1 = new TProfile("EmcTriggerId_adcHT1",
            "Emc Trigger Adcs, HT1 triggers;TriggerId;adc",
            fnbins_EmcTriggers, fbinlo_EmcTriggers, fbinhi_EmcTriggers);
    fHgram_EmcTriggerId_adcHT2 = new TProfile("EmcTriggerId_adcHT2",
            "Emc Trigger Adcs, HT3 triggers;TriggerId;adc",
            fnbins_EmcTriggers, fbinlo_EmcTriggers, fbinhi_EmcTriggers);
    fHgram_EmcTriggerId_adcHT3 = new TProfile("EmcTriggerId_adcHT3",
            "Emc Trigger Adcs, HT3 triggers;TriggerId;adc",
            fnbins_EmcTriggers, fbinlo_EmcTriggers, fbinhi_EmcTriggers);
    fHgram_EmcTriggerId_adcHT3_only = new TProfile("EmcTriggerId_adcHT3_only",
            "Emc Trigger Adcs, HT3_only triggers;TriggerId;adc",
            fnbins_EmcTriggers, fbinlo_EmcTriggers, fbinhi_EmcTriggers);

    fHgram_EmcTrigger_adcSpectra_all = new TH1F("EmcTrigger_adcSpectra_all",
            "EmcTrigger ADC Spectra;adc_{Trigger};N",
            fnbins_EmcTrigger_adcSpectra,
            fbinlo_EmcTrigger_adcSpectra,
            fbinhi_EmcTrigger_adcSpectra);

    fHgram_EmcTrigger_adcSpectra_HT1 = new TH1F("EmcTrigger_adcSpectra_HT1",
            "EmcTrigger ADC Spectra HT1;adc_{Trigger};N",
            fnbins_EmcTrigger_adcSpectra,
            fbinlo_EmcTrigger_adcSpectra,
            fbinhi_EmcTrigger_adcSpectra);
    fHgram_EmcTrigger_adcSpectra_HT2 = new TH1F("EmcTrigger_adcSpectra_HT2",
            "EmcTrigger ADC Spectra HT2;adc_{Trigger};N",
            fnbins_EmcTrigger_adcSpectra,
            fbinlo_EmcTrigger_adcSpectra,
            fbinhi_EmcTrigger_adcSpectra);
    fHgram_EmcTrigger_adcSpectra_HT3 = new TH1F("EmcTrigger_adcSpectra_HT3",
            "EmcTrigger ADC Spectra HT3;adc_{Trigger};N",
            fnbins_EmcTrigger_adcSpectra,
            fbinlo_EmcTrigger_adcSpectra,
            fbinhi_EmcTrigger_adcSpectra);
    fHgram_EmcTrigger_adcSpectra_HT3_only = new TH1F("EmcTrigger_adcSpectra_HT3_only",
            "EmcTrigger ADC Spectra HT3_{only} ;adc_{Trigger};N",
            fnbins_EmcTrigger_adcSpectra,
            fbinlo_EmcTrigger_adcSpectra,
            fbinhi_EmcTrigger_adcSpectra);

    fProfE_BTowHitId_eAll = new TProfile("BTowHitId_eAll_prof",
            "BTowHit energy per tower, all triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);

    fProfE_BTowHitId_e_noTrig = new TProfile("BTowHitId_e_noTrig",
            "BTowHit energy per tower, no Triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);

    fProfE_BTowHitId_adc_noTrig = new TProfile("BTowHitId_adc_noTrig",
            "BTowHit adc per tower, no Triggers;Tower ID;adc",
            48000, -0.5, 4799.5);

    fProfE_BTowHitId_eHT1 = new TProfile("BTowHitId_eHT1",
            "BTowHit energy per tower, HT1 triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);
    fProfE_BTowHitId_eHT2 = new TProfile("BTowHitId_eHT2",
            "BTowHit energy per tower, HT2 triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);
    fProfE_BTowHitId_eHT3 = new TProfile("BTowHitId_eHT3",
            "BTowHit energy per tower, HT3 triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);
    fProfE_BTowHitId_eHT3_only = new TProfile("BTowHitId_eHT3_only",
            "BTowHit energy per tower, HT3_only triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);

    fProfE_BTowHitId_adcAll = new TProfile("BTowHitId_adcAll",
            "BTowHit adc per tower, all triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);

    fProfE_BTowHitId_adcHT1 = new TProfile("BTowHitId_adcHT1",
            "BTowHit adc per tower, HT1 triggers;Tower ID;Energy",
            48000, -0.5, 4799.5);


    fHgram_BTowHit_eSpectra_noTrig = new TH1F("BTowHit_eSpectra_noTrig",
        "Spectra of BTowHit energy, no-triggers;energy;N",
        fnbins_BTowHitId_eSpectra,
        fbinlo_BTowHitId_eSpectra,
        fbinhi_BTowHitId_eSpectra);

    fHgram_BTowHit_eSpectra_all = new TH1F("BTowHit_eSpectra_all",
        "Spectra of BTowHit energy, all triggers;energy;N",
        fnbins_BTowHitId_eSpectra,
        fbinlo_BTowHitId_eSpectra,
        fbinhi_BTowHitId_eSpectra);

    fHgram_BTowHit_eSpectra_HT1 = new TH1F("BTowHit_eSpectra_HT1",
        "Spectra of BTowHit energy, HT1 triggers;energy;N",
        fnbins_BTowHitId_eSpectra,
        fbinlo_BTowHitId_eSpectra,
        fbinhi_BTowHitId_eSpectra);
    fHgram_BTowHit_eSpectra_HT2 = new TH1F("BTowHit_eSpectra_HT2",
        "Spectra of BTowHit energy, HT2 triggers;energy;N",
        fnbins_BTowHitId_eSpectra,
        fbinlo_BTowHitId_eSpectra,
        fbinhi_BTowHitId_eSpectra);
    fHgram_BTowHit_eSpectra_HT3 = new TH1F("BTowHit_eSpectra_HT3",
        "Spectra of BTowHit energy, HT3 triggers;energy;N",
        fnbins_BTowHitId_eSpectra,
        fbinlo_BTowHitId_eSpectra,
        fbinhi_BTowHitId_eSpectra);
    fHgram_BTowHit_eSpectra_HT3_only = new TH1F("BTowHit_eSpectra_HT3_only",
        "Spectra of BTowHit energy, HT3_only triggers;energy;N",
        fnbins_BTowHitId_eSpectra,
        fbinlo_BTowHitId_eSpectra,
        fbinhi_BTowHitId_eSpectra);

    fHgram_BTowHit_adcSpectra_noTrig = new TH1F("BTowHit_adcSpectra_noTrig",
        "BTowHit adc per tower, non triggered towers;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, 
        fnbins_BTowHit_adcSpectra + fbinlo_BTowHit_adcSpectra);
    fHgram_BTowHit_adcSpectra_HT1 = new TH1F("BTowHit_adcSpectra_HT1",
        "BTowHit adc per tower, HT1 triggers;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, 
        fnbins_BTowHit_adcSpectra + fbinlo_BTowHit_adcSpectra);
    fHgram_BTowHit_adcSpectra_HT2 = new TH1F("BTowHit_adcSpectra_HT2",
        "BTowHit adc per tower, HT2 triggers;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, 
        fnbins_BTowHit_adcSpectra + fbinlo_BTowHit_adcSpectra);
    fHgram_BTowHit_adcSpectra_HT3 = new TH1F("BTowHit_adcSpectra_HT3",
        "BTowHit adc per tower, HT3 triggers;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, 
        fnbins_BTowHit_adcSpectra + fbinlo_BTowHit_adcSpectra);
    fHgram_BTowHit_adcSpectra_HT3_only = new TH1F("BTowHit_adcSpectra_HT3_only",
        "BTowHit adc per tower, HT3_only triggers;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, 
        fnbins_BTowHit_adcSpectra + fbinlo_BTowHit_adcSpectra);


    fHgram_BTowHitId_eAll = new TH2F("BTowHitId_eAll",
        "Energy spectra in each tower, all triggers;Tower ID;Energy",
        4800, -0.5, 4799.5,
        fnbinsY_BTowHit_energy,
        fbinloY_BTowHit_energy,
        fbinhiY_BTowHit_energy);

    fHgram_nEmcTriggers = new TH1F("nEmcTriggers",
        "Spectra of number of Emc Triggers per Event;N_{Triggers};N",
        fnbins_nEmcTriggers, -0.5, fnbins_nEmcTriggers-0.5);

    fHgram_maxE_HT1 = new TH1F("maxE_HT1",
            "BTow maximum energy of HT1 trigger in an event;energy;N",
            fnbins_maxE, fbinlo_maxE, fbinhi_maxE);
    fHgram_maxE_HT2 = new TH1F("maxE_HT2",
            "BTow maximum energy of HT2 trigger in an event;energy;N",
            fnbins_maxE, fbinlo_maxE, fbinhi_maxE);
    fHgram_maxE_HT3 = new TH1F("maxE_HT3",
            "BTow maximum energy of HT3 trigger in an event;energy;N",
            fnbins_maxE, fbinlo_maxE, fbinhi_maxE);
    fHgram_maxE_HT3_only = new TH1F("maxE_HT3_only",
            "BTow maximum energy of HT3_only trigger in an event;energy;N",
            fnbins_maxE, fbinlo_maxE, fbinhi_maxE);

    fHgram_maxE_all = new TH1F("maxE_all",
            "maximum energy of any trigger in an event;energy;N",
            fnbins_maxE, fbinlo_maxE, fbinhi_maxE);
            
    fHgram_maxA_HT1 = new TH1F("maxA_HT1",
            "BTow maximum adc value, HT1 trigger;adc;N",
            fnbins_maxA, fbinlo_maxA, fbinhi_maxA);
    fHgram_maxA_HT2 = new TH1F("maxA_HT2",
            "BTow maximum adc value, HT2 trigger;adc;N",
            fnbins_maxA, fbinlo_maxA, fbinhi_maxA);
    fHgram_maxA_HT3 = new TH1F("maxA_HT3",
            "BTow maximum adc value, HT3 trigger;adc;N",
            fnbins_maxA, fbinlo_maxA, fbinhi_maxA);
    fHgram_maxA_HT3_only = new TH1F("maxA_HT3_only",
            "BTow maximum adc value, HT3_{only} trigger;adc;N",
            fnbins_maxA, fbinlo_maxA, fbinhi_maxA);


  return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t SimpleETowQA::Finish() {
    fout->Write();
    fout->Close();
    fclose(flog);
  return kStOK;
}
    
//----------------------------------------------------------------------------- 
void SimpleETowQA::Clear(Option_t *opt) { }

//----------------------------------------------------------------------------- 
Int_t SimpleETowQA::Make() {
    if(!mPicoDstMaker) {
        LOG_WARN << " No PicoDstMaker! Skip! " << endm;
        return kStWarn;
    }

    mPicoDst = mPicoDstMaker->picoDst();
    if(!mPicoDst) {
        LOG_WARN << " No PicoDst! Skip! " << endm;
        return kStWarn;
    }
    ev_cuts->Fill(0);
    StPicoEvent* mevent = mPicoDst->event();


    if (std::binary_search(bad_run_list.begin(), bad_run_list.end(), mevent->runId())) 
        return kStOk; 
    ev_cuts->Fill(1);

    ev_cuts->Fill(2);

    float rank = mevent->ranking();
    int sig = rank > 0 ? +1 : -1;
    float ln = TMath::Log(TMath::Abs(rank));
    ln_ranking->Fill(sig*ln);

    if (sig < 0) return kStOK;
    ev_cuts->Fill(3);

    float zdcX = mevent->ZDCx();
    if (zdcX < 4000. || zdcX > 22700.) { return kStOk; }
    ev_cuts->Fill(4);

    /* float sigZDC = 0; */
    /* for (int i = 0; i < 24; ++i){ */
    /*     sigZDC += */ 
    float z_vert = mevent->primaryVertex().z();
    vz->Fill(mevent->primaryVertex().z());

    if (TMath::Abs(z_vert) > 30) return kStOk;
    ev_cuts->Fill(5);

    if (TMath::Abs(z_vert - mevent->vzVpd())>3.0) return kStOk;
    ev_cuts->Fill(6);

    fHgram_nEmcTriggers->Fill(mPicoDst->numberOfEmcTriggers());
    float maxE_all = -1000;
    float maxE_HT1 = -1000;
    float maxE_HT2 = -1000;
    float maxE_HT3 = -1000;
    float maxE_HT3_only = -1000;
    float maxA_HT1 = -1000;
    float maxA_HT2 = -1000;
    float maxA_HT3 = -1000;
    float maxA_HT3_only = -1000;

    std::vector<int> emcTriggerIds;
    for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
        StPicoEmcTrigger* emcTrig = mPicoDst->emcTrigger(i);
        bool isJetPatch  = ( emcTrig->isJP0() || emcTrig->isJP1() || emcTrig->isJP2() );
        bool  HT1 = emcTrig->isHT1();
        bool  HT2 = emcTrig->isHT2();
        bool  HT3 = emcTrig->isHT3();
        bool isHighTower = (HT1 || HT2 || HT3);
        /* cout << "a7" << endl; */
        int   y_hit = (((int)emcTrig->flag())/2)%8;
        int   tId = emcTrig->id();
        /* bool  both = isHighTower && isJetPatch; */
        /* bool  neither = (!isHighTower) && (!isJetPatch); */
        if (!isHighTower){
            if (!isJetPatch) fHgram2_EmcTriggerId_hits->Fill(tId, -1);
            continue;
        }
        if (isJetPatch) fHgram2_EmcTriggerId_hits->Fill(tId, 1);
        emcTriggerIds.push_back(tId);

        float adc = emcTrig->adc();


        fHgram2_EmcTriggerId_hits->Fill(tId, y_hit);
        fHgram_EmcTriggerId_adcAll->Fill(tId, adc);
        fHgram_EmcTrigger_adcSpectra_all->Fill(adc);
        if (HT1){
            fHgram_EmcTriggerId_adcHT1->Fill(tId, adc);
            fHgram_EmcTrigger_adcSpectra_HT1->Fill(adc);
        } 
        if (HT2){
            fHgram_EmcTriggerId_adcHT2->Fill(tId, adc);
            fHgram_EmcTrigger_adcSpectra_HT2->Fill(adc);
        } 
        if (HT3){
            fHgram_EmcTriggerId_adcHT3->Fill(tId, adc);
            fHgram_EmcTrigger_adcSpectra_HT3->Fill(adc);
        } 
        if (HT3 && (!HT1) && (!HT2)){
            fHgram_EmcTriggerId_adcHT3_only->Fill(tId, adc);
            fHgram_EmcTrigger_adcSpectra_HT3_only->Fill(adc);
        } 

        if (tId < 1){ 
            fprintf(flog, "Warning, Trigger->id() = %i w/ adc %f\n", tId, adc); 
            continue; 
        }

        int   bTowId  = tId - 1;
        StPicoBTowHit* bTowHit = mPicoDst->btowHit(bTowId); 
        float tow_adc = bTowHit->adc();
        float tow_E   = bTowHit->energy();

        if (tow_E > maxE_all) maxE_all = tow_E;


        fProfE_BTowHitId_eAll->Fill(bTowId, tow_E);
        fProfE_BTowHitId_adcAll->Fill(bTowId, tow_adc);
        fHgram_BTowHit_eSpectra_all->Fill(tow_E);
        if (HT1){
            if (tow_E > maxE_HT1) maxE_HT1 = tow_E;
            if (tow_adc > maxA_HT1) maxA_HT1 = tow_adc;
            fProfE_BTowHitId_eHT1->Fill(bTowId, tow_E);
            fProfE_BTowHitId_adcHT1->Fill(bTowId, tow_adc);
            fHgram_BTowHit_eSpectra_HT1->Fill(tow_E);
            fHgram_BTowHit_adcSpectra_HT1->Fill(tow_adc);
        }
        if (HT2){ 
            if (tow_E > maxE_HT2) maxE_HT2 = tow_E;
            if (tow_adc > maxA_HT2) maxA_HT2 = tow_adc;
            fProfE_BTowHitId_eHT2->Fill(bTowId, tow_E);
            fHgram_BTowHit_eSpectra_HT2->Fill(tow_E);
            fHgram_BTowHit_adcSpectra_HT2->Fill(tow_adc);
        }
        if (HT3) {
            if (tow_E > maxE_HT3) maxE_HT3 = tow_E;
            if (tow_adc > maxA_HT3) maxA_HT3 = tow_adc;
            fProfE_BTowHitId_eHT3->Fill(bTowId, tow_E);
            fHgram_BTowHit_eSpectra_HT3->Fill(tow_E);
            fHgram_BTowHit_adcSpectra_HT3->Fill(tow_adc);
        }
        if (HT3 && (!HT1) && (!HT2)) {
            if (tow_E > maxE_HT3_only) maxE_HT3_only = tow_E;
            if (tow_adc > maxA_HT3_only) maxA_HT3_only = tow_adc;
            fHgram_BTowHit_eSpectra_HT3_only->Fill(tow_E);
            fProfE_BTowHitId_eHT3_only->Fill(bTowId, tow_E);
            fHgram_BTowHit_adcSpectra_HT3_only->Fill(tow_adc);
        }
        fHgram_BTowHitId_eAll->Fill(bTowId, tow_E);
    }
    if (maxE_all != -1000) fHgram_maxE_all->Fill(maxE_all);
    if (maxE_HT1 != -1000) fHgram_maxE_HT1->Fill(maxE_HT1);
    if (maxE_HT2 != -1000) fHgram_maxE_HT2->Fill(maxE_HT2);
    if (maxE_HT3 != -1000) fHgram_maxE_HT3->Fill(maxE_HT3);
    if (maxE_HT3_only != -1000) fHgram_maxE_HT3_only->Fill(maxE_HT3_only);
    if (maxA_HT1 != -1000) fHgram_maxA_HT1->Fill(maxA_HT1);
    if (maxA_HT2 != -1000) fHgram_maxA_HT2->Fill(maxA_HT2);
    if (maxA_HT3 != -1000) fHgram_maxA_HT3->Fill(maxA_HT3);
    if (maxA_HT3_only != -1000) fHgram_maxA_HT3_only->Fill(maxA_HT3_only);

    std::sort(emcTriggerIds.begin(), emcTriggerIds.end());
    for (int i = 0; i < 4800; i++){
        if (std::binary_search(emcTriggerIds.begin(), emcTriggerIds.end(), i)) continue;
        StPicoBTowHit* bTowHit = mPicoDst->btowHit(i); 
        fProfE_BTowHitId_e_noTrig->Fill((float) i, bTowHit->energy());
        fProfE_BTowHitId_adc_noTrig->Fill((float) i, bTowHit->adc());
        fHgram_BTowHit_eSpectra_noTrig->Fill(bTowHit->energy());
        fHgram_BTowHit_adcSpectra_noTrig->Fill(bTowHit->adc());
        /* if (true) */ 
            /* printf(" %i  e(%f)  adc(%i)\n", i, bTowHit->energy(), bTowHit->adc()); */
    }

	return kStOK;
}

