infile=root://xrdstar.rcf.bnl.gov:1095//home/starlib/home/starreco/reco/production_pAu200_2015/ReversedFullField/P17id/2015/128/16128022/st_ssdmb_16128022_raw_1000025.MuDst.root
root4star -q -b run.C\(100,1,\"$infile\",\"./out\",\"00\",500001,4,1,1\) 
