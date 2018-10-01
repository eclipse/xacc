from _pyxacc import *
import os
import platform
import sys
import sysconfig
import argparse
import inspect


def parse_args(args):
    parser = argparse.ArgumentParser(description="XACC Framework Utility.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter,
                                     fromfile_prefix_chars='@')
    parser.add_argument("-c", "--set-credentials", type=str,
                        help="Set your credentials for any of the remote Accelerators.", required=False)
    parser.add_argument("-k", "--api-key", type=str,
                        help="The API key for the remote Accelerators.", required=False)
    parser.add_argument("-u", "--user-id", type=str,
                        help="The User Id for the remote Accelerators.", required=False)
    parser.add_argument(
        "--url", type=str, help="The URL for the remote Accelerators.", required=False)
    parser.add_argument("-g", "--group", type=str,
                        help="The IBM Q Group.", required=False)
    parser.add_argument("--hub", type=str, help="The IBM Q Hub.",
                        default='ibm-q-ornl', required=False)
    parser.add_argument("-p", "--project", type=str,
                        help="The IBM Q Project.", required=False)
    parser.add_argument("-L", "--location", action='store_true',
                        help="Print the path to the XACC install location.", required=False)
    parser.add_argument("--python-include-dir", action='store_true',
                        help="Print the path to the Python.h.", required=False)
    parser.add_argument("-b", "--branch", default='master', type=str,
                        help="Print the path to the XACC install location.", required=False)

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
    setIsPyApi()


def setCredentials(opts):
    defaultUrls = {'ibm': 'https://quantumexperience.ng.bluemix.net',
                   'rigetti': 'https://api.rigetti.com/qvm', 'dwave': 'https://cloud.dwavesys.com'}
    acc = opts.set_credentials
    url = opts.url if not opts.url == None else defaultUrls[acc]
    if acc == 'rigetti' and not os.path.exists(os.environ['HOME']+'/.pyquil_config'):
        apikey = opts.api_key
        if opts.user_id == None:
            print('Error, must provide user-id for Rigetti accelerator')
            sys.exit(1)
        user = opts.user_id
        f = open(os.environ['HOME']+'/.pyquil_config', 'w')
        f.write('[Rigetti Forest]\n')
        f.write('key: ' + apikey + '\n')
        f.write('user_id: ' + user + '\n')
        f.write('url: ' + url + '\n')
        f.close()
    elif acc == 'ibm' and (opts.group != None or opts.project != None):
        # We have hub,group,project info coming in.
        if (opts.group != None and opts.project == None) or (opts.group == None and opts.project != None):
            print('Error, you must provide both group and project')
            sys.exit(1)
        f = open(os.environ['HOME']+'/.'+acc+'_config', 'w')
        f.write('key: ' + opts.api_key + '\n')
        f.write('url: ' + url + '\n')
        f.write('hub: ' + opts.hub + '\n')
        f.write('group: ' + opts.group + '\n')
        f.write('project: ' + opts.project + '\n')
        f.close()
    else:
        if not os.path.exists(os.environ['HOME']+'/.'+acc+'_config'):
            f = open(os.environ['HOME']+'/.'+acc+'_config', 'w')
            f.write('key: ' + opts.api_key + '\n')
            f.write('url: ' + url + '\n')
            f.close()
    fname = acc if 'rigetti' not in acc else 'pyquil'
    print('\nCreated '+acc+' config file:\n$ cat ~/.'+fname+'_config:')
    print(open(os.environ['HOME']+'/.'+fname+'_config', 'r').read())


class WrappedF(object):
    def __init__(self, f, *args, **kwargs):
        self.function = f
        self.args = args
        self.kwargs = kwargs
        self.__dict__.update(kwargs)

    def nParameters(self):
        if 'accelerator' in self.kwargs:
            if isinstance(self.kwargs['accelerator'], Accelerator):
                qpu = self.kwargs['accelerator']
            else:
                qpu = getAccelerator(self.kwargs['accelerator'])
        elif hasAccelerator('tnqvm'):
            qpu = getAccelerator('tnqvm')
        else:
            print(
                '\033[1;31mError, no Accelerators installed. We suggest installing TNQVM.\033[0;0m')
            exit(0)

        # Remove the @qpu line from the source
        src = '\n'.join(inspect.getsource(self.function).split('\n')[1:])

        # Get the compiler and compile the code
        compiler = getCompiler('xacc-py')
        ir = compiler.compile(src, qpu)
        program = Program(qpu, ir)
        compiledKernel = program.getKernels()[0]
        return compiledKernel.getIRFunction().nParameters()

    def getFunction(self):
        if 'accelerator' in self.kwargs:
            if isinstance(self.kwargs['accelerator'], Accelerator):
                qpu = self.kwargs['accelerator']
            else:
                qpu = getAccelerator(self.kwargs['accelerator'])
        elif hasAccelerator('tnqvm'):
            qpu = getAccelerator('tnqvm')
        else:
            print(
                '\033[1;31mError, no Accelerators installed. We suggest installing TNQVM.\033[0;0m')
            exit(0)

        # Remove the @qpu line from the source
        src = '\n'.join(inspect.getsource(self.function).split('\n')[1:])

        # Get the compiler and compile the code
        compiler = getCompiler('xacc-py')
        ir = compiler.compile(src, qpu)
        program = Program(qpu, ir)
        compiledKernel = program.getKernels()[0]
        return compiledKernel.getIRFunction()

    def __call__(self, *args, **kwargs):
        if 'accelerator' in self.kwargs:
            if isinstance(self.kwargs['accelerator'], Accelerator):
                qpu = self.kwargs['accelerator']
            else:
                qpu = getAccelerator(self.kwargs['accelerator'])
        elif hasAccelerator('tnqvm'):
            qpu = getAccelerator('tnqvm')
        else:
            print(
                '\033[1;31mError, no Accelerators installed. We suggest installing TNQVM.\033[0;0m')
            exit(0)

        # Remove the @qpu line from the source
        src = '\n'.join(inspect.getsource(self.function).split('\n')[1:])

        argsList = list(args)
        if not isinstance(argsList[0], AcceleratorBuffer):
            raise RuntimeError(
                'First argument of an xacc kernel must be the Accelerator Buffer to operate on.')

        buffer = argsList[0]

        functionBufferName = inspect.getargspec(self.function)[0][0]

        # Replace function arg0 name with buffer.name()
        src = src.replace(functionBufferName, buffer.name())

        # Get the compiler and compile the code
        compiler = getCompiler('xacc-py')
        ir = compiler.compile(src, qpu)
        program = Program(qpu, ir)
        compiledKernel = program.getKernels()[0]

        compiledKernel.execute(argsList[0], argsList[1:])
        return


class qpu(object):
    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        self.__dict__.update(kwargs)
        return

    def __call__(self, f):
        return WrappedF(f, *self.args, **self.kwargs)


def functionToLatex(function):
    try:
        import pyquil
    except ImportError:
        print('Error - We delegate to Pyquil for Latex generation. Install Pyquil.')
        return
    if not hasCompiler('quil'):
        print('Error - We use XACC QuilCompiler to generate Latex. Install XACC-Rigetti plugin.')
        return
    from pyquil.latex import to_latex
    return to_latex(pyquil.quil.Program(getCompiler('quil').translate('', function)))


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
