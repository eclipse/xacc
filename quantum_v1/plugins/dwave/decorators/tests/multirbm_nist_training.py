import xacc
import numpy as np
import h5py, os, csv, ast


# import requests
# import logging

# # These two lines enable debugging at httplib level (requests->urllib3->http.client)
# # You will see the REQUEST, including HEADERS and DATA, and RESPONSE with HEADERS but without DATA.
# # The only thing missing will be the response.body which is not logged.
# try:
#     import http.client as http_client
# except ImportError:
#     # Python 2
#     import httplib as http_client
# http_client.HTTPConnection.debuglevel = 1

# # You must initialize logging, otherwise you'll not see debug output.
# logging.basicConfig()
# logging.getLogger().setLevel(logging.DEBUG)
# requests_log = logging.getLogger("requests.packages.urllib3")
# requests_log.setLevel(logging.DEBUG)
# requests_log.propagate = True

# requests.get('https://httpbin.org/headers')

# File names to pass in for training and testing data
train_file = os.path.join('nist_data/optdigits.tra')
test_file = os.path.join('nist_data/optdigits.tes')
# Initialize the xacc framework
xacc.Initialize()
# Get the XACC D-Wave accelerator
dwave = xacc.getAccelerator('dwave')
# Select which D-Wave solver to use
xacc.setOption('dwave-solver', 'DW_2000Q_VFYC_2_1')
# Create an AcceleratorBuffer for use in the XACC D-Wave accelerator
buffer = dwave.createBuffer("buffer")
# Add embedding to AcceleratorBuffer
f = open('embedding.txt', 'r')
embedding = ast.literal_eval(f.read())
f.close()
buffer.addExtraInfo('embedding', embedding)

# Specify the settings for the algorithm execution
# These can also be passed directly into the xacc.qpu() decorator as keyword arguments
# an optional argument for naming the output buffer file
# is 'output': 'name_of_file' (no file extension)
settings = {'algo': "mnist_digit_train",
            'accelerator': dwave,
            'train_data': train_file,
            'test_data': test_file,
            'num_samples': 100,
            'chain_strength': 4700,
            'num_epochs': 1,
            'batch_size': 2,
            'rate': 0.1,
            'momentum': 0.5,
            'train_steps': 1,
            'max_classes': 1,
            'output': 'trained-rbm'}

# Define the XACC kernel
# In this case, we calling the XACC RBM IRgenerator to
# generate a D-Wave RBM IR instance with the specified number of visible and hidden units
# The XACC Python decorator arguments define how the training algorithm will be executed
@xacc.qpu(**settings)
def rbm_train(buffer):
    rbm(visible_units=64,hidden_units=10)

# Execute the XACC kernel, passing the AcceleratorBuffer as an argument
rbm_train(buffer)

# Finalize the XACC framework
xacc.Finalize()