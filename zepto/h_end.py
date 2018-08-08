#!/usr/bin/python
from glob import glob
from subprocess import call

goodnames = []
ln_names = []
for f in glob("ln_we_END/*"):
    name = f.split('/')[-1].split('.root')[0]
    ln_names.append(name)

for f in glob("out/zepto/*"):
    is_good = False
    for L in open(f,'r').readlines():
        if 'We END' in L:
            is_good = True
            break
    if is_good:
        name = f.split('/')[-1]
        if name not in ln_names:
            print "adding %s"%f
            call (['ln','-s','../root/zepto/%s.root'%name,"ln_we_END/%s.root"%name])

# for x in goodnames:
    # print x

