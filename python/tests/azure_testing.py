import unittest as test
import xacc


class TestAzurePlugin(test.TestCase):


    def test_install(self):
      try:
        qpu = xacc.getAccelerator('azure')
      except Exception as e:
        if 'Invalid Accelerator' in e.__str__():
          xacc.error("Azure plugin not installed. Build XACC from source with the -DXACC")   


    def test_initialize(self):
      qpu = xacc.getAccelerator('azure')


    def test_name(self):
      qpu = xacc.getAccelerator('azure')
      self.assertEqual(qpu.name(), "azure")


    def test_update_configuration(self):
      qpu = xacc.getAccelerator('azure')
      qpu.updateConfiguration({"backend": "microsoft.estimator"})


    def test_execute(self):
      xacc.qasm('''.compiler xasm
      .circuit ansatz
      .qbit q
      H(q[0]);
      CX(q[0],q[1]);
      Measure(q[0]);
      Measure(q[1]);
      ''')
      ansatz = xacc.getCompiled('ansatz')
      buffer = xacc.qalloc(2)
      qpu = xacc.getAccelerator('azure', {"shots": 1024, "backend": "microsoft.estimator",  "visitor": "pyqir"})
      qpu.execute(buffer, ansatz)


if __name__ == '__main__':
    test.main()