#include "ServiceRegistry.hpp"

namespace xacc {
void ServiceRegistry::initialize() {

	if (!initialized) {
		XACCLogger::instance()->info("Creating CppUs Framework.");
		framework = FrameworkFactory().NewFramework();

		// Initialize the framework, such that we can call
		// GetBundleContext() later.
		XACCLogger::instance()->info("Running Framework.Init.");
		framework.Init();

		XACCLogger::instance()->info(" Getting the Framework Bundle Context.");
		context = framework.GetBundleContext();
		if (!context) {
			XACCLogger::instance()->error(
					"Invalid XACC Framework plugin context.");
		}

		const std::string xaccLibDir = std::string(XACC_INSTALL_DIR) + std::string("/lib/");
		XACCLogger::instance()->info("Plugin Lib Directory: " + xaccLibDir);
		for (auto &entry : boost::make_iterator_range(
				boost::filesystem::directory_iterator(xaccLibDir), { })) {
			XACCLogger::instance()->info("TEST: " + entry.path().filename().string());
			// We want the gate and aqc bundles that come with XACC
			if (boost::contains(entry.path().filename().string(),
					"libxacc-quantum")) {
				XACCLogger::instance()->info("Installing plugin " + entry.path().string());
				context.InstallBundles(entry.path().string());
			}
		}

		// Add external plugins...
		boost::filesystem::directory_iterator end_itr;
		boost::filesystem::path xaccPluginPath(
				std::string(XACC_INSTALL_DIR) + std::string("/lib/plugins"));
		if (boost::filesystem::exists(xaccPluginPath)) {

			for (boost::filesystem::directory_iterator itr(xaccPluginPath);
					itr != end_itr; ++itr) {

				if (boost::filesystem::is_directory(itr->path())) {
					for (auto& entry : boost::make_iterator_range(
							boost::filesystem::directory_iterator(itr->path()),
							{ })) {
						XACCLogger::instance()->info("Installing plugin " + entry.path().string());
						context.InstallBundles(entry.path().string());
					}
				}

			}

		} else {
			XACCInfo(
					"There are no plugins. Install plugins to begin working with XACC.");
		}

		XACCLogger::instance()->info("Starting the CppUs Framework.");
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

