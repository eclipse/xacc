#include "DWDecorator.hpp"
#include "EmbeddingAlgorithm.hpp"
#include "xacc.hpp"

#include <algorithm>
#include "AnnealingProgram.hpp"

#include "dwave_sapi.h"

namespace {
namespace base64 {

const unsigned int ign = 0x100;
const unsigned int bad = 0x101;
const unsigned int pad = 0x102;
const unsigned int decode[128] = {
    /*   0 */ bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    ign,
    bad,
    bad,
    ign,
    bad,
    bad,
    /*  16 */ bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    /*  32 */ bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    62,
    bad,
    bad,
    bad,
    63,
    /*  48 */ 52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    bad,
    bad,
    bad,
    pad,
    bad,
    bad,
    /*  64 */ bad,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    /*  80 */ 15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    bad,
    bad,
    bad,
    bad,
    bad,
    /*  96 */ bad,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    /* 112 */ 41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    bad,
    bad,
    bad,
    bad,
    bad,
};
const int decodeSize = sizeof(decode) / sizeof(decode[0]);

const int blockSize = 4;
const int encodedBits = 6;
const int minPaddingIndex = 2;

const auto encode =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const auto padChar = '=';

} // namespace base64
} // namespace

std::string encodeBase64(const void *vdata, size_t len) {
  auto data = static_cast<const unsigned char *>(vdata);

  //   if (len > (std::string::npos - 1) / 4 * 3) xaccthrow
  //   std::length_error("sapiremote::encodeBase64");
  auto b64size = (len + 2) / 3 * 4;
  auto b64 = std::string(b64size, '\0');
  auto out = b64.begin();

  auto numFullBlocks = len / 3;
  for (auto i = 0u; i < numFullBlocks; ++i) {
    unsigned block = (static_cast<unsigned int>(data[0]) << 16) |
                     (static_cast<unsigned int>(data[1]) << 8) | data[2];
    *out++ = base64::encode[(block >> 18) & 0x3f];
    *out++ = base64::encode[(block >> 12) & 0x3f];
    *out++ = base64::encode[(block >> 6) & 0x3f];
    *out++ = base64::encode[block & 0x3f];
    data += 3;
  }

  switch (len % 3) {
  case 0:
    break;
  case 1: {
    unsigned block = (static_cast<unsigned int>(data[0]) << 16);
    ;
    *out++ = base64::encode[(block >> 18) & 0x3f];
    *out++ = base64::encode[(block >> 12) & 0x3f];
    *out++ = base64::padChar;
    *out++ = base64::padChar;
  } break;
  case 2: {
    unsigned block = (static_cast<unsigned int>(data[0]) << 16) |
                     (static_cast<unsigned int>(data[1]) << 8);
    *out++ = base64::encode[(block >> 18) & 0x3f];
    *out++ = base64::encode[(block >> 12) & 0x3f];
    *out++ = base64::encode[(block >> 6) & 0x3f];
    *out++ = base64::padChar;
  } break;
  }

  return b64;
}
std::vector<unsigned char> decodeBase64(const std::string &b64data) {
  std::vector<unsigned char> data;
  data.reserve(b64data.size() * 3 / 4);

  auto byteIndex = 0;
  auto padding = 0;
  auto finished = false;
  unsigned int block = 0;
  for (char c : b64data) {
    // if (c < 0 || c >= base64::decodeSize) xacc::error("base 64
    // exception");//throw Base64Exception();
    auto d = base64::decode[c];
    if (d == base64::ign)
      continue;
    if (finished)
      xacc::error("base 64 exception");

    switch (d) {
    case base64::bad:
      xacc::error("base 64 exception");
    case base64::pad:
      if (padding == 0 && byteIndex < base64::minPaddingIndex)
        xacc::error("base 64 exception");
      block <<= base64::encodedBits;
      ++padding;
      ++byteIndex;
      break;
    default:
      if (padding)
        xacc::error("base 64 exception");
      block = (block << base64::encodedBits) | d;
      ++byteIndex;
      break;
    }

    if (byteIndex == base64::blockSize) {
      data.push_back(static_cast<unsigned char>(block >> 16));
      if (padding < 2)
        data.push_back(static_cast<unsigned char>((block >> 8) & 0xff));
      if (padding < 1)
        data.push_back(static_cast<unsigned char>(block & 0xff));
      finished = padding > 0;
      byteIndex = 0;
      block = 0;
    }
  }

  if (byteIndex != 0)
    xacc::error("base 64 exception");
  return data;
}


std::vector<char> decodeSolutions(const std::string &answer, size_t numSols,
                                  int numVars, std::vector<int> activeVars,
                                  char zero) {
  auto solBits = decodeBase64(answer);
  //   auto activeVars = decodeBinary<int>(answer, answerkeys::activeVars);
  auto numActiveVars = activeVars.size();
  //   auto numVars = decodeNumVars(answer);
  const char unusedIsingVariable = 3;

  auto last = -1;
  for (auto av : activeVars) {
    // if (av <= last) throw BadAnswerValueException(answerkeys::activeVars);
    last = av;
  }
  if (last >= numVars)
    xacc::error(
        "bad answer1"); // throw
                        // BadAnswerValueException(answerkeys::activeVars);

  std::cout << numSols << ", " << numVars << ", " << numActiveVars << ", "
            << solBits.size() << "\n";
  //   if (numSols * ((numActiveVars + 7) / 8) != solBits.size())
  //   xacc::error("bad answer2");
  if (numVars > 0 &&
      std::numeric_limits<size_t>::max() / static_cast<unsigned>(numVars) <
          numSols) {
    xacc::error("solution data too large"); // throw DecodingException("solution
                                            // data too large");
  }

  auto solutions = std::vector<char>(numSols * numVars, unusedIsingVariable);
  if (!solBits.empty()) {
    auto solIter = solutions.begin();
    auto solBitsIter = solBits.begin();
    auto bitIndex = 7;
    for (size_t i = 0; i < numSols; ++i) {
      for (auto av : activeVars) {
        if (bitIndex < 0) {
          ++solBitsIter;
          bitIndex = 7;
        }
        solIter[av] = ((*solBitsIter >> bitIndex) & 1) ? 1 : zero;
        --bitIndex;
      }
      ++solBitsIter;
      bitIndex = 7;
      solIter += numVars;
    }
  }

  return solutions;
}

namespace xacc {

namespace quantum {

void DWDecorator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                          const std::shared_ptr<CompositeInstruction> problem) {

  // Compute embedding
  // ------------------------------
  Embedding embedding;
  auto probGraph = problem->toGraph();
  auto hardwareEdges = decoratedAccelerator->getConnectivity();

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

  // Execute embedded problem
  // ------------------------------
  auto provider = xacc::getIRProvider("quantum");
  auto embedded_ir = provider->createComposite(problem->name(), {}, "anneal");
  for (size_t i = 0; i < r->problem.len; ++i) {
    auto pe = r->problem.elements + i;
    auto inst = provider->createInstruction(
        "dwqmi",
        std::vector<std::size_t>{(std::size_t)pe->i, (std::size_t)pe->j},
        {pe->value});
    embedded_ir->addInstruction(inst);
  }

  decoratedAccelerator->execute(buffer, embedded_ir);
  // ------------------------------

  // get vector<char> from solutions
  // ------------------------------
  auto solutions = buffer->getInformation("solutions").as<std::string>();
  auto esize =
      buffer->getInformation("energies").as<std::vector<double>>().size();
  auto num_vars = buffer->getInformation("num_vars").as<int>();
  auto active_vars =
      buffer->getInformation("active-vars").as<std::vector<int>>();
  auto num_occurrences =
      buffer->getInformation("num-occurrences").as<std::vector<int>>();
  auto decoded = decodeSolutions(solutions, esize, num_vars, active_vars, -1);
  std::vector<int> tmp;
  for (auto &c : decoded) {
    tmp.push_back(static_cast<int>(c));
  }
  // ------------------------------

  // un-embed problem
  // ------------------------------
  size_t numNewSolutions = 999;
  std::vector<int> newSolutions(probGraph->order() * esize, -999);
  sapi_unembedAnswer(tmp.data(), tmp.size(), esize, &sapiEmbeddings,
                     SAPI_BROKEN_CHAINS_VOTE, 0, newSolutions.data(),
                     &numNewSolutions, 0);
  std::transform(newSolutions.begin(), newSolutions.end(), newSolutions.begin(),
                 [](const int &b) { return (b == 1) ? b : 0; });
  // ------------------------------

  for (int i = 0; i < esize; i++) {
    auto first = newSolutions.begin() + i * probGraph->order();
    auto end = newSolutions.begin() + probGraph->order() * (i + 1);
    std::vector<int> subBitString(first, end);
    std::stringstream ss;
    for (auto &s : subBitString)
      ss << s;
    buffer->appendMeasurement(ss.str(), num_occurrences[i]);
    if (i == 0)
      buffer->addExtraInfo("ground_state", ss.str());
  }
  //   std::cout << "new sols: " << newSolutions.size() << "\n";
  //   std::cout << newSolutions << "\n";
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