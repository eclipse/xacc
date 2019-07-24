#include "ServiceRegistry.hpp"
#include "xacc_config.hpp"

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
    rootPathStr = rootPath;

    // Load quantum-gate and quantum-aqc
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(libDir.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir(dir)) != NULL) {
        if (std::string(ent->d_name).find("libxacc-quantum") !=
            std::string::npos) {
          //   printf("[service-registry] Installing Core Library: %s\n",
          //   ent->d_name);
          context.InstallBundles(libDir + "/" + std::string(ent->d_name));
        }
        /*else if (std::string(ent->d_name).find("libxacc-pauli")!=std::string::npos) {
          context.InstallBundles(libDir + "/" + std::string(ent->d_name));
        } else if (std::string(ent->d_name).find("libxacc-fermion")!=std::string::npos) {
          context.InstallBundles(libDir + "/" + std::string(ent->d_name));
        } */
      }
      closedir(dir);
    }

    // Load plugins
    DIR *dir2;
    struct dirent *ent2;
    if ((dir2 = opendir(pluginDir.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent2 = readdir(dir2)) != NULL) {
        if (std::string(ent2->d_name).find("lib") != std::string::npos) {
          //   printf("[service-registry] Installing Plugin: %s\n",
          //   ent2->d_name);
          context.InstallBundles(pluginDir + "/" + std::string(ent2->d_name));
        }
      }
      closedir(dir2);
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
      b.Start();
    }

    initialized = true;
  }
}
}

