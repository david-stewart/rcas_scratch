#!/usr/bin/python
phi = []
eta = []

for x in open("tow_phi_eta_map","r").readlines():
    A = x.split()
    phi.append(float(A[1]))
    eta.append(float(A[2]))

phi_min = min(phi)
phi_max = max(phi)

eta_min = min(eta)
eta_max = max(eta)

phi_list = dict()
for x in phi:
    if x in phi_list.keys():
        phi_list[x] += 1
    else:
        phi_list[x] = 1
phi = phi_list.keys()
phi.sort()

eta_list = dict()
for x in eta:
    if x in eta_list.keys():
        eta_list[x] += 1
    else:
        eta_list[x] = 1
eta = eta_list.keys()
eta.sort()

fout = open("phi_eta_bins","w")
fout.write("This is to make a TH2D histogram for the EmcTowers\n"
        "Binning: (%i,%i) in (phi,eta)\n"%(len(phi),len(eta)))

val = eta
e = [] # bin edges
e.append( val[0] - 0.5*(val[1]-val[0]) )
for i in range(len(val)-1):
    e.append(0.5*(val[i]+val[i+1]))
e.append(val[-1]+0.5*(val[-1]-val[-2]))

fout.write("// eta binning edges:\n double eta_edges[%i] = {"%(len(e)))
for i in range(len(e)-1):
    if i%7 == 0:
        fout.write("\n")
    fout.write(" %8.5f,"%e[i])
fout.write(" %8.5f};\n\n"%e[-1])

val = phi
e = [] # bin edges
e.append( val[0] - 0.5*(val[1]-val[0]) )
for i in range(len(val)-1):
    e.append(0.5*(val[i]+val[i+1]))
e.append(val[-1]+0.5*(val[-1]-val[-2]))

fout.write("// phi binning edges:\n double phi_edges[%i] = {"%(len(e)))
for i in range(len(e)-1):
    if i%7 == 0:
        fout.write("\n")
    fout.write(" %8.5f,"%e[i])
fout.write(" %8.5f};\n\n"%e[-1])

fout.write("""
    TH2F* fHgram_BTowHit_phieta = new TH2F("BTowHit_phieta", 
    "NHits per tower;#eta;#phi", 40, eta_edges, 120, phi_edges);
""");
