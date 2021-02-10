import unittest, requests, sys, xacc, subprocess, time 
CURRENT_TEST_DIR=sys.argv[1]
sys.argv.remove(CURRENT_TEST_DIR)

class TestQCSRunner(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        print('setting up class')
        self.process = subprocess.Popen([sys.executable,CURRENT_TEST_DIR+'/qcs_integration_mock_servers.py', CURRENT_TEST_DIR])
        time.sleep(2)
        forest_url = 'http://localhost:64574'
        engagement_url = forest_url
        compiler_url = forest_url
        try:
            self.qpu = xacc.getAccelerator('qcs:FakeAspen-8', {'shots':500, 'auth_token':'user-fake-auth', 
                                                'forest_server_url':forest_url, 
                                                'engagement_url':engagement_url, 
                                                'qpu_compiler_url':compiler_url,
                                                'use_rpcq_auth_config':False})
        except:
            print('Failure in setup - shutting down the server')
            requests.post("http://localhost:64574/shutdown")
            time.sleep(2)
        
        xacc.qasm('''.compiler xasm
        .circuit super
        .parameters t0
        .qbit q
        H(q[0]);
        Measure(q[0]);
        ''')
        self.super = xacc.getCompiled('super')
        
        xacc.qasm('''.compiler xasm
        .circuit bell
        .parameters t0
        .qbit q
        H(q[0]);
        CX(q[0], q[1]);
        Measure(q[0]);
        Measure(q[1]);
        ''')
        self.bell = xacc.getCompiled('bell')


    def test_simple_hadamard(self):
        try:
            q = xacc.qalloc(1)
            self.qpu.execute(q, self.super)
            print(q)
        except:
            print('Failure in hadamard test - shutting down the server')
            requests.post("http://localhost:64574/shutdown")
            time.sleep(2)

    def test_simple_bell(self):
        try:
            q = xacc.qalloc(2)
            self.qpu.execute(q, self.bell)
            print(q)
        except:
            print('Failure in bell test - shutting down the server')
            requests.post("http://localhost:64574/shutdown")   
            time.sleep(2)    

    # def test_placement(self):
    #     p = self.qpu.defaultPlacementTransformation()
    #     placement = xacc.getIRTransformation(p)
    #     placement.apply(self.bell, None)
    #     print(self.bell.toString())



    @classmethod
    def tearDownClass(self):
        print('TearDownClass - shutting down the server')
        requests.post("http://localhost:64574/shutdown")   
        time.sleep(1) 
        self.process.wait() 

if __name__ == '__main__':
    unittest.main()