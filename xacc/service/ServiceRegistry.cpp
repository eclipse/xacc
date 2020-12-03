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
#include "ServiceRegistry.hpp"
#include "xacc_config.hpp"
#include "xacc.hpp"

namespace xacc {

void ServiceRegistry::initialize(const std::string rootPath) {

  if (!initialized) {
    framework = FrameworkFactory().NewFramework();

    // Initialize the framework
    framework.Init();
    context = framework.GetBundleContext();
    if (!context) {
      XACCLogger::instance()->error("Invalid XACC Framework plugin context.");
    }

    std::string libDir = rootPath + std::string("/lib");
    std::string pluginDir = rootPath + std::string("/plugins");
    
    extra_search_paths.push_back(pluginDir);

    rootPathStr = rootPath;

    // Load quantum-gate and quantum-aqc
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(libDir.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir(dir)) != NULL) {
        if (std::string(ent->d_name).find("libxacc-quantum") !=
            std::string::npos) {
          context.InstallBundles(libDir + "/" + std::string(ent->d_name));
        }
      }
      closedir(dir);
    }

    // Load plugins
    for (auto& path : extra_search_paths) {
      DIR *dir2;
      struct dirent *ent2;
      if ((dir2 = opendir(path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent2 = readdir(dir2)) != NULL) {
          if (std::string(ent2->d_name).find("lib") != std::string::npos) {
            //   std::cout << "INSTALLING: " << path << "/" << std::string(ent2->d_name) << "\n";
            context.InstallBundles(path + "/" + std::string(ent2->d_name));
          }
        }
        closedir(dir2);
      }
    }

    // XACCLogger::instance()->enqueueLog(
    // "Starting the C++ Microservices Framework.");
    // Start the framework itself.
    framework.Start();

    // Our bundles depend on each other in the sense that the consumer
    // bundle expects a ServiceTime service in its activator Start()
    // function. This is done here for simplicity, but is actually
    // bad practice.
    auto bundles = context.GetBundles();
    for (auto b : bundles) {
      try {
        b.Start();
      } catch (std::exception &e) {
        xacc::error("Could not load " + b.GetSymbolicName() + ", error message: " + e.what());
      }
    }

    initialized = true;
  }
}
} // namespace xacc
