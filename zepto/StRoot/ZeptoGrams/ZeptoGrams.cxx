//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "ZeptoGrams.h"
#include <iostream>

#include "fastjet/config.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
using namespace fastjet;

#define zmin -30
#define zmax 30

#define  kNMaxJets   60
#define  kNMaxTracks 180

class MyJetDefinitionTerms {
    public:
    
    /* JetMedianBackgroundEstimator* bge_rm0; */
    /* JetMedianBackgroundEstimator* bge_rm1; */
    JetMedianBackgroundEstimator* bge_rm2;
    Subtractor* subtractor;

    MyJetDefinitionTerms(float jet_R_background){ 
        float ghost_maxrap = 4.0;
        JetDefinition jet_def_bkgd(kt_algorithm, jet_R_background); // <--
        AreaDefinition area_def_bkgd(active_area_explicit_ghosts,GhostedAreaSpec (ghost_maxrap,1,0.01));

        Selector selector_rm2 = SelectorAbsEtaMax(1.0) * (!SelectorNHardest(2)); // <--
        bge_rm2 = new JetMedianBackgroundEstimator (selector_rm2, jet_def_bkgd, area_def_bkgd); // <--

        subtractor = new Subtractor( bge_rm2 ) ;
    };
};


ClassImp(ZeptoGrams)

//-----------------------------------------------------------------------------
ZeptoGrams::ZeptoGrams(
        const char* name, StPicoDstMaker *picoMaker,
        const char* outName, 
        int triggerid, 
        float TT_threshold,
        bool  input_debug)
  : StMaker(name), 
    /* max_ntracks_noTOF(0), */
    /* mTestConditions(false), */
    /* mAllTOF(false), */
    mPicoDstMaker(picoMaker),
    mOutName(outName),
    mtriggerid(triggerid), 
    mTT_threshold(TT_threshold),
    b_jets        ("ZeJet",   kNMaxJets),
    b_tracks      ("ZeTrack", kNMaxTracks),
    /* b_tracks_noTOF("ZeTrack", kNMaxTracks), */

    fdebug(input_debug),
    fmax_njets(0),
    fmax_ntracks(0)
{
    bad_run_list   = get_bad_runs();
	bad_tower_list = get_bad_towers();
}

//----------------------------------------------------------------------------- 
ZeptoGrams::~ZeptoGrams()
{ /*  */ }

class class_bemc_tower_tracks {
	private:
        /* std::vector<int> bad_tower_list; */
        std::vector<int> i_tr_m_BEMC; // i track match BEMC
        std::vector<int> i_tr_m_HT1;
    public:
        bool debug = true;
        std::vector<int> i_tow_HT1; // tower id w/ HT1
        class_bemc_tower_tracks (StPicoDst* mPicoDst, std::vector<int>& bad_tower_list)
        {
            // get list of HT1 towers
            if (debug) {
                cout << "**New Event**" <<endl;
                printf(" %3s %5s %5s %5s %5s %5s %5s %5s %5s  %5s %5s\n",
                        "#","TowId","HT0","HT1","HT2","HT3","JP0","JP1","JP2","HT&JP","noMatch");
            }

            for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
                StPicoEmcTrigger *emcTrig = static_cast<StPicoEmcTrigger*>(mPicoDst->emcTrigger(i));
                if(!emcTrig) continue;

                if (false) {
                    int id = emcTrig->id();
                    if (id >= 1) {
                        StPicoBTowHit* mhit = mPicoDst->btowHit(id-1); 
                        printf("adc_trig (%i) adc_hit (%i) delta(%i)\n",
                                emcTrig->adc(), mhit->adc(), 
                                emcTrig->adc()- mhit->adc());
                    }
                }
                
                if (debug) {
                    int test = (((emcTrig->flag()/2)%8) - (emcTrig->isHT1() ? 1 : 0)
                                                    - (emcTrig->isHT2() ? 2 : 0)
                                                    - (emcTrig->isHT3() ? 4 : 0));
                    bool both = (emcTrig->isHT0() || emcTrig->isHT1() || emcTrig->isHT2() || emcTrig->isHT3())
                        && ( emcTrig->isJP0() || emcTrig->isJP1() || emcTrig->isJP2() );

                    printf(" %3i %5i %5s %5s %5s %5s %5s %5s %5s  %5s %5s\n",
                            i,
                            emcTrig->id() ,
                            emcTrig->isHT0() ? "T" : "",
                            emcTrig->isHT1() ? "T" : "",
                            emcTrig->isHT2() ? "T" : "",
                            emcTrig->isHT3() ? "T" : "",
                            emcTrig->isJP0() ? "T" : "",
                            emcTrig->isJP1() ? "T" : "",
                            emcTrig->isJP2() ? "T" : "",
                            both ? "1" : "",
                            test ? "T" : "");
                    /* printf("---------\n"); */
                    /* printf("%10s %i\n", "flag", emcTrig->flag()); */
                    /* printf("%10s %i\n", "HT0",  emcTrig->isHT0()); */
                    /* printf("%10s %i\n", "HT1",  emcTrig->isHT1()); */
                    /* printf("%10s %i\n", "HT2",  emcTrig->isHT2()); */
                    /* printf("%10s %i\n", "HT3",  emcTrig->isHT3()); */
                    /* printf("%10s %i\n", "JP0",  emcTrig->isJP0()); */
                    /* printf("%10s %i\n", "JP1",  emcTrig->isJP1()); */
                    /* printf("%10s %i\n", "JP2",  emcTrig->isJP2()); */
                    /* printf("%10s %i\n", "f0",  (int)emcTrig->flag()%16); */
                    /* int test = (((emcTrig->flag()/2)%8) - (emcTrig->isHT1() ? 1 : 0) */
                    /*                                 - (emcTrig->isHT2() ? 2 : 0) */
                    /*                                 - (emcTrig->isHT3() ? 4 : 0)); */
                    /* printf(" %s \n", test == 0 ? "true" : " **** FALSE *****"); */
                }

                int towID = emcTrig->id(); // soft id. 1-4800
                if (std::binary_search(bad_tower_list.begin(), bad_tower_list.end(), towID)) continue;
                if (emcTrig->isHT1()) i_tow_HT1.push_back(towID);
            }
            std::sort(i_tow_HT1.begin(), i_tow_HT1.end());

            if (false) {
                std::vector<int> emcTriggerIds;
                for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
                    StPicoEmcTrigger* emcTrig = mPicoDst->emcTrigger(i);
                    if (emcTrig->isHT1() || emcTrig->isHT2() || emcTrig->isHT3()) emcTriggerIds.push_back(i);
                }
                std::sort(emcTriggerIds.begin(), emcTriggerIds.end());
                for (int i = 0; i < 4800; i++){
                    if (std::binary_search(emcTriggerIds.begin(), emcTriggerIds.end(), i)) continue;
                    StPicoBTowHit* bTowHit = mPicoDst->btowHit(i); 
                    printf(" %i  e(%f)  adc(%i)\n", i, bTowHit->energy(), bTowHit->adc());
                }
            }

            //now get the list of tracks matched to the BEMC
            for(unsigned int i = 0; i < mPicoDst->numberOfBEmcPidTraits(); i++){
                StPicoBEmcPidTraits* mBemcTraits = static_cast<StPicoBEmcPidTraits*>(mPicoDst->bemcPidTraits(i));
                if(!mBemcTraits) continue; 
                int towID = mBemcTraits->btowId();   // projected tower Id: 1 - 4800 (really is tower id, and not hit it)
                                                 // david: CHECK that this is the same numbering as emcTrig->id()
                //int towID2 = cluster->btowId2(); // emc 2nd and 3rd closest tower local id  ( 2nd X 10 + 3rd), each id 0-8
                //int towID3 = cluster->btowId3(); // emc 2nd and 3rd closest tower local id  ( 2nd X 10 + 3rd), each id 0-8
                if(towID < 0) continue;
                int trackIndex = mBemcTraits->trackIndex(); 
                StPicoTrack* trk = static_cast<StPicoTrack*>(mPicoDst->track(trackIndex));
                if(!trk || !trk->isPrimary()) continue;
                
                if (std::binary_search(bad_tower_list.begin(), bad_tower_list.end(), towID)) continue;
                bool is_ht1 = std::binary_search(i_tow_HT1.begin(), i_tow_HT1.end(), towID);
                /* printf("is_ht1: %i  %f\n", is_ht1, trk->pMom().perp()); */
                i_tr_m_BEMC.push_back(trackIndex);
                if (is_ht1) i_tr_m_HT1.push_back(trackIndex);
            }
            std::sort(i_tr_m_BEMC.begin(), i_tr_m_BEMC.end());
            std::sort(i_tr_m_HT1.begin(), i_tr_m_HT1.end());
        }
        bool match_BEMC(int i_track){
            return std::binary_search(i_tr_m_BEMC.begin(), i_tr_m_BEMC.end(), i_track);
        }
        bool match_HT1(int i_track){
            return std::binary_search(i_tr_m_HT1.begin(), i_tr_m_HT1.end(), i_track);
        }
};
class class_TT {
	private:
	float thresh_hold;
    StPicoDst* mPicoDst;

	public:
	std::vector<int>   i_tracks;
	std::vector<float> pt;
	bool  exists;
    int size() { return i_tracks.size(); }
	class_TT(float v_thresh_hold, StPicoDst* v_pdst) : 
		thresh_hold(v_thresh_hold),
        mPicoDst{v_pdst},
		exists(false)
    {}
	void  update(int i, float v_pt){
		if (v_pt < thresh_hold) return;
		exists = true;
		pt.push_back(v_pt);
		i_tracks.push_back(i);
		return;
	}
	float max_pt(){
		if (exists) {
			return *std::max_element(std::begin(pt), std::end(pt));
		} else {
			return 0;
		}
	};
	int i_max(){
		float m_pt = max_pt();
		for (unsigned int i = 0; i < pt.size(); ++i){
			if (pt[i] == m_pt) return i_tracks[i];
		}
		return -1;
	};
    int n_TOF () {
        int n = 0;
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bTofPidTraitsIndex()!=-1) ++n;
        }
        return n;
    }
    int n_notTOF () {
        int n = 0;
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bTofPidTraitsIndex()==-1) ++n;
        }
        return n;
    }
    bool has_TOF () {
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bTofPidTraitsIndex()!=-1) return true;
        }
        return false;
    }
    std::vector<int> vec_TOF () {
        std::vector<int> vec;
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bTofPidTraitsIndex()!=-1) vec.push_back(i);
        }
        return vec;
    }
    int n_BEMC () {
        int n = 0;
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bemcPidTraitsIndex()!=-1) ++n;
        }
        return n;
    }
    int n_notBEMC () {
        int n = 0;
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bemcPidTraitsIndex()==-1) ++n;
        }
        return n;
    }
    std::vector<int> vec_BEMC () {
        std::vector<int> vec;
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bemcPidTraitsIndex()!=-1) vec.push_back(i);
        }
        return vec;
    }
    bool match_BEMC(class_bemc_tower_tracks* m_tr){
        for (auto i : i_tracks){
            if (m_tr->match_BEMC(i)) return true;
        }
        return false;
    }
    bool match_HT1(class_bemc_tower_tracks* m_tr){
        for (auto i : i_tracks){
            if (m_tr->match_HT1(i)) return true;
        }
        return false;
    }
    bool has_TOF_andBEMC (class_bemc_tower_tracks* m_tr) {
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bTofPidTraitsIndex()!=-1 && m_tr->match_BEMC(i)) return true;
        }
        return false;
    }
    bool has_TOF_andHT1(class_bemc_tower_tracks* m_tr){
        for (auto i : i_tracks){
            StPicoTrack* mtrack = mPicoDst->track(i); 
            if (mtrack->bTofPidTraitsIndex()!=-1 && m_tr->match_HT1(i)) return true;
        }
        return false;
    }
};

//----------------------------------------------------------------------------- 
Int_t ZeptoGrams::Init() {
    fout = new TFile(mOutName.Data(),"RECREATE") ;

    tree = new TTree("tree","yocto jet tree");
    tree->Branch("jets",         &b_jets);
    tree->Branch("tracks",       &b_tracks);
    /* tree->Branch("tracks_noTOF", &b_tracks_noTOF); */
    tree->Branch("event",        &event);
    m_bge = new MyJetDefinitionTerms(0.4);

    frand = new TRandom3(0);

    ev_cuts = new TH1F("ev_cuts",
            ";0:db 1:!badrun 2:trig 3:rnk 4:zdc 5:vz10 6:(vz-vpd)<=3.0 7:no30tr 8:TT 9:TT_TOF",
            10, -0.5, 9.5);

	n_events = new TH1F("n_events",
		TString::Format("%s;%s",
			"number of events#; 1st:val 2nd:btow-match 3rd:HT1-match",
            "0:TT>4 1:TT_TOF>4 2:TT>9 3:TT_TOF>9 4:HT1_e>4/8/12 5:#Sigma_N;N").Data(),
			48,0,6);

    tower_hits = new TH1F("tower_hits","tower_hits;n_tower;hits",5000,-0.5,4999.5);
    tower_hits_TH1 = new TH1F("tower_hits_TH1","tower_hits_TH1;n_tower;hits",5000,-0.5,4999.5);

    max_e_tow = new TH1F("max_e_tow","maximum energy with HT1 tag;energy;N",
            5000, -0.5, 4999.5);
    i_max_e_tow = new TH1F("i_max_e_tow","tower with maximum HT1 energy;n_tower;N",
            5000, -0.5, 4999.5);
    max_e_tow_under10 = new TH1F("max_e_tow_under10","maximum energy with HT1 tag, |vz<10|;energy;N",
            5000, -0.5, 4999.5);
    i_max_e_tow_under10 = new TH1F("i_max_e_tow_under10","tower with maximum HT1 energy, |vz<10|;n_tower;N",
            5000, -0.5, 4999.5);

    i_tow_HT1_hgram = new TH1F("i_tow_HT1_hgram","tower numbers that have HT1 hits;i;N",
            5000, -0.5, 4999.5);
    i_tow_HT1_profile = new TProfile("i_tow_HT1_profile","profile of energy of HT1 hits;i;N",
            5000, -0.5, 4999.5);

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
    vz_9TT = new TH1F("vz_9TT","primary vz distribution in events with a 9GeV charged track",100,-30,30);

    tr_cuts = new TH1F("tr_cuts",
            ";0:tot 1:pt>0.2 2:dca<3 3:eta<1.0 4:NHitsRat>0.52 5:TOF-Matched 6: BEMC-Matched",
            7, -0.5, 6.5);


    ln_ranking = new TH1F("ln_ranking",
            "log of best vertex ranking - perseving the negative sign",
            400,-20,20);

    bbcSig = new TH1F("bbcSig", "bbc ES, all Min Bias events that pass basic cuts;bbcES;N",
           10000, 0, 100000);

    zdcSig = new TH1F("zdcSig", "zdc.adcSum(east), all Min Bias events that pass basic cuts;zdc.adcSum(east);N",
           5060,-0.5,5059.5);

    vz_zdcSig_course = new TH2F("vz_zdcSig_course", ";Vertex_{z};zdc East Signal",
           100,-100,100,100,0,5060);
    vz_zdcSig        = new TH2F("vz_zdcSig", ";Vertex_{z};zdc East Signal",
           120,-30,30,200,0,5060);

    vz_bbcSig_course = new TH2F("vz_bbcSig_course", ";Vertex_{z};bbc.adc",
           100,-100,100,100,0,100000);
    vz_bbcSig        = new TH2F("vz_bbcSig", ";Vertex_{z};bbc East Signal",
           120,-30,30,200,0,100000);

    vz_zdcSig_e = new TProfile("vz_zdcSig_e","error on mean;vz;zdc east signal", 120,-30,30,"e");
    vz_zdcSig_s = new TProfile("vz_zdcSig_s","spread about mean;vz;zdc east signal", 120,-30,30,"p");

    vz_bbcSig_e = new TProfile("vz_bbcSig_e","error on mean;vz;bbc east signal", 120,-30,30,"e");
    vz_bbcSig_s = new TProfile("vz_bbcSig_s","spread about mean;vz;bbc east signal", 120,-30,30,"p");
    
    track_pt   = new TH1F("track_pt","all tracks that pass basic cuts, not TOF-matched;track p_T;N",200,0,20);
    refMult    = new TH1F("refMult", "ref_mult", 300,0,300);

    zdc_nTOF = new TProfile("zdc_nTOF", 
            "tracks (TOF match) per event per zdcE;zdcSig;N_{TOF-matched}^{tracks}",
            200,0,5060);

    bbc_nTOF = new TProfile("bbc_nTOF", 
            "tracks (TOF match) per event per zdcE;bbcSig;N_{TOF-matched}^{tracks}",
            200,0,100000);

    zdc_refMult = new TProfile("zdc_refMult", 
            "tracks (TOF match) per event per zdcE;zdcSig;N_{refMult}^{tracks}",
            200,0,5060);

    bbc_refMult = new TProfile("bbc_refMult", 
            "tracks (TOF match) per event per zdcE;bbcSig;N_{refMult}^{tracks}",
            200,0,100000);

    vz_refMult = new TProfile("vz_refMult", 
            "refMult per event per vz;vz;N_{refMult}^{tracks}",
            200,0,100000);

    vz_nAll = new TProfile("vz_nAll",";V_{z};N_{all good primary}^{tracks}", 120,-30,30,"e");
    vz_nTOF = new TProfile("vz_nTOF",";V_{z};N_{TOF-matched}^{tracks}", 120,-30,30,"e");
    vz_nBEMC = new TProfile("vz_nBEMC",";V_{z};N_{TOF-matched}^{tracks}", 120,-30,30,"e");

    nBTOFMatch = new TH1F("nBTOFMatch",";nBTOFMatch;N",100, -0.5, 99.5);
    nBEMCMatch = new TH1F("nBEMCMatch",";nBEMCMatch;N",100, -0.5, 99.5);


    float adc_bin_max = 4000.5;
    int adc_nbins = 4001;
    float energy_bin_max = 40.5;
    int energy_nbins = 41;
    // tower results
    /* adc_energy = new TProfile("adc_energy",";adc;energy",200,-0.5, 199.5); */
    /* for (int i = 0; i < 5; ++i){ */
        adc_energy = new TProfile(
                "adc_energy",
                "energy per adc;adc;N",
                energy_nbins,-0.5,energy_bin_max);
    /* } */
    
	adc_0 = new TH1F(
			"adc all triggers","adc hits",
			adc_nbins,-0.5,adc_bin_max);
	energy_0 = new TH1F(
			"energy",
			"all triggers;energy;",
			adc_nbins,-0.5,adc_bin_max);
	isHT1_a = new TH1F(
			"isHT1_a", 
			"for HT1 trigger;adc;",
			adc_nbins,-0.5,adc_bin_max);
	isHT2_a = new TH1F(
			"isHT2_a", 
			"for HT2 trigger;adc;",
			adc_nbins,-0.5,adc_bin_max);
	isHT3_a = new TH1F(
			"isHT3_a", 
			"for HT3 trigger;adc;",
			adc_nbins,-0.5,adc_bin_max);
	isHT1_e = new TH1F(
            "isHT1_e",
             "for HT1 trigger;energy;N",
			adc_nbins,-0.5,adc_bin_max);
	isHT2_e = new TH1F(
            "isHT2_e",
             "for HT2 trigger;energy;N",
			adc_nbins,-0.5,adc_bin_max);
	isHT3_e = new TH1F(
            "isHT3_e",
             "for HT3 trigger;energy;N",
			adc_nbins,-0.5,adc_bin_max);

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

    if (true) if (!mevent->isTrigger(500206)) { cout << "cut" << endl; return kStOk; }


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

    std::vector<unsigned int> i_bemc;
    // keep what the cuts would have been to fill in the 

    /* cout << "a1" << endl; */
	class_TT TT4(4., mPicoDst);
	class_TT TT9(9., mPicoDst);
    class_TT i_tracks(0.2, mPicoDst);

    int tr_cuts_e[7]{0,0,0,0,0,0,0};
    unsigned int n_primary = mPicoDst->numberOfTracks();
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

    
    // check for a trigger tower
    float max_E_tower = -100;
    int   id_maxtower = -100;
    for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
        StPicoEmcTrigger* emcTrig = mPicoDst->emcTrigger(i);
        bool isHT  = ( emcTrig->isHT1() || emcTrig->isHT2() || emcTrig->isHT3() );
        if (isHT && emcTrig->id() > 0) {
            int   bTowId  = emcTrig->id() - 1;
            StPicoBTowHit* bTowHit = mPicoDst->btowHit(bTowId); 
            float E = bTowHit->energy();
            if (E > 9.0 && mTT_threshold && E > max_E_tower){
                max_E_tower = E;
                id_maxtower = bTowId;
            }
        }
    }
    if (max_E_tower == -100) return kStOK;

    event.rand = frand->Uniform();
    // get primart vertex (Again)
    StPicoBTowHit* bTowHit = mPicoDst->btowHit(id_maxtower); 
    StThreeVectorF mVertex = mevent->primaryVertex();
    StEmcPosition *mPosition = new StEmcPosition();
    StThreeVectorF towerPosition = mPosition->getPosFromVertex(mVertex, id_maxtower+1);

    event.TTphi = towerPosition.phi();
    if (event.TTphi < 0) event.TTphi += 2.0*TMath::Pi();
    if (event.TTphi > 2*TMath::Pi()) event.TTphi -= 2.0*TMath::Pi();
    event.TTeta = towerPosition.pseudoRapidity();
    event.TTpt  = max_E_tower;

    event.e_ntracks = i_tracks.size();
    if (event.e_ntracks > fmax_ntracks) fmax_ntracks = event.e_ntracks;

    event.zdc         = mevent->ZDCx();
    event.refMult     = mevent->grefMult();
    event.runId       = mevent->runId();
    event.eventNumber = mevent->eventId();

    event.zdcE   = mevent->bbcAdcEast(0);
    for (int i = 1; i < 16; ++i) event.zdcE += mevent->bbcAdcEast(i);

    for (unsigned int i = 0; i < i_tracks.size(); ++i){
        ZeTrack* b_track = (ZeTrack*) b_tracks.ConstructedAt(i);
        /* StMuTrack* track = mudst->primaryTracks(track_indices_noTOF[i]); */
        StPicoTrack* track = static_cast<StPicoTrack*>(mPicoDst->track(i_tracks.i_tracks[i]));
        StThreeVectorF mom = track->pMom();
        b_track->t_phi = mom.phi();
        b_track->t_eta = mom.pseudoRapidity();
        b_track->t_pt  = mom.perp();
        /* b_track->check_isCentral(); */
    }

    /* cout << "a6" << endl; */
    // fill trigger diagrams
    /* for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){ */
    /*     /1* cout << "a7" << endl; *1/ */
    /*     StPicoEmcTrigger* mtrigger = mPicoDst->emcTrigger(i); */
    /*     int tower_id = mtrigger->id(); */
    /*     /1* cout << "c0" << " " << tower_id << endl; *1/ */
    /*     tower_hits->Fill(tower_id); */
    /*     // FIXME */
    /*     // print out the flag and values for the towerID == 0; */ 
    /*     // check for towerID < 0, */ 
    /*     // runfiredtriggerQA -- fill histograms used to generation of bad towers */
    /*     // https://github.com/joelmazer/star-jetframework/blob/master/StPicoTrackClusterQA.cxx -- see this */


    /*     /1* cout << "c1" << endl; *1/ */
		/* if (mtrigger->isHT1()) tower_hits_TH1->Fill(tower_id); */
    /*     /1* cout << "c2" << endl; *1/ */
    /*     if (tower_id == 0) continue; */
    /*     StPicoBTowHit* mhit = mPicoDst->btowHit(tower_id-1); */ 
    /*     float adc = mhit->adc(); */
    /*     float energy = mhit->energy(); */
    /*     adc_0->Fill(adc); */
    /*     energy_0->Fill(energy); */
    /*     adc_energy->Fill(adc, mhit->energy()); */
    /*     if (mtrigger->isHT1()) {isHT1_a->Fill(adc); isHT1_e->Fill(energy); } */
    /*     if (mtrigger->isHT2()) {isHT2_a->Fill(adc); isHT2_e->Fill(energy); } */
    /*     if (mtrigger->isHT3()) {isHT3_a->Fill(adc); isHT3_e->Fill(energy); } */
    /* } */


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

