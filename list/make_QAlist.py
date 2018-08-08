#!/usr/bin/python

# edit the st_physics and st_ssdmb lists to remove the lists with teh bad runs from them

# collect the bad runs
bad_runs = []
for x in open("bad_runs","r").readlines():
    for A in x.split(','):
        if not A.strip(): 
            continue
        bad_runs.append(int(A.strip()))
bad_runs.sort()

# filter the files
fout = open("limQA_st_physics.list","w")
for x in open("lim_st_physics.list","r").readlines():
    run = int(x.split("/st_physics")[0].split('/')[-1])
    if run in bad_runs:
        continue
    fout.write(x)
fout.close()

fout = open("limQA_st_ssdmb.list","w")
for x in open("lim_st_ssdmb.list","r").readlines():
    run = int(x.split("/st_ssdmb")[0].split('/')[-1])
    if run in bad_runs:
        continue
    fout.write(x)
fout.close()
