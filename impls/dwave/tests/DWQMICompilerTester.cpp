/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DWaveCompilerTester

#include <boost/test/included/unit_test.hpp>
#include "DWQMICompiler.hpp"
#include "XACC.hpp"
#include "AQCAcceleratorBuffer.hpp"
#include "DefaultParameterSetter.hpp"

using namespace xacc::quantum;

class FakeDWAcc : public xacc::Accelerator {
public:

	virtual std::shared_ptr<xacc::AcceleratorGraph> getAcceleratorConnectivity() {
		return K44Bipartite().getAcceleratorGraph();
	}

	virtual xacc::AcceleratorType getType() {
		return xacc::AcceleratorType::qpu_aqc;
	}

	virtual bool isValidBufferSize(const int nBits) {
		return true;
	}

	virtual std::vector<std::shared_ptr<xacc::IRTransformation>> getIRTransformations() {

	};

	virtual std::shared_ptr<xacc::AcceleratorBuffer> createBuffer(
			const std::string& varId) {
		auto b = std::make_shared<AQCAcceleratorBuffer>(varId, 1);
		storeBuffer(varId, b);
	}

	virtual std::vector<std::string> getAllocatedBufferNames() {
		std::vector<std::string> names;
		names.push_back("hello");
		return names;
	}

	virtual std::shared_ptr<xacc::AcceleratorBuffer> getBuffer(
				const std::string& varid) {
		return std::make_shared<AQCAcceleratorBuffer>("hello", 1);
	}

	virtual void initialize() {

	}
	/**
	 * Execute the provided XACC IR Function on the provided AcceleratorBuffer.
	 *
	 * @param buffer The buffer of bits this Accelerator should operate on.
	 * @param function The kernel to execute.
	 */
	virtual void execute(std::shared_ptr<xacc::AcceleratorBuffer> buffer,
				const std::shared_ptr<xacc::Function> function) {
	}

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string and of the given number of bits.
	 * The string id serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId The variable name of the created buffer
	 * @param size The number of bits in the created buffer
	 * @return buffer The buffer instance created.
	 */
	virtual std::shared_ptr<xacc::AcceleratorBuffer> createBuffer(
			const std::string& varId, const int size) {
		auto b = std::make_shared<AQCAcceleratorBuffer>(varId, size);
		storeBuffer(varId, b);
	}

	virtual const std::string name() const {
		return "";
	}

	virtual const std::string description() const {
		return "";
	}

};

class FakeEmbedding : public EmbeddingAlgorithm {
public:

	virtual Embedding embed(
			std::shared_ptr<DWGraph> problem, std::shared_ptr<xacc::AcceleratorGraph> hardware,
			std::map<std::string, std::string> params = std::map<std::string,
					std::string>()) override {
		Embedding embedding;
		embedding.insert(std::make_pair(0, std::vector<int>{0, 4}));
		embedding.insert(std::make_pair(1, std::vector<int>{1}));
		embedding.insert(std::make_pair(2, std::vector<int>{5}));
		return embedding;
	}

	virtual const std::string name() const {
		return "fake-embedding";
	}

	virtual const std::string description() const {
		return "";
	}

};

class Factoring15FakeEmbedding : public EmbeddingAlgorithm {
public:

	virtual Embedding embed(
			std::shared_ptr<DWGraph> problem, std::shared_ptr<xacc::AcceleratorGraph> hardware,
			std::map<std::string, std::string> params = std::map<std::string,
					std::string>()) override {
		Embedding embedding;
		embedding.insert(std::make_pair(0, std::vector<int>{0}));
		embedding.insert(std::make_pair(1, std::vector<int>{1}));
		embedding.insert(std::make_pair(2, std::vector<int>{2}));
		embedding.insert(std::make_pair(4, std::vector<int>{4}));
		embedding.insert(std::make_pair(5, std::vector<int>{5}));
		embedding.insert(std::make_pair(6, std::vector<int>{6}));
		return embedding;
	}

	virtual const std::string name() const {
		return "fake-factor15-embedding";
	}

	virtual const std::string description() const {
		return "";
	}


};

//
//BOOST_AUTO_TEST_CASE(checkSimpleCompile) {
//
//	DefaultParameterSetter setter;
//	EmbeddingAlgorithmRegistry::instance()->add(FakeEmbedding().name(),
//			std::make_shared<EmbeddingAlgorithmRegistry::CreatorFunction>(
//					[]() {return std::make_shared<FakeEmbedding>();}));
//
//	auto compiler = std::make_shared<DWQMICompiler>();
//
//	const std::string simpleQMI =
//			"__qpu__ dwaveKernel() {\n"
//			"   0 0 0.98\n"
//			"   1 1 .33\n"
//			"   2 2 .44\n"
//			"   0 1 .22\n"
//			"   0 2 .55\n"
//			"   1 2 .11\n"
//			"}";
//
//	auto options = xacc::RuntimeOptions::instance();
//	options->insert(std::make_pair("dwave-embedding", "fake-embedding"));
//
//	auto acc = std::make_shared<FakeDWAcc>();
//
//	auto ir = compiler->compile(simpleQMI, acc);
//
//	auto qmi = ir->getKernel("dwaveKernel")->toString("");
//
//	const std::string expectedQMI = "0 0 0.49\n"
//			"4 4 0.49\n"
//			"1 1 0.33\n"
//			"5 5 0.44\n"
//			"0 4 -1.75\n"
//			"0 5 0.55\n"
//			"1 4 0.22\n"
//			"1 5 0.11\n";
//
//	std::cout << "\n" << qmi << "\n";
//	BOOST_VERIFY(expectedQMI == qmi);
//
//}

BOOST_AUTO_TEST_CASE(checkFactoring15OneToOneMapping) {

        xacc::Initialize();
	auto compiler = std::make_shared<DWQMICompiler>();

	const std::string factoring15QMI =
			"__qpu__ factor15() {\n"
			"   0 0 20\n"
			"   1 1 50\n"
			"   2 2 60\n"
			"   4 4 50\n"
			"   5 5 60\n"
			"   6 6 -160\n"
			"   1 4 -1000\n"
			"   2 5 -1000\n"
			"   0 4 -14\n"
			"   0 5 -12\n"
			"   0 6 32\n"
			"   1 5 68\n"
			"   1 6 -128\n"
			"   2 6 -128\n"
			"}";

	auto options = xacc::RuntimeOptions::instance();
	if (options->exists("dwave-embedding")) {
		(*options)["dwave-embedding"] = "trivial";
	} else {
		options->insert(std::make_pair("dwave-embedding", "trivial"));
	}

	auto acc = std::make_shared<FakeDWAcc>();

	auto ir = compiler->compile(factoring15QMI, acc);

	auto qmi = ir->getKernel("factor15")->toString("");

	std::cout << qmi << "\n";

	const std::string expected =
		"0 0 20\n"
		"1 1 50\n"
		"2 2 60\n"
		"3 3 0\n"
		"4 4 50\n"
		"5 5 60\n"
		"6 6 -160\n"
		"0 4 -14\n"
		"0 5 -12\n"
		"0 6 32\n"
		"1 4 -1000\n"
		"1 5 68\n"
		"1 6 -128\n"
		"2 5 -1000\n"
		"2 6 -128\n";

	std::cout << "QMI:\n" << qmi << "\n";
	BOOST_VERIFY(expected == qmi);
        xacc::Finalize();
}
