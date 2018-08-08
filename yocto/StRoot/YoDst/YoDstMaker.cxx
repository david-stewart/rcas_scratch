// StZdcTriggerDetector (how do I get it?) (see SL18c/StRoot/StEvent/StZdcTriggerDetector.h)

// simple class to read a data set and make cuts to it
//  note: "east" = 0, "west" = 1 for east/west enumerations. Therefore zdc.adcSum(0) is the one that I want

#include "YoDstMaker.h"
#include <iostream>
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StBTofHeader.h"
#include "StEnumerations.h"

#include "StZdcTriggerDetector.h"

#include "fastjet/config.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"

using namespace fastjet;
/* #include "StMuDSTMaker/COMMON/StEventInfo.h" */
/* #include "StMuDSTMaker/COMMON/StRunInfo.h" */

#define  kNMaxJets   60
#define  kNMaxTracks 180

#include "TH1.h"
#include "TFile.h"
#include <vector>

double phi_cor(double phi_in){
    while (phi_in < -TMath::Pi()) { phi_in += 2*TMath::Pi(); }
    while (phi_in >  TMath::Pi()) { phi_in -= 2*TMath::Pi(); }
    return phi_in;
}

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

        /* Selector selector_rm0 = SelectorAbsEtaMax(1.0) * (!SelectorNHardest(0)); // <-- */
        /* bge_rm0 = new JetMedianBackgroundEstimator (selector_rm0, jet_def_bkgd, area_def_bkgd); // <-- */

        /* Selector selector_rm1 = SelectorAbsEtaMax(1.0) * (!SelectorNHardest(1)); // <-- */
        /* bge_rm1 = new JetMedianBackgroundEstimator (selector_rm1, jet_def_bkgd, area_def_bkgd); // <-- */
        
        Selector selector_rm2 = SelectorAbsEtaMax(1.0) * (!SelectorNHardest(2)); // <--
        bge_rm2 = new JetMedianBackgroundEstimator (selector_rm2, jet_def_bkgd, area_def_bkgd); // <--

        subtractor = new Subtractor( bge_rm2 ) ;
    };
};


ClassImp(YoDstMaker)                   // Macro for CINT compatibility

// constructor
YoDstMaker::YoDstMaker( 
    StMuDstMaker* maker, 
    const char* fout_name, 
    int triggerid,
    float TT_min
): 
    StMaker("YoDstMaker"), 
    file_out_name(fout_name), 
    mTriggerId(triggerid),
    m_TT_min(TT_min),
    b_jets("YoJet",    kNMaxJets),
    b_tracks("YoTrack",kNMaxTracks),
    b_tracks_noTOF("YoTrack", kNMaxTracks),
    max_njets(0),
    max_ntracks(0),
    max_ntracks_noTOF(0),
    mTestConditions(false),
    mAllTOF(false),
    debug(0)
{   // Initialize and/or zero all public/private data members here.
    /* fout = new TFile( fout_name, "recreate" ) ;  // Name was set previously in calling macro */ 
    bad_run_list = get_bad_runs();
    mMuDstMaker      = maker ;               
    mEventsProcessed = 0     ;              
}

void YoDstMaker::setTestConditions(){
    mTestConditions = true;
}
void YoDstMaker::setAllTOF(){
    mAllTOF = true;
}

YoDstMaker::~YoDstMaker() 
{ } // Destroy and/or zero out all public/private data members here.


int YoDstMaker::Init( )
{ // Do once at the start of the analysis
  /* Event YoDst:
   * 0: read from database
   * 1: has vertex and ranking > 0
   * 2: zdc (4k to 22.7k)
   * 3: not bad-run list
   * 4: triggered
   * 5: Vz < 40 cm
   * 6: Vz < 10 cm
   * 7: TT_raw
   * 8: TT_TOF
   */

    TString file_out_root = file_out_name;
    dlog = fopen(TString::Format("%s.debug",file_out_name.Data()).Data(),"w");
    file_out_root.Append(".root");
    file_out = new TFile(file_out_root.Data(),"RECREATE") ;

    tree = new TTree("tree","yocto jet tree");
    tree->Branch("jets",         &b_jets);
    tree->Branch("tracks",       &b_tracks);
    tree->Branch("tracks_noTOF", &b_tracks_noTOF);
    tree->Branch("event",        &event);

    m_bge = new MyJetDefinitionTerms(0.4);

    yrand = new TRandom3(0);


    ev_cuts = new TH1F("ev_cuts",
            ";0:db 1:!badr 2:trigger 3:rnk>0 4:zdcX,4-22.7k 5:vz10 6:vz-vpd<3.0 7:no30tr 8:TT 9:TT_TOF",
            10, -0.5, 9.5);

    

  /* Track cuts - Yanfang
   * 0: total
   * 1: |dca| < 1
   * 2: |eta| < 1
   * 3: NHitsFit >= 10  // Yanfang's cuts
   * 4: FYI: NHitsHit > 14 & NHitsRatio > 0.52 // typical of JetCorr
   * 5: TOF Matched
   */
   tr_cuts = new TH1F("tr_cuts",
            ";0:tot 1:pt>0.2 2:dca 3:eta>0.9 4:NHits 5:NitsAlt 6:TOF-Matched",
            7, -0.5, 6.5);

   vz = new TH1F("vz","primary vz distribution",200,-100,100);

   ln_ranking = new TH1F("ln_ranking",
            "log of best vertex ranking - perseving the negative sign",
            400,-20,20);

   bbcES_percentile = new TH1F("bbcES_MB", "bbc ES, all Min Bias events that pass basic cuts;bbcES;N",
           10000, 0, 70000);

   bbc_Nch_TOF = new TProfile("bbc_Nch_TOF", "tracks (TOF match) per event per bbcES",
           10000, 0, 70000);

   bbc_refMult = new TProfile("bbc_refMult", "refernece multiplicity per event per bbcES",
           10000, 0, 70000);


   zdcE_percentile = new TH1F("zdcE_MB", "zdc.adcSum(east), all Min Bias events that pass basic cuts;zdc.adcSum(east);N",
           4060,-0.5,4059.5);

   zdc_Nch_TOF = new TProfile("zdc_Nch_TOF", "tracks (TOF match) per event per zdcE",
           4060,0,4060);

   zdc_refMult = new TProfile("zdc_refMult", "reference multiplicity per event per zdcE signal",
           4060,0,4060);

    // new cuts
    track_pt   = new TH1F("track_pt","all tracks that pass basic cuts, not TOF-matched;track p_T;N",200,0,20);
    refMult    = new TH1F("refMult", "ref_mult", 3000,0,3000);

    vz_Nch_all = new TProfile("vz_nch_all","all good tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000);
    vz_Nch_TOF  = new TProfile("vz_nch_TOF","TOF-match tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000);
    vz_Nch_BEMC = new TProfile("vz_nch_BEMC","BEMC-match tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000);
    vz_Nch_TOFBEMC = new TProfile("vz_nch_TOFBEMC","(TOF and BEMC)-match tracks: N_{ch} vs ZDC rate (Hz); ZDC rate; <N_{ch}>", 100,4000,23000);
 
   bemc_hits = 0;

   return kStOk;
}

int YoDstMaker::Make()
 
{ // Do each event
    /* bool test_conditions = false; */
    if (mTestConditions) fprintf(dlog,"--- new event\n");



    b_jets.Clear();
    b_tracks.Clear();
    b_tracks_noTOF.Clear();
    if (false) cout  << " .start YoDstMaker. " << endl;

    //   500001 for the st_ssdmb files
    //   500206 for the st_physics files


    /* cout << "A0" << endl; */
    StMuDst* mudst     =  mMuDstMaker->muDst();
    if (!mudst) { return kStWarn; }

    StMuEvent* muEvent =  mMuDstMaker->muDst()->event() ;
    if (!muEvent) { return kStWarn; }
    /* printf("  mult: %i\n",muEvent->refMult()); */
    ev_cuts->Fill(0);
    if(mTestConditions) {
        if (muEvent->eventId() == 2252779 || muEvent->eventId() == 2255219 ) {
            StMuDst *mDst = mMuDstMaker->muDst();
            int c_index = mDst->currentVertexIndex();
            fprintf(dlog," ----eventID  %i\n", muEvent->eventId());
            fprintf(dlog," There are %i primary vertices.\n", mDst->GetNPrimaryVertex());
            for (unsigned int i = 0; i <  mMuDstMaker->muDst()->GetNPrimaryVertex(); ++i){
                mDst->setVertexIndex(i);
                StMuPrimaryVertex* pv = mDst->primaryVertex();
                fprintf(dlog," ranking(%i): %10.4f\n",i,pv->ranking());
            }
            mDst->setVertexIndex(c_index);
        }
    }

    /* if (!mTestConditions){ */
    int runId =  muEvent->runId();
    if (mTestConditions) fprintf(dlog," %-20s %-10i\n", "runID", muEvent->runId());
    if (mTestConditions) fprintf(dlog," %-20s %-10i\n", "eventID", muEvent->eventId());
    if (std::binary_search(bad_run_list.begin(), bad_run_list.end(), runId)) return kStOk; 
    ev_cuts->Fill(1);
    if (mTestConditions) fprintf(dlog," %-20s\n", "*passed runID");
    /* } */

    if (!muEvent->triggerIdCollection().nominal().isTrigger(mTriggerId)) return kStOK; 
    ev_cuts->Fill(2);
    if (mTestConditions) fprintf(dlog," %-20s\n", "*passed isTrigger");



    float zvert = muEvent->primaryVertexPosition().z();

    vz->Fill(zvert);
    zvert = TMath::Abs(zvert);


    /* cout << "A1" << endl; */
    //
    if ((float)mudst->numberOfPrimaryVertices() == 0) return kStOk;
    /* cout << "A2" << endl; */
    float rank = mudst->primaryVertex()->ranking();
    int sig = rank > 0 ? +1 : -1;
    float ln = TMath::Log(TMath::Abs(rank));
    ln_ranking->Fill(sig*ln);
    if (mTestConditions) fprintf(dlog," %-20s %-10.2f\n", "ranking", rank);

    if (rank < 0) return kStOk;
    ev_cuts->Fill(3);

    float zdc = muEvent->runInfo().zdcCoincidenceRate();
    if (zdc < 4000. || zdc > 22700.) { return kStOk; }
    ev_cuts->Fill(4);
    if (mTestConditions) fprintf(dlog," %-20s %10.2f\n", "zdc", zdc);

    /* cout << "  0 0 0 0 0   runId " << runId << endl; */


    if (mTestConditions) fprintf(dlog," %-20s %10.4f\n", "zvert", muEvent->primaryVertexPosition().z());
    if (zvert > 10) { return kStOk; }
    ev_cuts->Fill(5);
    
    if (mTestConditions){
        fprintf(dlog," %-20s %10.4f\n", "vpdVz",StMuDst::btofHeader()->vpdVz());
 		fprintf(dlog," %-20s %10.4f\n","zdc.ADC", 
			muEvent->zdcTriggerDetector().adcSum(StBeamDirection::east));
		StBbcTriggerDetector &bbc = muEvent->bbcTriggerDetector();
		float bbcES_val = 0;
		for (int i = 0; i<16; ++i){
			bbcES_val += bbc.adc(i);
		}
 		fprintf(dlog," %-20s %10.4f\n","BBC.ADC", bbcES_val);
    }
    if(TMath::Abs(StMuDst::btofHeader()->vpdVz()-muEvent->primaryVertexPosition().z())>3) { return kStOk; }
    ev_cuts->Fill(6);

    if (mTestConditions) {
 		fprintf(dlog," %-20s\n","  *passed delta vz");
	}

	

    // * Loop through the tracks
    // * Collect indices of which tracks are good TOF-matched tracks
    // * Collect indices of which tracks are no TOF-matched tracks (but
    //   otherwise good)
    // * Get the index of the highest pt TOF-matched track (if it is
    //   over 9GeV)

    int TT_index = -1;
    float max_TTpt = 0;
    bool has_9GeV = false;
    bool has4to9 = false;
    bool has_9 = false;

    vector<unsigned int> track_indices;
    vector<unsigned int> track_indices_noTOF;

    unsigned int nTOF     = 0;
    unsigned int nBEMC    = 0;
    unsigned int nTOFBEMC = 0;
    unsigned int nAll     = 0;

    for (unsigned int i = 0; i < mudst->numberOfPrimaryTracks(); ++i) {

        StMuTrack* track = mudst->primaryTracks(i);
        tr_cuts->Fill(0);

        if (track->pt() > 30) return kStOK; // through out the events

        if (track->pt() < 0.2)  continue;
        tr_cuts->Fill(1);

        if (TMath::Abs(track->dca().mag()) >= 3.0)  continue;
        tr_cuts->Fill(2);

        if (TMath::Abs(track->eta())  >= 0.9) continue;
        tr_cuts->Fill(3);

        bool fail_hits = true;
        if (track->nHits() > 10) {
            /* cout << " track pt " << track->pt() << endl; */
            fail_hits = false;
            tr_cuts->Fill(4);
        }

        float nhit_ratio = ((float)track->nHits()) / (float)track->nHitsPoss();
        /* cout << "nhit_ratio " << nhit_ratio << " " << track->nHits() << endl; */
        if (    (track->nHits() > 14)
             && (nhit_ratio   > 0.52)
           ) tr_cuts->Fill(5);
        if (fail_hits)  continue;


        // WARNING THIS IS THE CHANGE
        bool TOF = false;
        if (mAllTOF || track->index2BTofHit() >= 0) {
            track_indices.push_back(i);
            TOF = true;
            tr_cuts->Fill(6);
        } else {
            track_indices_noTOF.push_back(i);
        }

        bool BEMC = track->matchBEMC();

        track_pt->Fill(track->pt());
        ++nAll;
        if (TOF) ++nTOF;
        if (BEMC) ++nBEMC;
        if (TOF && BEMC) ++nTOFBEMC;


        /* cout << " bemc is matched? pt(" << track->pt() << ")  e(" << track->energyBEMC() <<")  " << track->matchBEMC() << endl; */
        /* if (track->matchBEMC()) { */
            /* cout << "  * * * *  *  *  *   *   *   *    *    *     * ---- -   -      -> FOUND IT!" << endl; */
            /* ++bemc_hits; */
        /* } */

        // check for BEMC match
        
        if (track->pt() >m_TT_min && TOF){
            has_9GeV = true;
            if (track->pt() > max_TTpt) {
                max_TTpt = track->pt();
                TT_index = i;
            }
        }
        if (TOF && track->pt() >= 4){
            if (track->pt()<9) has4to9 = true;
            if (track->pt()>=9) has_9 = true;
        }
    }
    ev_cuts->Fill(7);
    /* cout << "nTOF and tracks.inices " << nTOF << " " << track_indices.size() << endl; */
    vz_Nch_all->Fill(muEvent->runInfo().zdcCoincidenceRate(),nAll);
    vz_Nch_TOF->Fill(muEvent->runInfo().zdcCoincidenceRate(),nTOF);
    vz_Nch_BEMC->Fill(muEvent->runInfo().zdcCoincidenceRate(),nBEMC);
    vz_Nch_TOFBEMC->Fill(muEvent->runInfo().zdcCoincidenceRate(),nTOFBEMC);

    /* cout << "TT_index(" << TT_index */
    /*      << ")  track_indices.size(" << track_indices.size() */
    /*      << ")  has_1GeV( " << has_9GeV << ")" << endl; */
    StBbcTriggerDetector &bbc = muEvent->bbcTriggerDetector();
    float bbcES_val = 0;
    for (int i = 0; i<16; ++i){
        bbcES_val += bbc.adc(i);
    }

    bbcES_percentile->Fill(bbcES_val);
    float zdcE = muEvent->zdcTriggerDetector().adcSum(StBeamDirection::east);
    zdcE_percentile->Fill(zdcE);

    bbc_Nch_TOF->Fill(bbcES_val,nTOF);
    zdc_Nch_TOF->Fill(zdcE, nTOF);

    zdc_refMult->Fill(zdcE, muEvent->refMult());
    bbc_refMult->Fill(bbcES_val, muEvent->refMult());
    /* StZdcTriggerDetector zdcTrig = muEvent->zdcTriggerDetector(); */

    if (has_9GeV) ev_cuts->Fill(8);
    if   (TT_index > -1) { ev_cuts->Fill(9); } 
    else { return kStOk; }

    // Fill out the event information
    // * Fill out the TT information
    event.rand = yrand->Uniform();

    StMuTrack* TT_track = mudst->primaryTracks(TT_index);
    event.TTphi = TT_track->phi();
    event.TTeta = TT_track->eta();
    event.TTpt  = TT_track->pt();

    event.has4to9 = has4to9;
    event.has4and9 = (has4to9 && has_9);
    event.has9 = has_9;


    event.e_ntracks = track_indices.size();
    event.e_ntracks_noTOF = track_indices_noTOF.size();

    //debug
    int ii = 0;
    if (debug > 1) {
        printf("OOOOOOOOOOO Trigger Track!:  pt(%5.3f)  phi(%5.3f)\n", event.TTpt, event.TTphi);

        printf("            ntracks: reg(%5i)   noTOF(%5i)\n",event.e_ntracks, event.e_ntracks_noTOF);
        for (unsigned int i = 0; i < track_indices.size(); ++i) {
            StMuTrack* track = mudst->primaryTracks(track_indices[i]);
            printf("                 track(%4i::%4i)  :: phi(%5.3f) eta(%5.3f) pt(%5.3f)\n",ii,track_indices[i], 
                    phi_cor(track->phi()-event.TTphi), track->eta(), track->pt());
            ++ii;
        }
        ii = 0;
        for (unsigned int i = 0; i < track_indices_noTOF.size(); ++i) {
            StMuTrack* track = mudst->primaryTracks(track_indices_noTOF[i]);
            printf("                no_TOF(%4i::%4i) :: phi(%5.3f) eta(%5.3f) pt(%5.3f)\n",ii,track_indices_noTOF[i],
                    phi_cor(track->phi()-event.TTphi), track->eta(), track->pt());
            ++ii;
        }
    }

    if (event.e_ntracks > max_ntracks) max_ntracks = event.e_ntracks;
    if (event.e_ntracks_noTOF > max_ntracks_noTOF) max_ntracks_noTOF = event.e_ntracks_noTOF;

    event.zdc = muEvent->runInfo().zdcCoincidenceRate();
    event.refMult = muEvent->refMult();
    event.runId    = muEvent->runId();
    event.eventNumber = muEvent->eventNumber();

    /* StBbcTriggerDetector &bbc = muEvent->bbcTriggerDetector(); */
    for (int i = 0; i < 48; ++i) event.bbcAdc[i] = bbc.adc(i); 

    event.bbcES = event.get_bbcES();
    /* cout << " -- --- -- --- Delta bbcES " << bbcES_val - event.bbcES << endl; */
    event.bbcEL = event.get_bbcEL();
    event.bbcWS = event.get_bbcWS();
    event.bbcWL = event.get_bbcWL();
    event.zdcE = zdcE;

	StBTofHeader* btof = mudst->btofHeader();
    event.vpdHitsE = btof->numberOfVpdHits(StBeamDirection::east);
    event.vpdHitsW = btof->numberOfVpdHits(StBeamDirection::west);

    // * Fill in the noTOF tracks.
    // * Note that teh TOF matched tracks cannot be filled in at this
    //   point because they they will be arrange according to their 
    //   jet (i.e. first tracks in the highest pt, next tracks in the
    //   next highest pT jet, and so forth)
    for (unsigned int i = 0; i < track_indices_noTOF.size(); ++i) {
        YoTrack*   b_track_noTOF = (YoTrack*) b_tracks_noTOF.ConstructedAt(i);
        StMuTrack* track = mudst->primaryTracks(track_indices_noTOF[i]);
        b_track_noTOF->t_phi = phi_cor( track->phi() - event.TTphi);
        b_track_noTOF->t_eta = track->eta();
        b_track_noTOF->t_pt  = track->pt();
        b_track_noTOF->check_isCentral();
    }

    // * Group the remaining tracks into jets
    vector<PseudoJet> particles;
    /* unsigned int i_temp = 0; */
    
    // need to update this for user event-ness
    for (unsigned int i = 0; i < track_indices.size(); ++i) {
        /* StThreeVectorF tr_P = mudst->primaryTracks(track_indices[i])->momentum(); */
        StMuTrack* track = mudst->primaryTracks(track_indices[i]);
        particles.push_back (PseudoJet());
        particles[i].reset_PtYPhiM(track->pt(), track->eta(), track->phi()-event.TTphi);
        particles[i].set_user_index(track_indices[i]);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /* cout << " event.vpdHitsEW " << event.vpdHitsE << " " << event.vpdHitsW << endl; */
    // Get background subtraction
    float ghost_maxrap = 2.0;
    float jet_R = 0.4;

    AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(ghost_maxrap,1,0.01));
    JetDefinition jet_def(antikt_algorithm, jet_R);
    ClusterSequenceArea cs(particles, jet_def, area_def);
    vector<PseudoJet> jets_all = cs.inclusive_jets();
    

    /* NOTE: the following lineof code cuts on the jet eta */
    /* Otherwise, use the line below: */
    /* Selector Fiducial_cut_selector = SelectorAbsEtaMax(0.9 - jet_R); */ 
    vector<PseudoJet> jets = sorted_by_pt( jets_all );
    m_bge->bge_rm2->set_particles(particles); // set background by the area of interest.
                                              // However, record all jets
    event.rho = m_bge->bge_rm2->rho();

    unsigned int njets = 0;
    unsigned int ntracks = 0;

    /* cout << " -- making jets -- " << endl; */
    std::vector<unsigned int> i_jets_edge; // jets with |eta| > 0.9
    for (unsigned int i = 0; i < jets.size(); ++i){
        PseudoJet& jet = jets[i];
        if ( jet.is_pure_ghost() ) { continue; }
        
        if ( jet.eta() > 0.5 || jet.eta() < -0.5) {
            i_jets_edge.push_back(i);
            continue;
        }

        YoJet* b_jet = (YoJet*) b_jets.ConstructedAt(njets); 

        //debug
        if (false) { printf("     tracks in jet(%4i)\n", njets); }

        ++njets;

        b_jet->j_phi = jet.phi_std();
        float phi_abs = TMath::Abs(jet.phi_std());
        if (phi_abs < 0.25 * TMath::Pi()) {
            b_jet->j012 = 0;
        } else if (phi_abs > 0.75*TMath::Pi()) {
            b_jet->j012 = 2;
        } else {
            b_jet->j012 = 1;
        }
        b_jet->j_eta = jet.eta();
        b_jet->j_pt  = jet.pt();
        b_jet->j_pt_corr  = (jet - jet.area_4vector()*m_bge->bge_rm2->rho(jet)).pt();
        b_jet->j_area = jet.area();
            
        vector<PseudoJet> constituents = sorted_by_pt(jet.constituents());
          
        unsigned int nch = 0;
        bool first = true;
        for ( unsigned int j = 0; j < constituents.size(); j++ ){
            PseudoJet c_jet = constituents[j];
            if ( c_jet.user_index() == -1 ) break; // check for ghost particles 

            //debug 
            if (false){
                printf("              re-track(%4i::%4i)  :: phi(%5.3f) eta(%5.3f) pt(%5.3f)\n",njets,c_jet.user_index(), 
                    c_jet.phi_std(), c_jet.eta(), c_jet.pt());
                ++ii;
            }

            YoTrack* b_track = (YoTrack*) b_tracks.ConstructedAt(ntracks);
            if (first) {
                first = false;
                b_jet->j_track0 = ntracks;
            }
            ++nch;
            ++ntracks;
            b_track->t_phi = c_jet.phi_std() ;
            b_track->t_eta = c_jet.eta();
            b_track->t_pt  = c_jet.pt();
            b_track->check_isCentral();
            if (debug){
                printf("              jet-track(%4i::%4i)  :: phi(%5.3f) eta(%5.3f) pt(%5.3f)\n",njets,c_jet.user_index(), 
                    c_jet.phi_std(), c_jet.eta(), c_jet.pt());
            }
        }
        b_jet->j_ntracks = nch;
    }
    
    event.edge_track0 = 0;
    event.n_edge_tracks = 0;
    bool is_first_edge = true;
    // Fill in the tracks from jets out of the eta range
    for (unsigned int i = 0; i < i_jets_edge.size(); ++i){
        PseudoJet& jet = jets[i_jets_edge[i]];
        vector<PseudoJet> constituents = sorted_by_pt(jet.constituents());
        for ( unsigned int j = 0; j < constituents.size(); j++ ){
            PseudoJet c_jet = constituents[j];
            if ( c_jet.user_index() == -1 ) break; // check for ghost particles 
            if (is_first_edge) {
                event.edge_track0 = ntracks;
                is_first_edge = false;
            }
            YoTrack* b_track = (YoTrack*) b_tracks.ConstructedAt(ntracks);
            ++ntracks;
            ++event.n_edge_tracks;
            b_track->t_phi = c_jet.phi_std() ;
            b_track->t_eta = c_jet.eta();
            b_track->t_pt  = c_jet.pt();
            b_track->check_isCentral();
            if (debug){
                printf("              edge-track(%4i::%4i)  :: phi(%5.3f) eta(%5.3f) pt(%5.3f)\n",ii,c_jet.user_index(), 
                    c_jet.phi_std(), c_jet.eta(), c_jet.pt());
            }
        }
    }

    // Fill in all of the non-trigger tracks
    event.njets = njets;

    // The following commented check only works if there isn't a fiducal check jet, which removes some jets.
    if (debug) {
        if (event.e_ntracks != ntracks) {
            cout << "WARNING A1 FATAL: ntracks from jet construction does not equal all the tracks!" << endl;
        } else {
            cout << " - O - O - ALL tracks found" << endl;
        }
    }

    if (njets > max_njets) max_njets = njets;

    /* cout << " || | | | | || " << "event.e_ntracks - ntracks :: " << event.e_ntracks */ 
         /* << " - " << ntracks << " :: " << event.e_ntracks - ntracks << endl; */

    // Get the background subtrackted jet Pts
    tree->Fill();
    mEventsProcessed++ ;
    return kStOK ;
}


int YoDstMaker::Finish( )
{ // Do once at the end the analysis
    cout << " bemc_hits " << bemc_hits << endl;
    cout << "END" << endl;
  file_out->Write();
  file_out->Close();
  cout << "Total Events Processed in DstMaker " << mEventsProcessed << endl ;

  file_out_name.Append(".maxTrJe");
  FILE *f = fopen(file_out_name,"w");
  fprintf(f, "Max number of jets:                 %12i\n", max_njets);
  fprintf(f, "Max number of tracks w/  TOF match: %12i\n", max_ntracks);
  fprintf(f, "Max number of tracks w/o TOF:       %12i\n", max_ntracks_noTOF);
  fclose(f);

  return kStOk ;  
}
