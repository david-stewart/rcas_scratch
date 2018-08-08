#!/bin/python
import glob as glob
from sys import argv

i = 0
for x in glob.glob("%s/*"%argv[1]):
    print x
    i += 1

print("there are %i files\n"%i)
