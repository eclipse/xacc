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

#include "py_plugin_loader.hpp"
#include "pybind11/embed.h"
#include <dlfcn.h>
namespace xacc {
namespace external {

bool PythonPluginLoader::load() {

  libpython_handle = dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
  guard = std::make_shared<py::scoped_interpreter>();
  try {
    py::module sys = py::module::import("xacc");
  } catch(std::exception& e) {
    std::cout << e.what() << "\n";
    return false;
  }

  return true;
}

bool PythonPluginLoader::unload() {
  int i = dlclose(libpython_handle);
  if (i != 0) {
      std::cout << "error closing python lib: " << i << "\n";
      return false;
  }
  return true;
}

} // namespace external
} // namespace xacc
