Files:
# run hijing
    * `hijing.C`: taken from `/star/u/akio/fms2015/hijing/` and modified slightly
    * `xml`: in theory to submit the jobs, but has a weird seg-fault when running on condor, but
    not locally.
# Files to clip hijing to smaller trees
 `run_clip_hijing.cc(input_file [, output_file=clipped_hijing.root])`: runs the `clip_hijing.C` macro
 `clip_hijing.C`: created a short tree from the hijing tree, from which
    final histograms can be made. This dramatically cuts down on the hijing tree size.
    Branches:
        note: "barrel" means -0.9<η<0.9
        px_max_barrel: highest charged track pT in the barrel
        pt_sum_barrel: sum of all charged pT in the barrel
        e_zdc: energy of all neutral tracks in east zdc (η<-6.58)
        n_barrel: number of charged tracks in the barrel
        n_zdc: number of neutral tracks in the East zdc
        pt_all: pt of all tracks (neutral and charged)
        pt_ch_barrel: pt of all charged tracks in the barrel
        n_bbc: number of charged tracks in bbc (-5.8 < η < -3.3)
        e_bbc: energy of charged tracks in bbc

    
