/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_GATE_ACCELERATORS_RIGETTIACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_RIGETTIACCELERATOR_HPP_

#include "AcceleratorBuffer.hpp"
#include "QPUGate.hpp"
#include "InstructionIterator.hpp"
#include "QuilVisitor.hpp"
#include "AsioNetworkingTool.hpp"
#include "RuntimeOptions.hpp"
#include <boost/algorithm/string.hpp>

using namespace xacc;

namespace xacc {
namespace quantum {

/**
 */
class RigettiAccelerator : virtual public QPUGate {
public:

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string. This string serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId
	 * @return
	 */
	std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId);

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string and of the given number of bits.
	 * The string id serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId
	 * @param size
	 * @return
	 */
	std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId,
			const int size);

	/**
	 * Return true if this Accelerator can allocated
	 * NBits number of bits.
	 * @param NBits
	 * @return
	 */
	virtual bool isValidBufferSize(const int NBits);

	/**
	 *
	 * @param ir
	 */
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<xacc::Function> kernel);

	virtual std::shared_ptr<options_description> getOptions() {
		auto desc = std::make_shared<options_description>(
				"Rigetti Accelerator Options");
		desc->add_options()("api-key", value<std::string>(),
				"Provide the Rigetti Forest API key.")("type",
				value<std::string>(),
				"Provide the execution type: multishot, wavefunction, "
				"multishot-measure, ping, or version.")
				("trials", value<std::string>(), "Provide the number of trials to execute.");
		return desc;
	}

	/**
	 * Register this Accelerator with the framework.
	 */
	static void registerAccelerator() {
		xacc::RegisterAccelerator<xacc::quantum::RigettiAccelerator> RIGETTITEMP(
				"rigetti");
	}

	RigettiAccelerator() :
			httpClient(
					std::make_shared<
							fire::util::AsioNetworkingTool<SimpleWeb::HTTPS>>(
							"api.rigetti.com", false)) {
	}

	RigettiAccelerator(std::shared_ptr<fire::util::INetworkingTool> http) :
			httpClient(http) {
	}

	/**
	 * The destructor
	 */
	virtual ~RigettiAccelerator() {}

protected:

	std::shared_ptr<fire::util::INetworkingTool> httpClient;

};

// Create an alias to search for.
RegisterAccelerator(xacc::quantum::RigettiAccelerator)

}
}




#endif
