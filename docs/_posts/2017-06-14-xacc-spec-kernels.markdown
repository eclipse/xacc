---
layout: post
title: XACC Kernel Specification
permalink: /xacc_spec/kernel_spec
category: xacc_spec
---

# XACC Kernels

The XACC Specification specification requires that clients express 
code intended for quantum acceleration in a manner similar to 
CUDA or OpenCL for GPU acceleration: code must be expressed as 
stand-alone quantum kernels. Figure 1 provides a graphical view of the requirements for a valid 
quantum kernel in the XACC Specification. A quantum kernel 
is represented by a classical function in the host language 
for the XACC implementation. This function must take as its 
first argument the AcceleratorBuffer instance representing the 
qubit register this kernel operates on. A quantum kernel, therefore, 
is a programmatic representation of a unitary operation on a 
register of qubits (a representation of a quantum circuit). Furthermore, 
in the XACC Specification quantum kernels do not specify a return type; 
all information about the results of a quantum kernel's operation are 
gathered from the AcceleratorBuffer's measurement mechanism, or, if 
measurements are invoked in the kernel code, the AcceleratorBit states. 
To differentiate from classical functions, 
all quantum kernels in the XACC Specification must be annotated with 
the \_\_qpu\_\_ keyword. 

Quantum kernel implementations may be expressed in any available
quantum programming language. An \emph{available} quantum programming 
language is one such that the XACC implementation provides a valid 
Compiler implementation for the language (see \ref{sec:compilers}). 
Finally, quantum kernels may take any number of kernel arguments 
that drive the overall execution of the quantum code. 

<figure>
<img src="/xacc/assets/quantum_kernel_def.png" {% if include.alt %} alt="{{ include.alt }}" {% endif %} {% if include.width %} width="{{ include.width }}" {% endif %}/>
<figcaption>Figure 1: XACC Specification requirements for quantum kernels.</figcaption>
</figure>
