#    increase a build number and create a cpp name with this info.
#    \file incbuildnumber.py
#    \author   Igor Sandler
#    \version 1.0
#
#


import buildlib
from contextlib import closing
import sys
from datetime import datetime
import string

print 'creating build number'
if len(sys.argv) < 3: 
    print  "ERROR in incbuildnumber.py; need two arguments:"
    print  "	1:  file name "
    print  "	2:  output file name "
    sys.exit(1)

else: # save project name
    fName =  sys.argv[1]
    dstName = sys.argv[2]
	
x = buildlib.get_StringFrom(fName)
x = string.atoi(x)
x = x + 1

#time: 
__currentTime__ = str(datetime.now())

#put everything back to a file:
try:
    with closing(open(fName,'wt')) as bif1:
        bif1.write(str(x))
except:
    print 'ERROR in incbuildnumber.py; can not create output file ' + fName
    raise
	
#put everything to a file:
try:
    with closing(open(dstName,'wt')) as bif:
        print 'BN #' + str(x)
        buildlib.writeCString(bif, 'BUILD_NUMBER', str(x))
        buildlib.writeCString(bif, 'COMPILE_TIME', __currentTime__)
		
except:
    print 'ERROR in build_info.py; can not create output file ' + dstName
    raise

	
	
	
	
