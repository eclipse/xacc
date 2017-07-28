/*=============================================================================

  Library: CppMicroServices

  Copyright (c) The CppMicroServices developers. See the COPYRIGHT
  file at the top-level directory of this distribution and at
  https://github.com/CppMicroServices/CppMicroServices/COPYRIGHT .

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

//! [Activator]
#include "TrivialEmbeddingAlgorithm.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 * This class implements a bundle activator that uses the bundle
 * context to register an English language dictionary service
 * with the C++ Micro Services registry during static initialization
 * of the bundle. The dictionary service interface is
 * defined in a separate file and is implemented by a nested class.
 */
class US_ABI_LOCAL EmbeddingAlgorithmActivator : public BundleActivator
{


public:

	EmbeddingAlgorithmActivator() {}

  /**
   * Implements BundleActivator::Start(). Registers an
	 * instance of a dictionary service using the bundle context;
	 * attaches properties to the service that can be queried
	 * when performing a service look-up.
	 * @param context the context for the bundle.
	 */
	void Start(BundleContext context) {
		auto trivialEmbService = std::make_shared<
				xacc::quantum::TrivialEmbeddingAlgorithm>();
		context.RegisterService<xacc::quantum::EmbeddingAlgorithm>(
				trivialEmbService);
	}

	/**
	 * Implements BundleActivator::Stop(). Does nothing since
	 * the C++ Micro Services library will automatically unregister any registered services.
	 * @param context the context for the bundle.
	 */
	void Stop(BundleContext /*context*/) {
		// NOTE: The service is automatically unregistered
	}

};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(EmbeddingAlgorithmActivator)
//![Activator]
