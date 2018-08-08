#!/usr/bin/python
from glob import glob

l206 = []
l215 = []

for x in glob("root/500206/*.root"):
    l206.append(int(x.split('_')[-1].split('.root')[0]))
for x in glob("root/500215/*.root"):
    l215.append(int(x.split('_')[-1].split('.root')[0]))

l206.sort()
l215.sort()

print 'missing from 500206'
for x in xrange(0, 1+ max(l206)):
    if x not in l206:
        print x 




