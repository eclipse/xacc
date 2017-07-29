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
 *
 **********************************************************************************/
#include "SimpleAccelerator.hpp"
#include <complex>

namespace xacc {
namespace quantum {

double sqrt2 = std::sqrt(2.0);
using ProductList = std::vector<fire::Tensor<2, fire::EigenProvider, std::complex<double>>>;
using ComplexTensor = fire::Tensor<2, fire::EigenProvider, std::complex<double>>;

std::shared_ptr<AcceleratorBuffer> SimpleAccelerator::createBuffer(
			const std::string& varId) {

	XACCError("NOT IMPLEMENTED YET");
}

std::shared_ptr<AcceleratorBuffer> SimpleAccelerator::createBuffer(
		const std::string& varId, const int size) {
	if (!isValidBufferSize(size)) {
		XACCError("Invalid buffer size.");
	}
	auto buffer = std::make_shared<SimulatedQubits>(varId, size);
	storeBuffer(varId, buffer);
	return buffer;
}

bool SimpleAccelerator::isValidBufferSize(const int NBits) {
	return NBits <= 10;
}

void SimpleAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
		const std::shared_ptr<xacc::Function> kernel) {

	// Cast to what we know should be SimulatedQubits
	auto qubits = std::static_pointer_cast<SimulatedQubits>(buffer);

	if (!qubits) {
		XACCError("Invalid derived AcceleratorBuffer passed to "
				"SimpleAccelerator. Must be of type SimulatedQubits.");
	}

	// Create a lambda for each type of gate we may encounter,
	// each lambda must perform a gate-specific unitary
	// on the qubits accelerator buffer.
	auto hadamard =
			[&] (Hadamard& hGate) {
				ComplexTensor h(2,2), I(2,2);
				I.setValues( { {1, 0}, {0, 1}});
				h.setValues(
						{	{	1.0 / sqrt2, 1.0 / sqrt2}, {1.0 / sqrt2, -1.0 / sqrt2}});
				auto actingQubits = hGate.bits();
				ProductList productList;
				for (int j = 0; j < qubits->size(); j++) {productList.push_back(I);}
				// If this is a one qubit gate, just replace
				// the currect I in the list with the gate
				productList.at(actingQubits[0]) = h;
				// Create a total unitary for this layer of the circuit
				ComplexTensor localU = productList.at(0);
				for (int j = 1; j < productList.size(); j++) {localU = localU.kronProd(productList.at(j));}
				qubits->applyUnitary(localU);
			};

	auto cnot = [&](CNOT& cn) {
		// If this is a 2 qubit gate, then we need t
		// to construct Kron(P0, I, ..., I) + Kron(P1, I, ..., Gate, ..., I)
		ComplexTensor x(2,2), I(2,2), p0(2,2), p1(2,2);
		x.setValues( { { 0, 1 }, { 1, 0 } });
		I.setValues( { { 1, 0 }, { 0, 1 } });
		p0.setValues( { { 1, 0 }, { 0, 0 } });
		p1.setValues( { { 0, 0 }, { 0, 1 } });

		auto actingQubits = cn.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		productList.at(actingQubits[0]) = p0;
		ComplexTensor localU = productList.at(0);
		for (int i = 1; i < productList.size(); i++) {
			localU = localU.kronProd(productList.at(i));
		}

			// Now create the second term in the sum
		productList.at(actingQubits[0]) = p1;
		productList.at(actingQubits[1]) = x;
		auto temp = productList.at(0);
		for (int i = 1; i < productList.size(); i++) {
			temp = temp.kronProd(productList.at(i));
		}
		// Sum them up
		localU = localU + temp;
		qubits->applyUnitary(localU);
	};

	auto x = [&] (X& xGate) {
		ComplexTensor x(2,2), I(2,2);
		I.setValues( { { 1, 0 }, { 0, 1 } });
		x.setValues( { { 0, 1 }, { 1, 0 } });
		auto actingQubits = xGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = x;
		// Create a total unitary for this layer of the circuit
		ComplexTensor localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = localU.kronProd(productList.at(j));
		}
		qubits->applyUnitary(localU);
	};

	auto y = [&] (Y& yGate) {
		ComplexTensor y(2,2), I(2,2);
		I.setValues( { { 1, 0 }, { 0, 1 } });
		y.setValues( { { 0, 1 }, { 1, 0 } });
		auto actingQubits = yGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = y;
		// Create a total unitary for this layer of the circuit
		ComplexTensor localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = localU.kronProd(productList.at(j));
		}
		qubits->applyUnitary(localU);
	};


	auto z = [&] (Z& zGate) {
		ComplexTensor z(2,2), I(2,2);
		I.setValues( { { 1, 0 }, { 0, 1 } });
		z.setValues( { { 1, 0 }, { 0, -1 } });

		auto actingQubits = zGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = z;
		// Create a total unitary for this layer of the circuit
		ComplexTensor localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = localU.kronProd(productList.at(j));
		}
		qubits->applyUnitary(localU);
	};

	auto measure = [&](Measure& mGate) {
		ComplexTensor I(2,2), p0(2,2), p1(2,2);
		I.setValues( { {1, 0}, {0, 1}});
		p0.setValues( { { 1, 0 }, { 0, 0 } });
		p1.setValues( { { 0, 0 }, { 0, 1 } });
		auto actingQubits = mGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}

		auto rho = qubits->getState() * qubits->getState();

		productList.at(actingQubits[0]) = p0;
		auto Pi0 = productList.at(0);
		for (int i = 1; i < productList.size(); i++) {
			Pi0 = Pi0.kronProd(productList.at(i));
		}

		double probZero = 0.0;
		std::array<IndexPair, 1> contractionIndices;
		contractionIndices[0] = std::make_pair(1, 0);
		auto Prob0 = Pi0.contract(rho, contractionIndices);
		for (int i = 0; i < Prob0.dimension(0); i++)
		probZero += std::real(Prob0(i, i));

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(0, 1.0);
		int result;
		auto val = dist(mt);
		if (val < std::real(probZero)) {
			result = 0;
			qubits->applyUnitary(Pi0);
			qubits->normalize();
		} else {
			result = 1;
			productList.at(actingQubits[0]) = p1;
			auto Pi1 = productList.at(0);
			for (int i = 1; i < productList.size(); i++) {
				Pi1 = Pi1.kronProd(productList.at(i));
			}
			qubits->applyUnitary(Pi1);
			qubits->normalize();
		}

		qubits->updateBit(actingQubits[0], result);
	};

	auto cond = [&](ConditionalFunction& c) {
		auto qubit = c.getConditionalQubit();
		auto bufResult = qubits->getAcceleratorBitState(qubit);
		if (bufResult == AcceleratorBitState::UNKNOWN) {
			XACCError("Conditional Node is conditional on unmeasured qubit.");
		}
		auto bufResultAsInt = bufResult == AcceleratorBitState::ONE ? 1 : 0;
		c.evaluate(bufResultAsInt);
		XACCInfo("Measurement on " + std::to_string(qubit) + " was a " +
				std::to_string(bufResultAsInt));
	};

	auto rx = [&] (Rx& rXGate) {
		const std::complex<double> i(0, 1);
		double angle = boost::get<double>(rXGate.getParameter(0));

		auto mat11 = std::cos(angle / 2.0);
		auto mat22 = mat11;
		auto mat12 = -1.0 * i * std::sin(angle / 2.0);
		auto mat21 = mat12;

		ComplexTensor rx(2,2), I(2,2);
		I.setValues( { {1, 0}, {0, 1}});
		rx.setValues( { {mat11, mat12}, {mat21, mat22}});

		auto actingQubits = rXGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = rx;
		// Create a total unitary for this layer of the circuit
		ComplexTensor localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = localU.kronProd(productList.at(j));
		}
		qubits->applyUnitary(localU);
	};

	auto ry = [&] (Ry& rYGate) {
		const std::complex<double> i(0, 1);
		double angle = boost::get<double>(rYGate.getParameter(0));

		auto mat11 = std::cos(angle / 2.0);
		auto mat22 = mat11;
		auto mat12 = -1.0 * std::sin(angle / 2.0);
		auto mat21 = -1.0 * mat12;

		ComplexTensor ry(2,2), I(2,2);
		I.setValues( { {1, 0}, {0, 1}});
		ry.setValues( { {mat11, mat12}, {mat21, mat22}});

		auto actingQubits = rYGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = ry;
		// Create a total unitary for this layer of the circuit
		ComplexTensor localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = localU.kronProd(productList.at(j));
		}
		qubits->applyUnitary(localU);
	};

	auto rz = [&] (Rz& rZGate) {
		const std::complex<double> i(0, 1);
		double angle = boost::get<double>(rZGate.getParameter(0));
		auto matElement11 = std::exp(-1.0 * i * angle);
		auto matElement12 = std::exp(i * angle);

		ComplexTensor rz(2,2), I(2,2);
		I.setValues( { {1, 0}, {0, 1}});
		rz.setValues( { {matElement11, 0.0}, {0.0, matElement12}});

		auto actingQubits = rZGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = rz;
		// Create a total unitary for this layer of the circuit
		ComplexTensor localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = localU.kronProd(productList.at(j));
		}
		qubits->applyUnitary(localU);
	};

	auto cphase = [&] (CPhase& cpGate) {
		const std::complex<double> i(0, 1);
		double angle = boost::get<double>(cpGate.getParameter(0));
		auto matElement = std::exp(i * angle);
		ComplexTensor rz(2,2), I(2,2);
		I.setValues( { {1, 0}, {0, 1}});
		rz.setValues( { {1, 0}, {0, matElement}});
		auto actingQubits = cpGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = rz;
		// Create a total unitary for this layer of the circuit
		ComplexTensor localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = localU.kronProd(productList.at(j));
		}
		qubits->applyUnitary(localU);
	};

	auto swap = [&] (Swap& swapGate) {
		XACCError("Swap Gate not implemented yet.");
	};

	// Create a Visitor that will execute our lambdas when
	// we encounter one
	auto visitor = std::make_shared<FunctionalGateInstructionVisitor>(hadamard,
			cnot, x, y, z, rx, ry, rz, measure, cond, cphase, swap);

	XACCInfo("Execution Simple Accelerator Simulation.");

	// Our QIR is really a tree structure
	// so create a pre-order tree traversal
	// InstructionIterator to walk it
	InstructionIterator it(kernel);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();

		// If enabled, invoke the accept
		// method which kicks off the visitor
		// to execute the appropriate lambda.
		if (nextInst->isEnabled()) {
			nextInst->accept(visitor);
		}
	}

//		qubits->getAcceleratorBitState()

}

}
}

