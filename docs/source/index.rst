.. XACC documentation master file, created by
   sphinx-quickstart on Tue Aug 29 20:23:35 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. image:: ../assets/xacc_full_logo.svg

\
\


XACC (pronounced as it's spelled) is an extensible compilation framework for hybrid quantum-classical
computing architectures. It provides extensible language frontend and hardware
backend compilation components glued together via a novel, polymorphic quantum intermediate
representation. XACC currently supports quantum-classical programming and enables
the execution of quantum kernels on IBM, Rigetti, IonQ, and D-Wave QPUs, as well as a
number of quantum computer simulators.

The XACC programming model follows the traditional co-processor model, akin to OpenCL or CUDA for GPUs, but
takes into account the subtleties and complexities inherent to the interplay
between classical and quantum hardware. XACC provides a high-level API that
enables classical applications to offload work
(represented as quantum kernels) to an attached quantum accelerator in a
manner that is independent to the quantum programming language and hardware.
This enables one to write quantum code once, and perform benchmarking,
verification and validation, and performance studies for a set of virtual
(simulators) or physical hardware.

Modular Infrastructure
----------------------
XACC relies on a project called `CppMicroServices <http://github.com/cppmicroservices/cppmicroservices>`_ -
a native C++ implementation of the `OSGi <https://www.osgi.org/developer/architecture/>`_
specification that enables an extensible, modular
plugin infrastructure for quantum compilers and accelerators.

Description of Architecture
---------------------------
For a comprehensive discussion of all components of the XACC programming model and architecture,
please refer to this `manuscript <https://arxiv.org/abs/1911.02452>`_.

For class documentation, check out this `site <https://ornl-qci.github.io/xacc-api-docs/>`_.

XACC Development Team
----------------------

XACC is developed and maintained by:

* `Alex McCaskey <mccaskeyaj@ornl.gov>`_
* `Travis Humble <humblets@ornl.gov>`_
* `Eugene Dumitrescu <dumitrescuef@ornl.gov>`_
* `Dmitry Liakh <liakhdi@ornl.gov>`_
* `Mengsu Chen <mschen@vt.edu>`_
* `Zach Parks <parkszp@ornl.gov>`_
* `Ryan Sand <rsand3642@gmail.com>`_
* `Charles Zhao <czhao39@gmail.com>`_
* `Jay Billings <billingsjj@ornl.gov>`_
* `Thien Nguyen <nguyentm@ornl.gov>`_ 
* `Daniel Chaves-Claudino <claudinodc@ornl.gov>`_
* `Tyler Kharazi <kharazitd@ornl.gov>`_ 
* `Anthony Santana <santanaam@ornl.gov>`_

Questions, Bug Reporting, and Issue Tracking
---------------------------------------------

Questions, bug reporting and issue tracking are provided by GitHub. Please
report all bugs by creating a `new issue <https://github.com/eclipse/xacc/issues/new>`_.
You can ask questions by creating a new issue with the question tag.

.. toctree::
   :maxdepth: 4
   :caption: Contents:

   install
   basics
   extensions
   advanced
   developers
   tutorials

Publications
------------
The following publications describe XACC or experiments leveraging the it.

[1] `XACC: A System-Level Software Infrastructure for Heterogeneous Quantum-Classical Computing <https://arxiv.org/abs/1911.02452>`_

[2] `A language and hardware independent approach to quantum-classical computing <https://www.sciencedirect.com/science/article/pii/S2352711018300700>`_

[3] `Validating Quantum-Classical Programming Models with Tensor Network Simulations <https://arxiv.org/abs/1807.07914>`_

[4] `Hybrid Programming for Near-term Quantum Computing Systems <https://arxiv.org/abs/1805.09279>`_

[5] `Cloud Quantum Computing of an Atomic Nucleus <https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.120.210501>`_

[6] `Quantum-Classical Computations of Schwinger Model Dynamics using Quantum Computers <https://journals.aps.org/pra/abstract/10.1103/PhysRevA.98.032331>`_


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
