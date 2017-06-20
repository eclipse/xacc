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
computational chemistry. They currently have a QVM simulation server 
that can be accessed via a REST API with a private API key. Rigetti has 
also done great work as of late in providing open source programming tools for 
interfacing with their QVM - specifically, the PyQuil python framework
[PyQuil](https://github.com/rigetticomputing/pyquil). 

Recently, the ORNL QCI [(https://quantum.ornl.gov)](quantum.ornl.gov), the XACC project, 
and the Software and Applications Team from Rigetti have begun collaborating in 
an effort to expose the Rigetti QVM server and programming tools to XACC and its user 
community. This article describes the results of that work - specifically, a 
new XACC Accelerator implementation that executes quantum kernels on the 
Rigetti QVM server. For a more hands-on tutorial on how to use XACC and the Rigetti 
Accelerator, check out [Rigetti Tutorial](https://ornl-qci.github.io/xacc/tutorials/rigetti).

## RigettiAccelerator

The RigettiAccelerator is an implementation or realization of the pluggable 
XACC Accelerator interface. The RigettAccelerator class architecture diagram is 
shown in Figure 1. The RigettiAccelerator's implementation of the Accelerator::execute() method 
is charged with two primary tasks: (1) the translation of the XACC IR to an equivalent 
Quil string, and (2) constructing and executing an 
appropriate HTTPS Post on the Rigetti QVM server. The only remaining 
thing to do once those two tasks are complete is to processes the resultant response from the server. 

<figure>
<img src="/xacc/assets/rigetti-acc-arch.png" {% if include.alt %} alt="{{ include.alt }}" {% endif %} {% if include.width %} width="{{ include.width }}" {% endif %}/>
<figcaption>Figure 1: UML diagram describing XACC RigettiAccelerator architecture.</figcaption>
</figure>

# Mapping XACC IR to Quil

For a good explanation on the structure of the XACC Intermediate Representation 
check out [XACC IR](https://ornl-qci.github.io/xacc/xacc_spec/ir_spec). Basically, 
at its core, the XACC IR provides a tree-like, in-memory representation and API for a 
compiled quantum kernel. The leaves of this tree are XACC Instructions and the nodes 
of the tree are XACC Functions, which are composed of further child Instructions. The 
XACC Quantum IR implementation provides a number of standard gate Instruction implementations 
(Hadamard, CNOT, rotations, etc...) These serve as the leaves of the IR tree. These 
instruction implementations know nothing of the Quil intermediate language and it would be tedious 
and a poor design decision to to update the entire XACC Quantum IR package (we would have to do the 
same for any and all current and future low-level languages). So XACC employs a common 
software engineering design pattern to enable this XACC IR to Quil mapping: the 
visitor pattern, which provides a mechansim for adding new operations to an 
existing object without modifying the design of that object ([Visitor Pattern](https://en.wikipedia.org/wiki/Visitor_pattern)). For each derived gate Instruction, a Visitor class implements a 
corresponding ```visit``` method (```visit(Hadamard& h)```, etc...). All gate instructions have the 
ability to accept an incoming Visitor, and upon doing so, invoke the ```visit``` method that 
corresponds to their type, thus giving the Visitor type information for the Gate Instruction. 
Therefore, mapping to Quil simply involves walking the IR tree, and telling each Instruction to 
accept the visitor: 

```cpp
auto visitor = std::make_shared<QuilVisitor>();
InstructionIterator it(kernel);
while (it.hasNext()) {
   // Get the next node in the tree
   auto nextInst = it.next();
   if (nextInst->isEnabled()) nextInst->accept(visitor);
}
auto quilStr = visitor->getQuilString();
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
After walking the IR tree, the Quil representation is produced with a call to getQuilString().


# Executing Quil code on Rigetti QVM

With the XACC IR mapped to Quil, the RigettiAccelerator is ready to execute 
on the Rigetti QVM. The main task here is to construct the proper JSON payload string
that contains information about the type of the execution, the classical memory address 
indices, and the Quil instructions string. The types of execution that the QVM allows are 
multishot, multishot-measure, wavefunction, and expectation. In this work, we have primarily focused 
on the multishot method. If the execution type is 
multishot, then we can provide a further JSON key that is an integer that gives the 
number of executions of the Quil code to run. 

The RigettiAccelerator leverages the HTTP support from the Fire Scientific Computing Framework 
[(fire)](https://github.com/jayjaybillings/fire) to execute an HTTPS Post from the users 
local machine to the Rigetti QVM server. Specifically, Fire provides an INetworkingTool interface 
(shown in Figure 1) that exposes simple post and get methods. For this work, we only need the post 
support. Under the hood, Fire provides a default implementation of the INetworkingTool called the 
AsioNetworkingTool. This implementation delegates all posts to the ASIO networking support found in the 
Boost C++ headers/libraries. 

Here is what the actual post looks like in code: 

```cpp
jsonStr += "{ \"type\" : \"" + type + "\", \"addresses\" : "
            + visitor->getClassicalAddresses()
            + ", \"quil-instructions\" : \"" + quilStr
            + (type == "wavefunction" ? "\"" : "\", \"trials\" : " + trials)
            + " }";

auto httpClient = std::make_shared<fire::util::AsioNetworkingTool>();
auto postResponse = httpClient->post("/qvm", jsonStr, headers);
```

# User Customization

The RigettiAccelerator exposes a number of command line options to the XACC framework, which 
are then displayed to the user. The following arguments can be passed to an XACC application 
using the RigettiAccelerator to influence its runtime behavior.

|    Argument          |        Command Line      |          Description                                                  |
| Execution Type       | --rigetti-type multishot | The execution type, multishot or wavefunction. |
| Trials               | --rigetti-trials 1000    | The number of executions, if this is a multishot execution.           |
| API Key              | --api-key KEY            | API Key if not in PYQUIL_CONFIG or ~/.pyquil_config |
