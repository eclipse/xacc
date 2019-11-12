Developers
==========
Here we describe how XACC developers can extend the framework
with new Compilers, Accelerators, Instructions, IR Transformations, etc.
This can be done from both C++ and Python.

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

    project
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

