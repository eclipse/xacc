import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from abc import ABC, abstractmethod
from multipledispatch import dispatch
import _pyxacc

@ComponentFactory("azure_accelerator_factory")
@Provides("accelerator")
@Property("_accelerator", "accelerator", "azure")
@Property("_name", "name", "azure")
@Instantiate("azure_accelerator_instance")
class AzureAccelerator(xacc.Accelerator):
    """Azure Quantum Accelerator plugin for XACC.

    Attributes:
        shots (int): number of shots.
        backend (str): name of the target backend.
        job_name (str): tag to name job to be submitted.
        visitor (str): name of the visitor that maps XACC IR into QIR (pyqir)
            or Qiskit QuantumCircuit (qiskit)
        location (str): geographical location associated with Azure account.
        resource_id (str): Azure resource ID.
        get_cost (bool): only valid for Quantinuum backends.
            If True, retrieves cost estimate.
        error_budget (float): only valid for the Microsoft Estimator.
            Error threshold for quantum error correction.
    """

    def __init__(self):
        xacc.Accelerator.__init__(self)
        self.shots = 1024
        self.backend = None
        self.job_name = None
        self.visitor = 'pyqir'
        self.location = None
        self.resource_id = None
        self.get_cost = False
        self.credentials_file = None
        self.error_budget = 0.005 # this is only for the estimator

        # retrieve credentials
        self.get_credentials()

        # instantiate provider
        from azure.quantum.qiskit import AzureQuantumProvider
        self.provider = AzureQuantumProvider(resource_id=self.resource_id,
                              location=self.location)

    def get_credentials(self):
        """Gets Azure Quantum account credentials.

        It looks for a file named .azure_config (notice the ".") in $HOME
        with the following structure:
        
        location: [location]
        Resource ID: [Azure resource ID]
        """

        from os import environ
        filename = environ["HOME"] + "/.azure_config"
        try:
            lines = open(filename, "r").readlines()
        except:
            xacc.error("File with credentials does not exist or is not $HOME/.azure_config")
        else:
            for line in lines:
                if line[:8] == 'location':
                    self.location = line[9:].strip().replace(" ", "").lower()
                if line[:11] == 'Resource ID':
                    self.resource_id = line[12:].strip()
        return

    def initialize(self, options):
        """Checks for required and optional parameters.
        The only required parameter is the name of the desired backend.

        Args:
            options (dict): options for Accelerator execution.

        """

        if 'backend' in options:
            self.backend = options['backend']
            if self.backend not in [b.name() for b in self.provider.backends()]:
                xacc.error("Backend not available.")
                
        else:
            error_msg = "Backend was not provided. Please choose one of the following:\n"
            for available_backend in self.provider.backends():
                error_msg += available_backend.name() + "\n"
            xacc.error(error_msg)

        if 'shots' in options:
            self.shots = options['shots']
            if self.shots < 1:
                xacc.error("Number of shots needs to be greater than 1.")

        if 'job-name' in options:
            self.job_name = options['job-name']

        if 'visitor' in options:
            self.visitor = options['visitor']

        if 'error-budget' in options:
            self.error_budget = options['error-budget']
            if self.backend != "microsoft.estimator":
                xacc.warning("Error budget is only required for the Microsoft Estimator.")

        if 'get-cost' in options:
            if "quantinuum" in self.backend:
                self.get_cost =  options['get-cost']
            else:
                xacc.warning("Cost estimate is a feature of Quantinuum backends.")

        return

    def updateConfiguration(self, options):
        self.initialize(options)

    def name(self):
        return 'azure'

    # took this from the Azure Quantum docs
    def job_from_qir(self, bitcode, **kwargs):
        """A generic function to create a resource estimation job from QIR bitcode"""

        # Find the Resource Estimator target from the provider
        backend = self.provider.get_backend(self.backend)

        # You can provide a name for the job via keyword arguments; if not,
        # use QIR job as a default name
        name = kwargs.pop("name", "QIR job")

        # Extract some job specific arguments from the backend's configuration
        config = backend.configuration()
        blob_name = config.azure["blob_name"]
        content_type = config.azure["content_type"]
        provider_id = config.azure["provider_id"]
        output_data_format = config.azure["output_data_format"]

        # Finally, create the Azure Quantum jon object and return it
        from azure.quantum.qiskit.job import AzureQuantumJob
        return AzureQuantumJob(
            backend=backend,
            target=backend.name(),
            name=name,
            input_data=bitcode,
            blob_name=blob_name,
            content_type=content_type,
            provider_id=provider_id,
            input_data_format="qir.v1",
            output_data_format=output_data_format,
            input_params = kwargs,
            metadata={}
        )


    def execute(self, buffer, program):
        """Execute quantum program on target backend.
        
        Args:
            buffer (xacc.AcceleratorBuffer): qubit register. 
                Also persists execution data.
            program (xacc. CompositeInstruction): quantum circuit.
        
        """

        from qiskit.visualization import plot_histogram
        from qiskit.tools.monitor import job_monitor

        # get instance of desired visitor
        if self.visitor == 'pyqir':
            visitor = PyQIRVisitor(program)
        else:
            visitor = QiskitVisitor(program)

        # visit each instruction and map onto the desired format
        for inst in program.getInstructions():
            inst.accept(visitor)

        # retrieve circuit in the desired format
        module = visitor.get_circuit_object()

        #proceed according to the desired backend/task
        if self.backend == "microsoft.estimator":
            bitcode = module.bitcode()
            job = self.job_from_qir(bitcode)
            job_monitor(job)
            result = job.result()
            from json import dumps
            buffer.addExtraInfo("estimate-data", dumps(result.data()))

        else:
            backend = self.provider.get_backend(self.backend)

            if self.get_cost:
                job = backend.estimate_cost(module, shots = self.shots)
                print(job.estimated_total)
                buffer.addExtraInfo("cost-estimate", job.estimated_total)
                return
            
            job = backend.run(module, count = self.shots)
            job_monitor(job)
            result = job.result()
            buffer.setMeasurements(result.get_counts())

        return

# Here are two implementations of a standard visitor pattern
# where it visits each instance of a xacc.Instruction and builds
# the corresponding instance with either PyQIR BasicQisBuilder
# or Qiskit QuantumCircuit builder.
# Because XACC implements gates (xacc.Instruction) by overloading 
# xacc::quantum::Gate(), we enable dynamic dispatch with the
# multipledispatch package
class PyQIRVisitor(xacc.quantum.AllGateVisitor):
    def __init__(self, xacc_circuit):
        xacc.quantum.AllGateVisitor.__init__(self)
        num_results = 0
        for inst in xacc_circuit.getInstructions():
            if inst.name() == "Measure":
                num_results +=1

        num_qubits = xacc_circuit.nLogicalBits()

        from pyqir import BasicQisBuilder, SimpleModule
        self.module = SimpleModule(xacc_circuit.name(), num_qubits=num_qubits, num_results=num_results)
        self.circuit = BasicQisBuilder(self.module.builder)

    def get_circuit_object(self):
        return self.module

    @dispatch(xacc.quantum.Hadamard)
    def visit(self, gate):
        self.circuit.h(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.X)
    def visit(self, gate):
        self.circuit.x(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Y)
    def visit(self, gate):
        self.circuit.y(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Z)
    def visit(self, gate):
        self.circuit.z(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.T)
    def visit(self, gate):
        self.circuit.t(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Tdg)
    def visit(self, gate):
        self.circuit.t_adj(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.S)
    def visit(self, gate):
        self.circuit.s(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Sdg)
    def visit(self, gate):
        self.circuit.s_adj(self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Measure)
    def visit(self, gate):
        self.circuit.mz(self.module.qubits[gate.bits()[0]], self.module.results[gate.bits()[0]])

    @dispatch(xacc.quantum.Rx)
    def visit(self, gate):
        self.circuit.rx(gate.getParameters()[0], self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Ry)
    def visit(self, gate):
        self.circuit.ry(gate.getParameters()[0], self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Rz)
    def visit(self, gate):
        self.circuit.rz(gate.getParameters()[0], self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.CNOT)
    def visit(self, gate):
        self.circuit.cx(self.module.qubits[gate.bits()[0]], self.module.qubits[gate.bits()[1]])

    @dispatch(xacc.quantum.CZ)
    def visit(self, gate):
        self.circuit.cz(self.module.qubits[gate.bits()[0]], self.module.qubits[gate.bits()[1]])


class QiskitVisitor(xacc.quantum.AllGateVisitor):
    def __init__(self, xacc_circuit):
        xacc.quantum.AllGateVisitor.__init__(self)
        num_results = 0
        for inst in xacc_circuit.getInstructions():
            if inst.name() == "Measure":
                num_results +=1

        num_qubits = xacc_circuit.nLogicalBits()

        from qiskit import QuantumCircuit
        self.circuit = QuantumCircuit(num_qubits, num_results)
        self.circuit.name = xacc_circuit.name()

    def get_circuit_object(self):
        return self.circuit

    @dispatch(xacc.quantum.Hadamard)
    def visit(self, gate):
        self.circuit.h(gate.bits()[0])

    @dispatch(xacc.quantum.X)
    def visit(self, gate):
        self.circuit.x(gate.bits()[0])

    @dispatch(xacc.quantum.Y)
    def visit(self, gate):
        self.circuit.y(gate.bits()[0])

    @dispatch(xacc.quantum.Z)
    def visit(self, gate):
        self.circuit.z(gate.bits()[0])

    @dispatch(xacc.quantum.T)
    def visit(self, gate):
        self.circuit.t(gate.bits()[0])

    @dispatch(xacc.quantum.Tdg)
    def visit(self, gate):
        self.circuit.tdg(gate.bits()[0])

    @dispatch(xacc.quantum.S)
    def visit(self, gate):
        self.circuit.s(gate.bits()[0])

    @dispatch(xacc.quantum.Sdg)
    def visit(self, gate):
        self.circuit.sdg(gate.bits()[0])

    @dispatch(xacc.quantum.Measure)
    def visit(self, gate):
        # most of the time the indices are the same
        self.circuit.measure(gate.bits()[0], gate.bits()[0])

    @dispatch(xacc.quantum.Rx)
    def visit(self, gate):
        self.circuit.rx(gate.getParameters()[0], self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Ry)
    def visit(self, gate):
        self.circuit.ry(gate.getParameters()[0], self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.Rz)
    def visit(self, gate):
        self.circuit.rz(gate.getParameters()[0], self.module.qubits[gate.bits()[0]])

    @dispatch(xacc.quantum.U)
    def visit(self, gate):
        theta = gate.getParameters()[0]
        phi = gate.getParameters()[1]
        lam = gate.getParameters()[2] 
        self.circuit.u(self.module.qubits[gate.bits()[0]], theta, phi, lam)

    @dispatch(xacc.quantum.CNOT)
    def visit(self, gate):
        self.circuit.cx(gate.bits()[0], gate.bits()[1])

    @dispatch(xacc.quantum.CZ)
    def visit(self, gate):
        self.circuit.cz(gate.bits()[0], gate.bits()[1])

    @dispatch(xacc.quantum.CY)
    def visit(self, gate):
        self.circuit.cy(gate.bits()[0], gate.bits()[1])

    @dispatch(xacc.quantum.CH)
    def visit(self, gate):
        self.circuit.ch(gate.bits()[0], gate.bits()[1])

    @dispatch(xacc.quantum.Swap)
    def visit(self, gate):
        self.circuit.swap(gate.bits()[0], gate.bits()[1])

    @dispatch(xacc.quantum.iSwap)
    def visit(self, gate):
        self.circuit.iswap(gate.bits()[0], gate.bits()[1])

    @dispatch(xacc.quantum.CPhase)
    def visit(self, gate):
        self.circuit.cp(gate.getParameters()[0], gate.bits()[0], gate.bits()[1])

    @dispatch(xacc.quantum.CRZ)
    def visit(self, gate):
        self.circuit.crz(gate.getParameters()[0], gate.bits()[0], gate.bits()[1])