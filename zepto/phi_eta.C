#include <TSystem>
class StMaker;
class StChain;
class StPicoDstMaker;

void phi_eta()
{
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    loadSharedLibraries();

    gSystem->Load("StPicoEvent");
    gSystem->Load("StPicoDstMaker");
    gSystem->Load("ZeptoGrams");

    double pi = 1.0*TMath::Pi();
    StThreeVectorF mVertex(0,0,0);

    FILE *text = fopen("phi_eta.txt","w");
    TFile f("phi_eta.root","recreate");

    TProfile* h_phi = new TProfile("phi",";i_tower;#phi",4801, -0.5, 4800.5);
    TProfile* h_eta = new TProfile("eta",";i_tower;#eta",4801, -0.5, 4800.5);
    TProfile2D* p_phieta = new TProfile2D("p_phi_eta",
            "location of towers;#phi;#eta", 140 ,-pi-0.1., pi+.1,40,-1.1, 1.1);
 
    float phi_last = 0;
    float eta_last = 0;

    fprintf(text, " %2s:  %6s   %6s  %6s  %6s\n", "i", "phi", "eta", "d-Phi", "d-eta");
    for (int towerID = 0; towerID<4801; ++towerID){
        StEmcPosition *mPosition = new StEmcPosition();
        StThreeVectorF towerPosition = mPosition->getPosFromVertex(mVertex, towerID);
        double phi = towerPosition.phi();
        if(phi < -pi) phi += 2.0*pi;
        if(phi >  pi) phi -= 2.0*pi;
        double eta = towerPosition.pseudoRapidity();
        fprintf(text," %2i:  %6.4f   %6.4f\n", towerID, phi, eta);
        h_phi->Fill(towerID,phi);
        h_eta->Fill(towerID,eta);
        p_phieta->Fill(phi,eta,towerID);
    }
    TCanvas *c = new TCanvas("c");
    p_phieta->Draw("colz");
    TCanvas *c1 = new TCanvas("c1");
    c1->WaitPrimitive();
    fclose(text);
    f.Write();
    f.Close();
}
