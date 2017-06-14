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
stand-alone \emph{quantum kernels}. Figure \ref{fig:quantum_kernel} 
provides a graphical view of the requirements for a valid 
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
measurements are invoked in the kernel code, the AcceleratorBit states 
(see \ref{sec:memory}). To differentiate from classical functions, 
all quantum kernels in the XACC Specification must be annotated with 
the \emph{\_\_qpu\_\_} keyword. 

Quantum kernel implementations may be expressed in any available
quantum programming language. An \emph{available} quantum programming 
language is one such that the XACC implementation provides a valid 
Compiler implementation for the language (see \ref{sec:compilers}). 
Finally, quantum kernels may take any number of kernel arguments 
that drive the overall execution of the quantum code. 

\begin{figure*}[htp]
\includegraphics[width=\textwidth]{figures/quantum_kernel_def.png}
\centering
\caption{XACC Specification requirements for quantum kernels.}
\label{fig:quantum_kernel}
\end{figure*}

