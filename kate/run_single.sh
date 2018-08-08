starver SL18b
# infile=/star/data20/reco/production_pAu200_2015/ReversedFullField/P16id.SL18b/2015/140/16140009/st_physics_16140009_raw_1000022.picoDst.root
infile=/star/data25/reco/production_pAu200_2015/ReversedFullField/P17id.SL18b/2015/128/16128022/st_ssdmb_16128022_raw_1000025.picoDst.root
badtfile=/direct/star+scratch/djs/bad_towers/BadTowers_Comb.08.02.list
badrfile=/direct/star+scratch/djs/bad_towers/bad_run_list.list
root4star -q -b ./jetTrees.C\(-1,0,\"$infile\",\"test\",\"$badtfile\",\"$badrfile\",1,1\)
