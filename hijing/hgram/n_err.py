#!/usr/bin/python
from glob import glob
fout = open("errs",'w')
for x in glob("err/*"):
    num = x.split('_')[1].split('.')[0]
    fout.write("   file :: %s\n"%num)
    has_error = False
    i = 0
    try:
    	print 'x: %s'%x
	for L in open(x,'r').readlines():
	    i += 1
	    if i > 20: break
	    
	    # fout.write(L)
	    if 'Error:' in L:
		fout.write("%s"%L)
		break
    except:
    	print "couldn't read %s"%x
fout.close()
