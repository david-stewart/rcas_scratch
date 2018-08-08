//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "TowerQA.h"
#include <iostream>
#define zmin -30
#define zmax 30


ClassImp(TowerQA)

//-----------------------------------------------------------------------------
TowerQA::TowerQA(const char* name, StPicoDstMaker *picoMaker,
        /* const char* prof,*/ 
        const char* outName, 
        int triggerid)
  : StMaker(name), 
    mPicoDstMaker(picoMaker),
    mtriggerid(triggerid), 
    mOutName(outName),
    fnbins_EmcTowers(4803);
    fbinlo_EmcTowers(-1.5);
    fbinhi_EmcTowers(4801.5);
    fnbinsY_EmcTriggerId_HTbreakout(8);   // 1: HT1
    fbinloY_EmcTriggerId_HTbreakout(-0.5);   // 1: HT1
    fbinhiY_EmcTriggerId_HTbreakout(7.5);   // 1: HT1
    fnbinsY_BTowHitE(37);
    fbinloY_BTowHitE(-2.);
    fbinhiY_BTowHitE(35);
    fnbins_BTowHitE_All(100);
    fnbins_nEmcTriggers(100);
{
    bad_run_list = get_bad_runs();
	bad_tower_list = get_bad_towers();
}

//----------------------------------------------------------------------------- 
TowerQA::~TowerQA()
{ /*  */ }

/* class class_bemc_tower_tracks { */
/* 	private: */
/*         /1* std::vector<int> bad_tower_list; *1/ */
/*         std::vector<int> i_tr_m_BEMC; // i track match BEMC */
/*         std::vector<int> i_tr_m_HT1; */
/*     public: */
/*         std::vector<int> i_tow_HT1; // tower id w/ HT1 */
/*         class_bemc_tower_tracks (StPicoDst* mPicoDst, std::vector<int>& bad_tower_list) */
/*         { */
/*             // get list of HT1 towers */
/*             for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){ */
/*                 StPicoEmcTrigger *emcTrig = static_cast<StPicoEmcTrigger*>(mPicoDst->emcTrigger(i)); */
/*                 if(!emcTrig) continue; */
/*                 int towID = emcTrig->id(); // soft id. 1-4800 */
/*                 if (std::binary_search(bad_tower_list.begin(), bad_tower_list.end(), towID)) continue; */
/*                 if (emcTrig->isHT1()) i_tow_HT1.push_back(towID); */
/*             } */
/*             std::sort(i_tow_HT1.begin(), i_tow_HT1.end()); */

/*             //now get the list of tracks matched to the BEMC */
/*             for(unsigned int i = 0; i < mPicoDst->numberOfBEmcPidTraits(); i++){ */
/*                 StPicoBEmcPidTraits* mBemcTraits = static_cast<StPicoBEmcPidTraits*>(mPicoDst->bemcPidTraits(i)); */
/*                 if(!mBemcTraits) continue; */ 
/*                 int towID = mBemcTraits->btowId();   // projected tower Id: 1 - 4800 (really is tower id, and not hit it) */
/*                                                  // david: CHECK that this is the same numbering as emcTrig->id() */
/*                 //int towID2 = cluster->btowId2(); // emc 2nd and 3rd closest tower local id  ( 2nd X 10 + 3rd), each id 0-8 */
/*                 //int towID3 = cluster->btowId3(); // emc 2nd and 3rd closest tower local id  ( 2nd X 10 + 3rd), each id 0-8 */
/*                 if(towID < 0) continue; */
/*                 int trackIndex = mBemcTraits->trackIndex(); */ 
/*                 StPicoTrack* trk = static_cast<StPicoTrack*>(mPicoDst->track(trackIndex)); */
/*                 if(!trk || !trk->isPrimary()) continue; */
                
/*                 if (std::binary_search(bad_tower_list.begin(), bad_tower_list.end(), towID)) continue; */
/*                 bool is_ht1 = std::binary_search(i_tow_HT1.begin(), i_tow_HT1.end(), towID); */
/*                 /1* printf("is_ht1: %i  %f\n", is_ht1, trk->pMom().perp()); *1/ */
/*                 i_tr_m_BEMC.push_back(trackIndex); */
/*                 if (is_ht1) i_tr_m_HT1.push_back(trackIndex); */
/*             } */
/*             std::sort(i_tr_m_BEMC.begin(), i_tr_m_BEMC.end()); */
/*             std::sort(i_tr_m_HT1.begin(), i_tr_m_HT1.end()); */
/*         } */
/*         bool match_BEMC(int i_track){ */
/*             return std::binary_search(i_tr_m_BEMC.begin(), i_tr_m_BEMC.end(), i_track); */
/*         } */
/*         bool match_HT1(int i_track){ */
/*             return std::binary_search(i_tr_m_HT1.begin(), i_tr_m_HT1.end(), i_track); */
/*         } */
/* }; */
/* class class_TT { */
/* 	private: */
/* 	float thresh_hold; */
/*     StPicoDst* mPicoDst; */

/* 	public: */
/* 	std::vector<int>   i_tracks; */
/* 	std::vector<float> pt; */
/* 	bool  exists; */
/* 	class_TT(float v_thresh_hold, StPicoDst* v_pdst) : */ 
/* 		thresh_hold(v_thresh_hold), */
/*         mPicoDst{v_pdst}, */
/* 		exists(false) */
/*     {} */
/* 	void  update(int i, float v_pt){ */
/* 		if (v_pt < thresh_hold) return; */
/* 		exists = true; */
/* 		pt.push_back(v_pt); */
/* 		i_tracks.push_back(i); */
/* 		return; */
/* 	} */
/* 	float max_pt(){ */
/* 		if (exists) { */
/* 			return *std::max_element(std::begin(pt), std::end(pt)); */
/* 		} else { */
/* 			return 0; */
/* 		} */
/* 	}; */
/* 	int i_max(){ */
/* 		float m_pt = max_pt(); */
/* 		for (unsigned int i = 0; i < pt.size(); ++i){ */
/* 			if (pt[i] == m_pt) return i_tracks[i]; */
/* 		} */
/* 		return -1; */
/* 	}; */
/*     int n_TOF () { */
/*         int n = 0; */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bTofPidTraitsIndex()!=-1) ++n; */
/*         } */
/*         return n; */
/*     } */
/*     int n_notTOF () { */
/*         int n = 0; */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bTofPidTraitsIndex()==-1) ++n; */
/*         } */
/*         return n; */
/*     } */
/*     bool has_TOF () { */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bTofPidTraitsIndex()!=-1) return true; */
/*         } */
/*         return false; */
/*     } */
/*     std::vector<int> vec_TOF () { */
/*         std::vector<int> vec; */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bTofPidTraitsIndex()!=-1) vec.push_back(i); */
/*         } */
/*         return vec; */
/*     } */
/*     int n_BEMC () { */
/*         int n = 0; */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bemcPidTraitsIndex()!=-1) ++n; */
/*         } */
/*         return n; */
/*     } */
/*     int n_notBEMC () { */
/*         int n = 0; */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bemcPidTraitsIndex()==-1) ++n; */
/*         } */
/*         return n; */
/*     } */
/*     std::vector<int> vec_BEMC () { */
/*         std::vector<int> vec; */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bemcPidTraitsIndex()!=-1) vec.push_back(i); */
/*         } */
/*         return vec; */
/*     } */
/*     bool match_BEMC(class_bemc_tower_tracks* m_tr){ */
/*         for (auto i : i_tracks){ */
/*             if (m_tr->match_BEMC(i)) return true; */
/*         } */
/*         return false; */
/*     } */
/*     bool match_HT1(class_bemc_tower_tracks* m_tr){ */
/*         for (auto i : i_tracks){ */
/*             if (m_tr->match_HT1(i)) return true; */
/*         } */
/*         return false; */
/*     } */
/*     bool has_TOF_andBEMC (class_bemc_tower_tracks* m_tr) { */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bTofPidTraitsIndex()!=-1 && m_tr->match_BEMC(i)) return true; */
/*         } */
/*         return false; */
/*     } */
/*     bool has_TOF_andHT1(class_bemc_tower_tracks* m_tr){ */
/*         for (auto i : i_tracks){ */
/*             StPicoTrack* mtrack = mPicoDst->track(i); */ 
/*             if (mtrack->bTofPidTraitsIndex()!=-1 && m_tr->match_HT1(i)) return true; */
/*         } */
/*         return false; */
/*     } */

/* }; */

//----------------------------------------------------------------------------- 
Int_t TowerQA::Init() {
    fout = new TFile(TString::Format("%s.root",mOutName.Data()).Data(),"RECREATE") ;
    flog = fopen(TString::Format("%s.log",mOutName.Data()).Data(),"w");

    ev_cuts = new TH1F("ev_cuts",
            ";0:db 1:!badrun 2:trig 3:rnk 4:zdc 5:vz10 6:(vz-vpd)<=3.0 7:no30tr 8:TT 9:TT_TOF",
            10, -0.5, 9.5);

	n_events = new TH1F("n_events",
		TString::Format("%s;%s",
			"number of events#; 1st:val 2nd:btow-match 3rd:HT1-match",
            "0:TT>4 1:TT_TOF>4 2:TT>9 3:TT_TOF>9 4:HT1_e>4/8/12 5:#Sigma_N;N").Data(),
			48,0,6);


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

    fHgram_EmcTriggerId_all = new TH1F("EmcTriggerId_all",
            "How many times each triggerId is called;triggerId;N",
            fnbins_EmcTowers, fbinlo_EmcTowers, fbinhi_EmcTowers);

    // note: HT0 trigger is not called at all.
    fHgram_EmcTriggerId_HT1 = new TH1F("EmcTriggerId_HT1",
            "How many times each triggerId is called with HT1 trigger;triggerId;N_{HT1}",
            fnbins_EmcTowers, fbinlo_EmcTowers, fbinhi_EmcTowers);

    fHgram_EmcTriggerId_HT2 = new TH1F("EmcTriggerId_HT2",
            "How many times each triggerId is called with HT2 trigger;triggerId;N_{HT2}",
            fnbins_EmcTowers, fbinlo_EmcTowers, fbinhi_EmcTowers);

    fHgram_EmcTriggerId_HT3 = new TH1F("EmcTriggerId_HT3",
            "How many times each triggerId is called with HT3 trigger;triggerId;N_{HT3}",
            fnbins_EmcTowers, fbinlo_EmcTowers, fbinhi_EmcTowers);

    fHgram2_EmcTriggerId_HTbreakout = new TH2F("EmcTriggerId_HTbreakout",
            TString::Format("%s%s",
            "n-times each trigger called#; (a/b/c) = (HT1/HT2/HT3);triggerId;",
            "0: 1:a 2:b 3:ab 4:c 5:ac 6:bc 7:abc").Data(),
            fnbins_EmcTowers, fbinlo_EmcTowers, fbinhi_EmcTowers,
            fnbinsY_EmcTriggerId_HTbreakout,
            fbinloY_EmcTriggerId_HTbreakout,
            fbinhiY_EmcTriggerId_HTbreakout);

    fProfE_EmcTriggerId_alladc = new TProfile("EmcTriggerId_alladc_E",
            "ADC counts in each trigger w/error on mean;TowerID;adc counts",
            fnbins_EmcTowers, fbinlo_EmcTowers, fbinhi_EmcTowers);

    fProfS_EmcTriggerId_alladc = new TProfile("EmcTriggerId_alladc_S",
            "ADC counts in each trigger w/std deviation;TowerID;adc counts",
            fnbins_EmcTowers, fbinlo_EmcTowers, fbinhi_EmcTowers);

    fProfE_BTowHitE_all = new TProfile("BTowHitE_all", "BTower Energy all hits;TowerID;Energy",
        4800, -0.5, 4799.5);

    fProfE_BTowHitE_HT1 = new TProfile("BTowHitE_HT1", "BTower Energy on HT1 hit towers;TowerID;Energy",
        4800, -0.5, 4799.5);
    
    fProfE_BTowHitE_HT2 = new TProfile("BTowHitE_HT2", "BTower Energy on HT1 hit towers;TowerID;Energy",
        4800, -0.5, 4799.5);

    fProfE_BTowHitE_HT3 = new TProfile("BTowHitE_HT3", "BTower Energy on HT1 hit towers;TowerID;Energy",
        4800, -0.5, 4799.5);

    fProfE_BTowHitAdc_HT1 = new TProfile("BTowHitE_HT1", "BTower Adc Hits on HT1 hit towers;TowerID;Energy",
        4800, -0.5, 4799.5);
    
    fProfE_BTowHitAdc_HT2 = new TProfile("BTowHitE_HT2", "BTower Adc Hits on HT1 hit towers;TowerID;Energy",
        4800, -0.5, 4799.5);

    fProfE_BTowHitAdc_HT3 = new TProfile("BTowHitE_HT3", "BTower Adc Hits on HT1 hit towers;TowerID;Energy",
        4800, -0.5, 4799.5);

    fHgram2_BTowHitE_All = new TH1F("BTowHitE_All2d", "BTower Energy on all hit towers;TowerID;Energy",
        4800, -0.5, 4799.5, fnbinsY_BTowHitE, fbinloY_BTowHitE, fbinhiY_BTowHitE);

    fHgram2_BTowHitE_HT1 = new TH1F("BTowHitE_All2d", "BTower Energy on HT1 hit towers;TowerID;Energy",
        4800, -0.5, 4799.5, fnbinsY_BTowHitE, fbinloY_BTowHitE, fbinhiY_BTowHitE);

    fProfE_EmcTrigger_adcall = new TH1F("EmcTrigger_adcall", "EmcTrigger ADC values, all hits;ADC Vales;N",
            500,0., 4000.);
    fProfE_EmcTrigger_adcHT1 = new TH1F("EmcTrigger_adcHT1", "EmcTrigger ADC values, HT1 hits;ADC Vales;N",
            500,0., 4000.);
    fProfE_EmcTrigger_adcHT2 = new TH1F("EmcTrigger_adcHT2", "EmcTrigger ADC values, HT2 hits;ADC Vales;N",
            500,0., 4000.);
    fProfE_EmcTrigger_adcHT3 = new TH1F("EmcTrigger_adcHT3", "EmcTrigger ADC values, HT3 hits;ADC Vales;N",
            500,0., 4000.);

    fHgram_BTowHitE_All = new TH1F("BTowHitE_spectraAll", "BTower Energy overall spectra;E;N",
        fnbins_BTowHitE,fbinloY_BTowHitE;fbinhiY_BTowHitE);
    fHgram_BTowHitE_HT1 = new TH1F("BTowHitE_spectraHT1", "BTower Energy overall spectra, HT1 events ;E;N",
        fnbins_BTowHitE,fbinloY_BTowHitE;fbinhiY_BTowHitE);
    fHgram_BTowHitE_HT2 = new TH1F("BTowHitE_spectraHT2", "BTower Energy overall spectra, HT2 events ;E;N",
        fnbins_BTowHitE,fbinloY_BTowHitE;fbinhiY_BTowHitE);
    fHgram_BTowHitE_HT3 = new TH1F("BTowHitE_spectraHT3", "BTower Energy overall spectra, HT3 events ;E;N",
        fnbins_BTowHitE,fbinloY_BTowHitE;fbinhiY_BTowHitE);

    fHgram_nEmcTriggers = new TH1F("nEmcTriggers","number of EMC Triggers per event;n_{Triggers};N",
         fnbins_nEmcTriggers, -0.5, fnbins_nEmcTriggers-0.5);

    ln_ranking = new TH1F("ln_ranking",
            "log of best vertex ranking - perseving the negative sign",
            400,-20,20);

  return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t TowerQA::Finish() {
    fout->Write();
    fout->Close();
    fclose(flog);
  return kStOK;
}
    
//----------------------------------------------------------------------------- 
void TowerQA::Clear(Option_t *opt) { }

//----------------------------------------------------------------------------- 
Int_t TowerQA::Make() {
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

    /* std::vector<unsigned int> i_bemc; */
    // keep what the cuts would have been to fill in the 

    /* cout << "a1" << endl; */
	/* class_TT TT4(4., mPicoDst); */
	/* class_TT TT9(9., mPicoDst); */
    /* class_TT i_tracks(0.2, mPicoDst); */

    /* int tr_cuts_e[7]{0,0,0,0,0,0,0}; */
    /* unsigned int n_primary = mPicoDst->numberOfTracks(); */
    for (unsigned int i = 0; i < n_primary; ++i){
    /* cout << "a3" << endl; */
        ++tr_cuts_e[0];
		StPicoTrack* mtrack = mPicoDst->track(i); 
		StThreeVectorF tr_vec = mPicoDst->track(i)->pMom(); 
        float pt = tr_vec.perp();
        if (pt > 30) return kStOK; // cut events
        if ( pt < 0.2 ) continue;
        ++tr_cuts_e[1];
        if ( mtrack->dcaPoint().mag() > 3. ) continue;
        ++tr_cuts_e[2];
        if ( TMath::Abs(tr_vec.pseudoRapidity()) > 1 ) continue;
        ++tr_cuts_e[3];
        if ( ((float)mtrack->nHits())/mtrack->nHitsMax() < 0.52 ) continue;
        ++tr_cuts_e[4];
        i_tracks.update(i, pt);
		TT4.update(i,pt);
		TT9.update(i,pt);
        if (mtrack->bTofPidTraitsIndex()!=-1) ++tr_cuts_e[5];
        if (mtrack->bemcPidTraitsIndex() != -1) ++tr_cuts_e[6];
    }
    ev_cuts->Fill(7);
    /* cout << "a5" << endl; */

    ntracks_TOF->Fill(i_tracks.n_TOF());
    ntracks_noTOF->Fill(i_tracks.n_notTOF());
    ntracks_BEMC->Fill(i_tracks.n_BEMC());
    ntracks_noBEMC->Fill(i_tracks.n_notBEMC());

    /* cout << "a6" << endl; */
    // fill trigger diagrams
    StThreeVectorF pvert = mevent->primaryVertex();
    for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
        /* cout << "a7" << endl; */
        StPicoEmcTrigger* mtrigger = mPicoDst->emcTrigger(i);
        int tower_id = mtrigger->id();
        /* cout << "c0" << " " << tower_id << endl; */
        tower_hits->Fill(tower_id);
        // FIXME
        // print out the flag and values for the towerID == 0; 
        // check for towerID < 0, 
        // runfiredtriggerQA -- fill histograms used to generation of bad towers
        // https://github.com/joelmazer/star-jetframework/blob/master/StPicoTrackClusterQA.cxx -- see this


        /* cout << "c1" << endl; */
		if (mtrigger->isHT1()) tower_hits_TH1->Fill(tower_id);
        /* cout << "c2" << endl; */
        if (tower_id == 0) continue;
        StPicoBTowHit* mhit = mPicoDst->btowHit(tower_id-1); 
        float adc = mhit->adc();
        float energy = mhit->energy();
        adc_0->Fill(adc);
        energy_0->Fill(energy);
        adc_energy->Fill(adc, mhit->energy());
        if (mtrigger->isHT1()) {isHT1_a->Fill(adc); isHT1_e->Fill(energy); }
        if (mtrigger->isHT2()) {isHT2_a->Fill(adc); isHT2_e->Fill(energy); }
        if (mtrigger->isHT3()) {isHT3_a->Fill(adc); isHT3_e->Fill(energy); }
    }


    /* for (auto i : i_tracks.vec_TOF()) cout << " " << i; */
    /* cout << endl; */
    /* /1* cout << "a12" << endl; *1/ */

    /* cout << "TTall_BEMC : " <<endl; */
    /* for (auto i : i_tracks.vec_BEMC()) cout << " " << i; */
    /* cout << endl; */
    /* cout << "a13" << endl; */

    float first  = 0.1875;
    float second = 0.4375;
    float third  = 0.6875;

    n_events->Fill(5+first);
    class_bemc_tower_tracks mTOW(mPicoDst, bad_tower_list);


    if (TT4.exists)           n_events->Fill(first);
    if (TT4.match_BEMC(&mTOW)) n_events->Fill(second);
    if (TT4.match_HT1(&mTOW))  n_events->Fill(third);

    if (TT4.has_TOF())         n_events->Fill(1+first);
    if (TT4.has_TOF_andBEMC(&mTOW)) n_events->Fill(1+second);
    if (TT4.has_TOF_andHT1(&mTOW)) n_events->Fill(1+third);

    if (TT9.exists)           n_events->Fill(2+first);
    if (TT9.match_BEMC(&mTOW)) n_events->Fill(2+second);
    if (TT9.match_HT1(&mTOW))  n_events->Fill(2+third);

    if (TT9.has_TOF())         n_events->Fill(3+first);
    if (TT9.has_TOF_andBEMC(&mTOW)) n_events->Fill(3+second);
    if (TT9.has_TOF_andHT1(&mTOW))  n_events->Fill(3+third);

    float v_max_e_tow = -1;
    int  iv_max_e_tow = -100;
    /* bool fill_4 = false; */
    for (auto i : mTOW.i_tow_HT1) {
        if (i==0) continue;
        StPicoBTowHit* mhit = mPicoDst->btowHit(i-1); 
        i_tow_HT1_hgram->Fill(i);
        i_tow_HT1_profile->Fill(i,mhit->energy());
        if (mhit->energy() > v_max_e_tow) {
            v_max_e_tow = mhit->energy();
            iv_max_e_tow = i; 
        }
    }
    if (v_max_e_tow > 4) n_events->Fill(4+first);
    if (v_max_e_tow > 8) n_events->Fill(4+second);
    if (v_max_e_tow > 12) n_events->Fill(4+third);


    if (iv_max_e_tow != -100) {
        max_e_tow->Fill(v_max_e_tow);
        i_max_e_tow->Fill(iv_max_e_tow);
        if (z_vert <= 10){
            max_e_tow_under10->Fill(v_max_e_tow);
            i_max_e_tow_under10->Fill(iv_max_e_tow);
        }
    }



    if (!TT4.exists) return kStOK;
    if (TT9.exists) vz_9TT->Fill(z_vert);
    /* cout << "a10" << endl; */
    ev_cuts->Fill(8);

    // loop through the tracks
    /* for (auto i : i_tracks) { */
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < tr_cuts_e[i]; ++j){
            tr_cuts->Fill(i);
        }
    }
    for (auto i : i_tracks.pt){
		StThreeVectorF tr_vec = mPicoDst->track(i)->pMom(); 
        track_pt->Fill(tr_vec.perp());
    }

    nBEMCMatch->Fill(mevent->nBEMCMatch());
    nBTOFMatch->Fill(mevent->nBTOFMatch());

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

	// experimental
	/* int nBEmcPidTraits = mPicoDst->numberOfBEmcPidTraits(); */
	/* std::vector<int> towI; */
	/* bool found_iTT = false; */
	/* for(unsigned short iClus = 0; iClus < nBEmcPidTraits; iClus++){ */
	/* 	StPicoBEmcPidTraits* cluster = static_cast<StPicoBEmcPidTraits*>(mPicoDst->bemcPidTraits(iClus)); */
	/* 	if(!cluster){ cout<<"Cluster pointer does not exist.. iClus = "<<iClus<<endl; continue; } */
	/* 	int trackIndex = cluster->trackIndex(); */
	/* 	if (trackIndex == i_TT) { found_iTT = true; break; } */
	/* 	/1* int towID = cluster->btowId();   // projected tower Id: 1 - 4800 *1/ */
	/* 	/1* printf("towID : %i\n",towID); *1/ */
	/* 	/1* if(towID < 0) continue; *1/ */
	/* 	/1* bool nomatch = true; *1/ */
	/*   	/1* for (auto i : towI){ *1/ */
	/* 	/1* 	if (towID == i) { cout << "Two tracks in same tower! ! ! ! "<<i<<endl;} *1/ */
	/* 	/1* 	nomatch = false; *1/ */
	/* 	/1* 	break; *1/ */
	/* 	/1* } *1/ */
	/* 	/1* if (nomatch) towI.push_back(towID); *1/ */
	/* 	/1* StPicoTrack* trk = static_cast<StPicoTrack*>(mPicoDst->track(trackIndex)); *1/ */
	/* 	/1* if(!trk) { cout<<"No trk pointer...."<<endl; continue; } *1/ */
	/* 	/1* printf(" track %i: is primary?(%i)\n",trackIndex, trk->isPrimary()); *1/ */
	/* } */
	/* if (found_iTT) { */
	/* 	printf("Found iTT! (%i) pt(%f) ---------------\n", i_TT, TT_pt); */
	/* } else { */
	/* 	printf("Not found iTT :(       ---------------\n"); */
	/* } */
	return kStOK;
}

