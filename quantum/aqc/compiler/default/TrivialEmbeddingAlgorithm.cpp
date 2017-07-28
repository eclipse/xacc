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
#include "TrivialEmbeddingAlgorithm.hpp"

namespace xacc {
namespace quantum {

Embedding TrivialEmbeddingAlgorithm::embed(
		std::shared_ptr<DWGraph> problem,
		std::shared_ptr<xacc::AcceleratorGraph> hdware,
		std::map<std::string, std::string> params) {

	Embedding xaccEmbedding;
	bool failHard = true;
	if (params.count("failhard")) {
		failHard = params["failhard"] == "false" ? false : true;
	}

	for (int i = 0; i < problem->order(); i++) {
		for (int j = 0; j < problem->order(); j++) {
			if (i < j && i != j
					&& (problem->edgeExists(i, j) && !hdware->edgeExists(i, j))) {
				if (failHard) {
					XACCError(
							"Trivial Embedding not possible, there is no hardware edge corresponding to ("
									+ std::to_string(i) + ", "
									+ std::to_string(j) + ") problem edge.");
				} else {
					XACCInfo("This embedding failed, but user requested to not fail hard. Returning empty embedding.");
					xaccEmbedding.clear();
					return xaccEmbedding;
				}
			}
		}
		xaccEmbedding.insert(std::make_pair(i, std::vector<int>{i}));
	}

	return xaccEmbedding;
}

}
}
