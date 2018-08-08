#!/usr/bin/python

runs = []

for x in open("limQA_st_ssdmb.list"):
    A = x.split('/')[-2]
    if A not in runs:
        runs.append(A)

runs.sort()

day = []
val = []

for x in runs:
    n = int(x)
    d = (n%1000000)/1000
    if d not in day:
        day.append(d)
    r = (n%1000)
    if r not in val:
        val.append(r)

day.sort()
val.sort()

fout = open('max_day_runs','w')
fout.write("days: %i-%i\n"%(min(day),max(day)))
fout.write("runs: %i-%i\n"%(min(val),max(val)))
fout.write("\ndays:\n")
for x in day:
    fout.write("  %i\n"%x)
fout.write("\nruns:\n")
for x in val:
    fout.write("  %i\n"%x)
