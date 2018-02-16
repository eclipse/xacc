#include "ServiceRegistry.hpp"
#include <boost/filesystem.hpp>

namespace xacc {

void ServiceRegistry::initialize() {

	if (!initialized) {
//		XACCLogger::instance()->info("Creating CppUs Framework.");
		framework = FrameworkFactory().NewFramework();

		// Initialize the framework, such that we can call
		// GetBundleContext() later.
//		XACCLogger::instance()->info("Running Framework.Init.");
		framework.Init();

//		XACCLogger::instance()->info("Getting the Framework Bundle Context.");
		context = framework.GetBundleContext();
		if (!context) {
			XACCLogger::instance()->error(
					"Invalid XACC Framework plugin context.");
		}

		const std::string xaccLibDir = std::string(XACC_INSTALL_DIR) + std::string("/lib");

		XACCLogger::instance()->enqueueLog("XACC Library Directory: " + xaccLibDir);
		for (auto &entry : boost::filesystem::directory_iterator(xaccLibDir)) {
			// We want the gate and aqc bundles that come with XACC
			if (boost::contains(entry.path().filename().string(),
					"libxacc-quantum")) {
				auto name = entry.path().filename().string();
				boost::replace_all(name,"lib","");
				boost::replace_all(name,".so","");
				XACCLogger::instance()->enqueueLog("Installing base plugin " + name);
				context.InstallBundles(entry.path().string());
			}
		}

		// Add external plugins...
		boost::filesystem::directory_iterator end_itr;
		boost::filesystem::path xaccPluginPath(
				std::string(XACC_INSTALL_DIR) + std::string("/lib/plugins"));
		if (boost::filesystem::exists(xaccPluginPath)) {
			for (auto& entry : boost::filesystem::directory_iterator(xaccPluginPath)) {
				auto p = entry.path();
				if (boost::filesystem::is_directory(p)) {
					for (auto& subentry : boost::filesystem::directory_iterator(p)) {
						auto name = subentry.path().filename().string();
						boost::replace_all(name,"lib","");
						boost::replace_all(name,".so","");
						XACCLogger::instance()->enqueueLog("Installing 3rd party plugin " + name);
						context.InstallBundles(subentry.path().string());
					}
				}

			}

		} else {
			XACCLogger::instance()->enqueueLog(
					"There are no plugins. Install plugins to begin working with XACC.");
		}

		XACCLogger::instance()->enqueueLog("Starting the C++ Microservices Framework.");
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

