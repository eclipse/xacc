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
#include "xacc_config.hpp"
#include "py_plugin_loader.hpp"
#include "pybind11/embed.h"
#include <dlfcn.h>
namespace xacc {
namespace external {

bool PythonPluginLoader::load() {
  xacc::debug("[PyPluginLoader] Loading all contributed Python plugins.");

  if (!XACC_IS_APPLE){
    libpython_handle = dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
  }
  py::initialize_interpreter();
  try {
    py::module sys = py::module::import("xacc");
    sys.attr("loaded_from_cpp_dont_finalize") = true;
  } catch(std::exception& e) {
    std::stringstream s;
    s << "Could not import xacc python module, errors was = " << e.what() << ". Will not enable external python plugins.";
    xacc::warning(s.str());
    return false;
  }

  return true;
}

bool PythonPluginLoader::unload() {
  if (!XACC_IS_APPLE) {
     xacc::debug("[PyPluginLoader] Unloading Python plugins");
     py::finalize_interpreter();
     int i = dlclose(libpython_handle);
     if (i != 0) {
        std::cout << "error closing python lib: " << i << "\n";
        std::cout << dlerror() << "\n";
        return false;
     }
  }
  return true;
}

} // namespace external
} // namespace xacc
