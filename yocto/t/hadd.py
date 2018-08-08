#!/usr/bin/python
from subprocess import call
from glob import glob
from datetime import datetime

cmd = "hadd hadd/0.root "
base = glob("root/*_0.root")[0].split('_0')[0] 
# print(base)
# exit(1)

for i in xrange(10):
    cmd += '%s_%i.root '%(base,i)
# print(cmd)
call(cmd, shell=True)
# exit(0)


# exit(1)
for i in xrange(100):
    # call ("echo %i"%i, shell=True)
    string = "%02i"%i
    cmd = "hadd hadd/%s.root "%string
    cmd += " ".join(glob("root/*%s.root"%string))
    call(cmd, shell=True)

name = datetime.now().strftime('hadd_%Y-%m-%d.root')
cmd = "hadd %s hadd/*"%name
call(cmd, shell=True)

exit(0)

