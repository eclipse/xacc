/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
#ifndef QUANTUM_GATE_ACCELERATORS_SIMPLE_SIMPLEACCELERATORVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_SIMPLE_SIMPLEACCELERATORVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include <boost/math/constants/constants.hpp>
#include <Eigen/Core>

namespace xacc {
namespace quantum {

double sqrt2 = boost::math::constants::root_two<double>();
using ProductList = std::vector<Eigen::MatrixXcd>;

/**
 * The SimpleAcceleratorVisitor is an InstructionVisitor that visits
 * quantum gate instructions and creates an equivalent
 * SimpleAccelerator string that can be executed by the Rigetti
 * superconducting quantum computer.
 *
 */
class SimpleAcceleratorVisitor: public AllGateVisitor {

	std::shared_ptr<SimulatedQubits> qubits;

	Eigen::MatrixXcd I;
	Eigen::MatrixXcd h;
	Eigen::MatrixXcd x;
	Eigen::MatrixXcd y;
	Eigen::MatrixXcd z;
	Eigen::MatrixXcd p0;
	Eigen::MatrixXcd p1;

public:

	SimpleAcceleratorVisitor(std::shared_ptr<SimulatedQubits> qbits) :
			qubits(qbits), I(Eigen::MatrixXcd(2, 2)), h(Eigen::MatrixXcd(2, 2)), x(
					Eigen::MatrixXcd(2, 2)), y(Eigen::MatrixXcd(2, 2)), z(
					Eigen::MatrixXcd(2, 2)), p0(Eigen::MatrixXcd(2,2)), p1(Eigen::MatrixXcd(2,2)) {

		std::complex<double> i(0,1);
		I << 1, 0, 0, 1;
		h << 1.0/sqrt2, 1.0/sqrt2, 1.0/sqrt2, -1.0/sqrt2;
		x << 0, 1, 1, 0;
		y << 0, -i, i, 0;
		z << 1, 0, 0, -1;
		p0 << 1, 0, 0, 0;
		p1 << 0, 0, 0, 1;

	}

	/**
	 * Visit hadamard gates
	 */
	void visit(Hadamard& hGate) {
		auto actingQubits = hGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {productList.push_back(I);}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = h;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}
		apply(localU);
	}

	/**
	 * Visit CNOT gates
	 */
	void visit(CNOT& cn) {
		// If this is a 2 qubit gate, then we need t
		// to construct Kron(P0, I, ..., I) + Kron(P1, I, ..., Gate, ..., I)

		//std::cout << "Constructing CNOT\n";

		auto actingQubits = cn.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		productList.at(actingQubits[0]) = p0;
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}

			// Now create the second term in the sum
		productList.at(actingQubits[0]) = p1;
		productList.at(actingQubits[1]) = x;
		Eigen::MatrixXcd temp = productList.at(0);
		for (int i = 1; i < productList.size(); i++) {
			temp = Eigen::kroneckerProduct(temp, productList.at(i)).eval();
		}
		// Sum them up
		localU = localU + temp;
		apply(localU);
	}

	/**
	 * Visit X gates
	 */
	void visit(X& xGate) {
		auto actingQubits = xGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = x;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}
		apply(localU);
	}

	/**
	 *
	 */
	void visit(Y& yGate) {
		auto actingQubits = yGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = y;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}
		apply(localU);
	}

	/**
	 * Visit Z gates
	 */
	void visit(Z& zGate) {
		auto actingQubits = zGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = z;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}
		apply(localU);
	}

	/**
	 * Visit Measurement gates
	 */
	void visit(Measure& mGate) {
		//std::cout << "MEASURING.\n";
		auto measuredQbit = mGate.bits()[0];
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}

		Eigen::VectorXcd currentState = qubits->getState();
		currentState.normalize();

		auto rho = currentState * currentState.transpose();

		productList.at(measuredQbit) = p0;
		auto Pi0 = productList.at(0);
		for (int i = 1; i < productList.size(); i++) {
			Pi0 = Eigen::kroneckerProduct(Pi0, productList.at(i)).eval();
		}

		auto probZero = std::real((Pi0*rho).trace());

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
			productList.at(measuredQbit) = p1;
			auto Pi1 = productList.at(0);
			for (int i = 1; i < productList.size(); i++) {
				Pi1 = Eigen::kroneckerProduct(Pi1, productList.at(i)).eval();
			}
			qubits->applyUnitary(Pi1);
			qubits->normalize();
		}

		qubits->updateBit(measuredQbit, result);
	}

	/**
	 * Visit Conditional functions
	 */
	void visit(ConditionalFunction& c) {
		auto qubit = c.getConditionalQubit();
		auto bufResult = qubits->getAcceleratorBitState(qubit);
		if (bufResult == AcceleratorBitState::UNKNOWN) {
			XACCError("Conditional Node is conditional on unmeasured qubit.");
		}
		auto bufResultAsInt = (bufResult == AcceleratorBitState::ONE ? 1 : 0);
		c.evaluate(bufResultAsInt);
		XACCInfo("Measurement on " + std::to_string(qubit) + " was a " +
				std::to_string(bufResultAsInt));
	}

	void visit(Rx& rXGate) {
		//std::cout << "Constructing RX:\n";
		const std::complex<double> i(0, 1);
		double angle = boost::get<double>(rXGate.getParameter(0));

		auto mat11 = std::cos(angle / 2.0);
		auto mat22 = mat11;
		auto mat12 = -1.0 * i * std::sin(angle / 2.0);
		auto mat21 = mat12;

		Eigen::MatrixXcd rx(2,2);
		rx << mat11, mat12, mat21, mat22;

		//std::cout << rx << "\n\n";
		auto actingQubits = rXGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = rx;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}

		apply(localU);
	}

	void visit(Ry& rYGate) {
		//std::cout << "Constructing RY:\n";
		const std::complex<double> i(0, 1);
		double angle = boost::get<double>(rYGate.getParameter(0));

		auto mat11 = std::cos(angle / 2.0);
		auto mat22 = mat11;
		auto mat12 = -1.0 * std::sin(angle / 2.0);
		auto mat21 = -1.0 * mat12;

		Eigen::MatrixXcd ry(2,2);
		ry << mat11, mat12, mat21, mat22;
		//std::cout << ry << "\n\n";

		auto actingQubits = rYGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = ry;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}
		apply(localU);
	}

	void apply(Eigen::MatrixXcd& mat) {
		//std::cout << "Operating:\n" << mat << "\n on " << qubits->getState() << "\n";
		qubits->applyUnitary(mat);
		qubits->normalize();
	}

	void visit(Rz& rZGate) {
		//std::cout << "Constructing RZ:\n";

		const std::complex<double> i(0, 1);
		std::stringstream ss;
		ss << rZGate.getParameter(0);
		double angle = std::stod(ss.str());//boost::get<double>(rZGate.getParameter(0));
		auto matElement11 = std::exp(-1.0 * i * angle);
		auto matElement12 = std::exp(i * angle);

		Eigen::MatrixXcd rz(2,2);
		rz << matElement11, 0.0, 0.0, matElement12;

		//std::cout << rz << "\n\n";
		auto actingQubits = rZGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = rz;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}
		apply(localU);
	}

	void visit(CPhase& cpGate) {
		const std::complex<double> i(0, 1);
		double angle = boost::get<double>(cpGate.getParameter(0));
		auto matElement = std::exp(i * angle);
		Eigen::MatrixXcd rz(2,2);
		rz << 1, 0, 0, matElement;
		auto actingQubits = cpGate.bits();
		ProductList productList;
		for (int j = 0; j < qubits->size(); j++) {
			productList.push_back(I);
		}
		// If this is a one qubit gate, just replace
		// the currect I in the list with the gate
		productList.at(actingQubits[0]) = rz;
		// Create a total unitary for this layer of the circuit
		Eigen::MatrixXcd localU = productList.at(0);
		for (int j = 1; j < productList.size(); j++) {
			localU = Eigen::kroneckerProduct(localU, productList.at(j)).eval();
		}
		apply(localU);
	}

	void visit(Swap& s) {
		XACCError("Swap Gate not implemented yet.");
	}

	void visit(GateFunction& f) {
		return;
	}
	/**
	 * The destructor
	 */
	virtual ~SimpleAcceleratorVisitor() {}
};


}
}

#endif
