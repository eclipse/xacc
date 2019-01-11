from _pyxacc import *
import os, time, json
import platform
import sys
import sysconfig
import argparse
import inspect
import pelix.framework
from pelix.utilities import use_service
from abc import abstractmethod, ABC
import configparser

hasPluginGenerator = False
try:
   from plugin_generator import plugin_generator
   hasPluginGenerator = True
except:
    pass

class BenchmarkAlgorithm(ABC):

    # Override this execute method to implement the algorithm
    # @input inputParams
    # @return buffer
    @abstractmethod
    def execute(self, inputParams):
         pass

    # Override this analyze method called to manipulate result data from executing the algorithm
    # @input buffer
    # @input inputParams
    @abstractmethod
    def analyze(self, buffer, inputParams):
        pass

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
    parser.add_argument("-f", "--framework-help", action='store_true',
                        help="Print the help information for XACC and its available plugins.", required=False)
    parser.add_argument("--python-include-dir", action='store_true',
                        help="Print the path to the Python.h.", required=False)
    parser.add_argument("-b", "--branch", default='master', type=str,
                        help="Print the path to the XACC install location.", required=False)
    parser.add_argument("--benchmark", type=str, help="Run the benchmark detailed in the given input file.", required=False)
    parser.add_argument("--list-backends", type=str, help="List the backends available for the provided Accelerator.", required=False)

    if hasPluginGenerator:
       subparsers = parser.add_subparsers(title="subcommands", dest="subcommand",
                                          description="Run python3 -m xacc [subcommand] -h for more information about a specific subcommand")
       plugin_generator.add_subparser(subparsers)

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

def qasm2Kernel(kernelName, qasmStr):
    return '__qpu__ {}(AcceleratorBuffer b) {{\n {} \n}}'.format(kernelName, qasmStr)

class DecoratorFunction(ABC):

    def __init__(self):
        pass

    def initialize(self, f, *args, **kwargs):
        self.function = f
        self.args = args
        self.kwargs = kwargs
        self.__dict__.update(kwargs)
        self.accelerator = None
        self.src = '\n'.join(inspect.getsource(self.function).split('\n')[1:])
        compiler = getCompiler('xacc-py')
        if self.accelerator == None:
            if 'accelerator' in self.kwargs:
                if isinstance(self.kwargs['accelerator'], Accelerator):
                    self.qpu = self.kwargs['accelerator']
                else:
                    self.qpu = getAccelerator(self.kwargs['accelerator'])
            elif hasAccelerator('tnqvm'):
                self.qpu = getAccelerator('tnqvm')
            else:
                print(
                    '\033[1;31mError, no Accelerators installed. We suggest installing TNQVM.\033[0;0m')
                exit(0)
        else:
            print('Setting accelerator: ', self.accelerator.name())
            self.qpu = self.accelerator
        
        ir = compiler.compile(self.src, self.qpu)
        program = Program(self.qpu, ir)
        self.compiledKernel = program.getKernels()[0]

    def overrideAccelerator(self, acc):
        self.accelerator = acc

    def nParameters(self):
        return self.getFunction().nParameters()

    def getFunction(self):
        return self.compiledKernel.getIRFunction()

    @abstractmethod
    def __call__(self, *args, **kwargs):
        pass

class WrappedF(DecoratorFunction):

    def __init__(self, f, *args, **kwargs):
        self.function = f
        self.args = args
        self.kwargs = kwargs
        self.__dict__.update(kwargs)
        self.accelerator = None

    def __call__(self, *args, **kwargs):
        super().__call__(*args, **kwargs)
        argsList = list(args)
        if not isinstance(argsList[0], AcceleratorBuffer):
            raise RuntimeError(
                'First argument of an xacc kernel must be the Accelerator Buffer to operate on.')
        #buffer = argsList[0]
        #functionBufferName = inspect.getargspec(self.function)[0][0]
        # Replace function arg0 name with buffer.name()
        #self.src = self.src.replace(functionBufferName, buffer.name())
        self.compiledKernel.execute(argsList[0], argsList[1:])
        return

class qpu(object):
    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        self.__dict__.update(kwargs)
        return

    def __call__(self, f):
        if 'algo' in self.kwargs:
            servName = self.kwargs['algo']
            function = serviceRegistry.get_service('decorator_algorithm_service', servName)
            function.initialize(f, *self.args, **self.kwargs)
            return function
        else:
            wf = WrappedF(f, *self.args, **self.kwargs)
            wf.initialize(f,*self.args, **self.kwargs)
            return wf


def compute_readout_error_probabilities(qubits, buffer, qpu, shots=8192, persist=True):
    p10Functions = []
    p01Functions = []
    p10CheckedBitStrings = [] # ['001','010','100'] for 3 bits
    p10s = []
    p01s = []

    zeros = '0'*buffer.size()
    for i, q in enumerate(qubits):
        measure = gate.create('Measure',[q],[i])
        f = gate.createFunction('meas_'+str(q), [])
        f.addInstruction(measure)
        p10Functions.append(f)
        tmp = list(zeros)
        tmp[buffer.size()-1-q] = '1'
        p10CheckedBitStrings.append(''.join(tmp))

    for i, q in enumerate(qubits):
        x = gate.create('X',[q])
        measure = gate.create('Measure',[q],[i])
        f = gate.createFunction('meas_'+str(q), [])
        f.addInstruction(x)
        f.addInstruction(measure)
        p01Functions.append(f)

    setOption(qpu.name()+'-shots', shots)

    # Execute
    b1 = qpu.createBuffer('tmp1',max(qubits)+1)
    b2 = qpu.createBuffer('tmp2',max(qubits)+1)

    results1 = qpu.execute(b1, p10Functions)
    results2 = qpu.execute(b2, p01Functions)

    # Populate with probability you saw a 1 but expected 0
    for i, b in enumerate(results1):
        p10s.append(b.computeMeasurementProbability(p10CheckedBitStrings[i]))
    for i, b in enumerate(results2):
        p01s.append(b.computeMeasurementProbability(zeros))

    filename = ''
    if persist:
        if not os.path.exists(os.getenv('HOME')+'/.xacc_cache/ro_characterization'):
            os.makedirs(os.getenv('HOME')+'/.xacc_cache/ro_characterization')

        backend = 'NullBackend'
        if optionExists(qpu.name()+'-backend'):
            backend = getOption(qpu.name()+'-backend')

        filename = os.getenv('HOME')+'/.xacc_cache/ro_characterization/'+qpu.name()+'_'+backend+"_ro_error_{}.json".format(time.ctime().replace(' ','_').replace(':','_'))

        data = {'shots':shots, 'backend':backend}
        for i in qubits:
            data[str(i)] = {'0|1':p01s[i],'1|0':p10s[i],'+':(p01s[i]+p10s[i]),'-':(p01s[i]-p10s[i])}
        with open(filename,'w') as outfile: json.dump(data, outfile)

    return p01s,p10s, filename


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

'''The following code provides the hooks necessary for executing benchmarks with XACC'''


class PyServiceRegistry(object):
    def __init__(self):
        self.framework = pelix.framework.create_framework((
            "pelix.ipopo.core",
            "pelix.shell.console"))
        self.framework.start()
        self.context = self.framework.get_bundle_context()
        self.services = []
    def initialize(self):
        serviceList = ['decorator_algorithm_service', 'benchmark_algorithm_service']
        xaccLocation = os.path.dirname(os.path.realpath(__file__))
        pluginDir = xaccLocation + '/py-plugins'
        if not os.path.exists(pluginDir):
            os.makedirs(pluginDir)
        sys.path.append(pluginDir)

        pluginFiles = [f for f in os.listdir(
            pluginDir) if os.path.isfile(os.path.join(pluginDir, f))]
        for f in pluginFiles:
            bundle_name = os.path.splitext(f)[0].strip()
            self.context.install_bundle(bundle_name).start()
        try:
            for servType in serviceList:
                self.services += self.context.get_all_service_references(servType)
        except:
            if len(self.services) > 0:
                pass
        #if not self.services:
            #print("No XACC algorithm bundles found in " + pluginDir + ".")
            #exit(1)

    def get_service(self, serviceName, name):
        services = self.context.get_all_service_references(serviceName)
        service = None
        for s in services:
            if s.get_properties()['name'] == name:
                service = self.context.get_service(s)
        return service

if not pelix.framework.FrameworkFactory.is_framework_running(None):
    serviceRegistry = PyServiceRegistry()
    serviceRegistry.initialize()

def benchmark(opts):
    if opts.benchmark is not None:
        inputfile = opts.benchmark
        xacc_settings = process_benchmark_input(inputfile)
    else:
        error('Must provide input file for benchmark.')
        return

    Initialize()

    if ':' in xacc_settings['accelerator']:
        accelerator, backend = xacc_settings['accelerator'].split(':')
        setOption(accelerator + "-backend", backend)
        xacc_settings['accelerator'] = accelerator

    accelerator = xacc_settings['accelerator']
    if 'n-shots' in xacc_settings:
        if 'local-ibm' in accelerator:
            accelerator = 'ibm'
        setOption(accelerator+('-trials' if 'rigetti' in accelerator else '-shots'), xacc_settings['n-shots'])

    # Using ServiceRegistry to getService (benchmark_algorithm_service) and execute the service
    algorithm = serviceRegistry.get_service(
        'benchmark_algorithm_service', xacc_settings['algorithm'])
    if algorithm is None:
        print("XACC algorithm service with name " +
                   xacc_settings['algorithm'] + " is not installed.")
        exit(1)

    starttime = time.time()
    buffer = algorithm.execute(xacc_settings)
    elapsedtime = time.time() - starttime
    buffer.addExtraInfo("benchmark-time", elapsedtime)
    for k,v in xacc_settings.items():
        buffer.addExtraInfo(k, v)
    # Analyze the buffer
    head, tail = os.path.split(inputfile)
    buffer.addExtraInfo('file-name', tail)
    algorithm.analyze(buffer, xacc_settings)
    timestr = time.strftime("%Y%m%d-%H%M%S")
    results_name = "%s_%s_%s_%s" % (os.path.splitext(tail)[0], xacc_settings['accelerator'], xacc_settings['algorithm'], timestr)
    f = open(results_name+".ab", 'w')
    f.write(str(buffer))
    f.close()

    Finalize()

def process_benchmark_input(filename):
    config = configparser.RawConfigParser()
    try:
        with open(filename) as f:
            framework_settings = {}
            config.read(filename)
            for section in config.sections():
                temp = dict(config.items(section))
                framework_settings.update(temp)
            return framework_settings
    except:
        print("Input file " + filename + " could not be opened.")
        exit(1)

def main(argv=None):
    opts = parse_args(sys.argv[1:])
    xaccLocation = os.path.dirname(os.path.realpath(__file__))
    if opts.location:
        print(xaccLocation)
        sys.exit(0)

    if hasPluginGenerator and opts.subcommand == "generate-plugin":
        plugin_generator.run_generator(opts, xaccLocation)
        sys.exit(0)

    if opts.python_include_dir:
        print(sysconfig.get_paths()['platinclude'])
        sys.exit(0)

    if opts.framework_help:
        Initialize(['--help'])
        return

    if opts.list_backends is not None:
        acc = opts.list_backends
        if acc == 'ibm':
            info('Retrieving remote IBM backend information')
            Initialize(['--'+acc+'-list-backends'])
        elif acc == 'dwave':
            info('Retrieving remote D-Wave solver information')
            Initialize(['--'+acc+'-list-solvers'])
        return

    if not opts.set_credentials == None:
        setCredentials(opts)

    if not opts.benchmark == None:
        benchmark(opts)

initialize()

if __name__ == "__main__":
    sys.exit(main())
