/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *    Alexander J. McCaskey - initial API and implementation
 *    Daniel Claudino - porting
 ******************************************************************************/
#ifndef XACC_IR_OBSERVABLETRANSFORM_BK_HPP_
#define XACC_IR_OBSERVABLETRANSFORM_BK_HPP_
#include "ObservableTransform.hpp"

namespace xacc {
namespace quantum {

class BK : public ObservableTransform {
public:
  std::shared_ptr<Observable>
  transform(std::shared_ptr<Observable> obs) override;
  const std::string name() const override { return "bk"; }

  const std::string description() const override { return ""; }
};
} // namespace quantum
} // namespace xacc
#endif