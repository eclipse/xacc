import xacc, re, unittest, sys, requests, os, asyncio, signal, json
from http.server import BaseHTTPRequestHandler, HTTPServer
from threading import Thread
from multiprocessing import Process

CURRENT_TEST_DIR=sys.argv[1]
sys.argv.remove(CURRENT_TEST_DIR)

globalShutdown = False

from rpcq._base import from_msgpack
from rpcq._server import Server

mock = Server()
mock_proc_id = None

@mock.rpc_handler
async def execute_qpu_request(*args, **kwargs):
    fin = open('.tmp_quil_code', 'r')
    lines = fin.readlines()
    fin.close()
    seen_quil_code = '__qpu__ void test(qreg q) {\n'
    for l in lines:
        if 'num_shots' in l:
            shots = int(l.split('=')[1])
        elif 'DECLARE' in l:
            continue
        else:
            seen_quil_code += l 
            
    seen_quil_code += '}'
    os.remove('.tmp_quil_code')
    program = xacc.getCompiler('quil').compile(seen_quil_code).getComposites()[0]
    b = xacc.qalloc(program.nLogicalBits())
    xacc.getAccelerator('qpp', {'shots':shots}).execute(b, program)
    print(b)
    fout = open('.tmp_results', 'w')
    fout.write(str(b))
    fout.close()
    return "JOBIDHERE"

@mock.rpc_handler
async def get_buffers(*args, **kwargs):
    fin = open('.tmp_results', 'r')
    data = fin.read()
    fin.close()
    os.remove('.tmp_results')
    data_json = json.loads(data)
    measurements = data_json["AcceleratorBuffer"]["Measurements"]
    shots = sum(list(measurements.values()))
    n_qubits = len(list(measurements.keys())[0])
    results = {'q{}'.format(i):{'data':[], 'dtype':'int8', 'shape':(shots,)} for i in range(n_qubits)}
    for k, v in measurements.items():
        for i in range(n_qubits-1,-1,-1):
            for j in range(v):
                results['q{}'.format(i)]['data'].append(int(k[i]))
    for i in range(n_qubits):
        results['q{}'.format(i)]['data'] = bytearray(results['q{}'.format(i)]['data'])
        results['q{}_unclassified'.format(i)] = results['q{}'.format(i)]
    return results


def run_mock(_, endpoint):
    # Need a new event loop for a new process
    mock.run(endpoint, loop=asyncio.new_event_loop())

def m_endpoints():
    return "tcp://localhost:5557", "tcp://*:5557"

class MockServerRequestHandler(BaseHTTPRequestHandler):
    FOREST_PATTERN = re.compile(r'/lattice')
    ENGAGEMENTS_PATTERN = re.compile(r'/engagements')
    COMPILER_PATTERN = re.compile(r'/devices/.*/native_quil_to_binary')
    SHUTDOWN_PATTERN = re.compile(r'/shutdown')

    def do_GET(self):
        if re.search(self.FOREST_PATTERN, self.path):
            # Add response status code.
            self.send_response(requests.codes.ok)

            # Add response headers.
            self.send_header('Content-Type', 'application/json')
            self.end_headers()

            f = open(CURRENT_TEST_DIR+'/resources/lattice.json', 'r')
            response_content = f.read()
            f.close()

            # Add response content.
            self.wfile.write(response_content.encode('utf-8'))
            return

    def do_POST(self):
        global globalShutdown
        if re.search(self.ENGAGEMENTS_PATTERN, self.path):
            # Add response status code.
            self.send_response(requests.codes.ok)

            # Add response headers.
            self.send_header('Content-Type', 'application/json')
            self.end_headers()

            response_content = '''{"address":"tcp://localhost:5557","endpointId":"3799ef49-e222-48e9-8d62-424872663139","minimumPriority":null,"expiresAt":"2021-01-27T18:23:13Z","quantumProcessorId":"Aspen-8","userId":"00u4i1atsfsrjqiiy356","credentials":{"clientPublic":"BFshSYpjB+=r%Y+Y?)pY.9i)MzfRL]aH{RxxGni:","clientSecret":"N{}@EVSpQ{LfTdNrSi$rRCcBY((D/ec>2N&1&7E4","serverPublic":"7juj@N=?QI(i1]:B)i>)HKf1^X]TjpPsBb$1n[r"}}'''

            # Add response content.
            self.wfile.write(response_content.encode('utf-8'))
            return

        elif re.search(self.COMPILER_PATTERN, self.path):
            # Add response status code.
            self.send_response(requests.codes.ok)

            content_length = int(self.headers['Content-Length']) 
            post_data = self.rfile.read(content_length).decode('utf-8') 
            post_data_json = json.loads(post_data)
            seen_quil_code = post_data_json['quil']
            seen_quil_code = 'num_shots = {}\n'.format(post_data_json['num_shots']) + seen_quil_code
            fout = open('.tmp_quil_code', 'w')
            fout.write(seen_quil_code)
            fout.close()

            # Add response headers.
            self.send_header('Content-Type', 'application/json')
            self.end_headers()

            f = open(CURRENT_TEST_DIR+'/resources/compiler_response.json', 'r')
            response_content = f.read()
            f.close()

            # Add response content.
            self.wfile.write(response_content.encode('utf-8'))
            return
        elif re.search(self.SHUTDOWN_PATTERN, self.path):
            # Add response status code.
            self.send_response(requests.codes.ok)

            # Add response headers.
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            response_content = ''
            self.wfile.write(response_content.encode('utf-8'))
            globalShutdown = True
            return

class TestQCS(unittest.TestCase):

    def setUp(self):
        global mock_proc_id
        self.mock_forest_port = 64574
        self.mock_forest_server = HTTPServer(('localhost', self.mock_forest_port), MockServerRequestHandler)

        self.mock_forest_server_thread = Thread(target=self.mock_forest_server.serve_forever)
        self.mock_forest_server_thread.setDaemon(True)
        self.mock_forest_server_thread.start()

        # fixme, need a rpcq mock server
        proc = Process(target=run_mock, args=m_endpoints())
        proc.start()
        mock_proc_id = proc.pid

    
    def testEventThread(self):
        while not globalShutdown:
            pass

    def tearDown(self):
        os.kill(mock_proc_id, signal.SIGINT)

if __name__ == '__main__':
    unittest.main()