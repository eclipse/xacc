import unittest as test
import sys
import xacc

class TestGateInstructions(test.TestCase):

    def test_X(self):
        x = xacc.gate.create("X", [0])
        x2 = xacc.gate.create("X", [1])
        self.assertEqual(x.name(), "X")
        self.assertEqual(x.bits(), [0])
        self.assertEqual(x2.bits(), [1])
        self.assertEqual(x.toString("qreg"), "X qreg0")
        self.assertFalse(x.isComposite())

    def test_Y(self):
        y = xacc.gate.create("Y", [2])
        self.assertEqual(y.name(), "Y")
        self.assertEqual(y.bits(), [2])
        self.assertEqual(y.toString("qreg"), "Y qreg2")
        self.assertFalse(y.isComposite())

    def test_Z(self):
        z = xacc.gate.create("Z", [3])
        self.assertEqual(z.name(), "Z")
        self.assertEqual(z.toString("qreg"), "Z qreg3")
        self.assertEqual(z.bits(), [3])

        self.assertFalse(z.isComposite())

    def test_Hadamard(self):

        h = xacc.gate.create("H", [2])
        self.assertEqual(h.name(), "H")
        self.assertEqual(h.toString("qreg"), "H qreg2")
        self.assertEqual(h.bits(), [2])

    def test_Rx(self):
        rx = xacc.gate.create("Rx", [2])
        rx.setParameter(0, "t0")
        self.assertEqual(rx.name(), "Rx")
        self.assertEqual(rx.bits(), [2])
        self.assertEqual(rx.nParameters(), 1)
        self.assertEqual(rx.getParameters(), ["t0"])
        self.assertEqual(rx.getParameter(0), "t0")
        self.assertEqual(rx.toString("qreg"), "Rx(t0) qreg2")
        rx.setParameter(0, 1.441)
        self.assertEqual(rx.toString("qreg"), "Rx(1.441) qreg2")
        self.assertFalse(rx.isComposite())

    def test_Rz(self):
        rz = xacc.gate.create("Rz", [1])
        rz.setParameter(0, "theta")
        self.assertEqual(rz.name(), "Rz")
        self.assertEqual(rz.bits(), [1])
        self.assertEqual(rz.nParameters(), 1)
        self.assertEqual(rz.getParameters(), ["theta"])
        self.assertEqual(rz.toString("qreg"), "Rz(theta) qreg1")
        self.assertEqual(rz.getParameter(0), "theta")

        self.assertFalse(rz.isComposite())

    def test_Ry(self):
        ry = xacc.gate.create("Ry", [3])
        ry.setParameter(0, "psi")
        self.assertEqual(ry.name(), "Ry")
        self.assertEqual(ry.bits(), [3])
        self.assertEqual(ry.nParameters(), 1)
        self.assertEqual(ry.getParameters(), ["psi"])
        self.assertEqual(ry.getParameter(0), "psi")
        self.assertEqual(ry.toString("qreg"), "Ry(psi) qreg3")
        self.assertFalse(ry.isComposite())

    def test_CNOT(self):

        cn = xacc.gate.create("CNOT", [1, 2])
        self.assertEqual(cn.name(), "CNOT")
        self.assertEqual(cn.bits(), [1, 2])
        self.assertEqual(cn.nParameters(), 0)
        self.assertEqual(cn.toString("qreg"), "CNOT qreg1,qreg2")
        self.assertFalse(cn.isComposite())

    def test_CZ(self):
        cz = xacc.gate.create("CZ", [2, 3])

        self.assertEqual(cz.name(), "CZ")
        self.assertEqual(cz.bits(), [2, 3])
        self.assertEqual(cz.nParameters(), 0)

        self.assertEqual(cz.toString("qreg"), "CZ qreg2,qreg3")

    def test_Measure(self):
        m = xacc.gate.create("Measure", [4])
        m.setParameter(0, 3)
        self.assertEqual(m.getParameter(0), 3)
        self.assertEqual(m.name(), "Measure")
        self.assertEqual(m.bits(), [4])
        self.assertEqual(m.nParameters(), 1)
        self.assertEqual(m.toString("qreg"), "Measure qreg4")

    def test_Identity(self):
        i = xacc.gate.create("I", [2])

        self.assertEqual(i.name(), "I")
        self.assertEqual(i.bits(), [2])
        self.assertEqual(i.nParameters(), 0)

        self.assertEqual(i.toString("qreg"), "I qreg2")

    def test_CPhase(self):
        cp = xacc.gate.create("CPhase", [3, 4], [3.14])

        self.assertEqual(cp.name(), "CPhase")
        self.assertEqual(cp.bits(), [3, 4])
        self.assertEqual(cp.nParameters(), 1)
        self.assertEqual(cp.getParameter(0), 3.14)
        self.assertEqual(cp.getParameters(), [3.14])
        self.assertEqual(cp.toString("qreg"), "CPhase(3.14) qreg3,qreg4")

    def test_Swap(self):
        sw = xacc.gate.create("Swap", [1, 5])
        sw2 = xacc.gate.create("Swap", [43, 54])

        self.assertEqual(sw.name(), "Swap")
        self.assertEqual(sw2.name(), "Swap")
        self.assertEqual(sw.bits(), [1, 5])
        self.assertEqual(sw2.bits(), [43, 54])
        self.assertEqual(sw.nParameters(), 0)
        self.assertEqual(sw2.nParameters(), 0)

        self.assertEqual(sw.toString("qreg"), "Swap qreg1,qreg5")
        self.assertEqual(sw2.toString("qreg"), "Swap qreg43,qreg54")


class TestGateFunctions(test.TestCase):

    def test_GateFunctionInstructions(self):

        expQasm = "H qreg1\nCNOT qreg1,qreg2\nCNOT qreg0,qreg1\nH qreg0\n"

        f = xacc.gate.createFunction("foo", [0, 1, 2])

        h = xacc.gate.create("H", [1])
        cn = xacc.gate.create("CNOT", [1, 2])
        cn2 = xacc.gate.create("CNOT", [0, 1])
        h2 = xacc.gate.create("H", [0])

        f.addInstruction(h)
        f.addInstruction(cn)
        f.addInstruction(cn2)
        f.addInstruction(h2)

        self.assertEqual(f.nInstructions(), 4)
        self.assertEqual(f.nParameters(), 0)

        self.assertEqual(f.toString("qreg"), expQasm)

        f.removeInstruction(0)

        self.assertEqual(f.nInstructions(), 3)

        newQasm = "CNOT qreg1,qreg2\nCNOT qreg0,qreg1\nH qreg0\n"

        self.assertEqual(f.toString("qreg"), newQasm)

        f.replaceInstruction(0, h)

        newQasm2 = "H qreg1\nCNOT qreg0,qreg1\nH qreg0\n"

        self.assertEqual(f.toString("qreg"), newQasm2)

        f.insertInstruction(0, cn)

        newQasm3 = "CNOT qreg1,qreg2\nH qreg1\nCNOT qreg0,qreg1\nH qreg0\n"


    def test_GateFunctionParameters(self):
        # add parameterized GateInstructions to a GateFunction

        expQasm = "Rz(theta) qreg1\nRy(psi) qreg2\n"
        f = xacc.gate.createFunction("foo", [])

        rz = xacc.gate.create("Rz", [1])
        ry = xacc.gate.create("Ry", [2])
        rz.setParameter(0, "theta")
        ry.setParameter(0, "psi")

        f.addInstruction(rz)
        f.addInstruction(ry)

        self.assertEqual(f.nInstructions(), 2)
        self.assertEqual(f.nParameters(), 2)
        self.assertEqual(f.getParameters(), ["theta", "psi"])
        self.assertEqual(f.getParameter(0), "theta")
        self.assertEqual(f.getParameter(1), "psi")
        self.assertEqual(f.toString("qreg"), expQasm)

        # Evaluate the GateFunction
        values = [3.14, 1.45]

        eval = f.eval(values)
        newQasm = "Rz(3.14) qreg1\nRy(1.45) qreg2\n"

        self.assertEqual(eval.toString("qreg"), newQasm)

        rx = xacc.gate.create("Rx", [2])
        rx.setParameter(0, "theta")
        # add a GateInstruction with a duplicate parameter
        f.addInstruction(rx)

        self.assertEqual(f.nInstructions(), 3)
        self.assertEqual(f.nParameters(), 2)
        self.assertEqual(f.getParameter(0), "theta")
        self.assertEqual(f.getParameter(1), "psi")

        # evaluate the new GateFunction again
        eval2 = f.eval(values)
        newQasm2 = "Rz(3.14) qreg1\nRy(1.45) qreg2\nRx(3.14) qreg2\n"

        self.assertEqual(eval2.toString("qreg"), newQasm2)

        rz2 = xacc.gate.create("Rz", [0])
        rz2.setParameter(0, "0.5 * psi")
        # add a GateInstruction with a duplicate parameter (with a coefficient)

        f.addInstruction(rz2)

        self.assertEqual(f.nParameters(), 2)
        self.assertEqual(f.getParameter(0), "theta")
        self.assertEqual(f.getParameter(1), "psi")

        newQasm3 = "Rz(3.14) qreg1\nRy(1.45) qreg2\nRx(3.14) qreg2\nRz(0.725) qreg0\n"
        eval3 = f.eval(values)

        self.assertEqual(eval3.toString("qreg"), newQasm3)

        f.removeInstruction(3)
        f.removeInstruction(2)

        f.replaceInstruction(1, rz2)

        newQasm4 = "Rz(theta) qreg1\nRz(0.5 * psi) qreg0\n"
        self.assertEqual(f.nInstructions(), 2)
        self.assertEqual(f.nParameters(), 2)

        self.assertEqual(f.getParameter(0), "theta")
        self.assertEqual(f.getParameter(1), "psi")
        self.assertEqual(f.toString("qreg"), newQasm4)


    def test_CoefficientParameters(self):
        # Testing GateInstructions with duplicate parameters and coefficients
        f = xacc.gate.createFunction("foo", [])

        rz = xacc.gate.create("Rz", [2])
        rx = xacc.gate.create("Rx", [1])
        rx.setParameter(0, "psi")
        rz.setParameter(0, "1 * psi")

        f.addInstruction(rz)

        self.assertEqual(f.nParameters(), 1)
        self.assertEqual(f.getParameter(0), "psi")

        f.addInstruction(rx)

        self.assertEqual(f.nParameters(), 1)
        self.assertEqual(f.getParameter(0), "psi")
        self.assertEqual(f.nInstructions(), 2)

        f.removeInstruction(1)
        f.removeInstruction(0)

        ry = xacc.gate.create("Ry", [0])
        rx2 = xacc.gate.create("Rx", [1])

        ry.setParameter(0, "0.5 * theta")
        rx2.setParameter(0, "theta")
        f.addInstruction(ry)
        f.addInstruction(rx2)

        self.assertEqual(f.nParameters(), 1)
        self.assertEqual(f.getParameter(0), "theta")

    def testManyParameterizedGates(self):

        f = xacc.gate.createFunction("foo", [])

        rz = xacc.gate.create("Rz", [1])
        rx = xacc.gate.create("Rx", [2])
        ry = xacc.gate.create("Ry", [3])
        rz2 = xacc.gate.create("Rz", [2])
        rx2 = xacc.gate.create("Rx", [3])

        rz.setParameter(0, "theta")
        rx.setParameter(0, "1 * theta")
        ry.setParameter(0, "phi")
        rz2.setParameter(0, "psi")
        rx2.setParameter(0, "0.5 * psi")

        f.addInstruction(rz)
        f.addInstruction(rx)
        f.addInstruction(ry)
        f.addInstruction(rz2)
        f.addInstruction(rx2)

        expQasm = "Rz(theta) qreg1\nRx(1 * theta) qreg2\nRy(phi) qreg3\nRz(psi) qreg2\nRx(0.5 * psi) qreg3\n"

        self.assertEqual(f.toString("qreg"), expQasm)
        self.assertEqual(f.nParameters(), 3)
        self.assertEqual(f.nInstructions(), 5)

        ry2 = xacc.gate.create("Ry", [3])
        ry2.setParameter(0, "theta")

        f.replaceInstruction(1, ry2)

        newQasm = "Rz(theta) qreg1\nRy(theta) qreg3\nRy(phi) qreg3\nRz(psi) qreg2\nRx(0.5 * psi) qreg3\n"
        self.assertEqual(f.toString("qreg"), newQasm)
        self.assertEqual(f.nParameters(), 3)
        self.assertEqual(f.nInstructions(), 5)





if __name__ == '__main__':
    xacc.Initialize()
    test.main()
    xacc.Finalize()