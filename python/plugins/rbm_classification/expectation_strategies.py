import xacc, os
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from abc import abstractmethod, ABC

class ExpectationStrategy(ABC):
    @abstractmethod
    def execute(self, global_buffer, features, w, v, h, options):
        pass


@ComponentFactory("rbm_cd_expectation_strategy_factory")
@Provides("rbm_expectation_strategy")
@Property("_rbm_expectation_strategy", "rbm_expectation_strategy", "cd")
@Property("_name", "name", "cd")
@Instantiate("rbm_cd_expectation_strategy_instance")
class CDExpectationStrategy(ExpectationStrategy):
    def execute(self, global_buffer, features, w, v, h, options):
        import tensorflow as tf
        def sample(probs):
            """ random sample of 0s and 1s based on probs """
            import tensorflow as tf
            return tf.floor(probs + tf.random.uniform(tf.shape(probs), 0, 1))
        h_sample = sample(h)
        x_recon = sample(
            tf.sigmoid(tf.matmul(h_sample, tf.transpose(w)) + v)
        )
        expectation_W = tf.matmul(tf.transpose(x_recon), h)
        expectation_v = x_recon
        expectation_h = h_sample
        return expectation_W, expectation_v, expectation_h

@ComponentFactory("rbm_gibbs_expectation_strategy_factory")
@Provides("rbm_expectation_strategy")
@Property("_rbm_expectation_strategy", "rbm_expectation_strategy", "gibbs")
@Property("_name", "name", "gibbs")
@Instantiate("rbm_gibbs_expectation_strategy_instance")
class GibbsExpectationStrategy(ExpectationStrategy):
    def execute(self, global_buffer, features, W, bv, bh, options):
        import tensorflow as tf
        import numpy as np
        def sample(probs):
            """ random sample of 0s and 1s based on probs """
            import tensorflow as tf
            return tf.floor(probs + tf.random.uniform(tf.shape(probs), 0, 1))
        n_samples = options['n-samples'] if 'n-samples' in options else 100
        visible_bias = bv.numpy()[0]
        hidden_bias = bh.numpy()[0]
        n_visible = len(visible_bias)
        n_hidden = len(hidden_bias)

        sum_vh = tf.zeros([n_visible, n_hidden], tf.float32)
        sum_v = tf.zeros([1, n_visible], tf.float32)
        sum_h = tf.zeros([1, n_hidden], tf.float32)
        # initial visible variables are random
        visibles = tf.Variable(
            np.random.randint(0, 2, [1, n_visible]), dtype=tf.float32
        )
        for _ in range(n_samples):
            hiddens = sample(tf.sigmoid(tf.matmul(visibles, W) + bh))
            visibles = sample(tf.sigmoid(tf.matmul(hiddens, tf.transpose(W)) + bv))
            sum_v += visibles
            sum_h += hiddens
            sum_vh += tf.matmul(tf.transpose(visibles), hiddens)
        expectation_W = sum_vh / float(n_samples)
        expectation_v = sum_v / float(n_samples)
        expectation_h = sum_h / float(n_samples)
        return expectation_W, expectation_v, expectation_h

@ComponentFactory("rbm_disc_expectation_strategy_factory")
@Provides("rbm_expectation_strategy")
@Property("_rbm_expectation_strategy", "rbm_expectation_strategy", "discriminative")
@Property("_name", "name", "discriminative")
@Instantiate("rbm_discriminative_expectation_strategy_instance")
class DiscriminativeExpectationStrategy(ExpectationStrategy):
    def execute(self, global_buffer, features, W, bv, bh, options):
        import tensorflow as tf
        def free_energy(vis, W, bh, bv):
            """
            F(v) = -sum_i v_i a_i - sum_j log(1 + e^{x_j}),
            where a_i = visible biases, v_i = binary state of visible unit i

            x_j = b_j + sum_i v_i w_{ij} = total input to hidden unit j
            """
            vis_b = vis * bv
            xj = tf.tensordot(vis, W, [[-1], [0]]) + bh
            expxj = tf.clip_by_value(1.0 + tf.exp(xj), tf.float32.min, tf.float32.max)
            loge = tf.math.log(expxj)
            Fv = -tf.reduce_sum(vis_b, axis=-1) - tf.reduce_sum(loge, axis=-1)
            return Fv


        visible_bias = bv.numpy()[0]
        hidden_bias = bh.numpy()[0]
        n_visible = len(visible_bias)
        n_hidden = len(hidden_bias)
        batch_size = features.shape.as_list()[0]

        features_0 = tf.concat([features[:, :-1], tf.zeros((batch_size, 1))],
                               axis=1)
        features_1 = tf.concat([features[:, :-1], tf.ones((batch_size, 1))], axis=1)
        features_stack = tf.stack([features_0, features_1])
        frees = free_energy(features_stack, W, bh, bv)
        probs_nonnorm = tf.exp(-frees)
        z = tf.reduce_sum(probs_nonnorm, axis=0)
        probs = probs_nonnorm / z
        h_probs = tf.sigmoid(tf.tensordot(features_stack, W, [[-1], [0]]) + bh)

        expectation_v = tf.tensordot(probs, features_stack,
                                     [[0, 1], [0, 1]]) / batch_size
        expectation_v = tf.reshape(expectation_v, (1, n_visible))
        expectation_h = tf.tensordot(probs, h_probs, [[0, 1], [0, 1]]) / batch_size
        expectation_h = tf.reshape(expectation_h, (1, n_hidden))
        all_W = tf.einsum('abc,abd->abcd', features_stack, h_probs)
        expectation_W = tf.tensordot(probs, all_W, [[0, 1], [0, 1]]) / batch_size
        return expectation_W, expectation_v, expectation_h



@ComponentFactory("rbm_data_expectation_strategy_factory")
@Provides("rbm_expectation_strategy")
@Property("_rbm_data_expectation_strategy", "rbm_data_expectation_strategy", "data")
@Property("_name", "name", "data")
@Instantiate("rbm_data_expectation_strategy_instance")
class DataExpectationStrategy(ExpectationStrategy):
    def execute(self, global_buffer, features, w, v, h, options):
        import tensorflow as tf
        batch_size = features.shape.as_list()[0]
        h_probs = tf.sigmoid(tf.matmul(features, w) + h)
        expectation_W = tf.matmul(tf.transpose(features), h_probs)/batch_size
        expectation_v = tf.reduce_mean(features, axis=0, keepdims=True)
        expectation_h = tf.reduce_mean(h_probs, axis=0, keepdims=True)
        return expectation_W, expectation_v, expectation_h


class NealSampler(object):
    """
    This module defines a sampler.
    :param num_samps: number of samples
    :type num_samps: int
    """

    def __init__(self, num_copies=1, chainstrength=4700, solver=None):
        from dimod.binary_quadratic_model import BinaryQuadraticModel
        import neal
        self.child = neal.SimulatedAnnealingSampler()

    def sample_qubo(self, Q, num_samps):
        self.num_samps = num_samps
        from dwave.system.samplers import DWaveSampler  # Library to interact with the QPU
        from dwave.system.composites import FixedEmbeddingComposite  # Library to embed our problem onto the QPU physical graph
        from dimod.binary_quadratic_model import BinaryQuadraticModel
        import minorminer
        self.sampler = self.child

        response = self.sampler.sample_qubo(Q, num_reads=self.num_samps)
        return response.record['sample'], \
            response.record['energy'], response.record['num_occurrences']

@ComponentFactory("rbm_dwave_expectation_strategy_factory")
@Provides("rbm_expectation_strategy")
@Property("_rbm_expectation_strategy", "rbm_expectation_strategy", "quantum-annealing")
@Property("_name", "name", "quantum-annealing")
@Instantiate("rbm_dwave_expectation_strategy_instance")
class DWaveExpectationStrategy(ExpectationStrategy):
    def execute(self, global_buffer, features, W, bv, bh, options):
        import numpy as np
        import tensorflow as tf
        def set_dwave_couplings(w, bh, bv):
            n_visible = len(bv)
            n_hidden = len(bh)

            # Set Qubo
            Q = {}
            for i in range(n_visible):
                Q[(i, i)] = -1 * bv[i]
            for i in range(n_hidden):
                Q[(i + n_visible, i + n_visible)] = -1 * bh[i]
            for i in range(n_visible):
                for j in range(n_hidden):
                    Q[(i, n_visible + j)] = -1 * w[i][j]

            return Q

        def gibbs_sample(k, x, h, W, bh, bv):
            """
            run sampling k times and return the visible and hidden outputs
            """
            import tensorflow as tf
            def gibbs_step(xk):
                """ single gibbs step """
                # visible values -> hidden values
                xk = tf.cast(xk, tf.float32)
                hk = sample(tf.sigmoid(tf.matmul(xk, W) + bh))
                # hidden values -> visible values
                xk = sample(tf.sigmoid(tf.matmul(hk, tf.transpose(W)) + bv))
                return xk, hk

            for _ in range(k):
                x, h = gibbs_step(x)
            return x, h

        beta = 1.
        w = W.numpy()
        hidden_bias = bh.numpy()[0]
        visible_bias = bv.numpy()[0]
        n_hidden = len(hidden_bias)
        n_visible = len(visible_bias)

        backend = options['backend'] if 'backend' in options else 'dwave-neal'
        save_embed = options['save_embed'] if 'save_embed' in options else False
        load_embed = options['load_embed'] if 'load_embed' in options else None
        n_samples = options['n-samples'] if 'n-samples' in options else 100
        num_gibbs_steps = options['n-gibbs-steps'] if 'n-gibbs-steps' in options else 0

        j = w/beta
        hh = hidden_bias/beta
        hv = visible_bias/beta

        # Set Qubo
        Q = set_dwave_couplings(j, hh, hv)
        QIR = xacc.annealing.create_composite_from_qubo(Q)
        QIR.setTag("qubo")

        qpu = xacc.getAccelerator(backend, {'shots':n_samples, 'mode':'qubo'})
        qbits = xacc.qalloc()
        if options['embedding'] is not None:
            qbits.addExtraInfo('embedding', options['embedding'])

        qpu.execute(qbits, QIR)

        global_buffer.appendChild(qbits.name(), qbits)
        # convert observed bit strings to
        # samples array
        s = []
        measurements = qbits.getMeasurementCounts()
        for k,v in measurements.items():
            arr = np.array(list(map(int, k)))
            for i in range(v):
                s.append(arr)

        samples = np.stack(s, axis=0)
        num_occurrences = np.ones(n_samples, dtype=int)

        visibles = samples[:, :n_visible]
        hidden = samples[:, n_visible:n_visible + n_hidden]
        visibles = np.repeat(visibles, num_occurrences, axis=0)
        hidden = np.repeat(hidden, num_occurrences, axis=0)
        visibles, hidden = gibbs_sample(num_gibbs_steps, visibles, hidden,
                                    w, bh, bv)
        sum_v = np.sum(visibles, axis=0)
        sum_h = np.sum(hidden, axis=0)
        sum_vh = np.matmul(np.transpose(visibles), hidden)
        sum_v = tf.reshape(tf.Variable(sum_v, dtype=tf.float32), (1, n_visible))
        sum_h = tf.reshape(tf.Variable(sum_h, dtype=tf.float32), (1, n_hidden))
        sum_vh = tf.Variable(sum_vh, dtype=tf.float32)

        expectation_W = sum_vh / float(n_samples)
        expectation_v = sum_v / float(n_samples)
        expectation_h = sum_h / float(n_samples)
        return expectation_W, expectation_v, expectation_h
