
#include "QCSAccelerator.hpp"
#include <algorithm>
#include <pybind11/embed.h>
#include "CountGatesOfTypeVisitor.hpp"
#include <pybind11/numpy.h>

#include "xacc_service.hpp"
#include "EmbeddingAlgorithm.hpp"

#include <regex>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace py = pybind11;
using namespace py::literals;

namespace xacc {
namespace quantum {

std::shared_ptr<IR> MapToPhysical::transform(std::shared_ptr<IR> ir) {

  auto embeddingAlgorithm = xacc::getService<EmbeddingAlgorithm>("cmr");

  std::map<int, int> physical2Logical, logical2Physical;
  int counter = 0;
  std::set<int> nUniqueBits;
  for (auto &edge : _edges) {
    nUniqueBits.insert(edge.first);
    nUniqueBits.insert(edge.second);
  }

  for (auto &i : nUniqueBits) {
    physical2Logical.insert({i, counter});
    logical2Physical.insert({counter,i});
    counter++;
  }

  int nBits = nUniqueBits.size();

  auto hardwareGraph = xacc::getService<Graph>("boost-ugraph");
  for (int i = 0; i < nBits; i++) {
    std::map<std::string, InstructionParameter> m{{"bias", 1.0}};
    hardwareGraph->addVertex(m);
  }
  for (auto &edge : _edges) {
    hardwareGraph->addEdge(physical2Logical[edge.first],
                           physical2Logical[edge.second]);
  }

  //hardwareGraph->write(std::cout);
  for (auto &function : ir->getKernels()) {
    auto logicalGraph = function->toGraph();
    InstructionIterator it(function);
    std::set<int> nUniqueProbBits;

    std::vector<std::pair<int, int>> probEdges;
    while (it.hasNext()) {
      // Get the next node in the tree
      auto nextInst = it.next();
      if (nextInst->isEnabled() && nextInst->bits().size() == 2) {
          probEdges.push_back({nextInst->bits()[0], nextInst->bits()[1]});
          nUniqueProbBits.insert(nextInst->bits()[0]);
          nUniqueProbBits.insert(nextInst->bits()[1]);
      }
    }

    auto nProbBits = nUniqueProbBits.size();
    auto problemGraph = xacc::getService<Graph>("boost-ugraph");

    for (int i = 0; i < nProbBits; i++) {
      std::map<std::string, InstructionParameter> m{{"bias", 1.0}};
      problemGraph->addVertex(m);
    }

    for (auto &inst : probEdges) {
      problemGraph->addEdge(inst.first, inst.second, 1.0);
    }

//    std::cout << "\n";
  //  problemGraph->write(std::cout);

    // Compute the minor graph embedding
    auto embedding = embeddingAlgorithm->embed(problemGraph, hardwareGraph);
    embedding.persist(std::cout);
    std::vector<int> physicalMap;
    for (auto& kv : embedding) {
        if (kv.second.size() > 1) {
            xacc::error("Invalid logical to physical qubit mapping.");
        }
        physicalMap.push_back(logical2Physical[kv.second[0]]);
    }

    function->mapBits(physicalMap);

  }

  return ir;
}

std::shared_ptr<AcceleratorBuffer>
QCSAccelerator::createBuffer(const std::string &varId) {
  auto buffer = std::make_shared<AcceleratorBuffer>(varId, 30);
  storeBuffer(varId, buffer);
  return buffer;
}

std::shared_ptr<AcceleratorBuffer>
QCSAccelerator::createBuffer(const std::string &varId, const int size) {
  std::shared_ptr<AcceleratorBuffer> buffer;
  buffer = std::make_shared<AcceleratorBuffer>(varId, size);
  storeBuffer(varId, buffer);
  return buffer;
}

bool QCSAccelerator::isValidBufferSize(const int NBits) { return NBits > 0; }

void QCSAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                             const std::shared_ptr<Function> function) {
  auto visitor = std::make_shared<QuilVisitor>(true);

  auto backend = xacc::optionExists("qcs-backend")
                     ? xacc::getOption("qcs-backend")
                     : "9q-generic-qvm";
  auto shots = xacc::optionExists("qcs-shots")
                   ? std::stoi(xacc::getOption("qcs-shots"))
                   : 1000;

  std::map<int, int> bitToQubit;
  std::vector<int> tobesorted;
  InstructionIterator it(function);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
      if (nextInst->name() == "Measure") {
        bitToQubit.insert(
            {mpark::get<int>(nextInst->getParameter(0)), nextInst->bits()[0]});
        tobesorted.push_back(nextInst->bits()[0]);
      }
    }
  }

  // std::sort(tobesorted.begin(),tobesorted.end());
  // bitToQubit.clear();
  // for (int i = 0; i < tobesorted.size(); i++) {
  //    bitToQubit[i] = tobesorted[i];
  //}

  // Create our usual old Quil string, but
  // then hack it to fit the new DECLARE stuff
  CountGatesOfTypeVisitor<Measure> count(function);
  int nMeasures = count.countGates();
  auto quilStr = visitor->getQuilString();
  //   quilStr = std::regex_replace(quilStr, std::regex("["), "ro[");
  //   boost::replace_all(quilStr, "[", "ro[");
  quilStr =
      "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]\n" + quilStr;

  std::shared_ptr<py::scoped_interpreter> guard;
  if (!xacc::isPyApi) {
    guard = std::make_shared<py::scoped_interpreter>();
  }

  //   py::print("quil:\n", quilStr);
  auto pyquil = py::module::import("pyquil");
  py::object get_qc = pyquil.attr("get_qc");
  auto program = pyquil.attr("Program")();
  program.attr("inst")(quilStr);
  program.attr("wrap_in_numshots_loop")(shots);

  auto qc = get_qc(backend);

  auto compiled = qc.attr("compile")(program);

  py::array_t<int> results = qc.attr("run")(compiled);
  auto shape = results.request().shape;
  // py::print(shape[0]);
  // py::print(shape[1]);
  // py::print(results);
  //  py::print("QUIL\n");
  // py::print(quilStr);
  // py::print(buffer->size());
  std::string zeroString = "";
  for (int i = 0; i < buffer->size(); i++)
    zeroString += "0";

  for (int i = 0; i < shots; i++) {
    std::string bitString = zeroString;
    std::stringstream s;

    for (int j = 0; j < buffer->size(); j++) {
      //   auto qbit = bitToQubit[j];
      s << *results.data(i, j);
      //   bitString[buffer->size()-1-qbit] = s.str()[0];
    }
    bitString = s.str();
    std::reverse(bitString.begin(), bitString.end());
    //    if (i<4) py::print("adding " + bitString);
    buffer->appendMeasurement(bitString);
  }

  return;
}

std::vector<std::shared_ptr<AcceleratorBuffer>> QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<Function>> functions) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> tmpBuffers;
  int counter = 1;
  for (auto f : functions) {
    auto tmpBuffer = createBuffer(f->name(), buffer->size());

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    xacc::info("Execution " + std::to_string(counter) + ": " + f->name());
    execute(tmpBuffer, f);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(t2 - t1).count();
    tmpBuffer->addExtraInfo("exec-time", ExtraInfo(duration * 1e-6));
    tmpBuffers.push_back(tmpBuffer);
    counter++;
  }

  return tmpBuffers;
}

} // namespace quantum
} // namespace xacc
