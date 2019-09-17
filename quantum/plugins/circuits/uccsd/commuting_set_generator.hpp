/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef VQE_TRANSFORMATION_COMMUTINGSETGENERATOR_HPP_
#define VQE_TRANSFORMATION_COMMUTINGSETGENERATOR_HPP_

#include "PauliOperator.hpp"
#include <Eigen/Core>
#include <numeric>

using namespace xacc::quantum;

namespace xacc {

namespace vqe {
class CommutingSetGenerator {

private:

	std::pair<Eigen::VectorXi, Eigen::VectorXi> bv(Term& op, int nQubits) {
		Eigen::VectorXi vx = Eigen::VectorXi::Zero(nQubits);
		Eigen::VectorXi vz = Eigen::VectorXi::Zero(nQubits);

		for (auto term : op.ops()) {
			if (term.second == "X") {
				vx(term.first) += 1;
			} else if (term.second == "Z") {
				vz(term.first) += 1;
			} else if (term.second == "Y") {
				vx(term.first) += 1;
				vz(term.first) += 1;
			}
		}

		return std::make_pair(vx, vz);
	};

	int bv_commutator(Term& term1, Term& term2, int nQubits) {
			auto pair1 = bv(term1, nQubits);
			auto pair2 = bv(term2, nQubits);
			auto scalar = pair1.first.dot(pair2.second) + pair1.second.dot(pair2.first);
			return scalar % 2;
		};

public:

	std::vector<std::vector<Term>> getCommutingSet(
			PauliOperator& composite, int n_qubits) {

		std::vector<std::vector<Term>> commuting_ops;
		std::vector<Term> allTerms;
		for (auto& kv : composite.getTerms()) {
			allTerms.push_back(kv.second);
		}

		for (int i = 0; i < allTerms.size(); i++) {

			auto t_i = allTerms[i];

			if (i == 0) {
				commuting_ops.push_back({t_i});
			} else {
				auto comm_ticker = 0;
				for (int j = 0; j < commuting_ops.size(); j++) {
					auto j_op_list = commuting_ops[j];
					int sum = 0;
					int innerCounter = 0;
					for (auto j_op : j_op_list) {
						auto t_jopPtr = allTerms[innerCounter];
						sum += bv_commutator(t_i, t_jopPtr,
								n_qubits);
						innerCounter++;
					}

					if (sum == 0) {
						commuting_ops[j].push_back(t_i);
						comm_ticker += 1;
						break;
					}
				}

				if (comm_ticker == 0) {
					commuting_ops.push_back({t_i});
				}
			}
		}

		return commuting_ops;
	}

};

}
}

#endif
