/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
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
#ifndef XACC_PYTHON_PLUGIN_LOADER_HPP_
#define XACC_PYTHON_PLUGIN_LOADER_HPP_

#include "xacc.hpp"
#include <pybind11/embed.h>
namespace py = pybind11;

namespace xacc {
namespace external {

class PythonPluginLoader : public ExternalLanguagePluginLoader {
protected:
  void* libpython_handle;
public:
  const std::string name() const override { return "python"; }
  const std::string description() const override { return ""; }
  bool load() override;
  bool unload() override;

};

} // namespace external
} // namespace xacc
#endif