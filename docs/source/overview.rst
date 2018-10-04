Getting started with XACC
=========================

Overview
---------

XACC is a programming framework for extreme-scale accelerator architectures that integrates alongside existing conventional classical applications. It is a pluggable framework for programming languages and accelerators developed for next-gen computing hardware architectures like quantum and neuromorphic computing. It lets computational scientists efficiently off-load classically intractable work to attached accelerators through a user-friendly kernel API. XACC makes post-exascale hybrid programming approachable for domain computational scientists.

The XACC programming model, and associated open-source reference implementation, follows the traditional co-processor model, akin to OpenCL or CUDA for GPUs, but takes into account the subtleties and complexities inherent to the interplay between classical and quantum hardware. XACC provides a high-level API that enables classical applications to offload computationally intractable work (represented as quantum kernels) to an attached quantum accelerator in a manner that is agnostic to the quantum programming language and the quantum hardware. This enables one to write quantum code once, and perform benchmarking, verification and validation, and performance studies for a set of virtual (simulators) or physical hardware.

To achieve this interoperability, XACC defines four primary abstractions or concepts: quantum kernels, intermediate representation, compilers, and accelerators. Quantum kernels are C-like functions that contain code intended for execution on the QPU. These kernels are compiled to the XACC intermediate representation (IR), an object model that is key for promoting the integration of a diverse set of languages and hardware. The IR provides four main forms for use by algorithm programmers: (1) an in-memory representation and API, (2) an on-disk persisted representation, (3) human-readable quantum assembly representation, and (4) a control flow graph or quantum circuit representation. This IR is produced by realizations of the XACC compiler concept, which delegates to the kernel language’s appropriate parser, compiler, and optimizer. Finally, XACC IR instances (and therefore programmed kernels) are executed by realizations of the Accelerator concept, which defines an interface for injecting physical or virtual quantum accelerators. Accelerators take this IR as input and delegate execution to vendor-supplied APIs for the QPU (or API for a simulator). The orchestration of these concepts enable an expressive API for quantum acceleration of classical applications.

XACC has support for a number of languages and physical and virtual hardware instances. XACC provides a Compiler realization that enables quantum kernel programming in the Scaffold programming language - an effort that came out of the IARPA QCS program. This compiler leverages the Clang/LLVM library extensions developed under that project that extend the LLVM IR with quantum gate operations. XACC extends this compiler with support for new constructs, like custom quantum functions and source-to-source translations (mapping Scaffold to other languages). XACC provides an Accelerator realization that enables execution of quantum kernels in any available language for both the Rigetti Quantum Virtual Machine (QVM, Forest API) and the physical two qubit (pyquillow) Rigetti QPU. These Accelerators map the XACC IR to Quil (the Rigetti low-level assembly language) and leverage an HTTP Rest client to post compiled quantum kernel code to the Rigetti QVM/QPU driver servers. XACC also has support for the D-Wave QPU, which demonstrates the wide applicability of this heterogeneous hybrid programming model across quantum computing models. XACC has Compiler and Accelerator realizations that enable minor graph embedding of binary optimization problems and execution on the D-Wave Qubist QPU driver server, respectively.

XACC Modular Infrastructure
---------------------------
XACC relies on a project called `CppMicroServices <http://github.com/cppmicroservices/cppmicroservices>`_ - a native C++ implementation of the OSGi specification that enables an extensible plugin infrastructure for compilers and accelerators. As such, installation of XACC provides the core infrastructure for describing Programs, Compilers, Accelerators, and IR. To enable support for various compilers and accelerators (like the Scaffold or Quil compilers, or the IBM or Rigetti QPUs) you must install the appropriate plugin (see `XACC Plugins <plugins.html>`_).

XACC Development Team
----------------------

XACC is developed and maintained by:

* `Alex McCaskey <mccaskeyaj@ornl.gov>`_
* `Travis Humble <humblets@ornl.gov>`_
* `Eugene Dumitrescu <dumitrescuef@ornl.gov>`_
* `Dmitry Liakh <liakhdi@ornl.gov>`_
* `Mengsu Chen <mschen@vt.edu>`_
* `Zach Parks <parkszp@ornl.gov>`_

Questions, Bug Reporting, and Issue Tracking
---------------------------------------------

Questions, bug reporting and issue tracking are provided by GitHub. Please
report all bugs by creating a new issue. You can ask questions by creating a
new issue with the question tag.

Publications and Presentations
-------------------------------

`A language and hardware independent approach to quantum-classical computing <https://www.sciencedirect.com/science/article/pii/S2352711018300700>`_

`Cloud Quantum Computing of an Atomic Nucleus <https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.120.210501>`_

`Quantum-Classical Computations of Schwinger Model Dynamics using Quantum Computers <https://journals.aps.org/pra/abstract/10.1103/PhysRevA.98.032331>`_

`Validating Quantum-Classical Programming Models with Tensor Network Simulations <https://arxiv.org/abs/1807.07914>`_

`Hybrid Programming for Near-term Quantum Computing Systems <https://arxiv.org/abs/1805.09279>`_
