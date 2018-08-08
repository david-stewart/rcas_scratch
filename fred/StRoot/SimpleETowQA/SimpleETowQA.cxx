//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "SimpleETowQA.h"
#include <iostream>
#define zmin -30
#define zmax 30

class cMax {
    private:
        bool  first;
        float fval;
    public:
        cMax() : first(true), fval(0) {};
        void operator()(float in_val) {
            if (first) {
                fval = in_val;
                first = false;
            } else if (in_val > fval) {
                fval = in_val;
            }
        };
        bool  has_val() { return !first; }
        float val() { return fval; }
};

ClassImp(SimpleETowQA)

//-----------------------------------------------------------------------------
SimpleETowQA::SimpleETowQA(
        const char* name, StPicoDstMaker *picoMaker,
        /* const char* prof,*/ 
        const char* outName, 
        int triggerid,
        const char* i_bad_tower_file,
        const char* i_bad_run_file)
  : StMaker(name), 
    mPicoDstMaker(picoMaker),
    mtriggerid(triggerid), 
    mOutName(outName),
    fbad_tower_file(i_bad_tower_file),
    fbad_run_file(i_bad_run_file),

    fnbins_EmcTriggerId(4800),
    fbinlo_EmcTriggerId(0.5),
    fbinhi_EmcTriggerId(4800.5),
     
    fnbins_EmcTrigger_adcSpectra(54),
    fbinlo_EmcTrigger_adcSpectra(10.5),
    fbinhi_EmcTrigger_adcSpectra(64.5),

    fnbinsY_TriggerBTow_adc(500),
    fbinloY_TriggerBTow_adc(-0.5),
    fbinhiY_TriggerBTow_adc(999.5),

    fnbinsY_BTowHit_EtSpectra(500),
    
    fnbins_BTowHitId(4800),
    fbinlo_BTowHitId(-0.5),
    fbinhi_BTowHitId(4799.5),

    fnbins_BTowHit_eSpectra(1000),
    fbinlo_BTowHit_eSpectra(-5),
    fbinhi_BTowHit_eSpectra(35),

    fnbins_BTowHit_EtSpectra(1000),
    fbinlo_BTowHit_EtSpectra(-5),
    fbinhi_BTowHit_EtSpectra(35),

    fnbins_BTowHit_adcSpectra(2000),
    fbinlo_BTowHit_adcSpectra(-10.5),
    fbinhi_BTowHit_adcSpectra(1989.5),

    fnbins_BTowHit_EtMax(1000),
    fbinlo_BTowHit_EtMax(-5.0),
    fbinhi_BTowHit_EtMax(50.),

    fnbins_BTowHit_eMax(1000),
    fbinlo_BTowHit_eMax(-5.0),
    fbinhi_BTowHit_eMax(50.),

    fnbins_BTowHit_adcMax(2000),
    fbinlo_BTowHit_adcMax(-0.5),
    fbinhi_BTowHit_adcMax(1999.5)

{ };

//----------------------------------------------------------------------------- 
SimpleETowQA::~SimpleETowQA()
{ /*  */ }

//----------------------------------------------------------------------------- 
Int_t SimpleETowQA::Init() {
    fout = new TFile(TString::Format("%s.root",mOutName.Data()).Data(),"RECREATE") ;
    flog = fopen(TString::Format("%s.log",mOutName.Data()).Data(),"w");
    fprintf(flog,"Initializing log\n");

    ifstream file;
    file.open(fbad_tower_file.Data());
    if (!file.is_open()) {
        fprintf(flog,"Warning: the bad tower file %s could not be opened.\n"
                     "  No bad tower list obtained.\n",
                fbad_tower_file.Data());
        fbad_tower_list = vector<int>{};
    } else {
        int bad_tower;
        while (file >> bad_tower) fbad_tower_list.push_back(bad_tower);
        std::sort(fbad_tower_list.begin(), fbad_tower_list.end());
        fprintf(flog,"Read in bad tower list from %s\n",fbad_tower_file.Data());
    }
    file.close();

    file.open(fbad_run_file.Data());
    if (!file.is_open()) {
        fprintf(flog,"Warning: the bad run file %s could not be opened.\n"
                     "  No bad run list obtained.\n",
                fbad_run_file.Data());
        fbad_run_list = vector<int>{};
    } else {
        int bad_run;
        while (file >> bad_run) fbad_run_list.push_back(bad_run);
        std::sort(fbad_run_list.begin(), fbad_run_list.end());
        fprintf(flog,"Read in bad run list from %s\n",fbad_run_file.Data());
    }
    file.close();
      
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

    fHgram_EmcTrighits_phieta = new TH2F("EmcTrigHits_phieta", 
    "NHits per tower;#eta;#phi", 40, eta_edges, 120, phi_edges);

    fHgram_badT_EmcTrighits_phieta = new TH2F("badT_EmcTrigHits_phieta", 
    "Bad Towers, NHits per trigger tower;#eta;#phi", 
    40, eta_edges, 120, phi_edges);

    fHgram_ev_cuts = new TH1F("ev_cuts",
            ";0:db 1:!badrun 2:trig 3:rnk 4:vz10 5:(vz-vpd)<=3.0",
            6, -0.5, 5.5);

    fHgram_vz = new TH1F("vz","primary vz distribution",200,-100,100);

    fHgram_ln_ranking = new TH1F("ln_ranking",
            "log of best vertex ranking - perseving the negative sign",
            400,-20,20);

    fHgram_EmcTriggerId_nHT1 = new TH1F("TriggerId_nHT1","HT1;id of trigger;N_{hits}^{HT1}",
            fnbins_EmcTriggerId, fbinlo_EmcTriggerId, fbinhi_EmcTriggerId);

    fHgram_EmcTriggerId_nHT2 = new TH1F("TriggerId_nHT2","HT2;id of trigger;N_{hits}^{HT2}",
            fnbins_EmcTriggerId, fbinlo_EmcTriggerId, fbinhi_EmcTriggerId);

 
    fProfE_EmcTriggerId_adcHT1 = new TProfile("TriggerId_adcHT1","HT1;id of trigger;adc",
            fnbins_EmcTriggerId, fbinlo_EmcTriggerId, fbinhi_EmcTriggerId);
    fProfE_badT_EmcTriggerId_adcHT1 = new TProfile("badT_TriggerId_adcHT1","badT_HT1;id of trigger;adc",
            fnbins_EmcTriggerId, fbinlo_EmcTriggerId, fbinhi_EmcTriggerId);

    fProfE_EmcTriggerId_adcHT2 = new TProfile("TriggerId_adcHT2","HT2;id of trigger;adc",
            fnbins_EmcTriggerId, fbinlo_EmcTriggerId, fbinhi_EmcTriggerId);


    fHgram_EmcTrigger_adcSpectra_HT1 = new TH1F("Trigger_adcSpectra_HT1", "HT1;adc;N",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra);
    fHgram_badT_EmcTrigger_adcSpectra_HT1 = new TH1F("badT_Trigger_adcSpectra_HT1", "badT_HT1;adc;N",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra);

    fHgram_EmcTrigger_adcSpectra_HT2 = new TH1F("Trigger_adcSpectra_HT2", "HT2;adc;N",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra);


    fHgram_EmcTrigBTow_AdcAdc_all = new TH2F("EmcTrigBTow_AdcAdc_all",
        "HT1 triggered towers;ADC_{EmcTrigger};ADC_{BTowHit}",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra,
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc);
    fHgram_badT_EmcTrigBTow_AdcAdc = new TH2F("badT_EmcTrigBTow_AdcAdc",
        "badT_HT1 triggered towers;ADC_{EmcTrigger};ADC_{BTowHit}",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra,
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc);
    fHgram_EmcTrigBTow_AdcAdc_482 = new TH2F("EmcTrigBTow_AdcAdc_482",
        "HT1 triggered BTowHit 482 (hot tower);ADC_{EmcTrigger};ADC_{BTowHit}",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra,
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc);
    fHgram_EmcTrigBTow_AdcAdc_236 = new TH2F("EmcTrigBTow_AdcAdc_236",
        "HT1 triggered BTowHit 236 (\"cold\" tower);ADC_{EmcTrigger};ADC_{BTowHit}",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra,
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc);
    fHgram_EmcTrigBTow_AdcAdc_799 = new TH2F("EmcTrigBTow_AdcAdc_799",
        "HT1 triggered BTowHit 799 (\"normal\" tower);ADC_{EmcTrigger};ADC_{BTowHit}",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra,
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc);
    fHgram_EmcTrigBTow_AdcAdc_3836 = new TH2F("EmcTrigBTow_AdcAdc_3836",
        "HT1 triggered BTowHit 3836 (\"normal\" tower);ADC_{EmcTrigger};ADC_{BTowHit}",
        fnbins_EmcTrigger_adcSpectra, fbinlo_EmcTrigger_adcSpectra, fbinhi_EmcTrigger_adcSpectra,
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc);

    fHgram_BTowHitId_Et = new TH2F("BTowHitId_Et","Et for each good BTower;EmcTrig Id;E_{transverse}",
        fnbins_EmcTriggerId, fbinlo_EmcTriggerId, fbinhi_EmcTriggerId, 62, -1, 30);
    fHgram_badT_BTowHitId_Et = new TH2F("badT_BTowHitId_Et","Et for each good BTower;EmcTrig Id;E_{transverse}",
        fnbins_EmcTriggerId, fbinlo_EmcTriggerId, fbinhi_EmcTriggerId, 62, -1, 30);


    fHgram_BTowHit_adcEt_all = new TH2F("BTowHit_adcEt_all",
            "HT1 Trigger, all BTows;ADC_{BTowHit};Et_{BTowHit}",
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc,
        fnbinsY_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);
    fHgram_badT_BTowHit_adcEt_all = new TH2F("badT_BTowHit_adcEt_all",
            "badT_HT1 Trigger, all BTows;ADC_{BTowHit};Et_{BTowHit}",
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc,
        fnbinsY_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);
    fHgram_BTowHit_adcEt_482 = new TH2F("BTowHit_adcEt_482",
            "HT1 Trigger, BTower 482 (high mean Et);ADC_{BTowHit};Et_{BTowHit}",
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc,
        fnbinsY_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);
    fHgram_BTowHit_adcEt_236 = new TH2F("BTowHit_adcEt_236",
            "HT1 Trigger, BTower 236 (low mean Et);ADC_{BTowHit};Et_{BTowHit}",
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc,
        fnbinsY_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);
    fHgram_BTowHit_adcEt_799 = new TH2F("BTowHit_adcEt_799",
            "HT1 Trigger, BTower 799 (~average mean Et);ADC_{BTowHit};Et_{BTowHit}",
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc,
        fnbinsY_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);
    fHgram_BTowHit_adcEt_3836 = new TH2F("BTowHit_adcEt_3836",
            "HT1 Trigger, BTower 3836 (~average mean Et);ADC_{BTowHit};Et_{BTowHit}",
        fnbinsY_TriggerBTow_adc, fbinloY_TriggerBTow_adc, fbinhiY_TriggerBTow_adc,
        fnbinsY_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);


    fProfE_BTowHitId_eHT1 = new TProfile("BTowHitId_eHT1", "HT1;Tower Id;energy",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);
    
    fProfE_BTowHitId_eHT2 = new TProfile("BTowHitId_eHT2", "HT2;Tower Id;energy",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);


    fProfE_BTowHitId_EtHT1 = new TProfile("BTowHitId_EtHT1", "HT1;Tower Id;E_{t}",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);
    fProfE_badT_BTowHitId_EtHT1 = new TProfile("badT_BTowHitId_EtHT1", "badT_HT1;Tower Id;E_{t}",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);
    
    fProfE_BTowHitId_EtHT2 = new TProfile("BTowHitId_EtHT2", "HT2;Tower Id;E_{t}",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);


    fProfE_BTowHitId_adcHT1 = new TProfile("BTowHitId_adcHT1", "HT1;Tower Id;adc",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);
    fProfE_badT_BTowHitId_adcHT1 = new TProfile("badT_BTowHitId_adcHT1", "badT_HT1;Tower Id;adc",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);
    
    fProfE_BTowHitId_adcHT2 = new TProfile("BTowHitId_adcHT2", "HT2;Tower Id;adc",
        fnbins_BTowHitId, fbinlo_BTowHitId, fbinhi_BTowHitId);


    fHgram_BTowHit_eSpectra_HT1 = new TH1F("BTowHit_eSpectraHT1", "HT1;E;N",
        fnbins_BTowHit_eSpectra, fbinlo_BTowHit_eSpectra, fbinhi_BTowHit_eSpectra);

    fHgram_BTowHit_eSpectra_HT2 = new TH1F("BTowHit_eSpectraHT2", "HT2;E;N",
        fnbins_BTowHit_eSpectra, fbinlo_BTowHit_eSpectra, fbinhi_BTowHit_eSpectra);


    fHgram_BTowHit_EtSpectra_HT1 = new TH1F("BTowHit_EtSpectraHT1", "HT1;E_{t};N",
        fnbins_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);
    fHgram_badT_BTowHit_EtSpectra_HT1 = new TH1F("badT_BTowHit_EtSpectraHT1", "badT_HT1;E_{t};N",
        fnbins_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);

    fHgram_BTowHit_EtSpectra_HT2 = new TH1F("BTowHit_EtSpectraHT2", "HT2;E_{t};N",
        fnbins_BTowHit_EtSpectra, fbinlo_BTowHit_EtSpectra, fbinhi_BTowHit_EtSpectra);


    fHgram_BTowHit_adcSpectra_HT1 = new TH1F("BTowHit_adcSpectraHT1", "HT1;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, fbinhi_BTowHit_adcSpectra);
    fHgram_badT_BTowHit_adcSpectra_HT1 = new TH1F("badT_BTowHit_adcSpectraHT1", "badT_HT1;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, fbinhi_BTowHit_adcSpectra);

    fHgram_BTowHit_adcSpectra_HT2 = new TH1F("BTowHit_adcSpectraHT2", "HT2;adc;N",
        fnbins_BTowHit_adcSpectra, fbinlo_BTowHit_adcSpectra, fbinhi_BTowHit_adcSpectra);


    fHgram_BTowHit_EtMaxHT1 = new TH1F("BTowHit_EtMaxHT1", "HT1;E_{t}^{max};N",
        fnbins_BTowHit_EtMax, fbinlo_BTowHit_EtMax, fbinhi_BTowHit_EtMax);
    fHgram_badT_BTowHit_EtMaxHT1 = new TH1F("badT_BTowHit_EtMaxHT1", "badT_HT1;E_{t}^{max};N",
        fnbins_BTowHit_EtMax, fbinlo_BTowHit_EtMax, fbinhi_BTowHit_EtMax);

    fHgram_BTowHit_EtMaxHT2 = new TH1F("BTowHit_EtMaxHT2", "HT2;E_{t}^{max};N",
        fnbins_BTowHit_EtMax, fbinlo_BTowHit_EtMax, fbinhi_BTowHit_EtMax);


    fHgram_BTowHit_eMaxHT1 = new TH1F("BTowHit_eMaxHT1", "HT1;E^{max};N",
        fnbins_BTowHit_eMax, fbinlo_BTowHit_eMax, fbinhi_BTowHit_eMax);

    fHgram_BTowHit_eMaxHT2 = new TH1F("BTowHit_eMaxHT2", "HT2;E^{max};N",
        fnbins_BTowHit_eMax, fbinlo_BTowHit_eMax, fbinhi_BTowHit_eMax);


    fHgram_BTowHit_adcMaxHT1 = new TH1F("BTowHit_adcMaxHT1", "HT1;adc^{max};N",
        fnbins_BTowHit_adcMax, fbinlo_BTowHit_adcMax, fbinhi_BTowHit_adcMax);
    fHgram_badT_BTowHit_adcMaxHT1 = new TH1F("badT_BTowHit_adcMaxHT1", "badT_HT1;adc^{max};N",
        fnbins_BTowHit_adcMax, fbinlo_BTowHit_adcMax, fbinhi_BTowHit_adcMax);

    fHgram_BTowHit_adcMaxHT2 = new TH1F("BTowHit_adcMaxHT2", "HT2;adc^{max};N",
        fnbins_BTowHit_adcMax, fbinlo_BTowHit_adcMax, fbinhi_BTowHit_adcMax);


    fHgram_goodbadtower = new TH1F("goodbadtower","nhits good and bad towers;0(bad) 1(good);N",
            2, -0.5, 1.5);


  return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t SimpleETowQA::Finish() {
    fout->Write();
    fout->Close();
    fprintf(flog,"Closing log at end of file.\n");
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
    fHgram_ev_cuts->Fill(0);
    StPicoEvent* mevent = mPicoDst->event();


    if (std::binary_search(fbad_run_list.begin(), fbad_run_list.end(), mevent->runId())) 
        return kStOk; 
    fHgram_ev_cuts->Fill(1);

    if (mtriggerid != 0) {
        if (!mevent->isTrigger(mtriggerid)) return kStOk;
    }
    fHgram_ev_cuts->Fill(2);

    float rank = mevent->ranking();
    int sig = rank > 0 ? +1 : -1;
    float ln = TMath::Log(TMath::Abs(rank));
    fHgram_ln_ranking->Fill(sig*ln);

    if (sig < 0) return kStOK;
    fHgram_ev_cuts->Fill(3);

    /* float zdcX = mevent->ZDCx(); */
    /* if (zdcX < 4000. || zdcX > 22700.) { return kStOk; } */
    /* fHgram_ev_cuts->Fill(4); */

    /* float sigZDC = 0; */
    /* for (int i = 0; i < 24; ++i){ */
    /*     sigZDC += */ 
    float z_vert = mevent->primaryVertex().z();
    fHgram_vz->Fill(mevent->primaryVertex().z());

    if (TMath::Abs(z_vert) > 10) return kStOk;
    fHgram_ev_cuts->Fill(4);

    if (TMath::Abs(z_vert - mevent->vzVpd())>3.0) return kStOk;
    fHgram_ev_cuts->Fill(5);


    std::vector<int> emcTriggerIds;

    cMax max_HT1_E  ;
    cMax max_HT1_Et ;
    cMax badT_max_HT1_Et  ;
    cMax max_HT1_Adc;
    cMax badT_max_HT1_Adc;

    cMax max_HT2_E  ;
    cMax max_HT2_Et ;
    cMax max_HT2_Adc;

    for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
        StPicoEmcTrigger* emcTrig = mPicoDst->emcTrigger(i);
        bool  HT1 = emcTrig->isHT1();
        bool  HT2 = emcTrig->isHT2();
        if (!HT1) continue; // HT1 always fires for HT2

        int   trigId  = emcTrig->id();
        float trigAdc = emcTrig->adc();

        bool badT = (std::binary_search(fbad_tower_list.begin(), fbad_tower_list.end(), trigId));

        int   towId   = trigId - 1;
        StPicoBTowHit* bTowHit = mPicoDst->btowHit(towId); 
        // get tower location in order to get the transverse energy
        StEmcPosition *mPosition = new StEmcPosition();
        StThreeVectorF towLoc = mPosition->getPosFromVertex(StThreeVectorF(0,0,0), towId);

        // thanks to Joel Mazer
        float towE     = bTowHit->energy();
        float towEt    = towE / ((double)TMath::CosH(towLoc.pseudoRapidity()));
        float towAdc   = bTowHit->adc();
        float phi      = towLoc.phi();
        float eta      = towLoc.pseudoRapidity();

        // tower specific measurements
        if (HT1) {
            if (towId == 482) {
                fHgram_EmcTrigBTow_AdcAdc_482 ->Fill(trigAdc, towAdc);
                fHgram_BTowHit_adcEt_482      ->Fill(towAdc,  towEt);
            } else if (towId == 236) {
                fHgram_EmcTrigBTow_AdcAdc_236 ->Fill(trigAdc, towAdc);
                fHgram_BTowHit_adcEt_236      ->Fill(towAdc,  towEt);
            } else if (towId == 799) {
                fHgram_EmcTrigBTow_AdcAdc_799 ->Fill(trigAdc, towAdc);
                fHgram_BTowHit_adcEt_799      ->Fill(towAdc,  towEt);
            } else if (towId == 3836) {
                fHgram_EmcTrigBTow_AdcAdc_3836 ->Fill(trigAdc, towAdc);
                fHgram_BTowHit_adcEt_3836      ->Fill(towAdc,  towEt);
            } 

        }
        // if bad tower ("badT") take a limited subset of data
        if (badT) {
            fHgram_goodbadtower->Fill(0);
            if (HT1) {
                fHgram_badT_EmcTrighits_phieta->Fill(eta, phi);
                fProfE_badT_EmcTriggerId_adcHT1      ->Fill(trigId, trigAdc);
                fHgram_badT_EmcTrigger_adcSpectra_HT1->Fill(trigAdc);
                fHgram_badT_EmcTrigBTow_AdcAdc       ->Fill(trigAdc, towAdc);
                fHgram_badT_BTowHit_adcEt_all        ->Fill(towAdc,  towEt);
                fProfE_badT_BTowHitId_EtHT1          ->Fill(towId, towEt);
                fProfE_badT_BTowHitId_adcHT1         ->Fill(towId, towAdc);
                fHgram_badT_BTowHit_EtSpectra_HT1    ->Fill(towEt);
                fHgram_badT_BTowHit_adcSpectra_HT1->Fill(towAdc);
                fHgram_badT_BTowHitId_Et->Fill(trigId, towEt);

                badT_max_HT1_Et (towEt);
                badT_max_HT1_Adc(towAdc);
            }
        } else { // not a bad tower
            fHgram_goodbadtower->Fill(1);
            if (HT1){
                fHgram_EmcTrighits_phieta->Fill(eta, phi);
                fHgram_EmcTriggerId_nHT1->Fill(trigId);
                fProfE_EmcTriggerId_adcHT1->Fill(trigId, trigAdc);
                fHgram_EmcTrigger_adcSpectra_HT1->Fill(trigAdc);
                
                fProfE_BTowHitId_eHT1  ->Fill(towId, towE);
                fProfE_BTowHitId_EtHT1 ->Fill(towId, towEt);
                fProfE_BTowHitId_adcHT1->Fill(towId, towAdc);

                fHgram_BTowHitId_Et->Fill(trigId, towEt);

                fHgram_BTowHit_eSpectra_HT1  ->Fill(towE);
                fHgram_BTowHit_EtSpectra_HT1 ->Fill(towEt);
                fHgram_BTowHit_adcSpectra_HT1->Fill(towAdc);
                
                fHgram_EmcTrigBTow_AdcAdc_all ->Fill(trigAdc, towAdc);
                fHgram_BTowHit_adcEt_all      ->Fill(towAdc,  towEt);
                
                max_HT1_E  (towE);
                max_HT1_Et (towEt);
                max_HT1_Adc(towAdc);
            } 
            if (HT2){
                fHgram_EmcTriggerId_nHT2->Fill(trigId);
                fProfE_EmcTriggerId_adcHT2->Fill(trigId, trigAdc);
                fHgram_EmcTrigger_adcSpectra_HT2->Fill(trigAdc);

                fProfE_BTowHitId_eHT2  ->Fill(towId, towE);
                fProfE_BTowHitId_EtHT2 ->Fill(towId, towEt);
                fProfE_BTowHitId_adcHT2->Fill(towId, towAdc);

                fHgram_BTowHit_eSpectra_HT2  ->Fill(towE);
                fHgram_BTowHit_EtSpectra_HT2 ->Fill(towEt);
                fHgram_BTowHit_adcSpectra_HT2->Fill(towAdc);

                max_HT2_E  (towE);
                max_HT2_Et (towEt);
                max_HT2_Adc(towAdc);
            } 
        }
    }
    if (max_HT1_E.has_val())   fHgram_BTowHit_eMaxHT1  ->Fill(max_HT1_E.val());
    if (max_HT1_Et.has_val())  fHgram_BTowHit_EtMaxHT1 ->Fill(max_HT1_Et.val());
    if (max_HT1_Adc.has_val()) fHgram_BTowHit_adcMaxHT1->Fill(max_HT1_Adc.val());

    if (max_HT2_E.has_val())   fHgram_BTowHit_eMaxHT2  ->Fill(max_HT2_E.val());
    if (max_HT2_Et.has_val())  fHgram_BTowHit_EtMaxHT2 ->Fill(max_HT2_Et.val());
    if (max_HT2_Adc.has_val()) fHgram_BTowHit_adcMaxHT2->Fill(max_HT2_Adc.val());

	return kStOK;
}

