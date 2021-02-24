/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "staq_compiler.hpp"

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "InstructionIterator.hpp"
#include "AcceleratorBuffer.hpp"

#include "staq_visitors.hpp"

#include "transformations/desugar.hpp"
#include "transformations/inline.hpp"
#include "transformations/oracle_synthesizer.hpp"
#include "optimization/simplify.hpp"

#include "output/cirq.hpp"

#include <streambuf>

using namespace staq::ast;

namespace xacc {

static std::vector<std::string> builtins{
    "u3", "u2",   "u1",  "cx",  "id",  "u0",  "x",   "y",  "z",
    "h",  "s",    "sdg", "t",   "tdg", "rx",  "ry",  "rz", "cz",
    "cy", "swap", "ch",  "ccx", "crz", "cu1", "cu2", "cu3"};

static std::map<std::string, std::string> missing_builtins{
    {"u", R"#(
gate u(theta,phi,lambda) q { U(theta,phi,lambda) q; })#"},
    {"p", R"#(gate p(theta) a
{
  rz(theta) a;
})#"},
    {"sx", R"#(gate sx a { sdg a; h a; sdg a; })#"},
    {"sxdg", R"#(gate sxdg a { s a; h a; s a; })#"},
    {"cswap", R"#(gate cswap a,b,c
{
  cx c,b;
  ccx a,b,c;
  cx c,b;
})#"},
    {"crx", R"#(gate crx(lambda) a,b
{
  u1(pi/2) b;
  cx a,b;
  u3(-lambda/2,0,0) b;
  cx a,b;
  u3(lambda/2,-pi/2,0) b;
})#"},
    {"cry", R"#(gate cry(lambda) a,b
{
  u3(lambda/2,0,0) b;
  cx a,b;
  u3(-lambda/2,0,0) b;
  cx a,b;
  ry(lambda) a;
})#"},
    {"cp", R"#(gate cp(lambda) a,b
{
  p(lambda/2) a;
  cx a,b;
  p(-lambda/2) b;
  cx a,b;
  p(lambda/2) b;
})#"},
    {"csx", R"#(gate csx a,b { h b; cu1(pi/2) a,b; h b; })#"},
    {"cu", R"#(gate cu(theta,phi,lambda,gamma) c, t
{ p(gamma) c;
  p((lambda+phi)/2) c;
  p((lambda-phi)/2) t;
  cx c,t;
  u(-theta/2,0,-(phi+lambda)/2) t;
  cx c,t;
  u(theta/2,phi,0) t;
})#"},
    {"rxx", R"#(gate rxx(theta) a,b
{
  u3(pi/2, theta, 0) a;
  h b;
  cx a,b;
  u1(-theta) b;
  cx a,b;
  h b;
  u2(-pi, pi-theta) a;
})#"},
    {"rzz", R"#(gate rzz(theta) a,b
{
  cx a,b;
  u1(theta) b;
  cx a,b;
})#"},
    {"rccx", R"#(gate rccx a,b,c
{
  u2(0,pi) c;
  u1(pi/4) c;
  cx b, c;
  u1(-pi/4) c;
  cx a, c;
  u1(pi/4) c;
  cx b, c;
  u1(-pi/4) c;
  u2(0,pi) c;
})#"}};

StaqCompiler::StaqCompiler() {}

bool StaqCompiler::canParse(const std::string &src) {
  std::string _src = src;
  bool isXaccKernel = false;
  std::string prototype;
  auto xasm = xacc::getCompiler("xasm");

  /** staq prints to cerr, override this **/
  class buffer_override {
    std::ostream &os;
    std::streambuf *buf;

  public:
    buffer_override(std::ostream &os) : os(os), buf(os.rdbuf()) {}

    ~buffer_override() { os.rdbuf(buf); }
  };

  buffer_override b(std::cerr);
  std::stringstream xx;

  std::cerr.rdbuf(xx.rdbuf());

  /** --------------------------------- **/

  if (src.find("__qpu__") != std::string::npos) {
    prototype = _src.substr(0, _src.find_first_of("{")) + "{}";
    auto bufferNames = xasm->getKernelBufferNames(prototype);

    isXaccKernel = true;

    std::string tmp = "";
    auto first = _src.find_first_of("{");
    auto last = _src.find_last_of("}");
    auto sub = _src.substr(first + 1, last - first - 1);
    auto lines = xacc::split(sub, '\n');
    bool addedNames = false;
    for (auto &l : lines) {
      xacc::trim(l);
      if (l.find("measure") != std::string::npos) {
        // don't add measures
        continue;
      }
      tmp += l + "\n";
      if (l.find("include") != std::string::npos) {
        for (auto &b : bufferNames) {
          auto size = std::numeric_limits<int>::max();
          tmp += "qreg " + b + "[" + std::to_string(size) + "];\n";
        }
        addedNames = true;
      }
    }
    _src = tmp;
    if (!addedNames) {
      for (auto &b : bufferNames) {
        auto size = std::numeric_limits<int>::max();
        _src = "qreg " + b + "[" + std::to_string(size) + "];\n" + _src;
      }
    }
  }

  // we allow users to leave out OPENQASM 2.0 and include qelib.inc
  // If they did we need to add it for them
  std::string tmp = "";
  auto lines = xacc::split(_src, '\n');
  bool foundOpenQasm = false, foundInclude = false;
  for (auto &l : lines) {
    if (l.find("OPENQASM") != std::string::npos) {
      foundOpenQasm = true;
    }
    if (l.find("include") != std::string::npos) {
      foundInclude = true;
    }
  }

  if (!foundInclude) {
    _src = "include \"qelib1.inc\";\n" + _src;
  }
  if (!foundOpenQasm) {
    _src = "OPENQASM 2.0;\n" + _src;
  }

  // std::cout << " HELLO:\n" << _src << "\n";
  using namespace staq;

  try {
    auto prog = parser::parse_string(_src);
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

std::shared_ptr<IR> StaqCompiler::compile(const std::string &src,
                                          std::shared_ptr<Accelerator> acc) {
  // IF src contains typical xacc quantum kernel prototype, then take
  // buffers names and add qreg declarations before parsing
  // IF it does not, then assume qreg decls already declared.
  auto xasm = xacc::getCompiler("xasm");
  std::string prototype;

  std::string _src = src;
  bool isXaccKernel = false;
  if (src.find("__qpu__") != std::string::npos) {
    prototype = _src.substr(0, _src.find_first_of("{")) + "{}";
    auto bufferNames = xasm->getKernelBufferNames(prototype);

    isXaccKernel = true;

    std::string tmp = "";
    auto first = _src.find_first_of("{");
    auto last = _src.find_last_of("}");
    auto sub = _src.substr(first + 1, last - first - 1);
    auto lines = xacc::split(sub, '\n');
    bool addedNames = false;
    for (auto &l : lines) {
      xacc::trim(l);
      tmp += l + "\n";
      if (l.find("include") != std::string::npos) {
        for (auto &b : bufferNames) {
          auto size = xacc::getBuffer(b)->size();
          tmp += "qreg " + b + "[" + std::to_string(size) + "];\n";
        }
        addedNames = true;
      }
    }
    _src = tmp;
    if (!addedNames) {
      for (auto &b : bufferNames) {
        auto size = xacc::getBuffer(b)->size();
        _src = "qreg " + b + "[" + std::to_string(size) + "];\n" + _src;
      }
    }
  }

  // we allow users to leave out OPENQASM 2.0 and include qelib.inc
  // If they did we need to add it for them
  std::string tmp = "";
  auto lines = xacc::split(_src, '\n');
  bool foundOpenQasm = false, foundInclude = false;
  for (auto &l : lines) {
    if (l.find("OPENQASM") != std::string::npos) {
      foundOpenQasm = true;
    }
    if (l.find("include") != std::string::npos) {
      foundInclude = true;
    }
  }

  if (!foundInclude) {
    _src = "include \"qelib1.inc\";\n" + _src;
  }
  if (!foundOpenQasm) {
    _src = "OPENQASM 2.0;\n" + _src;
  }


  std::string preamble = "include \"qelib1.inc\";";
  auto preamble_start = _src.find(preamble);


  // Add any required missing pre-defines that we 
  // know the impl for.
  std::vector<std::string> added;
  std::string extra_insts = "\n";
  bool hasMeasures = false;
  auto tmp_lines = split(_src, '\n');
  for (auto line : tmp_lines) {
    if (line.find("OPENQASM") == std::string::npos &&
        line.find("include") == std::string::npos &&
        line.find("measure") == std::string::npos &&
        line.find("qreg") == std::string::npos &&
        line.find("creg") == std::string::npos
        && !line.empty()) {
      auto inst_name = split(line, ' ')[0];
      if (inst_name.find("(") != std::string::npos) {
        inst_name = inst_name.substr(0, inst_name.find("("));
      }
      if (std::find(builtins.begin(), builtins.end(), inst_name) ==
              builtins.end() &&
          std::find(added.begin(), added.end(), inst_name) == added.end()) {
        extra_insts += missing_builtins[inst_name] + "\n";
        added.push_back(inst_name);
      }
    }
  }
  _src.insert(preamble_start+preamble.length(), extra_insts);


  using namespace staq;
  ast::ptr<ast::Program> prog;
  try {
    prog = parser::parse_string(_src);
        // transformations::inline_ast(*prog);

    transformations::desugar(*prog);
    transformations::synthesize_oracles(*prog);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << e.what();
    ss << "\nXACC Error in Staq Compiler, here was the src:\n" << src << "\n";
    xacc::error(ss.str());
  }

  if (run_staq_optimize)
    optimization::simplify(*prog);

  // at this point we have to find out if we have any ancilla
  // registers
  internal_staq::CountAncillas ancillas;
  dynamic_cast<ast::Traverse &>(ancillas).visit(*prog);

  // add ancilla registers to xacc
  for (auto &kv : ancillas.ancillas) {
    if (!xacc::hasBuffer(kv.first)) {
      auto q = xacc::qalloc(kv.second);
      q->setName(kv.first);
      xacc::storeBuffer(q);
    } else {
      xacc::getBuffer(kv.first)->resetBuffer();
    }
  }

  transformations::inline_ast(
      *prog, {false, transformations::default_overrides, "anc"});

  //   std::cout <<"PROG: " << *prog << "\n";
  
  // Determine the number of qreqs
  internal_staq::CountQregs countQreq;
  dynamic_cast<ast::Traverse &>(countQreq).visit(*prog);
  const auto nbQreqs = countQreq.qregs.size() + ancillas.ancillas.size();
  // Direct Staq's AST -> XACC's IR translation:
  // This can only be used (reliably) for simple QASM source,
  // that uses a single qreg (we can use simple qubit indexing to construct IR)
  // Note: we don't handle *embedded* QASM source in this direct translate mode.
  if (!isXaccKernel && nbQreqs == 1) {
    // Create a temporary kernel name:
    std::string name = "tmp";
    if (xacc::hasCompiled(name)) {
      int counter = 0;
      while (true) {
        name = "tmp" + std::to_string(counter);
        if (!xacc::hasCompiled(name)) {
          break;
        }
        counter++;
      }
    }

    // Direct translation
    internal_staq::StaqToIr translate(name, countQreq.qregs[0]);
    translate.visit(*prog);
    return translate.getIr();
  }

  // Otherwise, translate Staq's AST to XASM source string
  // then recompile.
  internal_staq::StaqToXasm translate;
  translate.visit(*prog);

  std::string kernel;
  if (isXaccKernel) {
    if (!ancillas.ancillas.empty()) {
      kernel = prototype.substr(0, prototype.find_first_of(")"));
      for (auto &kv : ancillas.ancillas) {
        kernel += ", qreg " + kv.first;
      }
      kernel += ") {\n" + translate.ss.str() + "}";
    } else {
      kernel = prototype.substr(0, prototype.length() - 1) + "\n" +
               translate.ss.str() + "}";
    }
  } else {
    internal_staq::CountQregs c;
    dynamic_cast<ast::Traverse &>(c).visit(*prog);

    std::stringstream xx;
    std::string name = "tmp";
    if (xacc::hasCompiled(name)) {
      int counter = 0;
      while (true) {
        name = "tmp" + std::to_string(counter);
        if (!xacc::hasCompiled(name)) {
          break;
        }
        counter++;
      }
    }
    xx << "__qpu__ void " << name << "(";
    xx << "qreg " << c.qregs[0];
    for (int i = 1; i < c.qregs.size(); i++) {
      xx << ", qreg " << c.qregs[i];
    }
    for (auto &kv : ancillas.ancillas) {
      xx << ", qreg " << kv.first;
    }
    xx << ") {\n" << translate.ss.str() << "}";
    kernel = xx.str();
  }
  // std::cout << "\n\nFinal:\n" << kernel << "\n";
  return xasm->compile(kernel, acc);
}

std::shared_ptr<IR> StaqCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string
StaqCompiler::translate(std::shared_ptr<xacc::CompositeInstruction> function) {
  std::map<std::string, int> bufNamesToSize;
  InstructionIterator iter(function);
  // First search buffer names and see if we have
  while (iter.hasNext()) {
    auto &next = *iter.next();
    if (next.isEnabled()) {
      for (int i = 0; i < next.nRequiredBits(); i++) {
        auto bufName = next.getBufferName(i);
        int size = next.bits()[i] + 1;
        if (bufNamesToSize.count(bufName)) {
          if (bufNamesToSize[bufName] < size) {
            bufNamesToSize[bufName] = size;
          }
        } else {
          bufNamesToSize.insert({bufName, size});
        }
      }
    }
  }

  auto translate =
      std::make_shared<internal_staq::XACCToStaqOpenQasm>(bufNamesToSize);
  InstructionIterator iter2(function);
  while (iter2.hasNext()) {
    auto &next = *iter2.next();
    if (next.isEnabled()) {
      next.accept(translate);
    }
  }

  return translate->ss.str();
}

const std::string
StaqCompiler::translate(std::shared_ptr<CompositeInstruction> program,
                        HeterogeneousMap &options) {
  if (options.stringExists("lang-type")) {
    auto langType = options.getString("lang-type");

    // map xacc to staq program
    std::map<std::string, int> bufNamesToSize;
    InstructionIterator iter(program);
    // First search buffer names and see if we have
    while (iter.hasNext()) {
      auto &next = *iter.next();
      if (next.isEnabled()) {
        for (int i = 0; i < next.nRequiredBits(); i++) {
          auto bufName = next.getBufferName(i);
          int size = next.bits()[i] + 1;
          if (bufNamesToSize.count(bufName)) {
            if (bufNamesToSize[bufName] < size) {
              bufNamesToSize[bufName] = size;
            }
          } else {
            bufNamesToSize.insert({bufName, size});
          }
        }
      }
    }
    auto translate =
        std::make_shared<internal_staq::XACCToStaqOpenQasm>(bufNamesToSize);
    InstructionIterator iter2(program);
    while (iter2.hasNext()) {
      auto &next = *iter2.next();
      if (next.isEnabled()) {
        next.accept(translate);
      }
    }
    // std::cout << "HELLO:\n" << translate->ss.str() << "\n";
    using namespace staq;
    auto prog = parser::parse_string(translate->ss.str());
    transformations::desugar(*prog);
    transformations::synthesize_oracles(*prog);

    optimization::simplify(*prog);

    std::stringstream ss;
    if (langType == "cirq") {
      staq::output::CirqOutputter outputter(ss);
      outputter.run(*prog);
    }

    return ss.str();

  } else {
    return translate(program);
  }
}

} // namespace xacc
