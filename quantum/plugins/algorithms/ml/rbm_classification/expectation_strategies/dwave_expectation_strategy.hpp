/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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
#ifndef XACC_ALGORITHM_RBM_CLASSIFICATION_DWAVE_DATAEXP_HPP_
#define XACC_ALGORITHM_RBM_CLASSIFICATION_DWAVE_DATAEXP_HPP_

#include "rbm_classification.hpp"
#include "xacc.hpp"

namespace xacc {
namespace algorithm {

class DWaveDataExpectationStrategy : public ExpectationStrategy {
public:
  const std::string name() const override { return "dwave-exp"; }
  const std::string description() const override { return ""; }
  std::tuple<Eigen::MatrixXd, Eigen::VectorXd, Eigen::VectorXd>
  compute(Eigen::MatrixXd &features, Eigen::MatrixXd &w, Eigen::VectorXd &v,
          Eigen::VectorXd &h, HeterogeneousMap options = {}) override {

// def set_dwave_couplings(w, bh, bv):
//     n_visible = len(bv)
//     n_hidden = len(bh)

//     # Set Qubo
//     Q = {}
//     for i in range(n_visible):
//         Q[(i, i)] = -1 * bv[i]
//     for i in range(n_hidden):
//         Q[(i + n_visible, i + n_visible)] = -1 * bh[i]
//     for i in range(n_visible):
//         for j in range(n_hidden):
//             Q[(i, n_visible + j)] = -1 * w[i][j]

//     return Q

    auto qpu = xacc::getAccelerator("dwave");
    auto provider = xacc::getIRProvider("quantum");
    auto qubo = xacc::ir::asComposite(provider->createInstruction("rbm", {}));
    int nv = qubo->getParameter(0).as<int>();
    int nh = qubo->getParameter(1).as<int>();

    std::vector<double> evaled_params(nv + nh + nv*nh);
    for (int i = 0; i < nv; i++) {
        evaled_params[i] = v(i);
    }
    for (int i = nv; i < nv+nh; i++) {
        evaled_params[i] = h(i);
    }

    auto w_vec = w.data();
    for (int i = nv+nh; i <nv+nh+nv*nh ; i++) {
        evaled_params[i] = w_vec[i];
    }

    auto evaled_qubo = (*qubo)(evaled_params);

    auto buffer = xacc::qalloc();
    qpu->execute(buffer, evaled_qubo);
    
    //  w = W.numpy()
    // hidden_bias = bh.numpy()[0]
    // visible_bias = bv.numpy()[0]
    // n_hidden = len(hidden_bias)
    // n_visible = len(visible_bias)

    // j = w/beta
    // hh = hidden_bias/beta
    // hv = visible_bias/beta

    // # Set Qubo
    // Q = set_dwave_couplings(j, hh, hv)

    // samples, energies, num_occurrences = sampler.sample_qubo(Q, num_samples,
    //                                                          save_embed,
    //                                                          load_embed)

    // if save_samples:
    //     np.save(f'samples_{str(step).zfill(4)}.npy', samples)
    //     np.save(f'energies_{str(step).zfill(4)}.npy', energies)
    //     np.save(f'nocc_{str(step).zfill(4)}.npy', num_occurrences)
    //     np.save(f'w_{str(step).zfill(4)}.npy', w)
    //     np.save(f'bv_{str(step).zfill(4)}.npy', visible_bias)
    //     np.save(f'bh_{str(step).zfill(4)}.npy', hidden_bias)

    // visibles = samples[:, :n_visible]
    // hidden = samples[:, n_visible:n_visible + n_hidden]
    // visibles = np.repeat(visibles, num_occurrences, axis=0)
    // hidden = np.repeat(hidden, num_occurrences, axis=0)
    // visibles, hidden = gibbs_sample(num_gibbs_steps, visibles, hidden,
    //                                 w, bh, bv)
    // sum_v = np.sum(visibles, axis=0)
    // sum_h = np.sum(hidden, axis=0)
    // sum_vh = np.matmul(np.transpose(visibles), hidden)
    // sum_v = tf.reshape(tfe.Variable(sum_v, dtype=tf.float32), (1, n_visible))
    // sum_h = tf.reshape(tfe.Variable(sum_h, dtype=tf.float32), (1, n_hidden))
    // sum_vh = tfe.Variable(sum_vh, dtype=tf.float32)

    // expectation_W = sum_vh / float(num_samples)
    // expectation_v = sum_v / float(num_samples)
    // expectation_h = sum_h / float(num_samples)
    // return expectation_W, expectation_v, expectation_h

    return;
  }
};
} // namespace algorithm
} // namespace xacc
#endif