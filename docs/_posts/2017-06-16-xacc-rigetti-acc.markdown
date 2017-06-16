---
layout: post
title: XACC Rigetti Accelerator
permalink: /accelerators/rigetti
category: accelerators
---

## XACC and Rigetti Computing, Inc.

Rigetti Computing, Inc [(rigetti.com)](rigetti.com) is a recently founded startup that is focused on 
developing quantum computing hardware and software and bring it to market. 
They are working to build a cloud quantum computing platform for AI and 
computational chemistry. They currently have a 4 qubit superconducting QPU 
that can be accessed via a REST API with a private API key. They 
are also working hard to provide open source programming tools for 
interfacing with the QPU - specifically, the pyquil python framework
[PyQuil](https://github.com/rigetticomputing/pyquil). 

Recently, the ORNL QCI [(quantum.ornl.gov)](quantum.ornl.gov), the XACC project, 
and the Software and Applications Team from Rigetti have begun collaborating in 
an effort to expose the Rigetti QPU and programming tools to XACC and its user 
community. This article describes the results of that work - specifically, a 
new XACC Accelerator implementation that executes quantum kernels on the 
physical Rigetti QPU. For a tutorial on how to use XACC and the Rigetti 
Accelerator, check out [Rigetti Tutorial](https://ornl-qci.github.io/xacc/tutorials/rigetti).


## RigettiAccelerator

The RigettiAccelerator is an implementation or realization of the pluggable 
XACC Accelerator interface. The RigettAccelerator class architecture diagram is 
shown in Figure 1. The RigettiAccelerator's implementation of the Accelerator::execute method 
is charged with two primary tasks: (1) the translation of the XACC IR (found in the provided 
Function instance) to an equivalent Quil string, and (2) constructing and executing an 
appropriate HTTPS Post on the [(api.rigetti.com)](api.rigetti.com) server. The only remaining 
thing to do once those two tasks are complete is to processes the resultant response from the server. 

<figure>
<img src="/xacc/assets/rigetti-acc-arch.png" {% if include.alt %} alt="{{ include.alt }}" {% endif %} {% if include.width %} width="{{ include.width }}" {% endif %}/>
<figcaption>Figure 1: UML diagram describing XACC RigettiAccelerator architecture.</figcaption>
</figure>

# Mapping XACC IR to Quil

For a good explanation on the structure of the XACC Intermediate Representation 
check out [XACC IR](https://ornl-qci.github.io/xacc/xacc_spec/ir_spec). Basically, 
at its core, the XACC IR provides a tree-like, in-memory representation and API for a 
compiled quantum kernel. This leaves of this tree are XACC Instructions and the nodes 
of this tree are XACC Functions, which are composed of further child Instructions. The 
XACC Quantum IR implementation provides a number of standard gate Instruction implementations 
- Hadamard, CNOT, rotations, etc... These serve as the leaves of the IR tree. These 
instruction implementations know nothing of the Quil intermediate language and it would be tedious 
and a poor design decision to to update the entire XACC Quantum IR package (we would have to do the 
same for any and all current and future low-level languages). So XACC employs a common 
software engineering design pattern to enable this XACC IR to Quil mapping - the 
visitor pattern, which provides a mechansim for adding new operations to an 
existing object without modifying the design of that object ([Visitor Pattern](https://en.wikipedia.org/wiki/Visitor_pattern)). For each derived gate Instruction, a Visitor class implements a 
corresponding ```visit``` method (```visit(Hadamard& h)```, etc...). All gate instructions have the 
ability to accept an incoming Visitor, and upon doing so, invoke the ```visit``` method that 
corresponds to their type, thus giving the Visitor type information for the Gate Instruction. 
Therefore, mapping to Quil simply involves walking the IR tree, and telling each Instruction to 
accept the visitor: 

```cpp
InstructionIterator it(kernel);
while (it.hasNext()) {
   // Get the next node in the tree
   auto nextInst = it.next();
   if (nextInst->isEnabled()) nextInst->accept(visitor);
}
```

The visitor implementation is known as the QuilVisitor, and its visit methods look like this (Hadamard for example):

```cpp
void visit(Hadamard& h) {
   quilStr += "H " + std::to_string(h.bits()[0]) + "\n";
}
```
or for a more complicated gate Instruction:

```cpp
void visit(ConditionalFunction& c) {
   auto visitor = std::make_shared<QuilVisitor>();
   quilStr += "JUMP-UNLESS @" + c.getName() + " [" + std::to_string(c.getConditionalQubit()) + "]\n";
   for (auto inst : c.getInstructions()) {
      inst->accept(visitor);
   }
   quilStr += visitor->getQuilString();
   quilStr += "LABEL @" + c.getName() + "\n";
}

```

# Executing Quil code on Rigetti QVM

With the XACC IR mapped to Quil, the RigettiAccelerator is ready to execute 
on the Rigetti QVM. 

