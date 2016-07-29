#***
#   make (configure and build) one single 
#***
#
import sys
import os
from subprocess import call
from datetime import datetime
from contextlib import closing
import buildlib

if len(sys.argv) < 1: 
    print "we need a project name"
    quit() 

project = sys.argv[1]      

#os.chdir('../')  #  go to one level up
basePath = os.getcwd() # remember this path
try:
    buildlib.buildProject(project, 'c')
except  Exception as einst: # 
    print "project configuration faild: " + str(einst)
    quit() 

os.chdir(basePath) 
buildlib.buildProject(project, 'b')


