#!/user/bin/python
from glob import glob

for L in glob('../run/hijing.*.root'):
    fout = open('k_int/%s'%L.split('.')[-2],'w')
    fout.close()
