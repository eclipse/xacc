from _pyxacc import *
import os, platform

def initialize():
   xaccHome = os.environ['HOME']+'/.xacc'
   if not os.path.exists(xaccHome):
      os.makedirs(xaccHome)

   try:
      file = open(xaccHome+'/.internal_plugins', 'r')
      contents = file.read()
   except IOError:
      file = open(xaccHome+'/.internal_plugins', 'w')
      contents = ''

   file.close()

   file = open(xaccHome+'/.internal_plugins', 'w')
   
   xaccLocation = os.path.dirname(os.path.realpath(__file__))   
   if platform.system() == "Darwin":
       libname1 = "libxacc-quantum-gate.dylib"
       libname2 = "libxacc-quantum-aqc.dylib"
   else:
       libname1 = "libxacc-quantum-gate.so"
       libname2 = "libxacc-quantum-aqc.so"

   if xaccLocation+'/lib/'+libname1+'\n' not in contents:
      file.write(xaccLocation+'/lib/'+libname1+'\n')
   if xaccLocation+'/lib/'+libname2+'\n' not in contents:
      file.write(xaccLocation+'/lib/'+libname2+'\n')
   
   file.write(contents)
   file.close()

initialize()
