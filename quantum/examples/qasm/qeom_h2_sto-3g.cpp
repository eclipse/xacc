#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "OperatorPool.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

auto str = std::string(
      "(-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + "
      "(-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + "
      "(0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + "
      "(0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + "
      "(-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + "
      "(0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + "
      "(-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + "
      "(-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + "
      "(0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + "
      "(0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + "
      "(-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + "
      "(0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + "
      "(0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + "
      "(0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + "
      "(0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + "
      "(0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + "
      "(0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + "
      "(0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + "
      "(-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + "
      "(0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + "
      "(0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + "
      "(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + "
      "(0.7080240981,0)");
  auto H = xacc::quantum::getObservable("fermion", str);
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(4);

  auto pool = xacc::getService<xacc::quantum::OperatorPool>("singlet-adapted-uccsd");
  pool->optionalParameters({{"n-electrons", 2}});
  pool->generate(buffer->size());
  auto ansatz = xacc::getIRProvider("quantum")->createComposite("ansatz");
  ansatz->addInstruction(
      xacc::getIRProvider("quantum")->createInstruction("X", {0}));
  ansatz->addInstruction(
      xacc::getIRProvider("quantum")->createInstruction("X", {2}));
  ansatz->addVariable("x0");
  for (auto &k : pool->getOperatorInstructions(2, 0)->getInstructions()) {
    ansatz->addInstruction(k);
  }
  auto kernel = ansatz->operator()({0.0808});

  auto qeom = xacc::getService<xacc::Algorithm>("qeom");
  qeom->initialize({
      {"accelerator", acc},
      {"observable", H},
      {"n-electrons", 2},
      {"ansatz", kernel},
  });
  qeom->execute(buffer);

  auto e =
      buffer->getInformation("excitation-energies").as<std::vector<double>>();

  std::cout << "Excitation energies = ";
  for (int i = 0; i < e.size() - 1; i++) {
    std::cout << e[i] << ", ";
  }
  std::cout << e.back() << "\n";

  xacc::Finalize();

  return 0;
}