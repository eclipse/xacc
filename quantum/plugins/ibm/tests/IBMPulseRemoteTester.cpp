#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include "Pulse.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(IBMPulseRemoteTester, checkPulseInst) {
    xacc::set_verbose(true);
    auto acc = xacc::getAccelerator("ibm:ibmq_armonk");
    auto buffer = xacc::qalloc(1);
    auto provider = xacc::getService<xacc::IRProvider>("quantum");
    auto f = provider->createComposite("tmp");
    const size_t nbSamples = 100;
    const std::vector<std::vector<double>> samples(nbSamples, { 1.0 });
    auto pulse = std::make_shared<xacc::quantum::Pulse>("square", "d0");
    pulse->setSamples(samples);
    xacc::contributeService("square", pulse);
    
    
    auto pulseInst = provider->createInstruction("square", {0});

    f->addInstruction(pulseInst);

    acc->execute(buffer, f);
    buffer->print();
}


int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}