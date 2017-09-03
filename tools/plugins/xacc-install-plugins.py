#!/usr/bin/env python
import argparse
import sys
import os
import subprocess
import multiprocessing

def parse_args(args):
   parser = argparse.ArgumentParser(description="XACC Plugin Installer.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter,
                                     fromfile_prefix_chars='@')
   parser.add_argument("-p", "--plugins", nargs='*', type=str, help="The XACC Plugins to install.", required=True)
   opts = parser.parse_args(args)
   return opts

ornlqci = 'https://github.com/ornl-qci'
availablePluginUrls = { 'xacc-scaffold' : ornlqci+'/xacc-scaffold',
                    'xacc-rigetti' : ornlqci+'/xacc-rigetti',
                    'xacc-dwave' : ornlqci+'/xacc-dwave',
                    'xacc-ibm' : ornlqci+'/xacc-ibm',
                    'xacc-python' : ornlqci+'/xacc-python', 
                    'tnqvm' : ornlqci+'/tnqvm',
                    'ibm' : ornlqci+'/xacc-ibm',
                    'rigetti' : ornlqci+'/xacc-rigetti',
                    'scaffold' : ornlqci+'/xacc-scaffold',
                    'dwave' : ornlqci+'/xacc-dwave' }

def mkdir_p(path):
    """ Operates like mkdir -p in a Unix-like system """
    try:
        os.makedirs(path)
    except OSError as e:
        if os.path.exists(path) and os.path.isdir(path):
            pass
        else:
            print("")
            print("--------------------------- ERROR -----------------------------")
            print("Cannot create directory " + path)
            print("--------------------------- ERROR -----------------------------")
            print("")
            exit()

def main(argv=None):
   opts = parse_args(sys.argv[1:])

   # This python script should be in ${XACC_ROOT}/bin, 
   # we need to get XACC_ROOT
   xaccLocation = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
   xacc_cwd = os.getcwd()
   cpus = str(multiprocessing.cpu_count())

   # Get the plugins we're supposed to install
   plugins = opts.plugins

   # Loop over the plugins and install them
   for plugin in plugins:
      if plugin not in availablePluginUrls:
         print("")
         print("--------------------------- ERROR -----------------------------")
         print("Invalid plugin name - " + plugin)
         print("--------------------------- ERROR -----------------------------")
         print("")
         exit()
         
      # Create a CMakeLists.txt file
      cmakeContents = """
      project(""" + plugin + """-project LANGUAGES CXX)
      cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
      set(CMAKE_STANDARD_REQUIRED ON)
      set(CMAKE_CXX_STANDARD 14)
      set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
      set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
      include(ProcessorCount)
      ProcessorCount(N)
      if(N EQUAL 0)
        set(N 1)
      endif()
      include(ExternalProject)
      ExternalProject_Add("""+plugin+"""
                 GIT_REPOSITORY """+availablePluginUrls[plugin]+"""
                 GIT_TAG master
                 CMAKE_ARGS -DXACC_DIR="""+xaccLocation+"""
                 BUILD_ALWAYS 1
                 INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
                 TEST_BEFORE_INSTALL 1
              )
          """
          
      # Create a build directory
      mkdir_p(plugin+'-install')
      mkdir_p(plugin+'-install/build')
      os.chdir(plugin+'-install')

      # Write the CMakeLists file 
      cmakelists = open("CMakeLists.txt", "w")
      cmakelists.write("%s" % cmakeContents)
      cmakelists.close()
      
      # Execute the build
      os.chdir('build')
      cmakecmd = ['cmake', '..']
      subprocess.check_call(cmakecmd, stderr=subprocess.STDOUT, shell=False)
      subprocess.check_call(['make', '-j'+cpus], stderr=subprocess.STDOUT, shell=False)
      
      os.chdir(xacc_cwd)

if __name__ == "__main__":
    sys.exit(main())
