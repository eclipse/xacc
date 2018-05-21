from _pyxacc import *
import os, platform, sys, sysconfig
import argparse

def parse_args(args):
   parser = argparse.ArgumentParser(description="XACC Framework Utility.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter,
                                     fromfile_prefix_chars='@')
   parser.add_argument("-c", "--set-credentials", type=str, help="Set your credentials for any of the remote Accelerators.", required=False)
   parser.add_argument("-k", "--api-key", type=str, help="The API key for the remote Accelerators.", required=False)
   parser.add_argument("-u", "--user-id", type=str, help="The User Id for the remote Accelerators.", required=False)
   parser.add_argument("--url", type=str, help="The URL for the remote Accelerators.", required=False)
   parser.add_argument("-L", "--location", action='store_true', help="Print the path to the XACC install location.", required=False)
   parser.add_argument("--python-include-dir", action='store_true', help="Print the path to the Python.h.", required=False)
   parser.add_argument("-b", "--branch", default='master',type=str, help="Print the path to the XACC install location.", required=False)
   
   opts = parser.parse_args(args)
   if opts.set_credentials and not opts.api_key:
      print('Error in arg input, must supply api-key if setting credentials')
      sys.exit(1)

   return opts

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

def setCredentials(opts):
   defaultUrls = {'ibm':'https://quantumexperience.ng.bluemix.net','rigetti':'https://api.rigetti.com/qvm'}
   acc = opts.set_credentials
   url = opts.url if not opts.url == None else defaultUrls[acc]
   if acc == 'rigetti' and not os.path.exists(os.environ['HOME']+'/.pyquil_config'):
      apikey = opts.api_key
      if opts.user_id == None:
         print('Error, must provide user-id for Rigetti accelerator')
         sys.exit(1)
      user = opts.user_id
      f = open(os.environ['HOME']+'/.pyquil_config','w')
      f.write('key: ' + apikey + '\n')
      f.write('user_id: ' + user + '\n')
      f.write('url: ' + url + '\n')
      f.close()
   else:
      if not os.path.exists(os.environ['HOME']+'/.'+acc+'_config'):
         f = open(os.environ['HOME']+'/.'+acc+'_config','w')
         f.write('key: '+ opts.api_key + '\n')
         f.write('url: '+ url + '\n') 
         f.close()
   fname = acc if 'rigetti' not in acc else 'pyquil'
   print('\nCreated '+acc+' config file:\n$ cat ~/.'+fname+'_config:')
   print(open(os.environ['HOME']+'/.'+fname+'_config','r').read())

def main(argv=None):
   opts = parse_args(sys.argv[1:])
   xaccLocation = os.path.dirname(os.path.realpath(__file__))
   if opts.location:
       print(xaccLocation)
       sys.exit(0)

   if opts.python_include_dir:
      print(sysconfig.get_paths()['platinclude'])
      sys.exit(0)

   if not opts.set_credentials == None:
      setCredentials(opts)

initialize()

if __name__ == "__main__":
    sys.exit(main())
