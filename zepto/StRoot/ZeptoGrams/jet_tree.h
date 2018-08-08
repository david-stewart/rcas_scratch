#ifndef JET_TREE_HH
#define JET_TREE_HH
#include <iostream>
#include <vector>
using namespace std;

/** @file    tree
    @author  David Stewart
    @version draft 0.2
    @brief   simple tree into which to sort physics runs
    @details put in hadron-triggered jet events
    @date    2018.03.15
*/

using namespace std;

struct ZeTrack : public TObject {
	virtual ~ZeTrack() { };
    double t_phi;
    double t_eta;
    double t_pt;
    /* bool  isCentral; // is central */
    /* void check_isCentral() { isCentral = (eta <= 0.5); } */
	ClassDef(ZeTrack,1);
};

class ZeJet : public TObject {
	public:
	virtual ~ZeJet() {};
    double j_phi;//
    double j_eta;//
    double j_pt;//
    double j_pt_corr;
    double j_area;//
    unsigned int j_track0;
    unsigned int j_ntracks;
    unsigned int j012; // which zone it is in, -8 to +8
	ClassDef(ZeJet,1);
};

struct ZeEvent : public TObject {
	virtual ~ZeEvent() {} ;
    unsigned int njets; //
    unsigned int e_ntracks; //
    unsigned int e_ntracks_noTOF;
    unsigned int edge_track0; // index of first edge track
    unsigned int n_edge_tracks; // how many edge tracks
    double zdc; //
    int vpdHitsE; //
    int vpdHitsW; //
    unsigned int runId; //
    unsigned int eventNumber; //
    double bbcAdc[48]; //
    double bbcES;
    double bbcEL;
    double bbcWS;
    double bbcWL;
    double zdcE; // zdc.adcSum(east)
    double TTphi; //
    double TTeta; //
    double TTpt; //
    bool has4to9;
    bool has4and9;
    bool has9;
    double rho;
    double rand;
    double refMult;
    /* Event() {} ; */
     
    float get_bbcES() { 
        float rval = 0; 
        for (int i=0; i<16; ++i) rval+=bbcAdc[i];
        return rval; 
    };
    float get_bbcEL() { 
        float rval = 0; 
        for (int i=16; i<24; ++i) rval+=bbcAdc[i];
        return rval; 
    };
    float get_bbcWS() { 
        float rval = 0; 
        for (int i=0; i<16; ++i) rval+=bbcAdc[i+24];
        return rval; 
    }; 
    float get_bbcWL() { 
        float rval = 0; 
        for (int i=16; i<24; ++i) rval+=bbcAdc[i+24];
        return rval; 
    };
    ClassDef(ZeEvent,1);
};
#endif
