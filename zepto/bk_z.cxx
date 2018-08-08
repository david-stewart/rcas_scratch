#include "ZeptoGrams.h"
#define zmin -30
#define zmax 30

ClassImp(ZeptoGrams)

//-----------------------------------------------------------------------------
ZeptoGrams::ZeptoGrams(const char* name, StPicoDstMaker *picoMaker,
        /* const char* prof,*/ 
        const char* outName, 
        int triggerid, 
        float TT_threshold)
  : StMaker(name), 
    mPicoDstMaker(picoMaker),
    mtriggerid(triggerid), 
    mOutName(outName),
    mTT_threshold(TT_threshold)
{
    /* mPicoDstMaker = picoMaker; */
    /* mPicoDst = 0; */
//  mProfile = prof;
    /* mOutName = outName; */
    bad_run_list = get_bad_runs();
}

//----------------------------------------------------------------------------- 
ZeptoGrams::~ZeptoGrams()
{ /*  */ }

//----------------------------------------------------------------------------- 
Int_t ZeptoGrams::Init() {
    fout = new TFile(mOutName.Data(),"RECREATE") ;

    ev_cuts = new TH1F("ev_cuts",
            ";0:db 1:!badrun 2:trig 3:rnk 4:zdc 5:vz10 6:(vz-vpd)<=3.0 7:no30tr 8:TT 9:TT_TOF",
            10, -0.5, 9.5);

  /* Track cuts - Yanfang
   * 0: total
   * 1: pt > 0.2
   * 2: |dca| < 1
   * 3: |eta| < 1
   * -- cut -- 3: NHitsFitRatio >= 10  // Yanfang's cuts
   * 4: NHitsRatio > 0.52 // typical of JetCorr
   * 5: TOF Matched
   * 6: FYI: eta<=0.9
   * 7: TOF-Matched
   */
    vz = new TH1F("vz","primary vz distribution",200,-100,100);

    tr_cuts = new TH1F("tr_cuts",
            ";0:tot 1:pt>0.2 2:dca<1 3:eta<1.0 4:NHitsRat>0.52 5:eta<0.9 6:TOF-Matched",
            7, -0.5, 6.5);


    ln_ranking = new TH1F("ln_ranking",
            "log of best vertex ranking - perseving the negative sign",
            400,-20,20);

    bbcSig = new TH1F("bbcSig", "bbc ES, all Min Bias events that pass basic cuts;bbcES;N",
           10000, 0, 70000);

    zdcSig = new TH1F("zdcSig", "zdc.adcSum(east), all Min Bias events that pass basic cuts;zdc.adcSum(east);N",
           4061,-0.5,4059.5);

    vz_zdcSig_course = new TH2F("vz_zdcSig_course", ";Vertex_{z};zdc East Signal",
           100,-100,100,100,0,4060);
    vz_zdcSig        = new TH2F("vz_zdcSig", ";Vertex_{z};zdc East Signal",
           120,-30,30,200,0,4060);

    vz_bbcSig_course = new TH2F("vz_bbcSig_course", ";Vertex_{z};bbc.adc",
           100,-100,100,100,0,70000);
    vz_bbcSig        = new TH2F("vz_bbcSig", ";Vertex_{z};bbc East Signal",
           120,-30,30,200,0,70000);

    vz_zdcSig_e = new TProfile("vz_zdcSig_e","error on mean;vz;zdc east signal", 120,-30,30,"e");
    vz_zdcSig_s = new TProfile("vz_zdcSig_s","spread about mean;vz;zdc east signal", 120,-30,30,"p");

    vz_bbcSig_e = new TProfile("vz_bbcSig_e","error on mean;vz;bbc east signal", 120,-30,30,"e");
    vz_bbcSig_s = new TProfile("vz_bbcSig_s","spread about mean;vz;bbc east signal", 120,-30,30,"p");
    
    track_pt   = new TH1F("track_pt","all tracks that pass basic cuts, not TOF-matched;track p_T;N",200,0,20);
    refMult    = new TH1F("refMult", "ref_mult", 3000,0,300);

    zdc_nTOF = new TProfile("zdc_nTOF", 
            "tracks (TOF match) per event per zdcE;zdcSig;N_{TOF-matched}^{tracks}",
            200,0,4060);

    bbc_nTOF = new TProfile("bbc_nTOF", 
            "tracks (TOF match) per event per zdcE;bbcSig;N_{TOF-matched}^{tracks}",
            200,0,70000);

    zdc_refMult = new TProfile("zdc_refMult", 
            "tracks (TOF match) per event per zdcE;zdcSig;N_{refMult}^{tracks}",
            200,0,4060);

    bbc_refMult = new TProfile("bbc_refMult", 
            "tracks (TOF match) per event per zdcE;bbcSig;N_{refMult}^{tracks}",
            200,0,70000);

    vz_refMult = new TProfile("vz_refMult", 
            "refMult per event per vz;vz;N_{refMult}^{tracks}",
            200,0,70000);

    vz_nAll = new TProfile("vz_nAll",";V_{z};N_{all good primary}^{tracks}", 120,-30,30,"e");
    vz_nTOF = new TProfile("vz_nTOF",";V_{z};N_{TOF-matched}^{tracks}", 120,-30,30,"e");
    vz_nBEMC = new TProfile("vz_nBEMC",";V_{z};N_{TOF-matched}^{tracks}", 120,-30,30,"e");

    nBTOFMatch = new TH1F("nBTOFMatch",";nBTOFMatch;N",100, -0.5, 99.5);
    nBEMCMatch = new TH1F("nBEMCMatch",";nBEMCMatch;N",100, -0.5, 99.5);
    /* vz_nTOF = new TProfile("vz_nTOF",";V_{z};N_{all good primary tracks}", 120,-30,30,"e"); */
    /* vz_bbcSig_s = new TProfile("vz_bbcSig_s","spread about mean;vz;bbc east signal", 120,-30,30,"p"); */



    /* bbc_refMult = new TProfile("bbc_refMult", "refernece multiplicity per event per bbcES", */
    /*        10000, 0, 70000); */

   /* zdc_refMult = new TProfile("zdc_refMult", "reference multiplicity per event per zdcE signal", */
   /*         4060,0,4060); */

    // new cuts

    /* vz_nAll = new TProfile("vz_nAll","all good tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000); */
    /* vz_nTOF  = new TProfile("vz_nTOF","TOF-match tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000); */
    /* vz_nBEMC = new TProfile("vz_nBEMC","BEMC-match tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000); */
    /* vz_nTOFBEMC = new TProfile("vz_nTOFBEMC","(TOF and BEMC)-match tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000); */


    // tower results
    tower_hits = new TH1F("tower_hits","tower_hits;n_tower;hits",5000,-0.5,4999.5);
    /* adc_energy = new TProfile("adc_energy",";adc;energy",200,-0.5, 199.5); */
    for (int i = 0; i < 5; ++i){
        adc_energy[i] = new TProfile(
                TString::Format("adc_energy_2l%i",i).Data(),
                TString::Format("energy per adc, tower + %i (2 - %i);adc;N",2-i,i).Data(),
                200,-0.5,199.5);
    }
    
    /* isHT0_adc  = new TH1F("isHT0_adc","HT0 triggers;adc;N",200,-0.5,199.5); */
    /* isHT1_adc  = new TH1F("isHT1_adc","HT1 triggers;adc;N",200,-0.5,199.5); */
    /* isHT2_adc  = new TH1F("isHT2_adc","HT2 triggers;adc;N",200,-0.5,199.5); */
    /* isHT3_adc  = new TH1F("isHT3_adc","HT3 triggers;adc;N",200,-0.5,199.5); */

    for (int i = 0; i < 5; ++i){
        adc_0[i] = new TH1F(
                TString::Format("adc_2l%i",i).Data(),
                TString::Format("adc hits, tower + %i (2 - %i);adc;N",2-i,i).Data(),
                500,-0.5,599.5);
    }
    for (int i = 0; i < 5; ++i){
        energy_0[i] = new TH1F(
                TString::Format("energy_2l%i",i).Data(),
                TString::Format("energy, tower + %i,  (2 - %i);energy;N",2-i,i).Data(),
                200,-5,20);
    }
    for (int i = 0; i < 5; ++i){
        isHT0[i] = new TH1F(
                TString::Format("isHT0_2l%i",i).Data(),
                TString::Format("for HT0 trigger, adc hits, tower + %i,  (2 - %i);energy;N",2-i,i).Data(),
                100,-0.5,99.5);
    }
    for (int i = 0; i < 5; ++i){
        isHT1[i] = new TH1F(
                TString::Format("isHT1_2l%i",i).Data(),
                TString::Format("for HT1 trigger, adc hits, tower + %i,  (2 - %i);energy;N",2-i,i).Data(),
                100,-0.5,99.5);
    }
    for (int i = 0; i < 5; ++i){
        isHT2[i] = new TH1F(
                TString::Format("isHT2_2l%i",i).Data(),
                TString::Format("for HT2 trigger, adc hits, tower + %i,  (2 - %i);energy;N",2-i,i).Data(),
                100,-0.5,99.5);
    }
    for (int i = 0; i < 5; ++i){
        isHT3[i] = new TH1F(
                TString::Format("isHT3_2l%i",i).Data(),
                TString::Format("for HT3 trigger, adc hits, tower + %i,  (2 - %i);energy;N",2-i,i).Data(),
                100,-0.5,99.5);
    }

    ntracks_TOF = new TH1F("ntracks_TOF","nTracks tof matched",300,-0.5,299.5);
    ntracks_noTOF = new TH1F("ntracks_noTOF","nTracks not tof matched",300,-0.5,299.5);
    ntracks_BEMC = new TH1F("ntracks_BEMC","nTracks matched to BEMC",300,-0.5,299.5);
    ntracks_noBEMC = new TH1F("ntracks_noBEMC","nTracks not matched to BEMC",300,-0.5,299.5);

    for (int i = 0; i < 5; ++i){
        bemc_match[i] = new TH1F(
                TString::Format("bemc_match_2l%i",i).Data(),
                TString::Format("bemc match for HT1 tower + %i,  (2 - %i);energy;N",2-i,i).Data(),
                2,-0.5,1.5);
    }
    for (int i = 0; i < 5; ++i){
        bemc_matchTT[i] = new TH1F(
                TString::Format("bemc_matchTT_2l%i",i).Data(),
                TString::Format("bemc match trigger track w/HT1 tower + %i,  (2 - %i);energy;N",2-i,i).Data(),
                2,-0.5,1.5);
    }
    bemc_hasTT = new TH1F("bemc_hasTT","of all trigger tracks, which ones have a bemc match 0(no) 1(yes)",
            2,-0.5,1.5);
  return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t ZeptoGrams::Finish() {
    fout->Write();
    fout->Close();
  return kStOK;
}
    
//----------------------------------------------------------------------------- 
void ZeptoGrams::Clear(Option_t *opt) { }

//----------------------------------------------------------------------------- 
Int_t ZeptoGrams::Make() {
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

    /* printf("These are the trigger ids:\n"); */
    /* vector<unsigned int> triggerids = mevent->triggerIds(); */
    /* for (unsigned int i = 0; i < triggerids.size(); i++){ */
    /*     printf(" %2i : %i\n", i, triggerids[i]); */
    /* } */
    /* for (auto a : mevent->triggerIds()) { */
    /*     cout << " -*-*- " << a << endl; */
    /* } */

    if (!mevent->isTrigger(mtriggerid)) return kStOK;
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

    float bbc_adc = 0;
    for (int i = 0; i < 24; ++i) bbc_adc += mevent->bbcAdcEast(i);
    float zdc_adc = mevent->ZdcSumAdcEast();  

    vz_zdcSig_course->Fill(z_vert, zdc_adc);
    vz_bbcSig_course->Fill(z_vert, bbc_adc);

    vz->Fill(mevent->primaryVertex().z());

    if (TMath::Abs(z_vert) > 30) return kStOk;
    ev_cuts->Fill(5);

    if (TMath::Abs(z_vert - mevent->vzVpd())>3.0) return kStOk;
    ev_cuts->Fill(6);

    // temporary test data: check for towers
    // TIGGER
    /* cout << " -- number of emc triggers " << mPicoDst->numberOfEmcTriggers() << endl; */
    /* int nHT1 = 0; */
    /* int nHT2 = 0; */
    /* int nHT3 = 0; */
    /* int nJP0 = 0; */
    /* int nJP1 = 0; */
    /* int nJP2 = 0; */
    /* float e_max = 0.; */
    /* float e_sum = 0.; */


        /* cout << "trigger " << i << endl; */
        /* printf(" %10s %u\n", "flag", mEmcTrigger->flag()); */
        /* printf(" %10s %i\n", "id", mEmcTrigger->id()); */
        /* printf(" %10s %i\n", "adc", mEmcTrigger->adc()); */
        /* printf(" %10s %i\n", "isHT0", mEmcTrigger->isHT0()); */
        /* printf(" %10s %i\n", "isHT1", mEmcTrigger->isHT1()); */
        /* printf(" %10s %i\n", "isHT2", mEmcTrigger->isHT2()); */
        /* printf(" %10s %i\n", "isHT3", mEmcTrigger->isHT3()); */
        /* printf(" %10s %i\n", "isJP0", mEmcTrigger->isJP0()); */
        /* printf(" %10s %i\n", "isJP1", mEmcTrigger->isJP1()); */
        /* printf(" %10s %i\n", "isJP2", mEmcTrigger->isJP2()); */

        /* // now for the trigger itself */
        /* StPicoBTowHit* mhit = mPicoDst->btowHit(mEmcTrigger->id()); */ 
        /* int id = mhit->id(); */
        /* StEmcPosition *mPosition = new StEmcPosition(); */
        /* StThreeVectorF towerPosition = mPosition->getPosFromVertex(pvert, id); */
        /* printf (" This tower is at: %f %f ", towerPosition.phi(), towerPosition.pseudoRapidity()); */
        /* printf("  -- data for tow %i\n", mEmcTrigger->id()); */
        /* printf("     %10s   %i\n", "id:", mhit->id()); */
        /* printf("     %10s   %i\n", "adc:", mhit->adc()); */
        /* printf("     %10s   %-10f\n", "energy:", mhit->energy()); */

        /* int adc = mhit->adc(); */
        /* tower_hits->Fill(mhit->id()); */
        /* adc_energy->Fill(adc,mhit->energy()); */
        /* if (mEmcTrigger->isHT0()) isHT0_adc->Fill(adc); */
        /* if (mEmcTrigger->isHT1()) isHT1_adc->Fill(adc); */
        /* if (mEmcTrigger->isHT2()) isHT2_adc->Fill(adc); */
        /* if (mEmcTrigger->isHT3()) isHT3_adc->Fill(adc); */


        /* for (int j = 0; j < 5; ++j) { */
        /*     if (mEmcTrigger->id() + j < 0) j = 0; */
        /*     cout << " --- tower " << j << " ----" << endl; */
        /*     mhit = mPicoDst->btowHit(mEmcTrigger->id()+j); */ 
        /*     id = mhit->id(); */
        /*     mPosition = new StEmcPosition(); */
        /*     towerPosition = mPosition->getPosFromVertex(pvert, id); */
        /*     printf (" This tower is at: %f %f ", towerPosition.phi(), towerPosition.pseudoRapidity()); */
        /*     printf ("   id ( %i )   adc ( %i )  energy ( %f )\n", */
        /*             mhit->id(), mhit->adc(), mhit->energy()); */
        /*     /1* printf("  -- data for tow %i\n", mEmcTrigger->id()); *1/ */
        /*     /1* printf("     %10s   %i\n", "id:", mhit->id()); *1/ */
        /*     /1* printf("     %10s   %i\n", "adc:", mhit->adc()); *1/ */
        /*     /1* printf("     %10s   %-10f\n", "energy:", mhit->energy()); *1/ */
        /* } */
    /* } */
     

    // pre-look through all tracks; fill good tracks into list;
    // find out if too-high a track is found
    std::vector<unsigned int> i_tracks;
    std::vector<unsigned int> i_tracks_TOF;
    std::vector<bool>         isTOF;
    bool has_TT      = false;
    bool has_TT_TOF  = false;
    bool has_TT_BEMC = false;
    unsigned int n_primary = mPicoDst->numberOfTracks();
    float TT_pt = 0;
    unsigned int i_TT = 0;

    std::vector<unsigned int> i_bemc;
    std::vector<int> i_bemc_towers;
    int TT_bemc_tower;
    int nTOF = 0;
    int nNoTOF = 0;
    int nBEMC = 0;
    int nNoBEMC = 0;

    for (unsigned int i = 0; i < n_primary; ++i){
		StPicoTrack* mtrack = mPicoDst->track(i); 
        /* printf("This is the BTOF %i\n", mtrack->bemcPidTraitsIndex()); */
		StThreeVectorF tr_vec = mPicoDst->track(i)->pMom(); 
        float pt = tr_vec.perp();
        if (pt > 30) return kStOK; // cut events
        if (   pt < 0.2
            || mtrack->dcaPoint().mag() > 3.
            || TMath::Abs(tr_vec.pseudoRapidity()) > 1
            || ((float)mtrack->nHits())/mtrack->nHitsMax() < 0.52 
        ) continue;
        i_tracks.push_back(i);

        if (mtrack->bTofPidTraitsIndex()!=-1){
            i_tracks_TOF.push_back(i);
            ++nTOF;
        } else { 
            ++nNoTOF;
        }

        if (mtrack->bemcPidTraitsIndex() != -1){
            i_bemc.push_back(i);
            i_bemc_towers.push_back(mtrack->bemcPidTraitsIndex());
            ++nBEMC;
        } else {
            ++nNoBEMC;
        }

        if (pt >= mTT_threshold) {
            has_TT = true;
            if (pt > TT_pt) {
                TT_pt = pt;
                i_TT = i;
                if (mtrack->bemcPidTraitsIndex() != -1){
                   TT_bemc_tower = mtrack->bemcPidTraitsIndex();
                   has_TT_BEMC = true;
                } else {
                    has_TT_BEMC = false;
                }
                if (mtrack->bTofPidTraitsIndex()!=-1){
                    has_TT_TOF = true;
                } else {
                    has_TT_TOF = false;
                }
            }
        }
    }
    ev_cuts->Fill(7);

    ntracks_TOF->Fill(nTOF);
    ntracks_noTOF->Fill(nNoTOF);
    ntracks_BEMC->Fill(nBEMC);
    ntracks_noBEMC->Fill(nNoBEMC);
    bemc_hasTT->Fill(has_TT_BEMC ? 1 : 0);

    // fill trigger diagrams
    StThreeVectorF pvert = mevent->primaryVertex();
    for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
        StPicoEmcTrigger* mtrigger = mPicoDst->emcTrigger(i);
        int tower_id = mtrigger->id();
        tower_hits->Fill(tower_id);

        if (tower_id>1 && tower_id<4798) {
            for (int j = 0; j < 5; ++j){
                StPicoBTowHit* mhit = mPicoDst->btowHit(tower_id+j-2); 
                float adc = mhit->adc();
                adc_0[j]->Fill(adc);
                energy_0[j]->Fill(mhit->energy());
                adc_energy[j]->Fill(adc, mhit->energy());
                if (mtrigger->isHT0()) isHT0[j]->Fill(adc);
                if (mtrigger->isHT1()) isHT1[j]->Fill(adc);
                if (mtrigger->isHT2()) isHT2[j]->Fill(adc);
                if (mtrigger->isHT3()) isHT3[j]->Fill(adc);
                if (!has_TT) continue;
                if (!mtrigger->isHT1()) continue;
                if (std::binary_search(i_bemc_towers.begin(), i_bemc_towers.end(), tower_id+j-2)){
                    bemc_match[j]->Fill(1);
                } else {
                    bemc_match[j]->Fill(0);
                }
                if (!has_TT_BEMC) continue;
                /* cout << "tower_id +j -2 :: " << tower_id +j - 2 << "  " << TT_bemc_tower << endl; */
                if (tower_id+j-2 == TT_bemc_tower) {
                    bemc_matchTT[j]->Fill(1);
                } else {
                    bemc_matchTT[j]->Fill(0);
                }
            }
        }
    }

    if (!has_TT) return kStOK;
    ev_cuts->Fill(8);

    nBEMCMatch->Fill(mevent->nBEMCMatch());
    nBTOFMatch->Fill(mevent->nBTOFMatch());

    // skip the TOF matching for now...
    /* if (!has_TT_TOF) { cout << "skipping TOF requirement" << endl; } //return kStOK; */

    //Fill event-level histograms
    /* float bbc_adc2 = 0; */
    /* for (auto sig : mevent->bbcAdcEast()) bbc_adc2 += sig; */
    /* printf( " bbcAdcEast (%f) and rate (%f) and delta (%f)  || 1 to 2 (%f)\n", */
            /* bbc_adc, mevent->bbcEastRate(), bbc_adc - mevent->bbcEastRate(), */
            /* bbc_adc-bbc_adc2); */

    zdcSig->Fill(zdc_adc);
    bbcSig->Fill(bbc_adc);

    vz_zdcSig   ->Fill(z_vert, zdc_adc);
    vz_bbcSig   ->Fill(z_vert, bbc_adc);

    vz_zdcSig_e ->Fill(z_vert, zdc_adc);
    vz_zdcSig_s ->Fill(z_vert, zdc_adc);

    vz_bbcSig_e ->Fill(z_vert, bbc_adc);
    vz_bbcSig_s ->Fill(z_vert, bbc_adc);
    
    float v_refMult = mevent->refMult();
    refMult->Fill(v_refMult);
    
    zdc_refMult->Fill(zdc_adc, v_refMult);
    bbc_refMult->Fill(bbc_adc, v_refMult);
    vz_refMult->Fill(z_vert,   v_refMult);

    // loop through the tracks
    /* cout << " o o o   this" << endl; */
    /* for (auto i : i_tracks) cout << i << endl; */
    
    /* StPicoTrack* mtrack; */
    /* StPicoEvent* mevent; */
    /* const int ZdcMax=24; */
    /* int n=mPicoDst->numberOfTracks(); */
    /* int mult=0; */
    /* int i_zdc; */
    /* double mPt; */
    /* int runid; */
    /* float mvx,mvy,mvz; */
    /* float mbbcAdcEast=0, mbbcAdcWest=0; */
    /* float mbbceastrate, mbbcwestrate; */
    /* mevent=mPicoDst->event(); */
    /* runid=mevent->runId(); */
    //cout<<runid<<endl;
    /* runid=RunIdAlter(runid); */
    //cout<<runid<<endl;


/*std::vector<unsigned int> mvec=mevent->triggerIds();
int length=mvec.size();
//cout<<length<<endl;

for(int kk=0;kk<length;kk++)
	if(mvec[kk]==trigid){
		cout<<mvec[kk]<<"\t";
		cout<<mevent->isTrigger(trigid)<<endl;
	}
*/

    /* float mvz=(float) mevent->primaryVertex().z(); */
    /* int mselect=PicoSelect(mevent,mvz); */
//cout<<mselect<<"\t";
/* histcut->Fill(mselect); */
/* if(mselect==0){ */
/* 	mvx=(float) mevent->primaryVertex().x(); */
/* 	mvy=(float) mevent->primaryVertex().y(); */
/* //	mvz=(float) mevent->primaryVertex().z(); */

/* 	histVxy->Fill(mvx,mvy); */
/* 	histVxz->Fill(mvx,mvz); */
/* 	histVyz->Fill(mvy,mvz); */

/* 	for(i_zdc=0;i_zdc<ZdcMax;i_zdc++){ */
/* 		mbbcAdcEast+=(float) mevent->bbcAdcEast(i_zdc); */
/* 		mbbcAdcWest+=(float) mevent->bbcAdcWest(i_zdc); */
/* 	} */
	
/* 	mbbceastrate=mevent->bbcEastRate(); */
/* 	mbbcwestrate=mevent->bbcWestRate(); */

/* //	int indice=NumBin(200,-200,200,mvz); */
/* //	mbbcAdcEast/=east->GetBinContent(indice); */
/* //	mbbcAdcWest/=west->GetBinContent(indice); */

/* 	Z_bbcAdcEast   ->Fill(mvz,mbbcAdcEast); */
/* 	Z_bbcAdcWest   ->Fill(mvz,mbbcAdcWest); */
/* 	runid_AdcEast  ->Fill(runid,mbbcAdcEast); */
/* 	runid_AdcWest  ->Fill(runid,mbbcAdcWest); */
/* 	runid_RateEast ->Fill(runid,mbbceastrate); */
/* 	runid_RateWest ->Fill(runid,mbbcwestrate); */
/* 	bbceastprofile ->Fill(mvz,mbbcAdcEast); */
/* 	bbcwestprofile ->Fill(mvz,mbbcAdcWest); */
/* 	Rate_AdcEast   ->Fill(mbbceastrate,mbbcAdcEast); */
/* 	Rate_AdcWest   ->Fill(mbbcwestrate,mbbcAdcWest); */
/* 	for(int i=0;i<n;i++){ */
/* 		mtrack=mPicoDst->track(i); */ 
/* 		mPt=mtrack->pMom().perp(); */
/* 		if(mtrack->pMom().mag()){ */
/* 			histPt->Fill(mPt); */
/* 			mult++; */
/* 		} */
/* 	} */
/* 	histmult->Fill(mult); */
/* } */		


  return kStOK;
}


