
#include "QCSAccelerator.hpp"
#include <algorithm>
#include <pybind11/embed.h>
#include "CountGatesOfTypeVisitor.hpp"
#include <pybind11/numpy.h>

#include <regex>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace py = pybind11;
using namespace py::literals;

namespace xacc {
namespace quantum {

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

bool QCSAccelerator::isValidBufferSize(const int NBits) {
  return NBits > 0;
}

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<Function> function) {
  auto visitor = std::make_shared<QuilVisitor>(true);

  auto backend = xacc::optionExists("qcs-backend") ? xacc::getOption("qcs-backend") : "9q-generic-qvm";
  auto shots = xacc::optionExists("qcs-shots") ? std::stoi(xacc::getOption("qcs-shots")) : 1000;

  std::map<int,int> bitToQubit;
  std::vector<int> tobesorted;
  InstructionIterator it(function);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
      if (nextInst->name() == "Measure") {
        // bitToQubit.insert({boost::get<int>(nextInst->getParameter(0)), nextInst->bits()[0]});
        tobesorted.push_back(nextInst->bits()[0]);
      }
    }
  }

  std::sort(tobesorted.begin(),tobesorted.end());
  bitToQubit.clear();
  for (int i = 0; i < tobesorted.size(); i++) {
      bitToQubit[i] = tobesorted[i];
  }

  // Create our usual old Quil string, but
  // then hack it to fit the new DECLARE stuff
  CountGatesOfTypeVisitor<Measure> count(function);
  int nMeasures = count.countGates();
  auto quilStr = visitor->getQuilString();
  std::cout << "QUIL\n" << quilStr << "\n";
//   quilStr = std::regex_replace(quilStr, std::regex("["), "ro[");
//   boost::replace_all(quilStr, "[", "ro[");
  quilStr = "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]\n" + quilStr;

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
//   py::print(shape[0]);
//   py::print(shape[1]);
//   py::print(results);
  std::string zeroString = "";
  for (int i = 0; i < buffer->size(); i++) zeroString += "0";

   for (int i = 0; i < shots; i++) {
       std::string bitString = zeroString;
       std::stringstream s;

       for (int j = 0; j < buffer->size(); j++) {
        //   auto qbit = bitToQubit[j];
          s << *results.data(i,j);
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
      auto tmpBuffer = createBuffer(f->name(),
                                    buffer->size());

      high_resolution_clock::time_point t1 = high_resolution_clock::now();
      xacc::info("Execution " + std::to_string(counter) + ": " + f->name());
      execute(tmpBuffer, f);
      high_resolution_clock::time_point t2 = high_resolution_clock::now();

      auto duration = duration_cast<microseconds>( t2 - t1 ).count();
      tmpBuffer->addExtraInfo("exec-time", ExtraInfo(duration*1e-6));
      tmpBuffers.push_back(tmpBuffer);
      counter++;
    }

    return tmpBuffers;
}

} // namespace quantum
} // namespace xacc
