/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *   Extension to VQE algorithm - 29 MAR 2017
 *
 **********************************************************************************/
#include "XACC.hpp"
#include "FireTensorAccelerator.hpp"

// Scaffold Kernel for Variational Quantum Eigensolver
const std::string src("__qpu__ vqe(qbit qreg, int theta) {\n"
        "   //Declare variables to store values\n"
	"   cbit creg[1];\n"
        "   \n"
        "   // Declare operator to minimize\n"
        "   // FIXME: This should be a string of Pauli operators\n"
        "   \n"
	"   // Initialize register using current paramterization \n"
        "   // FIXME: This should depend on theta \n" 
	"   X(qreg[0]);\n"
        "   \n"
        "   // Apply operator to minimize to current register state\n"
	"   H(qreg[1]);\n"
        "   \n"
        "   // Read out result from register\n"
	"   creg[0] = MeasZ(qreg[0]);\n"
	"}\n");

int main (int argc, char** argv) {

	// Create a convenient alias for our simulator...
	using CircuitSimulator = xacc::quantum::FireTensorAccelerator<6>;

	// Create a reference to the 6 qubit simulation Accelerator
	auto qpu = std::make_shared<CircuitSimulator>();

        // Loop over QPU's and allocate resources for each thread
	// Allocate 3 qubits, give them a unique identifier...
	auto qubitReg = qpu->createBuffer("qreg", 3);
	using QubitRegisterType = decltype(qubitReg);

	// Construct a new XACC Program
	xacc::Program quantumProgram(qpu, src);

	// Build the program using Scaffold comipler
	// and output the Graph Intermediate Representation
	quantumProgram.build("--compiler scaffold "
			"--writeIR vqe.xir");

	// Retrieve the created kernel. It takes a 
	// qubit register as input
	auto vqe = quantumProgram.getKernel<QubitRegisterType>("vqe");

        // Declare range of parameters to search for minimization of expectation value
        float theta = 0.0;
        float dtheta = 3.14159265353/10.0;
        for(int j = 0; theta < 10; j++ ){

          // Set value of theta
          theta = j*dtheta;

	  // Execute the kernel with the qubit register
          // Pass current initialization parameters
	  vqe(qubitReg, theta);

          // Compose current value from last measurement results
          // FIXME: Need to extract measurements values from QPU to compute
          float current_value = 0.;

          // Compare output of last instance with current best
          // FIXME: This is part of the brute force search method; need to try better methods
          if(current_minimum > current_value) {
            current_minimum = current_value;
          };

	  // Pretty print the resultant state
	  qubitReg->printBufferState(std::cout);

        }; // theta

        printf("Minimized value = %f at theta = %f\n", current_minimum, theta);

	return 0;

} // end VQE example

