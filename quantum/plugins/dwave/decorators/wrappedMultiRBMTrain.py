from pelix.ipopo.decorators import (ComponentFactory, Property, Requires,
                                    Provides, Instantiate)
import xacc
import inspect, csv
import numpy as np
import time
from scipy.special import expit as sigmoid
from collections import Counter
@ComponentFactory("wrapped_rbm_train_mnist_factory")
@Provides("decorator_algorithm_service")
@Property("_algorithm", "algorithm", "mnist_digit_train")
@Property("_name", "name", "mnist_digit_train")
@Instantiate("wrapped_rbm_train_mnist_instance")
class WrappedMultiRBMTrain(xacc.DecoratorFunction):

    def __call__(self, *args, **kwargs):
        super().__call__(*args, **kwargs)

        self.initial_rbm = True
        self.buffer = args[0]
        self.embedding = self.buffer.getInformation('embedding')

        # Get the training parameters (rate, num_epochs, batch_size, momentum)
        self.learn_rate = self.kwargs['rate']
        self.num_epochs = self.kwargs['num_epochs']
        self.momentum = self.kwargs['momentum']
        self.batch_size = self.kwargs['batch_size']
        self.num_classes = self.kwargs['max_classes']
        self.train_steps = self.kwargs['train_steps']

        if 'chain-strength' in self.kwargs:
            xacc.setOption('chain-strength', self.kwargs['chain_strength'])
        if 'num_samples' in self.kwargs:
            xacc.setOption('dwave-num-reads', self.kwargs['num_samples'])

        # Get parameterized DWK
        self.rbm_function = self.compiledKernel

        # Might be a better way to get these values, but this is what I'm shooting for right now
        self.numV = 0
        self.numH = 0
        self.numW = 0
        for inst in self.rbm_function.getParameters():
            if 'v' in inst:
                self.numV += 1
            if 'h' in inst:
                self.numH += 1
            if 'w' in inst:
                self.numW += 1
                
        # Initializing the weights from a random normal distribution
        # Initializing the hidden and visible biases to be zero
        self.weights = np.random.normal(0.01, 1.0, (self.numV, self.numH))
        self.visible_bias = np.zeros((1, self.numV))
        self.hidden_bias = np.zeros((1, self.numH))
        tic = time.clock()

        for digit in range(self.num_classes):

            print("Begin training for digit {}.".format(digit))
            self.data, self.targets = self.readTrainData(self.kwargs['train_data'], digit)
            self.data = self.batchData(self.data, self.batch_size)

            for epoch in range(self.num_epochs):
                train_step = 0
                for batch in self.data:
                    if train_step >= self.train_steps > -1:
                        break
                    # get data expectation values
                    dataExpW, dataExpV, dataExpH = self.getDataExpectations(batch)

                    # set the RBM
                    self.setRBM()

                    training_buffer = self.qpu.createBuffer("buffer")
                    training_buffer.addExtraInfo('embedding', self.embedding)

                    # Execute the RBM with the new parameters
                    self.executeRBM(training_buffer)

                    # Get the expectation values from the D-Wave execution
                    modelExpW, modelExpV, modelExpH = self.getExpectations()

                    # Update the parameters for this batch
                    self.updateParameters(dataExpW, modelExpW, dataExpV, modelExpV, dataExpH, modelExpH)

                    train_step += 1

            self.buffer.addExtraInfo("{}_visible".format(digit), self.visible_bias.flatten().tolist())
            self.buffer.addExtraInfo("{}_hidden".format(digit), self.hidden_bias.flatten().tolist())
            self.buffer.addExtraInfo("{}_weights".format(digit), self.weights.flatten().tolist())

        toc = time.clock()
        training_time = toc - tic
        if 'test_data' in self.kwargs:
            self.test_data, self.test_targets, n_tests = self.readTestData(self.kwargs['test_data'])
            self.test_data = self.batchData(self.test_data, self.batch_size)
            evals = np.zeros((n_tests, self.num_classes))
            truth_vals = np.zeros(n_tests)

            for digit in range(self.num_classes):
                w = np.reshape(np.array(self.buffer.getInformation("{}_weights".format(digit))), (self.numV,self.numH))
                v = np.reshape(np.array(self.buffer.getInformation("{}_visible".format(digit))), (1, self.numV))
                h = np.reshape(np.array(self.buffer.getInformation("{}_hidden".format(digit))), (1,self.numH))
                count = 0
                for i,batch in enumerate(self.test_data):
                    Fv = self.freeEnergy(batch, w, v, h)
                    targets = np.reshape(self.test_targets[i], (-1,))
                    ll = list(zip(targets, Fv))
                    for j, item in enumerate(ll):
                        idx = i * self.batch_size + j
                        evals[idx, digit] = item[1]
                        if digit == 0:
                            truth_vals[idx] = item[0]

            softmax = np.exp(-evals)/sum(np.exp(-evals))
            predictions = np.argmax(softmax, axis=1)
            accuracy = np.sum(predictions == truth_vals) / len(truth_vals)
            self.buffer.addExtraInfo('accuracy', str(accuracy))
        timestr = time.strftime("%Y%m%d-%H%M%S")
        self.buffer.addExtraInfo("training-time", training_time)
        print("Finished Training")
        print("Accuracy: ", self.buffer.getInformation('accuracy'))
        print("Training Time: ", self.buffer.getInformation('training-time'))
        output_name = self.kwargs['output']+'.ab' if 'output' in self.kwargs else "rbm-buffer-{}.ab".format(timestr)
        f = open(output_name, "w")
        f.write(str(self.buffer))
        f.close()
        return

    def readTrainData(self, csv_location, digit):
        if '.npy' in csv_location:
            data = np.load(csv_location)
            images = data[:, :self.numV-1]
            labels = data[:, -1:]
            return images.astype('float32'), labels.astype('float32')
        else:
            with open(csv_location) as csv_file:
                csv_reader = csv.reader(csv_file, delimiter=',')
                imageL, labelL = [], []
                for row in csv_reader:
                    imageL.append(row[:64])
                    labelL.append(row[64:65])
                images = np.asarray(imageL).astype(dtype=np.uint8)/16.
                images = (images > 0.705).astype(int)
                labels = np.asarray(labelL).astype(dtype=np.uint8)
                idx = np.where(labels==digit)[0]
                images_idx = images[idx]
                labels_idx = labels[idx]
            return images_idx.astype('float32'), labels_idx.astype('float32')

    def readTestData(self, csv_location):
        with open(csv_location) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            imageL, labelL = [], []
            for row in csv_reader:
                imageL.append(row[:64])
                labelL.append(row[64:65])
            n = len(imageL)
            images = np.asarray(imageL).astype(dtype=np.uint8)/16.
            images = (images > 0.705).astype(int)
            labels = np.asarray(labelL).astype(dtype=np.uint8)
        return images.astype('float32'), labels.astype('float32'), n

    def batchData(self, array, batch_size):
        part_batches = array.shape[0] % batch_size
        padding = batch_size - part_batches
        padded_array = np.zeros((padding, array.shape[1]))
        array = np.append(array, padded_array, axis=0)
        whole_batches = array.shape[0] // batch_size
        final_array = np.zeros((whole_batches, batch_size,array.shape[1]))
        for i in range(final_array.shape[0]):
            final_array[i] = array[i*batch_size:batch_size+(batch_size*i)]
        print("Batched Array: ", final_array.shape)
        return final_array
    
    def getDataExpectations(self, batch):

        hidden_probs = sigmoid(np.matmul(batch, self.weights) + self.hidden_bias)

        w_expectation = np.matmul(batch.T, hidden_probs)
        v_expectation = batch
        h_expectation = hidden_probs
        return w_expectation, v_expectation, h_expectation

    def setRBM(self):
        # Initialize the RBM with random weights and biases
        # Only does this once; then, the weights and biases obtained from training are used to sample
        # Note: to set the XACC RBM function, the parameters must be 1d lists
        if self.initial_rbm:
            v = ((np.random.rand(self.numV) * 2 - 1) * -0.01).tolist()
            h = ((np.random.rand(self.numH) * 2 - 1) * -0.01).tolist()
            w = ((np.random.rand(self.numV, self.numH) * 2 - 1) * -0.01).flatten().tolist()
            self.initial_rbm = False
        else:
            v = (self.visible_bias).flatten().tolist()
            h = (self.hidden_bias).flatten().tolist()
            w = (self.weights).flatten().tolist()
        params = v + h + w
        # print("Checking to make sure parameter order is correct...")
        # print(self.v_biases == params[:len(self.v_biases)])
        # print(self.h_biases == params[len(self.v_biases):len(self.h_biases) + len(self.v_biases)])
        # print(w == params[len(self.h_biases)+len(self.v_biases):])
        self.training_rbm = self.rbm_function.eval(params)

    def executeRBM(self, buffer):
        self.qpu.execute(buffer, self.training_rbm)
        self.energies = buffer.getInformation('energies')
        self.energies /= np.max(np.abs(self.energies))
        self.measurements = buffer.getMeasurementCounts()
        embedding = buffer.getInformation('embedding')

        # Unembed  the sample data
        unembedded = {}
        activeVars = buffer.getInformation('active-vars')
        for bitstring, count in buffer.getMeasurementCounts().items():
            newBitString = []
            for logicalId, physicalIds in embedding.items():
                physicalBitstring = [bitstring[activeVars.index(pid)] for pid in physicalIds]
                bit = str(Counter(physicalBitstring).most_common(1)[0][0])
                newBitString.append(bit)
            fullNew = ''.join(x for x in newBitString)
            if fullNew not in unembedded:
                unembedded[fullNew] = count
            else:
                unembedded[fullNew] += count

        # Construct numpy array from unembedded samples
        outerList = []
        for bits, num in unembedded.items():
            bitString = []
            for char in bits:
                if char == "1":
                    bitString.append(1)
                else:
                    bitString.append(0)
            for i in range(num):
                outerList.append(bitString)
        self.samples = np.array(outerList)


    def getExpectations(self):
        num_samples = self.samples.shape[0]
        sum_v = np.reshape(np.sum(self.samples[:, :self.numV], axis=0), (1, self.numV))
        sum_h = np.reshape(np.sum(self.samples[:, self.numV:self.numV + self.numH], axis=0), (1, self.numH))
        sum_vh = np.matmul(sum_v.T, sum_h)
        expectation_W = sum_vh / float(num_samples)
        expectation_v = sum_v / float(num_samples)
        expectation_h = sum_h / float(num_samples)

        return expectation_W, expectation_v, expectation_h


    def updateParameters(self, dataExpW, modExpW, dataExpV, modExpV, dataExpH, modExpH):
        deltaW = np.subtract(dataExpW, modExpW)
        deltaV = np.sum(np.subtract(dataExpV, modExpV), axis=0, keepdims=True)
        deltaH = np.sum(np.subtract(dataExpH, modExpH), axis=0, keepdims=True)

        var_scale = self.learn_rate / self.batch_size

        self.weights = self.momentum * self.weights + var_scale * deltaW
        self.visible_bias = self.momentum * self.visible_bias + var_scale * deltaV
        self.hidden_bias = self.momentum * self.hidden_bias + var_scale * deltaH

    def freeEnergy(self, data, weights, v_bias, h_bias):
        vis_b = data * v_bias
        xj = np.dot(data, weights) + h_bias
        expxj = (1.0 + np.exp(xj)).clip(np.finfo(float).min, np.finfo(float).max)
        loge = np.log(expxj)
        Fv = -np.sum(vis_b, axis=1) - np.sum(loge, axis=1)
        return Fv
