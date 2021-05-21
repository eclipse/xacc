#include "xacc.hpp"

int main(int argc, char **argv) {

    xacc::Initialize(argc, argv);
    xacc::external::load_external_language_plugins();
    xacc::set_verbose(true);

    auto accelerator = xacc::getAccelerator(
        "ibm", {std::make_pair("backend", "lowest-queue-count"),
                std::make_pair("n-qubits", 5),
                std::make_pair("check-jobs-limit", true)}
                );

    xacc::qasm(R"(
  .compiler xasm
  .circuit bell
  .qbit q
  H(q[0]);
  CX(q[0],q[1]);
  Measure(q[0]);
  Measure(q[1]);
  )");
    auto bell = xacc::getCompiled("bell");

    // Allocate some qubits and execute
    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, bell);

    buffer->print();

    xacc::external::unload_external_language_plugins();
    xacc::Finalize();

    return 0;
}
