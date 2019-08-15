/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef IR_IRGENERATOR_HPP_
#define IR_IRGENERATOR_HPP_

#include "Function.hpp"

#include <vector>
#include <memory>

namespace xacc {

class IRGenerator : public Instruction {

protected:
  std::map<std::string, InstructionParameter> options;

public:

  virtual std::shared_ptr<Function>
  generate(std::map<std::string, InstructionParameter>& parameters) = 0;

  virtual std::shared_ptr<Function>
  generate(std::map<std::string, InstructionParameter>&& parameters) {
    return generate(parameters);
  }

 const std::string toString(const std::string &bufferVarName) override {
     std::string n = name();
     return n + "()";
 }

  const std::string toString() override {
      std::string n = name();
      return n + "()";
  }

  const std::vector<int> bits() override {
      return {};
  }

  void setBits(const std::vector<int> bits) override {
    return;
  }

  /**
   * Return this Instruction's parameter at the given index.
   *
   * @param idx The index of the parameter.
   * @return param The InstructionParameter at the given index.
   */
  InstructionParameter getParameter(const int idx) const override {
      return InstructionParameter(0);
  }

  std::vector<InstructionParameter> getParameters() override {
      return {};
  }

  void setParameter(const int idx, InstructionParameter &inst) override {
      return;
  }

  const int nParameters() override {
      return 0;
  }

  bool isParameterized() override { return false; }

  void mapBits(std::vector<int> bitMap) override {
      return;
  }

  void setOption(const std::string optName, InstructionParameter option) override {
      if (options.count(optName)) {
          options[optName] = option;
      } else {
        options.insert({optName, option});
      }
      return;
  }

  /**
   * Get the value of an option with the given name.
   *
   * @param optName Then name of the option.
   * @return option The value of the option.
   */
  InstructionParameter getOption(const std::string optName) override {
      return options[optName];
  }

  std::map<std::string, InstructionParameter> getOptions() override {
      return options;
  }

  const int nRequiredBits() const override {
      return 0;
  }

  virtual bool validateOptions() { return false; }

  DEFINE_VISITABLE();

  /**
   * The destructor
   */
  virtual ~IRGenerator() {}
};

} // namespace xacc
#endif
