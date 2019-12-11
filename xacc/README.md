Core XACC package and its sub-packages
=================

This package contains all interfaces describing the XACC programming
model, plus a few utility classes that are used throughout XACC. It contains the
following packages:

**accelerator**
----------------

This package defines the Accelerator interface, which provides the overall
communication protocol for clients interacting with backend accelerators. An
accelerator serves as a post-Moore's law co-processor. At its core, it takes
CompositeInstructions and executes them, persisting the results and any other
metadata to a provided AcceleratorBuffer.

This package defines two derived interfaces of the Accelerator. First, the RemoteAccelerator
describes an interface for remotely hosted accelerator co-processors that can
be accessed through a typical REST API. Second, the AcceleratorDecorator describes
a familiar decorator pattern for the Accelerator itself, enabling general
pre- and post-processing of Accelerator execution.

This package defines the AcceleratorBuffer class. This data structure serves as
a general register of post-Moore's law bits (qubits, for example), and serves as
the go-between for clients and Accelerator execution results. AcceleratorBuffers
contain a HeterogeneousMap of metadata for persisting execution and accelerator
 specific results.

**algorithm**
--------------

This package defines the Algorithm interface, which provides a general mechanism for
describing hybrid algorithms that leverage Accelerator co-processors. Algorithms take
as input a heterogeneous mapping of input data, and execute their specific algorithm,
persisting results to a provided AcceleratorBuffer.

**compiler**
------------

This package defines the Compiler interface, which provides a mechanism for taking XACC
kernel source strings and compiling them to an instance of the XACC intermediate representation.
Compilers also expose source-to-source translation capabilities, which take IR to an
instance of this Compilers corresponding source language.

**ir**
------

This package defines the interfaces necessary to make up the XACC intermediate representation (IR).
First, it defines an Instruction interface, which exposes an API describing post-Moore's law low-level
assembly instructions. To do so it exposes a unique instruction name, a vector of bits that it
operates on, and a vector of general parameters (which can be int, double, string) to describe
runtime parameters for the instruction. The Instruction also exposes a general visitor pattern to enable
type-specific information at runtime.

Next, this package defines a CompositeInstruction, which subclasses
Instruction but contains further Instructions (a vector of Instructions). The CompositeInstruction - Instruction
combination therefore forms a familiar n-ary tree pattern, where nodes are CompositeInstructions and leaves
are concrete Instructions. CompositeInstruction exposes its list of contained Instructions, a list of
string variables that the CompositeInstruction depends on, can be persisted to an ostream,
and can be expanded on a set of heterogeneous input data (to enable dynamic, runtime instructions).

Finally, the IR interface provides a general container for CompositeInstructions.

This package also exposes an InstructionIterator utility, enabling pre-order traversal of the CompositeInstruction tree. It exposes
a Graph interface, enabling the mapping of CompositeInstructions to a Graph-like structure. It exposes
the IRTransformation interface, enabling general transformations of CompositeInstructions (useful for program
optimization and ensuring a given program can be run on a given backend).

The IRProvider describes an factory-like interface for the generation of Instructions and CompositeInstructions.

The Observable describes an interface describing sub-types that can 'observe' CompositeInstructions. This is
useful for variational quantum algorithms whereby quantum circuit measurements are dictated
by a problem-specific operator or observable. This concept comes from the QCOR specification, and is
 here in XACC because it spans a number of projects. The ObservableTransform describes a mechanism for
the general transformation of Observables.

**optimizer**
--------------

This package puts forth the Optimizer interface, which takes general functors that are
parameterized and returns the optimal evaluation of that function at the
optimal set of parameters. This concept was put forth by the QCOR specification.

**service**
-----------

This package contains an implementation and API for exposing the core service registry
that XACC relies on. The ServiceRegistry delegates to the CppMicroServices framework and
loads available plugins (provided as standard shared libraries). It exposes the services
provided by these plugins via a templated getService() call which takes the type of the interface
and the string name it exposes.

**utils**
----------

This package contains various utility classes. The most important is the HeterogeneousMap, which
provides a mechanism for mapping string keys to any type. Other things in this package are
an expression parsing utility, the Identifiable and Persistable interface, command line parsing
utilities, and a logger.
