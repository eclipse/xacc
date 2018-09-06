/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_GATEQIR_QFUNCTION_HPP_
#define QUANTUM_GATEQIR_QFUNCTION_HPP_

#include "Function.hpp"
#include "IRProvider.hpp"
#include "GraphProvider.hpp"
#include "XACC.hpp"
#include "exprtk.hpp"
#include <boost/math/constants/constants.hpp>

namespace xacc {
namespace quantum {

static constexpr double pi = boost::math::constants::pi<double>();

using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

/**
 * CircuitNode subclasses XACCVertex to provide the following
 * parameters in the given order:
 *
 * Parameters: Gate, Layer (ie time sequence), Gate Vertex Id,
 * Qubit Ids that the gate acts on, enabled state, vector of parameters names
 */
class CircuitNode: public XACCVertex<std::string, int, int, std::vector<int>,
		bool, std::vector<std::string>> {
public:
	CircuitNode() :
			XACCVertex() {
		propertyNames[0] = "Gate";
		propertyNames[1] = "Circuit Layer";
		propertyNames[2] = "Gate Vertex Id";
		propertyNames[3] = "Gate Acting Qubits";
		propertyNames[4] = "Enabled";
		propertyNames[5] = "RuntimeParameters";

		// by default all circuit nodes
		// are enabled and
		std::get<4>(properties) = true;
	}

    CircuitNode(std::string name, int layer, int id, std::vector<int> bits, bool enabled, std::vector<std::string> params) {
        std::get<0>(properties) = name;
        std::get<1>(properties) = layer;
        std::get<2>(properties) = id;
        std::get<3>(properties) = bits;
        std::get<4>(properties) = enabled;
        std::get<5>(properties) = params;
    }

    const std::string name() {return std::get<0>(properties);}
    const int layer() {return std::get<1>(properties);}
    const int id() {return std::get<2>(properties);}
    const std::vector<int> bits() {return std::get<3>(properties);}
    bool twoQubit() {return bits().size() == 2;}
};

/**
 * The GateFunction is a realization of Function for gate-model
 * quantum computing. It is composed of QInstructions that
 * are themselves derivations of the GateInstruction class.
 */
class GateFunction : public Function, public GraphProvider<CircuitNode, Directed>, public std::enable_shared_from_this<GateFunction> {

public:
  /**
   * The constructor, takes the function unique id and its name.
   *
   * @param id
   * @param name
   */
  GateFunction(const std::string &name)
      : functionName(name), parameters(std::vector<InstructionParameter>{}) {}
  GateFunction(const std::string &name,
               std::vector<InstructionParameter> params)
      : functionName(name), parameters(params) {}

  GateFunction(const std::string &name, const std::string &_tag)
      : functionName(name), parameters(std::vector<InstructionParameter>{}),
        tag(_tag) {}
  GateFunction(const std::string &name, const std::string &_tag,
               std::vector<InstructionParameter> params)
      : functionName(name), parameters(params), tag(_tag) {}

  GateFunction(const GateFunction &other)
      : functionName(other.functionName), parameters(other.parameters) {}

  const std::string getTag() override;

  virtual void mapBits(std::vector<int> bitMap) override;

  const int nInstructions() override;

  InstPtr getInstruction(const int idx) override;

  std::list<InstPtr> getInstructions() override;

  std::shared_ptr<Function> enabledView() override {
      auto newF = std::make_shared<GateFunction>(functionName, parameters);
      for (int i = 0; i < nInstructions(); i++) {
          auto inst = getInstruction(i);
          if (inst->isEnabled()) {
              newF->addInstruction(inst);
          }
      }
      return newF;
  }
  /**
   * Remove an instruction from this
   * quantum intermediate representation
   *
   * @param instructionID
   */
  void removeInstruction(const int idx) override;

  /**
   * Add an instruction to this quantum
   * intermediate representation.
   *
   * @param instruction
   */
  void addInstruction(InstPtr instruction) override;

  /**
   * Replace the given current quantum instruction
   * with the new replacingInst quantum Instruction.
   *
   * @param currentInst
   * @param replacingInst
   */
  void replaceInstruction(const int idx, InstPtr replacingInst) override;

  void insertInstruction(const int idx, InstPtr newInst) override;

  /**
   * Return the name of this function
   * @return
   */
  const std::string name() const override;

  /**
   * Return the description of this instance
   * @return description The description of this object.
   */
  const std::string description() const override { return ""; }

  /**
   * Return the qubits this function acts on.
   * @return
   */
  const std::vector<int> bits() override;

  /**
   * Return an assembly-like string representation for this function .
   * @param bufferVarName
   * @return
   */
  const std::string toString(const std::string &bufferVarName) override;

  InstructionParameter getParameter(const int idx) const override;

  void setParameter(const int idx, InstructionParameter &p) override;

  void addParameter(InstructionParameter instParam) override;

  std::vector<InstructionParameter> getParameters() override;

  bool isParameterized() override;

  const int nParameters() override;

  std::shared_ptr<Function> operator()(const Eigen::VectorXd &params) override;

  Graph<CircuitNode, Directed> toGraph() override;
//   void fromGraph(Graph<CircuitNode>& graph) override;
//   void fromGraph(std::istream& input) override;

  DEFINE_VISITABLE()

protected:
  /**
   * The name of this function
   */
  std::string functionName;

  std::list<InstPtr> instructions;

  std::vector<InstructionParameter> parameters;

  std::string tag = "";

};

} // namespace quantum
} // namespace xacc

#endif
