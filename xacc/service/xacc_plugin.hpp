#pragma once

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
using namespace cppmicroservices;

namespace {

// Define a templated Activator that enables easy 
// creation of a concrete Activator that registers 
// SubType instance to InterfaceType.
template <typename SubType, typename InterfaceType>
class US_ABI_LOCAL ConcreteActivator : public BundleActivator {
 public:
  ConcreteActivator() {}
  void Start(BundleContext context) {
    auto concrete_instance = std::make_shared<SubType>();
    context.RegisterService<InterfaceType>(concrete_instance);
  }
  void Stop(BundleContext /*context*/) {}
};
}  // namespace

#define REGISTER_PLUGIN(SubType, InterfaceType)                           \
  using SubTypeActivatorType = ConcreteActivator<SubType, InterfaceType>; \
  CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(SubTypeActivatorType)

// Convenience Macros...
#define REGISTER_OPTIMIZER(SubType) REGISTER_PLUGIN(SubType, xacc::Optimizer)
#define REGISTER_COMPILER(SubType) REGISTER_PLUGIN(SubType, xacc::Compiler)
#define REGISTER_IRTRANSFORMATION(SubType) REGISTER_PLUGIN(SubType, xacc::IRTransformation)
#define REGISTER_ACCELERATOR(SubType) REGISTER_PLUGIN(SubType, xacc::Accelerator)
#define REGISTER_ACCELERATOR_DECORATOR(SubType) REGISTER_PLUGIN(SubType, xacc::AcceleratorDecorator)
#define REGISTER_ALGORITHM(SubType) REGISTER_PLUGIN(SubType, xacc::Algorithm)
