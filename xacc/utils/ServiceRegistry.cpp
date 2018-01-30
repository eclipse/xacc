#include "ServiceRegistry.hpp"

namespace xacc {
void ServiceRegistry::initialize() {

	if (!initialized) {
		framework = FrameworkFactory().NewFramework();

		try {
			// Initialize the framework, such that we can call
			// GetBundleContext() later.
			framework.Init();
		} catch (const std::exception& e) {
			XACCLogger::instance()->error(
					std::string(e.what())
							+ " - Could not initialize XACC Framework");
		}

		context = framework.GetBundleContext();
		if (!context) {
			XACCLogger::instance()->error(
					"Invalid XACC Framework plugin context.");
		}

		const std::string xaccLibDir = std::string(XACC_INSTALL_DIR) + std::string("/lib");

		for (auto &entry : boost::make_iterator_range(
				boost::filesystem::directory_iterator(xaccLibDir), { })) {
			// We want the gate and aqc bundles that come with XACC
			if (boost::contains(entry.path().filename().string(),
					"libxacc-quantum")) {
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
						context.InstallBundles(entry.path().string());
					}
				}

			}

		} else {
			XACCInfo(
					"There are no plugins. Install plugins to begin working with XACC.");
		}


		try {
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
		} catch (const std::exception& e) {
			XACCLogger::instance()->error(
					"Failure to start XACC plugins. " + std::string(e.what()));
		}
	}
}

}

