#include "DWDecorator.hpp"
#include "EmbeddingAlgorithm.hpp"
#include "xacc.hpp"

#include <algorithm>
#include "AnnealingProgram.hpp"

namespace xacc {

namespace quantum {

void DWDecorator::searchAPIKey(std::string &key) {

  // Search for the API Key in $HOME/.dwave_config,
  // $DWAVE_CONFIG, or in the command line argument --dwave-api-key
  std::string dwaveConfig(std::string(getenv("HOME")) + "/.dwave_config");

  if (xacc::fileExists(dwaveConfig)) {
    findApiKeyInFile(key, dwaveConfig);
  } else if (const char *nonStandardPath = getenv("DWAVE_CONFIG")) {
    std::string nonStandardDwaveConfig(nonStandardPath);
    findApiKeyInFile(key, nonStandardDwaveConfig);
  }

  // If its still empty, then we have a problem
  if (key.empty()) {
    xacc::error("Error. The API Key is empty. Please place it "
                "in your $HOME/.dwave_config file, $DWAVE_CONFIG env var, "
                "or provide --dwave-api-key argument.");
  }
}

void DWDecorator::findApiKeyInFile(std::string &apiKey, const std::string &p) {
  std::ifstream stream(p);
  std::string contents((std::istreambuf_iterator<char>(stream)),
                       std::istreambuf_iterator<char>());

  std::vector<std::string> lines;
  lines = xacc::split(contents, '\n');
  for (auto l : lines) {
    if (l.find("key") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto key = split[1];
      xacc::trim(key);
      apiKey = key;
    }
  }
}

std::vector<std::pair<int, int>> DWDecorator::getConnectivity() {
  sapi_Problem *adj = NULL;
  auto code = sapi_getHardwareAdjacency(solver, &adj);
  std::vector<std::pair<int, int>> ret;
  for (int i = 0; i < adj->len; i++) {
    ret.push_back({adj->elements[i].i, adj->elements[i].j});
  }
  return ret;
}

void DWDecorator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                          const std::shared_ptr<CompositeInstruction> problem) {

  // Compute embedding
  // ------------------------------
  Embedding embedding;
  auto probGraph = problem->toGraph();
  auto hardwareEdges = getConnectivity();
  int num_variables = probGraph->order();
  auto d = std::dynamic_pointer_cast<AnnealingProgram>(problem);
  int maxBit = 0;
  for (auto &e : hardwareEdges) {
    if (e.first > maxBit) {
      maxBit = e.first;
    }
    if (e.second > maxBit) {
      maxBit = e.second;
    }
  }

  auto hardwareGraph = xacc::getService<Graph>("boost-ugraph");
  for (int i = 0; i < maxBit + 1; i++) {
    HeterogeneousMap props{std::make_pair("bias", 1.0)};
    hardwareGraph->addVertex(props); //
  }

  std::vector<sapi_ProblemEntry> adjData;
  for (auto &e : hardwareEdges) {
    hardwareGraph->addEdge(e.first, e.second);
    adjData.emplace_back(sapi_ProblemEntry{e.first, e.second, 0.0});
  }
  auto adj = sapi_Problem{adjData.data(), adjData.size()};

  if (!buffer->hasExtraInfoKey("embedding")) {

    auto embeddingAlgo = xacc::getService<EmbeddingAlgorithm>("cmr");
    embedding = embeddingAlgo->embed(probGraph, hardwareGraph);
  } else {
    std::cout << "using existing embedding\n";
    auto tmppp = buffer->getInformation("embedding")
                     .as<std::map<int, std::vector<int>>>();
    for (auto &kv : tmppp) {
      embedding.insert(kv);
    }
    embedding.persist(std::cout);
  }
  // ------------------------------

  // embed problem
  // ------------------------------
  std::vector<sapi_ProblemEntry> problemData;
  for (auto &pInst : problem->getInstructions()) {
    problemData.emplace_back(
        sapi_ProblemEntry{(int)pInst->bits()[0], (int)pInst->bits()[1],
                          pInst->getParameter(0).as<double>()});
  }
  auto sapiProblem = sapi_Problem{problemData.data(), problemData.size()};

  std::vector<int> embData(hardwareGraph->order(), -1);
  for (auto &kv : embedding) {
    for (auto &ii : kv.second) {
      embData[ii] = kv.first;
    }
  }
  auto sapiEmbeddings = sapi_Embeddings{embData.data(), embData.size()};

  sapi_EmbedProblemResult *r;

  auto code = sapi_embedProblem(&sapiProblem, &sapiEmbeddings, &adj, false,
                                false, 0, &r, 0);
  // ------------------------------

  std::vector<sapi_ProblemEntry> tmpEntries(r->problem.len + r->jc.len);
  sapi_Problem embedded_problem = {tmpEntries.data(), tmpEntries.size()};

  /* store embedded problem result in new problem */
  for (int i = 0; i < r->problem.len; i++) {
    embedded_problem.elements[i].i = r->problem.elements[i].i;
    embedded_problem.elements[i].j = r->problem.elements[i].j;
    embedded_problem.elements[i].value = r->problem.elements[i].value;
  }
  for (int i = 0; i < embedded_problem.len; i++) {
    embedded_problem.elements[i].i = r->jc.elements[i - r->problem.len].i;
    embedded_problem.elements[i].j = r->jc.elements[i - r->problem.len].j;
    /* set value differently in each loop iteration below */
  }

  for (int i = r->problem.len; i < embedded_problem.len; i++) {
    embedded_problem.elements[i].value = chain_strength;
  }
  // Execute embedded problem
  // ------------------------------
  sapi_QuantumSolverParameters solver_params =
      SAPI_QUANTUM_SOLVER_DEFAULT_PARAMETERS;
  solver_params.num_reads = shots;
  sapi_Solver *solver = NULL;
  sapi_Connection *connection = NULL;
  char err_msg[SAPI_ERROR_MESSAGE_MAX_SIZE];
  const sapi_SolverProperties *solver_properties = NULL;
  sapi_IsingResult *answer = NULL;
  sapi_globalInit();
  code = sapi_remoteConnection("https://cloud.dwavesys.com/sapi",
                               apiKey.c_str(), NULL, &connection, err_msg);
  solver = sapi_getSolver(connection, "DW_2000Q_VFYC_2_1");
  solver_properties = sapi_getSolverProperties(solver);
  code = sapi_solveIsing(solver, &embedded_problem,
                         (sapi_SolverParameters *)&solver_params, &answer,
                         err_msg);

  int *new_solutions = NULL;
  int *nsr;
  size_t num_new_solutions;
  std::vector<int> nsrtmp(answer->num_solutions * num_variables);
  nsr = nsrtmp.data();
  new_solutions = nsr;
  code = sapi_unembedAnswer(answer->solutions, answer->solution_len,
                            answer->num_solutions, &sapiEmbeddings,
                            SAPI_BROKEN_CHAINS_MINIMIZE_ENERGY, &sapiProblem,
                            new_solutions, &num_new_solutions, err_msg);

  int first;
  std::map<std::string, int> measurements;
  for (int i = 0; i < num_new_solutions; ++i) {
    printf("solution %d:\n", (int)i);
    std::stringstream ss;
    for (int j = 0; j < num_variables; ++j) {
      ss << (new_solutions[i * num_variables + j] == -1
                 ? 0
                 : new_solutions[i * num_variables + j]);
    }
    std::cout << ss.str() << "\n";
    buffer->appendMeasurement(ss.str(), answer->num_occurrences[i]);
    if (measurements.count(ss.str())) {
      measurements[ss.str()] += answer->num_occurrences[i];
    } else {
      measurements.insert({ss.str(), answer->num_occurrences[i]});
    }
  }
  buffer->setMeasurements(measurements);
  std::vector<double> energies;
  std::vector<int> numOccurrences;
  for (int i = 0; i < num_new_solutions; i++) {
    energies.push_back(answer->energies[i]);
    numOccurrences.push_back(answer->num_occurrences[i]);
  }

  buffer->addExtraInfo("energies", energies);
  buffer->addExtraInfo("num_occurrences", numOccurrences);

}

void DWDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {
  for (auto &f : functions) {
    // FIXME MAKE CHILD BUFFER
    execute(buffer, f);
  }
}

} // namespace quantum
} // namespace xacc