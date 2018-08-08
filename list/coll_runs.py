#!/usr/bin/python

runs_mb = []

for x in open("limQA_st_ssdmb.list"):
    A = x.split('/')[-2]
    if A not in runs_mb:
        runs_mb.append(A)

runs_mb.sort()

for x in runs_mb:
    print x

runs_h1 = []
for x in open("limQA_st_physics.list"):
    A = x.split('/')[-2]
    if A not in runs_h1:
        runs_h1.append(A)

runs_h1.sort()

for x in runs_h1:
    print x

mb = []
h1 = []
both = []
fout = open("runs","w")
for x in runs_mb:
    if x in runs_h1:
        both.append(x)
    else:
        mb.append(x)
for x in runs_h1:
    if x not in runs_mb:
        h1.append

fout.write("%i in MB,  %i in HT1,  %i in both\n"%(len(mb),len(h1),len(both)))
for (x,name) in ((mb,"min bias"),(h1,"HT1"),(both,'both')):
    fout.write("Runs in %s\n"%name)
    for y in x:
        fout.write("%s\n"%y)
