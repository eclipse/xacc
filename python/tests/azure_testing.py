import unittest as test
import xacc

def check_plugin_install():
  try:
      f = open("../../build/CMakeCache.txt", "r").read()
  except FileNotFoundError:
      print("Could not find CMakeCache.txt in the build directory. Make sure you build XACC before running tests.")
      return True

  if 'XACC_AZURE_PLUGIN' not in f:
      return True
  else:
      line = [l for l in f.splitlines() if "XACC_AZURE_PLUGIN"  in l][0]
      if "OFF" in line:
          return True
      else:
          return False

@test.skipIf(check_plugin_install(), "Azure plugin not installed.")
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