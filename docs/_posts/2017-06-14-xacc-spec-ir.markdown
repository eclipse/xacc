---
layout: post
title: XACC Intermediate Representation Specification
permalink: /xacc_spec/ir_spec
category: xacc_spec
---

# XACC Intermediate Representation

In order to promote interoperability and programmability across the 
wide range of available QPU types and quantum programming languages 
(embedded or stand-alone), there must be some common, standard 
low-level program representation that is simple to understand 
and manipulate. An example of this in the classical computing 
world is the LLVM - a compiler infrastructure that maps classical 
languages to an intermediate representation (IR) that can be used 
to perform hardware dependent and independent optimizations, and 
generate native, hardware-specific executable code. It is this IR 
that enables efficient classical language and hardware interoperability. 
Similarly for quantum computing, with the variety of available QPU 
types (superconducting, ion trap, adiabatic, etc.) and quantum 
programming languages (Scaffold, QCL, Quipper, pyQuil, etc.) there 
is a strong need for some standard low-level intermediate representation 
that serves as the glue between languages and hardware (see Figure 1). A 
standard IR would enable a wide range of quantum programming tools, and 
provide early users the benefit of programming their domain-specific 
algorithms in a manner that best suits their research and application. 
It will enable the execution of those programmed algorithms on a number 
of differing hardware types. 

![Figure 1: Diagram describing Quantum Accelerator Intermediate Representation (IR). This IR enables the XACC Specification standard's language and hardware interoperability.](https://github.com/ORNL-QCI/xacc/blob/master/docs/assets/acc_ir.png)

The XACC Specification defines a novel intermediate representation 
that promotes the overall integration of existing programming 
techniques and hardware realizations. The IR specification provides 
four main forms for use by clients: (1) an in-memory representation 
and API, (2) an on-disk persisted representation, (3) human-readable 
quantum assembly representation, and (4) a control flow graph or 
quantum circuit representation. This specification enables efficient 
analysis and isomorphic transformation of quantum kernel code, 
and provides a common representation for executing code written 
in any quantum language on any available quantum hardware.

The interface specification for the IR is shown in Figure 2. The 
foundation of the XACC IR specification is the Instruction interface, 
which abstracts the concept of an executable instruction for an 
attached Accelerator. Instructions have a unique name and reference 
to the Accelerator Bits that they operate on. Instructions can 
operate on one or many Accelerator Bits. Instructions can be in 
an enabled or disabled state to aid in the definition of conditional branching. 

![Figure 2](https://github.com/ORNL-QCI/xacc/blob/master/docs/assets/acc_ir_spec.png)

To provide a mechanism for expressing kernels of instructions, the 
specification defines a Function interface. The Function interface 
is a derivation of the Instruction interface that itself contains 
Instructions. Therefore, the Instruction/Function combination models 
the familiar tree, or composite, design pattern. In the XACC 
specification, kernels to be executed on an attached accelerator 
are modeled as an n-ary tree, with Function references as nodes, 
and Instruction references a leaves. 

The specification defines a container for Functions - the XACC IR 
interface. This interface provides an abstraction to model a list 
of compiled Functions, with the ability to map those Functions to 
both an assembly-like, human-readable string, and a control flow 
graph (CFG) data structure. For the case of gate model quantum 
acceleration, the CFG models the quantum circuit and provides a 
convenient data structure for isomorphic transformations and analysis. 
For adiabatic quantum computing, the control flow graph structure 
itself can be leveraged to model the Ising Hamiltonian parameters 
that form the machine-level instruction for the quantum instruction.

The structures just described give us three of the four forms that 
the XACC specification's intermediate representation provides - the 
in-memory data structure representation and API, the human-readable 
assembly representation, and the CFG/quantum circuit representation. 
To provide an on-disk representation, the IR interface exposes persist 
and load methods that take a file path to read in, and to write to, 
respectively. In this way, IR instances that are generated from a 
given set of kernels can be persisted and reused, enabling faster 
just-in-time compilation. 
