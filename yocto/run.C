#include <TSystem>
/* #include "yocto.h" */
void run(Int_t nEvents, 
        Int_t nFiles, 
        TString InputFileList, 
        TString OutputDir, 
        TString JobIdName,
        int TriggerId, 
        float TT_min, 
        int allTOF = 0, 
        int testCond = 0) 
//void SimpleAnalysis() 
{
//	int nEvents = 0;
//	int nFiles = 1;
//	TString InputFileList = "st_physics_sample_data.MuDst.root";
//	TString JobIdName = "01";
//
//	TString OutputDir = ""; 
  // Load libraries

  gROOT   -> Macro("loadMuDst.C");
  gSystem -> Load("$FASTJET/lib/libfastjet");
  gSystem -> Load("$FASTJET/lib/libfastjettools");
  gSystem -> Load("YoDst.so") ;
  cout << "Hello World!" << endl ;

  // List of member links in the chain
  StChain*               chain  =  new StChain ;
  StMuDstMaker*          muDstMaker  =  new StMuDstMaker(0,0,"",InputFileList,"MuDst",nFiles) ;
  YoDstMaker* YoDstMaker  =  new YoDstMaker(muDstMaker, JobIdName.Data(), TriggerId, TT_min) ;

  if (testCond != 0){ YoDstMaker->setTestConditions(); }
  if (allTOF != 0){   YoDstMaker->setAllTOF(); }

  muDstMaker -> SetStatus("*",1) ;                // Turn off all branches
  /* muDstMaker -> SetStatus("EztHead", 0); */
  /* muDstMaker -> SetStatus("EztTrig", 0); */
  /* muDstMaker -> SetStatus("EztETow", 0); */
  /* muDstMaker -> SetStatus("EztESmd", 0); */
  /* muDstMaker -> SetStatus("EztFpd", 0); */
  /* muDstMaker -> SetStatus("EmcAll", 0); */
  /* muDstMaker -> SetStatus("PmdAll", 0); */
  /* muDstMaker -> SetStatus("TofAll", 0); */
  /* muDstMaker -> SetStatus("MTDAll", 0); */
  /* muDstMaker -> SetStatus("FgtAll", 0); */
  /* muDstMaker -> SetStatus("EztAll", 0); */
  muDstMaker -> SetStatus("MuEvent",1) ;    // Turn on the primary track data
  muDstMaker -> SetStatus("StEventInfo",1) ;    // Turn on the primary track data
  /* muDstMaker -> SetStatus("EpdHit", 1); */
  /* muDstMaker -> SetStatus("TofHit",0) ;    // Turn on the primary track data */
  muDstMaker -> SetStatus("BTofHit",1) ;    // Turn on the primary track data
  muDstMaker -> SetStatus("BTofHeader",1) ;    // Turn on the primary track data
  muDstMaker -> SetStatus("PrimaryTracks",1) ;    // Turn on the primary track data
  muDstMaker -> SetStatus("PrimaryVertices",1) ;    // Turn on the primary track data
  /* muDstMaker -> SetStatus("*",1) ;    // Turn on the primary track data */
  if ( nEvents == 0 )  nEvents = 100000000 ;       // Take all events in nFiles if nEvents = 0

  // Loop over the links in the chain
  chain -> Init() ;
  chain -> EventLoop(1,nEvents) ;
  chain -> Finish() ;
  cout << " nEvents " << chain->GetNTotal() << endl;

  // Cleanup
  delete chain ;
}

