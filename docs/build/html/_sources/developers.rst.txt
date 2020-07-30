Developers
==========
Here we describe how XACC developers can extend the framework
with new Compilers, Accelerators, Instructions, IR Transformations, etc.
This can be done from both C++ and Python.

Quick Start with Docker
-----------------------
We have put together a docker image based on Ubuntu 18.04 that has all required
dependencies for building XACC. Moreover, we have set this image up to serve an
Eclipse Theia IDE on ``localhost:3000``. To use this image run the following from some
scratch development directory:

.. code:: bash

   $ docker run --security-opt seccomp=unconfined --init -it -p 3000:3000 xacc/xacc

Now navigate to ``localhost:3000`` in your web browser. This will open
the Theia IDE and you are good to go. Open a terminal with ``ctrl + ```.

Writing a Plugin in C++
-----------------------
Let's demonstrate how one might add a new IR Transformation
implementation to XACC. This is a simple case, but the overall structure
works across most plugins.

First, we create a new project folder ``test_ir_transformation`` and
populate it with a ``CMakeLists.txt`` file, and a ``src`` folder containing another
``CMakeLists.txt`` file as well as ``manifest.json``, ``test_ir_transformation.hpp``,
``test_ir_transformation.cpp``, and ``test_ir_transformation_activator.cpp``. You should have
the following directory structure

::

    test_ir_transformation
    ├── CMakeLists.txt
    ├── src
        ├── CMakeLists.txt
        └── manifest.json
        └── test_ir_transformation.hpp
        └── test_ir_transformation.cpp
        └── test_ir_transformation_activator.cpp

In the top-level ``CMakeLists.txt`` we add the following:

.. code:: bash

   project(test_ir_transformation CXX)
   cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
   find_package(XACC REQUIRED)
   add_subdirectory(src)

Basically here we are defining a CMake project, setting the minimum version, locating our XACC install, and
adding the ``src`` directory to the build.

In the ``src/CMakeLists.txt`` file, we add the following

.. code:: bash

   set(LIBRARY_NAME test-ir-transformation)
   file(GLOB SRC *.cpp)
   usfunctiongetresourcesource(TARGET
                               ${LIBRARY_NAME}
                               OUT
                               SRC)
   usfunctiongeneratebundleinit(TARGET
                               ${LIBRARY_NAME}
                               OUT
                               SRC)
   add_library(${LIBRARY_NAME} SHARED ${SRC})
   target_link_libraries(${LIBRARY_NAME} PRIVATE xacc::xacc)
   set(_bundle_name test_ir_transformation)
   set_target_properties(${LIBRARY_NAME}
                       PROPERTIES COMPILE_DEFINITIONS
                                   US_BUNDLE_NAME=${_bundle_name}
                                   US_BUNDLE_NAME
                                   ${_bundle_name})
   usfunctionembedresources(TARGET
                           ${LIBRARY_NAME}
                           WORKING_DIRECTORY
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           FILES
                           manifest.json)
   xacc_configure_plugin_rpath(${LIBRARY_NAME})
   install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)

Here we define the library name, collect all source files, run some
CppMicroServices functions that append extra information to our library,
build the library and link in all required XACC libraries. Next we add
more information to this shared library from the ``manifest.json`` file,
configure the libraries RPATH, and install to the correct
``plugins`` folder in XACC. ``manifest.json`` should contain the following json

.. code:: bash

   {
     "bundle.symbolic_name" : "test_ir_transformation",
     "bundle.activator" : true,
     "bundle.name" : "Test IR Transformation",
     "bundle.description" : ""
   }

Next we provide the actual code for the test IR Transformation. In the ``test_ir_transformation.hpp``
we add the following

.. code:: cpp

   #pragma once
   #include "IRTransformation.hpp"

   using namespace xacc;

   namespace test {

   class Test : public IRTransformation {
   public:
     Test() {}
     void apply(std::shared_ptr<CompositeInstruction> program,
                        const std::shared_ptr<Accelerator> accelerator,
                        const HeterogeneousMap& options = {}) override;
     const IRTransformationType type() const override {return IRTransformationType::Optimization;}

     const std::string name() const override { return "test-irt"; }
     const std::string description() const override { return ""; }
   };
   }

and in ``test_ir_transformation.cpp`` we implement ``apply``

.. code:: cpp

   #include "test_ir_transformation.hpp"

   namespace test {

   void Test::apply(std::shared_ptr<CompositeInstruction> circuit,
                                const std::shared_ptr<Accelerator> accelerator,
                                const HeterogeneousMap &options) {

     // do transformation on circuit here...
   }
   }

Finally, we add a ``BundleActivator`` that creates a ``shared_ptr`` to our
IR Transformation and registers it with the CppMicroServices framework.

.. code:: cpp

   #include "test_ir_transformation.hpp"

   #include "cppmicroservices/BundleActivator.h"
   #include "cppmicroservices/BundleContext.h"
   #include "cppmicroservices/ServiceProperties.h"

   #include <memory>

   using namespace cppmicroservices;

   namespace {

   class US_ABI_LOCAL TestIRTransformationActivator: public BundleActivator {

   public:

	   TestIRTransformationActivator() {
	   }
	   void Start(BundleContext context) {
		   auto t = std::make_shared<test::Test>();
           context.RegisterService<xacc::IRTransformation>(t);
	   }
	   void Stop(BundleContext /*context*/) {
	   }
   };

   }

   CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(TestIRTransformationActivator)

The majority of this is standard CppMicroservices boilerplate code. The crucial bit that
requires your attention when developing a new plugin is the implementation of ``Start``.
Here you create a ``shared_ptr`` to your instances and register it against the
correct XACC interface type, here ``IRTransformation``.

Now, all that is left to do is build your shared library, and install it for use
in the XACC framework

.. code:: bash

   $ cd test_ir_transformation && mkdir build && cd build
   $ cmake .. -DXACC_DIR=~/.xacc
   $ make install


Writing a Plugin in Python
--------------------------
For this example, let's wrap a Qiskit transpiler pass with an XACC
``IRTransformation`` to demonstrate how one might integrate novel tools from
vendor frameworks with XACC. This will require creating a new Python class in a
standalone python file that extends the core C++ ``IRTransformation`` interface.
Note that this can be done for other interfaces as well, including ``Accelerator``,
``Observable``, ``Optimizer``, etc.

First lets show the code to do this, and then we'll walk through it. We will wrap the simple
qiskit cx-cancellation pass (this is already in XACC from the ``circuit-optimizer`` ``IRTransformation``,
but this is for demonstration purposes). Create a python file named ``easy_qiskit_pass.py`` and add the following

.. code:: python

   import xacc
   from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
       Validate, Invalidate, Instantiate

   @ComponentFactory("easy_qiskit_pass_factory")
   @Provides("irtransformation")
   @Property("_irtransformation", "irtransformation", "qiskit-cx-cancellation")
   @Property("_name", "name", "qiskit-cx-cancellation")
   @Instantiate("easy_qiskit_pass_instance")
   class EasyQiskitIRTransformation(xacc.IRTransformation):
       def __init__(self):
           xacc.IRTransformation.__init__(self)

       def type(self):
           return xacc.IRTransformationType.Optimization

       def name(self):
           return 'qiskit-cx-cancellation'

       def apply(self, program, accelerator, options):
           # Import qiskit modules here so that users
           # who don't have qiskit can still use rest of xacc
           from qiskit import QuantumCircuit, transpile
           from qiskit.transpiler import PassManager
           from qiskit.transpiler.passes import CXCancellation

           # Map CompositeInstruction program to OpenQasm string
           openqasm_compiler = xacc.getCompiler('openqasm')
           src = openqasm_compiler.translate(program).replace('\\','')

           # Create a QuantumCircuit
           circuit = QuantumCircuit.from_qasm_str(src)

           # Create the PassManager and run the pass
           pass_manager = PassManager()
           pass_manager.append(CXCancellation())
           out_circuit = transpile(circuit, pass_manager=pass_manager)

           # Map the output to OpenQasm and map to XACC IR
           out_src = out_circuit.qasm()
           out_src = '__qpu__ void '+program.name()+'(qbit q) {\n'+out_src+"\n}"
           out_prog = openqasm_compiler.compile(out_src, accelerator).getComposites()[0]

           # update the given program CompositeInstruction reference
           program.clear()
           for inst in out_prog.getInstructions():
               program.addInstruction(inst)

           return

This class subclasses the Pybind11-exposed C++ ``IRTransformation`` interface, and provides
implementations in python of its pertinent methods - a constructor, ``type()``, ``name()``, and
``apply()``. The constructor must invoke the superclass constructor. We implement ``type()`` to
indicate that this is an ``IRTransformation`` that is of type ``Optimization``. Crucially important is the
``name()`` method, you must implement this to contribute the unique name of this ``IRTransformation``.
This name will be how users get reference to this ``IRTransformation`` implementation. And finally, you
must implement the primary method for ``IRTransformation``, ``apply``. This is where the actual
transformation (optimization) is performed.

To insure that users can leverage the XACC framework Python API without qiskit installed, we have
to place our imports in the ``apply`` method so that they are not imported at framework initialization.
The rest of the ``apply`` code takes the XACC ``CompositeInstruction`` (``program``) and converts it
to an OpenQasm string with the appropriate ``openqasm`` ``Compiler`` implementation. From this we can construct
a Qiskit ``QuantumCircuit`` and pass this to the ``transpile`` command orchestrating the execution of the
``CXCancellation`` pass. Now we get the optimized circuit back out and map back to XACC IR and update the
provided ``program`` instance.

In order to contribute this ``IRTransformation`` to XACC as a plugin, we rely on the IPOPO project. To expose
this class as a plugin, we annotate it with the demonstrated class decorators, indicating what it provides and its
unique name. These lines are basic boilerplate, update them for your specific plugin contribution.

If this file is installed to the ``py-plugins`` directory of your XACC install, then when someone runs ``import xacc``,
this plugin will be loaded and contributed to the core C++ XACC plugin registry, and users can query it like any other
service.

.. code:: python

   import xacc

   qpu = xacc.getAccelerator('aer')
   qbits = xacc.qalloc(2)

   # Create a bell state program with too many cnots
   xacc.qasm('''
   .compiler xasm
   .circuit foo
   .qbit q
   H(q[0]);
   CX(q[0], q[1]);
   CX(q[0], q[1]);
   CX(q[0], q[1]);
   Measure(q[0]);
   Measure(q[1]);
   ''')
   f = xacc.getCompiled('foo')

   # Run the python contributed IRTransformation that uses qiskit
   optimizer = xacc.getIRTransformation('qiskit-cx-cancellation')
   optimizer.apply(f, None, {})

   # should have 4 instructions, not 6
   assert(4 == f.nInstructions())


Extending Accelerator for new Simulators
-----------------------------------------
Here we document how one might extend the ``Accelerator`` interface for
new simulators.