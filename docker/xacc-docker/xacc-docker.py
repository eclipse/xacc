import os
import sys
import fnmatch
import platform
import argparse
import subprocess
import multiprocessing
import glob
import shutil
import docker
from docker.utils import kwargs_from_env

dockerClient = docker.from_env()
imageName = 'xacc/xacc-all'
containerName = 'xacc'
xaccContainer = None
for container in dockerClient.containers.list():
    if container.name == containerName:
       xaccContainer = container
       break

def start():
   # start the xacc-all docker container
   xaccContainer = dockerClient.containers.run(imageName, detach=True, name=containerName, stdin_open=True, tty=True, command=['tail','-f','/dev/null']) 
   
def stop():
   # stop the xacc-all docker container
   xaccContainer.stop()

def executePythonScript():
   # Execute python script inside container
   hello = ''

def execute():
   # execute python or cpp file, if 
   # cpp we have to build it
   hello = ''

def parse_args(args):
    parser = argparse.ArgumentParser(description="XACC Docker Driver.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter,
                                     fromfile_prefix_chars='@')
    parser.add_argument("--start", help="Start the XACC virtual environment.", action='store_true', required=False)
    parser.add_argument("--stop", help="Stop the XACC virtual environment.", action='store_true', required=False)
    return parser.parse_args(args)



def main(argv=None):
    opts = parse_args(sys.argv[1:])
   
    if opts.start:
       start()
       return 0

    if opts.stop:
       stop()
       return 0

if __name__ == "__main__":
    sys.exit(main())
