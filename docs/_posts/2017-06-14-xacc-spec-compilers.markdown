---
layout: post
title: XACC Compiler Specification
permalink: /xacc_spec/compiler_spec
category: xacc_spec
---

# XACC Compilers

To provide extensibility in the languages, the XACC Specification 
standard describes an interface for quantum programming language 
(QPL) compilers - simply called the Compiler interface (see FIGURE). 
This interface provides a compile method that takes as input the 
quantum code to be compiled - and that code can be written in any 
language the framework supports (i.e. has a Compiler implementation 
for the language). The main purpose of the compile method on the 
Compiler interface is to take in a snippet of quantum source code 
and produce a valid instance of the XACC Specification intermediate 
representation. Derived Compilers (subclasses of Compiler) are 
free to perform quantum compilation in any way they see fit, 
but must return a valid IR instance. 

Furthermore, Compilers can be provided with information on the 
targeted accelerator. This enables hardware-specific details to 
be present at compile time and thus influence the way compilation 
is performed. For example, quantum compilation often requires 
information about the qubit connectivity graph - XACC Specification 
and its compiler mechanism ensures this type of hardware-specific 
information is available at compile time. 

