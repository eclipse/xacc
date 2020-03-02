# In order to use this plugin, you must install tensorflow and dwave ocean...
import xacc, os
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from abc import abstractmethod, ABC

class DataLoadingStrategy(ABC):
    @abstractmethod
    def load(self, file):
        pass

def get_model_parameter_updates(
    dataexp_W, modexp_W, dataexp_v, modexp_v, dataexp_h, modexp_h
):
    import tensorflow as tf
    W_delta = tf.subtract(dataexp_W, modexp_W)
    bv_delta = tf.subtract(dataexp_v, modexp_v)
    bh_delta = tf.subtract(dataexp_h, modexp_h)
    return W_delta, bv_delta, bh_delta

def make_rbm(n_visible, n_hidden, w_std=0.01):
    import tensorflow as tf
    W = tf.Variable(tf.random.normal([n_visible, n_hidden], stddev=w_std),
                     name='W')
    bh = tf.Variable(tf.zeros([1, n_hidden], tf.float32), name='bh')
    bv = tf.Variable(tf.zeros([1, n_visible], tf.float32), name='bv')
    return W, bh, bv

def _read_compressed_dataset(filename, data_bits):
    """
    read compressed (bitwise) data - assume encoding from `dbytes` bytes.
    `filename` is the full path to the data.
    """
    import numpy as np
    data = np.load(filename)
    images = data[:, :data_bits]
    labels = data[:, -1:]
    return images.astype('float32'), labels.astype('float32')


def make_dataset_train(filename, num_epochs=1, batch_size=10, shuffle=False,
                       n_visible=33, filter=None, n_images=None):
    """
    if `filename` is `None`, load synthetic data, if `filter` is not None,
    use the value to filter for only that class
    """
    import tensorflow as tf
    import numpy as np
    # TODO- currently we split off the label when reading the file and then
    # put it back on here. we should stop that...
    if filename is not None:
        if filename.startswith('/c/'):
            filename = 'c:' + filename[2:]
        features, targets = _read_compressed_dataset(filename, n_visible-1)
    else:
        features, targets = _make_synthetic_examples(2500, n_visible-1)
    if filter is not None:
        features, targets = _filter_data(features, targets, filter)
    features_plus_labels = np.concatenate((features, targets), axis=1)
    n = len(features)
    dataset = tf.data.Dataset.from_tensor_slices((
        features_plus_labels[:n_images]
    ))
    if shuffle:
        dataset = dataset.shuffle(n)
    dataset = dataset.repeat(num_epochs)
    dataset = dataset.batch(batch_size)
    return dataset, n

@ComponentFactory("rbm_algorithm_factory")
@Provides("algorithm")
@Property("_algorithm", "algorithm", "rbm-classification")
@Property("_name", "name", "rbm-classification")
@Instantiate("rbm_classification_algorithm_instance")
class RBMClassification(xacc.Algorithm):

    def __init__(self):
        xacc.Algorithm.__init__(self)

    def clone(self):
        return RBMClassification()

    def initialize(self, options):
        self.nv = options['nv']
        self.nh = options['nh']
        self.num_epochs = options['epochs'] if 'epochs' in options else 1
        self.num_gibbs = options['n-gibbs-steps'] if 'n-gibbs-steps' in options else 0
        self.train_file = options['train-file']
        self.batch_size = options['batch-size'] if 'batch-size' in options else 1
        self.train_steps = options['train-steps'] if 'train-steps' in options else -1
        self.shots = options['shots'] if 'shots' in options else 100
        self.backend = options['backend'] if 'backend' in options else 'dwave-neal'
        self.exp_strategy = options['expectation-strategy'] if 'expectation-strategy' in options else 'cd'
        self.embedding = options['embedding'] if 'embedding' in options else None
        self.store_embedding = options['store-first-embedding'] if 'store-first-embedding' in options else False
        return True

    def name(self):
        return 'rbm-classification'

    def execute(self, buffer):
        import tensorflow as tf
        import numpy as np, logging
        from tqdm import tqdm

        l2reg = 0.
        LOGGER = logging.getLogger(__name__)
        lr = tf.constant(0.1, tf.float32)

        beta = 5.
        n_visible = self.nv
        n_hidden = self.nh
        model_dir = '.'
        train_file = self.train_file
        num_epochs = self.num_epochs
        batch_size = self.batch_size
        num_images = None #self.num_images
        train_steps = self.train_steps
        expectation_method = self.exp_strategy

        W, bh, bv = make_rbm(n_visible, n_hidden)

        writer = tf.summary.create_file_writer(model_dir)
        global_step = tf.compat.v1.train.get_or_create_global_step()
        checkpoint_prefix = os.path.join(model_dir, 'ckpt')
        checkpoint = tf.train.Checkpoint(W=W, bh=bh, bv=bv, optimizer_step=global_step)
        checkpoint.restore(tf.train.latest_checkpoint(model_dir))
        LOGGER.info('training type = {}'.format(expectation_method))
        LOGGER.info('batch_size = {}'.format(batch_size))
        # LOGGER.info('train_steps = {}'.format(train_steps))
        LOGGER.info('num_epochs = {}'.format(num_epochs))
        # LOGGER.info('num_samples = {}'.format(num_samples))

        # if training already happened, make sure we restart from an integer number of epochs
        prev_last_step = checkpoint.save_counter.numpy()
        n_train = len(np.load(train_file))
        steps_per_epoch = int(n_train/batch_size)
        # TODO: make this give right result if batch size does not divide n_train
        n_epochs_past = int(prev_last_step/steps_per_epoch)
        if num_epochs <= n_epochs_past:
            raise ValueError('You specified {} epochs, but the model has already trained for {}.'.format(num_epochs, n_epochs_past))
        num_epochs = num_epochs - n_epochs_past

        n_steps_past = steps_per_epoch*n_epochs_past
        if n_steps_past < prev_last_step:
            checkpoint.restore('{}'.format(checkpoint_prefix-n_steps_past))
        print('Starting from checkpoint {}.'.format(n_steps_past))

        dataset, n_evts = make_dataset_train(
            train_file, num_epochs, batch_size, True, n_visible, n_images=num_images
        )

        data_exp_strategy = xacc.serviceRegistry.get_service('rbm_expectation_strategy','data')
        model_exp_strategy = xacc.serviceRegistry.get_service('rbm_expectation_strategy',self.exp_strategy)

        with writer.as_default():
            for i, features in enumerate(tqdm(dataset)):
                global_step.assign_add(1)
                LOGGER.debug('{}, feat-shp {}'.format(
                    i, features.shape
                ))
                if i % 100 == 0:
                    LOGGER.info('train step {}'.format(i))
                if i >= train_steps > -1:
                    LOGGER.info('Stopping at train step {}'.format(i))
                    break

                current_step = i + n_steps_past

                dataexp_W, dataexp_v, dataexp_h = data_exp_strategy.execute(buffer, features, W, bv, bh, {})
                modexp_W, modexp_v, modexp_h = model_exp_strategy.execute(buffer, features, W, bv, dataexp_h,
                                    {'shots':self.shots, 'backend':self.backend, 'embedding':self.embedding, 'n-gibbs-steps':self.num_gibbs})

                # compute model parameter adjustments
                W_delta, bv_delta, bh_delta = get_model_parameter_updates(
                    dataexp_W, modexp_W,
                    dataexp_v, modexp_v,
                    dataexp_h, modexp_h
                )
                if i % 10 == 0:
                    LOGGER.debug('weight updates: {}'.format(W_delta))
                    LOGGER.debug('visible bias updates: {}'.format(bv_delta))
                    LOGGER.debug('hidden bias updates: {}'.format(bh_delta))

                W.assign((1 - l2reg) * W + lr * W_delta)
                bv.assign((1 - l2reg) * bv + lr * bv_delta)
                bh.assign((1 - l2reg) * bh + lr * bh_delta)


        buffer.addExtraInfo('w', W.numpy().flatten().tolist())
        buffer.addExtraInfo('bv', bv.numpy().flatten().tolist())
        buffer.addExtraInfo('bh', bh.numpy().flatten().tolist())
