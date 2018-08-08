//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "JetTrees.h"
#include <iostream>

#include "fastjet/config.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"

#define kNMaxJets 30
using namespace fastjet;

struct TT_list {
    // short structure to add towers (Et, phi, eta) into vectors
    // and also keep track of which is the "trigger tower"
    float Et_threshold;
    std::vector<float> Et;
    std::vector<float> phi;
    std::vector<float> eta;
    int index;
    int size;
    float Et_max;
    TT_list(float i_Et_threshold) :
        Et_threshold{i_Et_threshold},
        Et {std::vector<float>{}},
        phi{std::vector<float>{}},
        eta{std::vector<float>{}},
        index{-1},
        size{0},
        Et_max{-1.}
    {};
    void add(float i_Et, float i_phi, float i_eta) {
        /* cout << " adding " << i_Et << endl; */
        ++size;
        Et.push_back(i_Et);
        phi.push_back(i_phi);
        eta.push_back(i_eta);
        if (i_Et > Et_max) {
            Et_max = i_Et;
            if (Et_max > Et_threshold) index = size-1;
        }
    };
    bool hasTT() { return index != -1; }
    float TT_Et()  { 
        if (index == -1) return -1.;
        else return Et[index]; 
    }
    float TT_phi() { return phi[index]; }
    float TT_eta() { return eta[index]; }
};
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


ClassImp(JetTrees)
//-----------------------------------------------------------------------------
JetTrees::JetTrees(
        const char*     i_name,
        StPicoDstMaker* i_picoMaker,
        const char*     i_outName,
        int             i_triggerid,
        const char*     i_bad_tower_file,
        const char*     i_bad_run_file,
        bool            i_debug,
        bool            i_makePreCuts

    )
  : StMaker(i_name), 
    /* max_ntracks_noTOF(0), */
    /* mTestConditions(false), */
    /* mAllTOF(false), */
    fPicoDstMaker   {i_picoMaker},
    fOutName        {i_outName},
    ftriggerid      {i_triggerid}, 
    fbad_tower_file {i_bad_tower_file},
    fbad_run_file   {i_bad_run_file},
    fdebug          {i_debug},
    fmakePreCuts    {i_makePreCuts},
    fTT_threshold   {4.0},
    f_jetR          {0.4},
    b_jets          {"JtJet",   kNMaxJets},
    fEventsProcessed{0}
{ 
    f_jet_etaMax = 1.0 - f_jetR;
};

//----------------------------------------------------------------------------- 
JetTrees::~JetTrees()
{ };

//----------------------------------------------------------------------------- 
Int_t JetTrees::Init() {
    // a. Open the TFile
    // b. Open the log file
    // c. Set the BadTower and BadRun lists
    // d. Set the TTree

    // a. Open the TFile
    fout = new TFile(TString::Format("%s.root",fOutName.Data()).Data(),"RECREATE") ;

    // b. Open the log file
    if (fdebug) dlog = fopen(TString::Format("%s.debug",fOutName.Data()).Data(),"w");
    flog = fopen(TString::Format("%s.log",fOutName.Data()).Data(),"w");
    fprintf(flog,"Initializing log\n");

    // c. Set the BadTower and BadRun lists
    ifstream file;
    file.open(fbad_tower_file);
    if (!file.is_open()) {
        fprintf(flog,"Warning: the bad tower file %s could not be opened.\n"
                     "  No bad tower list obtained.\n",
                fbad_tower_file);
        fbad_tower_list = vector<int>{};
    } else {
        int bad_tower;
        while (file >> bad_tower) fbad_tower_list.push_back(bad_tower);
        std::sort(fbad_tower_list.begin(), fbad_tower_list.end());
        fprintf(flog,"Read in bad tower list from %s\n",fbad_tower_file);
    }
    file.close();

    file.open(fbad_run_file);
    if (!file.is_open()) {
        fprintf(flog,"Warning: the bad run file %s could not be opened.\n"
                     "  No bad run list obtained.\n",
                fbad_run_file);
        fbad_run_list = vector<int>{};
    } else {
        int bad_run;
        while (file >> bad_run) fbad_run_list.push_back(bad_run);
        std::sort(fbad_run_list.begin(), fbad_run_list.end());
        fprintf(flog,"Read in bad run list from %s\n",fbad_run_file);
    }
    file.close();

    // d. Set the TTree
    ftree = new TTree("tree","Jet Tree Recoiling from Highest Et Tower");
    ftree->Branch("jets",         &b_jets);
    ftree->Branch("event",        &fevent);

    if (fmakePreCuts) {
        fpc_tree = new TTree("pc_tree","Some conditions after event cuts but priot to Et Tower > Et Min cut");
        fpc_tree->Branch("pre_cut_data",  &fpc);
    }

    // e. Initialize fbge, frand, and histograms
    f_bge = new MyJetDefinitionTerms(0.4);
    frand = new TRandom3(0);

    fHgram_ev_cuts = new TH1F("ev_cuts",
    TString::Format(
        ";0:db 1:!badrun 2:trig 3:rnk 4:zdcX 5:vz10 6:(vz-vpd)<=3.0 7:allTr<30 8:allBTowHit<30 9:TT_Et>%f",
                fTT_threshold).Data(),
            10, -0.5, 9.5);

    fHgram_tr_cuts = new TH1F("tr_cuts",
            "tracks passing each criterion (TOF matching not required"
            ";0:all 1:pT>0.2 2:|dca|<3 3:|eta|<1 4:nRat>0.52 5:TOF-matched;N_{tracks}",
            6, -0.5, 5.5);

    /* fHgram_bbcES_percentile = new TH1F("bbcES_percentile", */
    /*         "BBC.ADC East Small signal, all events (after event cuts, excepting trigger;BBC.ADC East Small; N", */
    /*        10000, 0, 70000); */

    /* fHgram_zdcE_percentile = new TH1F("zdcE_percentile", "zdc.adcSum(east), all events that pass basic cuts;zdc.adcSum(east);N", */
    /*        4060,-0.5,4059.5); */

    int nbins_prun = (128-127)*120;
    fHgram_ev_prun = new TH1F("Nev_prun","number of events in each run, WARNING if val in 148001; DD + (RRR - 0.5)*120.; N",
            nbins_prun, 127, 148);
    fProfE_vz_prun  = new TProfile("vz_prun","vz per run;DD+(RRR-0.5)*120;N",
            nbins_prun, 127, 148);
    fProfE_zdcSumAdcEast_prun  = new TProfile("zdcE_prun","zdcSumAdcEast per run;DD+(RRR-0.5)*120;N",
            nbins_prun, 127, 148);
    fProfE_bbcAdcES_prun  = new TProfile("bbcAdcES_prun","bbcAdcES per run;DD+(RRR-0.5)*120;N",
            nbins_prun, 127, 148);
    fProfE_zdcX_prun  = new TProfile("zdcX_prun","zdcX per run;DD+(RRR-0.5)*120;N",
            nbins_prun, 127, 148);

    fHgram_vz = new TH1F("vz","primary vz distribution (prior to vz and (vz-vpd) cut);V_{z};N",200,-100,100);

    fHgram_zdcX = new TH1F("zdcX","primary zdcX distribution (prior to zdcX cut);zdcX;N",10000,0,50000);

    fHgram_ln_ranking = new TH1F("ln_ranking",
            "log of best vertex ranking - perseving the negative sign",
            400,-20,20);

    fHgram_track_pt = new TH1F("track_pt","p_{T} of all good tracks;p_{T};N",
            500,0,31);

    fHgram_BTowHit_pt = new TH1F("BTowHit_pt", 
    "pt hits per tower (not just trigger towers);p_{T};N", 500,0,31);

    // eta binning edges:
     double eta_edges[41] = {
     -0.98800, -0.94600, -0.90000, -0.85000, -0.80000, -0.75000, -0.70000,
     -0.65000, -0.60000, -0.55000, -0.50000, -0.45000, -0.40000, -0.35000,
     -0.30000, -0.25000, -0.20000, -0.15000, -0.10000, -0.05085,  0.00005,
      0.05090,  0.10000,  0.15000,  0.20000,  0.25000,  0.30000,  0.35000,
      0.40000,  0.45000,  0.50000,  0.55000,  0.60000,  0.65000,  0.70000,
      0.75000,  0.80000,  0.85000,  0.90000,  0.94600,  0.98800};

    // phi binning edges:
     double phi_edges[121] = {
     -3.14435, -3.08925, -3.03690, -2.98450, -2.93215, -2.87980, -2.82745,
     -2.77510, -2.72270, -2.67035, -2.61800, -2.56565, -2.51330, -2.46090,
     -2.40855, -2.35620, -2.30385, -2.25150, -2.19910, -2.14675, -2.09440,
     -2.04205, -1.98970, -1.93730, -1.88495, -1.83260, -1.78025, -1.72790,
     -1.67550, -1.62315, -1.57080, -1.51845, -1.46610, -1.41370, -1.36135,
     -1.30900, -1.25665, -1.20430, -1.15190, -1.09955, -1.04720, -0.99485,
     -0.94250, -0.89010, -0.83775, -0.78540, -0.73305, -0.68070, -0.62830,
     -0.57595, -0.52360, -0.47125, -0.41890, -0.36650, -0.31415, -0.26180,
     -0.20945, -0.15710, -0.10470, -0.05235,  0.00000,  0.05235,  0.10470,
      0.15710,  0.20945,  0.26180,  0.31415,  0.36650,  0.41890,  0.47125,
      0.52360,  0.57595,  0.62830,  0.68070,  0.73305,  0.78540,  0.83775,
      0.89010,  0.94250,  0.99485,  1.04720,  1.09955,  1.15190,  1.20430,
      1.25665,  1.30900,  1.36135,  1.41370,  1.46610,  1.51845,  1.57080,
      1.62315,  1.67550,  1.72790,  1.78025,  1.83260,  1.88495,  1.93730,
      1.98970,  2.04205,  2.09440,  2.14675,  2.19910,  2.25150,  2.30385,
      2.35620,  2.40855,  2.46090,  2.51330,  2.56565,  2.61800,  2.67035,
      2.72270,  2.77510,  2.82745,  2.87980,  2.93215,  2.98450,  3.03690,
      3.08925,  3.14435};

    fHgram_BTowHit_phieta = new TH2F("BTowHit_phieta", 
    "NHits per tower;#eta;#phi", 40, eta_edges, 120, phi_edges);

    fHgram_TT_phieta = new TH2F("TT_phieta", 
    "NHits per trigger tower;#eta;#phi", 40, eta_edges, 120, phi_edges);

  /* Track cuts - Yanfang
   * 0: total
   * 1: pt > 0.2
   * 2: |dca| < 1
   * 3: |eta| < 1
   * -- cut -- 3: NHitsFitRatio >= 10  // Yanfang's cuts
   */
  return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t JetTrees::Finish() {
    fout->Write();
    fout->Close();
    fprintf(flog," Finished running. Read %lli jet events.\n\n", fEventsProcessed);
    fclose(flog);
    if (fdebug) fclose(dlog);
    return kStOK;
}
    
//----------------------------------------------------------------------------- 
void JetTrees::Clear(Option_t *opt) { }

//----------------------------------------------------------------------------- 
Int_t JetTrees::Make() {
    if (fdebug) fprintf(dlog,"--- new event\n");
    b_jets.Clear();
    if(!fPicoDstMaker) {
        LOG_WARN << " No PicoDstMaker! Skip! " << endm;

        return kStWarn;
    }

    fPicoDst = fPicoDstMaker->picoDst();
    if(!fPicoDst) {
        LOG_WARN << " No PicoDst! Skip! " << endm;
        return kStWarn;
    }
    fHgram_ev_cuts->Fill(0);
    StPicoEvent* mevent = fPicoDst->event();

    if (fdebug) fprintf(dlog," %-20s %-10i\n", "runID", mevent->runId());
    if (fdebug) fprintf(dlog," %-20s %-10i\n", "eventID", mevent->eventId());
    /* if (fdebug) cout << " runID " << mevent->runId() << endl; */
    if (std::binary_search(fbad_run_list.begin(), fbad_run_list.end(), mevent->runId())) 
        return kStOk; 
    fHgram_ev_cuts->Fill(1);
    if (fdebug) fprintf(dlog," %-20s\n", "*passed runID");

    if (ftriggerid != 0) {
        if (!mevent->isTrigger(ftriggerid)) return kStOk;
    }
    fHgram_ev_cuts->Fill(2);
    if (fdebug) fprintf(dlog," %-20s\n", "*passed isTrigger");

    float rank = mevent->ranking();
    int sig = rank > 0 ? +1 : -1;
    float ln = TMath::Log(TMath::Abs(rank));
    fHgram_ln_ranking->Fill(sig*ln);
    if (fdebug) fprintf(dlog," %-20s %-10.2f\n", "ranking", rank);

    if (sig < 0) return kStOK;
    fHgram_ev_cuts->Fill(3);

    float zdcX = mevent->ZDCx();
    fHgram_zdcX->Fill(zdcX);
    if (zdcX < 4000. || zdcX > 22700.) { return kStOk; }
    fHgram_ev_cuts->Fill(4);
    if (fdebug) fprintf(dlog," %-20s %10.2f\n", "zdc", zdcX);

    /* float sigZDC = 0; */
    /* for (int i = 0; i < 24; ++i){ */

    /*     sigZDC += */ 
    float z_vert = mevent->primaryVertex().z();
    fHgram_vz->Fill(mevent->primaryVertex().z());
    if (fdebug) fprintf(dlog," %-20s %10.4f\n", "zvert",mevent->primaryVertex().z());

    if (TMath::Abs(z_vert) > 10) return kStOk;
    fHgram_ev_cuts->Fill(5);

    if (fdebug){
        fprintf(dlog," %-20s %10.4f\n", "vpdVz",  mevent->vzVpd());
 		fprintf(dlog," %-20s %10.4f\n","zdc.ADC", 
			mevent->ZdcSumAdcEast());
		float bbcAdcES = mevent->bbcAdcEast(0);
		for (int i = 1; i < 16; ++i) bbcAdcES += mevent->bbcAdcEast(i);
 		fprintf(dlog," %-20s %10.4f\n","BBC.ADC", bbcAdcES);
    }
    if (TMath::Abs(z_vert - mevent->vzVpd())>3.0) return kStOk;
    fHgram_ev_cuts->Fill(6);
    if (fdebug) {
 		fprintf(dlog," %-20s\n","  *passed delta vz");
	}



    // get the tracks and check for > 30 GeV
    //
    // note: won't use particles are tracks until it is assured that there
    // are not >30 GeV tracks/TT
    int track_cuts[6] = {0,0,0,0,0,0};
    vector<PseudoJet> particles;
    for (unsigned int i = 0; i < fPicoDst->numberOfTracks(); ++i){
        StPicoTrack* track = static_cast<StPicoTrack*>(fPicoDst->track(i));
        if (!track->isPrimary()) continue;

        // thanks to joel mazer
        ++track_cuts[0];
        StThreeVectorF Ptrack = track->pMom();
        float pt  = Ptrack.perp();
        if (pt > 30)   return kStOK; // cut out this event
        if (pt < 0.2)  continue;
        ++track_cuts[1];

        float phi = Ptrack.phi();
        float eta = Ptrack.pseudoRapidity();
        

        // thanks to joel mazer
        float dca = (track->dcaPoint() - mevent->primaryVertex()).mag();
        if (dca > 3.0)  continue;
        ++track_cuts[2];

        if (TMath::Abs(eta)  >= 1.0) continue;
        ++track_cuts[3];

        float nhit_ratio = ((float)track->nHitsFit()) / (float)track->nHitsMax();
        if (nhit_ratio <= 0.52) continue;
        ++track_cuts[4];

        if (track->bTofPidTraitsIndex() != -1) ++track_cuts[5];

        particles.push_back (PseudoJet());
        unsigned int j = particles.size() - 1;
        particles[j].reset_PtYPhiM(pt, eta, phi);
        particles[j].set_user_index(i);
    }
    fHgram_ev_cuts->Fill(7);

    //Check for there not being any +30 jets
    TT_list towlist(fTT_threshold);
    for (unsigned int i=0 ; i < fPicoDst->numberOfEmcTriggers(); ++i){
        StPicoEmcTrigger* emcTrig = fPicoDst->emcTrigger(i);
        if (! emcTrig->isHT1()) continue;
        int   trigId  = emcTrig->id();
        if (trigId < 1) {
            fprintf(flog,"Warning! in ev(%i) emcTriggerId(%i) (should be< 1)\n",
                    trigId, mevent->eventId());
            continue;
        }
        if (std::binary_search(fbad_tower_list.begin(), 
            fbad_tower_list.end(), trigId)) continue;
        StPicoBTowHit* bTowHit = fPicoDst->btowHit(trigId-1); 
        StEmcPosition *mPosition = new StEmcPosition();
        // thanks to Joel Mazer
        StThreeVectorF towLoc = mPosition->getPosFromVertex( 
                mevent->primaryVertex(), trigId);
        float towEt    = bTowHit->energy() / ((double)TMath::CosH(towLoc.pseudoRapidity()));
        if (towEt > 30) return kStOk;
        if (towEt < 0.2) {
            // given triggers, this shouldn't happen  
            fprintf(flog,"Warning: in ev(%i) tower with Et(%f) found.\n",
                    mevent->eventId(), towEt);
            continue;
        }
        towlist.add(towEt, towLoc.phi(), towLoc.pseudoRapidity());  
    }
    fHgram_ev_cuts->Fill(8);

    // fill the signal histograms (for getting the percentile on the minimum bias data sets)
    float bbcAdcES = mevent->bbcAdcEast(0);
    for (int i = 1; i < 16; ++i) bbcAdcES += mevent->bbcAdcEast(i);

    /* double bbcAdj   = (bbcAdcES > 1. && bbcAdcES < 64380.) */ 
    /*     ? bbcAdcES - (383.7*(z_vert) - 7.937*(z_vert)*(z_vert)) */
    /*     : -1. ; */

    /* double zval = mevent->ZdcSumAdcEast(); */
    /* double zdcAdj  = (zval> 0.5 && zval < 4028.5) */
    /*     ?  zval - (15.38*(z_vert) - 0.5913*(z_vert)*(z_vert)) */
    /*     :  -1. ; */

    if (fmakePreCuts) {
        fpc.bbcAdcES = bbcAdcES;
        /* fpc.bbcAdj   = bbcAdj; */
        fpc.zdcSumAdcEast  = mevent->ZdcSumAdcEast();
        /* fpc.zdcAdj   = zdcAdj; */
        fpc.vz       = z_vert;
        fpc.zdcX     = mevent->ZDCx();
        fpc.ntracks  = particles.size();
        fpc.ntowers  = towlist.size;
        fpc.max_Et   = towlist.Et_max;
        fpc.has_TT   = towlist.hasTT();
        fpc.runId    = mevent->runId();

        fpc_tree->Fill();
    }
    int runId = mevent->runId();
    /* cout << runId << "  <<< " << endl; */
    int day = (runId/1000)%1000;
    int run = runId%1000;
    /* cout << runId << "  " << day << " " << run << endl; */
    float xrun = day + ((float)run-0.5)*120.;
    fHgram_ev_prun           ->Fill(xrun);
    fProfE_vz_prun           ->Fill( z_vert                 , xrun);
    fProfE_zdcSumAdcEast_prun->Fill( mevent->ZdcSumAdcEast(), xrun);
    fProfE_bbcAdcES_prun     ->Fill( bbcAdcES               , xrun);
    fProfE_zdcX_prun         ->Fill( mevent->ZDCx()         , xrun);

    if (!towlist.hasTT()) { return kStOk; }
    fHgram_ev_cuts->Fill(9);

    for (auto p : particles){
        fHgram_track_pt->Fill(p.pt());
    }

    //now that the event is kept, fill in the histograms, make the jet, and filli the tree
    fevent.TTphi = towlist.TT_phi();
    fevent.TTeta = towlist.TT_eta();
    fevent.TT_Et = towlist.TT_Et();
    fevent.sum_ntracks = particles.size();
    fevent.sum_ntracks_TOFmatched = track_cuts[5];

    // fill signals
    fevent.bbcAdcES = bbcAdcES;
    /* fevent.bbcAdj   = bbcAdj; */
    fevent.bbcAdcEL = fevent.bbcAdcES;
    for (int i = 16; i < 24; ++i) fevent.bbcAdcES += mevent->bbcAdcEast(i);
    fevent.zdcSumAdcEast = mevent->ZdcSumAdcEast();
    /* fevent.zdcAdj = zdcAdj; */

    fevent.runId = mevent->runId();
    /* cout << fevent.runId << "  <<< " << endl; */
    fevent.eventId = mevent->eventId();
    fevent.rand  = frand->Uniform();
    fevent.nBTow_HT1 = towlist.size;

    // fill histograms
    for (int i=0; i < 6; ++i){
        for (int j =0; j<track_cuts[i]; ++j) 
            fHgram_tr_cuts->Fill(i);
    }
    for (int i{0}; i<towlist.size; ++i){
        fHgram_BTowHit_pt->Fill(towlist.Et[i]);
        fHgram_BTowHit_phieta->Fill(towlist.eta[i], towlist.phi[i]);
    }
    fHgram_TT_phieta->Fill(towlist.TT_eta(), towlist.TT_phi());

    //make and fill jets
    AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(4.0,1,0.01));
    JetDefinition jet_def(antikt_algorithm, f_jetR);
    ClusterSequenceArea cs(particles, jet_def, area_def);
    vector<PseudoJet> jets_all = cs.inclusive_jets();
    vector<PseudoJet> jets = sorted_by_pt( jets_all );
    f_bge->bge_rm2->set_particles(particles); // set background by the area of interest.
                                              // However, record all jets
                                              //
    fevent.rho = f_bge->bge_rm2->rho();

    unsigned int njets = 0;
    for (unsigned int i = 0; i < jets.size(); ++i){
        PseudoJet& jet = jets[i];
        if ( jet.is_pure_ghost() ) { continue; }
        if ( jet.eta() > f_jet_etaMax || jet.eta() < -f_jet_etaMax) continue;

        JtJet* b_jet = (JtJet*) b_jets.ConstructedAt(njets); 
        ++njets;

        b_jet->phi = jet.phi_std();
        double dphi = jet.phi_std() - fevent.TTphi;
        while (dphi <-3.1415926536) dphi += 6.2831853072; // 2*pi
        while (dphi > 3.1415926536) dphi -= 6.2831853072;
        b_jet->dphi = dphi;
        float dphi_abs = TMath::Abs(dphi);
        if (dphi_abs < 0.7853981634) { // 0.25*pi
            b_jet->dphi_bin = 0;
        } else if (dphi_abs > 2.3561944902) { // 0.75*pi
            b_jet->dphi_bin = 2;
        } else if (dphi_abs > 0) { // break up the transverse side into positive and negative
            b_jet->dphi_bin = 1;
        } else {
            b_jet->dphi_bin = -1;
        }

        b_jet->eta = jet.eta();
        b_jet->pt  = jet.pt();
        b_jet->pt_corr  = (jet - jet.area_4vector()*f_bge->bge_rm2->rho(jet)).pt();
        b_jet->area = jet.area();
            
        vector<PseudoJet> constituents = sorted_by_pt(jet.constituents());
        unsigned int nch = 0;
        for ( unsigned int j = 0; j < constituents.size(); j++ ){
            PseudoJet c_jet = constituents[j];
            if ( c_jet.user_index() == -1 ) break; // check for ghost particles 
            ++nch;
        }
        b_jet->ntracks = nch;
    }
    fevent.njets = njets;
    fevent.refMult = mevent->refMult();
    fevent.zdcX = mevent->ZDCx();
    fevent.vz = z_vert;

    //fill the jet branches
    ftree->Fill();
    fEventsProcessed++ ;
	return kStOK;
}

