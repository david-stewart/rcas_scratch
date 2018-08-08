#define genevents_cxx
#include "genevents.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

int get_n_digits(int pdg_id){
    return 
        pdg_id / 10000 > 0 ? -1 :
        pdg_id / 1000  > 0 ?  4 :
        pdg_id / 100   > 0 ?  3 :
        pdg_id / 10    > 0 ?  2 : -1;
}

bool get_charge(int pdg_id, int n_digits){
    if (n_digits == 2) { 
        return 
            pdg_id == 2 ? false :
            pdg_id == 24 ? true : // W boson
            pdg_id == 34 ? true : // W boson
            false;
    }
    if (n_digits != 3 && n_digits != 4){
        cout << "fatal error: get_charge only works for n_digits = 2,3,4" << endl;
    }
    int n_one_thirds = 0;
    int n_two_thirds = 0;
    int quarks[3];
    if (n_digits == 3) {
        quarks[0] = -1;
    } else {
        quarks[0] = pdg_id / 1000; // thousands place
    }
    quarks[1] = pdg_id / 100 - ( pdg_id / 1000 ) * 10; // hundred's place
    quarks[2] = pdg_id / 10  - ( pdg_id / 100 )  * 10; // ten's place

    /* cout << "  pdg_id " << pdg_id << " "; */
    for (int i = 0; i < 3; ++i){
        if (i == 0) if (quarks[0] == -1) continue;
        if (quarks[i] > 6){ cout << "fatal error: quark \"" << quarks[i]<<"\" in "<<pdg_id<<" not found."<<endl; }
        /* cout << " |"<<quarks[i] << " "; */
        if (quarks[i] % 2 == 0) {
            /* cout << " (2/3)"; */
            ++n_two_thirds;
        } else {
            /* cout << " (1/3)"; */
            ++n_one_thirds;
        }
    } 
    /* cout << endl; */
    if (n_digits == 3){
        if (n_two_thirds == 0 || n_one_thirds == 0) { return false; }
        else { return true; }
    } else { // n_digits == 4 and there are three quarks
        if (n_one_thirds == 2 && n_two_thirds == 1) return false;
        else { return true; }
    }
        /* if (n_one_thirds == 3) return true; */ 
        /* if (n_one_thirds == 1 && n_two_thirds == 2) return true; */
        /* if (n_two_thirds == 3) return true; */
}

/* inline float pseudorapidity(float px, float py, float pz){ */
/*     /1* return 3; *1/ */
/*     TVector3 v(px,py,pz); */
/*     return v.PseudoRapidity(); */
/* } */

/* inline bool in_EastZDC(float rap){ return rap <= -6.58; } */

void genevents::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L genevents.C
//      Root > genevents t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;
   /* cout << "a1" << endl; */

   Long64_t nentries = fChain->GetEntriesFast();
   // only look at fist 13008 events (errors at 13008)
   /* if (nentries > 13008) nentries = 13008; */

   Long64_t nbytes = 0, nb = 0;


    TFile fout(TString::Format("hgram.%i.root",n_hijing_file).Data(), "recreate");
    /* TTree tree("tree","hijing processed data"); */
    int tpc_nbin = 100;
    float val_e_zdc = 800;
    float val_e_bbc = 1200;
    float i_n_bbc = 100;
    float i_n_zdc = 20;
    TH1F* pt_max_tpc = new TH1F("pt_max_tpc",";max p_{T,charged track}^{TPC};",   100,0,20);
    TH1F* pt_sum_tpc = new TH1F("pt_sum_tpc",";#Sigma p_{T,charged track}^{TPC};",300,0,100);
    TH1F* e_bbc      = new TH1F("e_bbc",";     #Sigma E_{charged tracks}^{East BBC};",100,0,val_e_bbc);
    TH1F* e_zdc      = new TH1F("e_zdc",";     #Sigma E_{neutral tracks}^{East ZDC};",100,0,val_e_zdc);
    TH1F* n_bbc      = new TH1F("n_bbc",";N_{charged tracks}^{East BBC};",(int)i_n_bbc,0,i_n_bbc);
    TH1F* n_zdc      = new TH1F("n_zdc",";N_{neutral tracks}^{East ZDC};",(int)i_n_zdc,0,i_n_zdc);
    TH1F* pt_tpc     = new TH1F("pt_tpc",";p_{T,charged tracks}^{TPC};",100,0,20);
    TH1F* pt_all     = new TH1F("pt_all",";p_{T} all tracks in events",100,0,20);
    TH1F* n_tpc      = new TH1F("n_tpc",";N_{charged tracks}^{TPC}", tpc_nbin,0, tpc_nbin);
    TH1F* n_all    = new TH1F("n_all",";N_{all tracks}^{Event}",   600,0,600);
    TH1F* n_rap    = new TH1F("n_rap",";#eta;N_{all charged tracks};", 500,-10,10);
    TH1F* n_y      = new TH1F("n_y",";y;N_{all charged tracks};", 500,-10,10);

    TH2F* ptMax_eZdc = new TH2F("ptMax_eZdc",";max p_{T,charged tracks}^{TPC};#Sigma E_{neutral tracks}^{East ZDC}",
            100,0,20, 100, 0, val_e_zdc);
    TH2F* ptMax_eBbc = new TH2F("ptMax_eBbc",";max p_{T,charged tracks}^{TPC};#Sigma E_{charged tracks}^{East BBC}",
            100,0,20, 100, 0, val_e_bbc);
    TH2F* ptMax_nZdc = new TH2F("ptMax_nZdc",";max p_{T,charged tracks}^{TPC}; N_{neutral tracks}^{East ZDC}",
            100,0,20, (int) i_n_zdc, 0, i_n_zdc);
    TH2F* ptMax_nBbc = new TH2F("ptMax_nBbc",";max p_{T,charged tracks}^{TPC}; N_{charged tracks}^{East BBC}",
            100,0,20, (int) i_n_bbc, 0, i_n_bbc);
    TH2F* nTpc_eZdc = new TH2F("nTpc_eZdc",";N_{charged tracks}^{TPC};#Sigma E_{neutral tracks}^{East ZDC}",
            tpc_nbin,0,(float)tpc_nbin, 100, 0, val_e_zdc);
    TH2F* nTpc_eBbc = new TH2F("nTpc_eBbc",";N_{charged tracks}^{TPC};#Sigma E_{charged tracks}^{East BBC}",
             tpc_nbin,0,(float)tpc_nbin, 100, 0, val_e_bbc);
    TH2F* nTpc_nZdc = new TH2F("nTpc_nZdc",";N_{charged tracks}^{TPC}; N_{neutral tracks}^{East ZDC}",
             tpc_nbin,0,(float)tpc_nbin, (int) i_n_zdc, 0, i_n_zdc);
    TH2F* nTpc_nBbc = new TH2F("nTpc_nBbc",";N_{charged tracks}^{TPC}; N_{charged tracks}^{East BBC}",
             tpc_nbin,0,(float)tpc_nbin, (int) i_n_bbc, 0, i_n_bbc);


    cout << "Processing " << nentries << " entries" << endl;
    /* for (Long64_t jentry=0; jentry<nentries;jentry++) { */
    /* aloop */
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
   /* cout << "a2" << endl; */
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        if (jentry % 1000 == 0) { cout << "entry " << jentry << endl; }
        /* cout << "entry " << jentry << endl; */
   /* cout << "a3" << endl; */
        float ptMaxTpc = 0;
        float ptSumTpc = 0;
        int   nTpc = 0;

        float eBbc = 0;
        int   nBbc = 0;
        float eZdc = 0;
        int   nZdc = 0;

        int n_total = 0;

        for (int i = 0; i < mParticles_; ++i){
   /* cout << "a4" << endl; */
            /* cout << " particle " << i << endl; */
            if ( mParticles_mStatus[i] != 1 ) continue;
            int abs_id = TMath::Abs(mParticles_mId[i]);
            int n_digits = get_n_digits(abs_id);
            if (n_digits == -1) {
                cout << "error: couldn't read pdg number: " << mParticles_mId[i]
                     <<" of("<<i<<") "<<mParticles_mStatus[i]<<endl;
                continue;
            }
            bool is_charged = get_charge(abs_id, n_digits);
            float pt = TMath::Sqrt(mParticles_mPx[i]*mParticles_mPx[i]
                                  +mParticles_mPy[i]*mParticles_mPy[i]);
            float ptot = TMath::Sqrt(pt*pt+mParticles_mPz[i]*mParticles_mPz[i]);
            float rap = 0.5*TMath::Log(
                    (ptot+mParticles_mPz[i])/
                    (ptot-mParticles_mPz[i])
            );
            float y = 0.5*TMath::Log(
                   ( mParticles_mEnergy[i]+mParticles_mPz[i]) /
                   ( mParticles_mEnergy[i]-mParticles_mPz[i])
           );
            if (is_charged) {
                n_rap->Fill(rap);
                n_y->Fill(y);
            }
            
            pt_all->Fill(pt);
            ++n_total;

            if (rap >= -0.9 && rap <= 0.9  && is_charged){ 
                ++nTpc;
                ptSumTpc += pt;
                pt_tpc->Fill(pt);
                if (pt > ptMaxTpc) (ptMaxTpc = pt);
            } else if (rap <= -6.58 && !is_charged) { 
                eZdc += mParticles_mEnergy[i];
                ++nZdc;
            } else if (-5.8 <= rap && rap <= -3.3 && is_charged){
                eBbc += mParticles_mEnergy[i];
                ++nBbc;
            }
        }
        pt_max_tpc->Fill(ptMaxTpc);
        pt_sum_tpc->Fill(ptSumTpc);
        n_tpc->Fill(nTpc);
        n_all->Fill(n_total);

        e_bbc->Fill(eBbc);
        n_bbc->Fill(nBbc);

        e_zdc->Fill(eZdc);
        n_zdc->Fill(nZdc);

        ptMax_eZdc->Fill(ptMaxTpc, eZdc);
        ptMax_nZdc->Fill(ptMaxTpc, nZdc);
        ptMax_eBbc->Fill(ptMaxTpc, eBbc);
        ptMax_nBbc->Fill(ptMaxTpc, nBbc);

        nTpc_eZdc->Fill(nTpc, eZdc);
        nTpc_nZdc->Fill(nTpc, nZdc);
        nTpc_eBbc->Fill(nTpc, eBbc);
        nTpc_nBbc->Fill(nTpc, nBbc);

   }
   fout.Write();
   fout.Close();
   /* cout << "a0" << endl; */
}
