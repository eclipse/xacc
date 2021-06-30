#include "xacc.hpp"
#include <fstream>

int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);
    // State-vector simulator: "aer", "qpp"
    auto accelerator = xacc::getAccelerator("aer");
    auto compiler = xacc::getCompiler("staq");
    const std::string QASM_SRC_FILE = std::string(QASM_SOURCE_DIR) + "/qasm_src.txt";
    // Read source file:
    std::ifstream inFile;
    inFile.open(QASM_SRC_FILE);
    std::stringstream strStream;
    strStream << inFile.rdbuf();
    const std::string qasmSrcStr = strStream.str();
   
    // Compile:
    xacc::ScopeTimer timer("compile", false);
    auto IR = compiler->compile(qasmSrcStr);
    auto program = IR->getComposites()[0];
    std::cout << "Number of instructions: " << program->nInstructions() << "\n";
    std::cout << "Elapsed time: " << timer.getDurationMs() << " ms.\n";
    // Allocate some qubits and execute:
    auto buffer = xacc::qalloc(10);
    accelerator->execute(buffer, program);
    buffer->print();
    xacc::Finalize();
}