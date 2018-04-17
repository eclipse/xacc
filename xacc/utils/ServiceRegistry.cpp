#include "ServiceRegistry.hpp"
#include <boost/filesystem.hpp>
#ifdef XACC_HAS_PYTHON
#include <pybind11/embed.h>
#endif

namespace xacc {

void ServiceRegistry::initialize() {

	if (!initialized) {
		framework = FrameworkFactory().NewFramework();

		// Initialize the framework
		framework.Init();
		context = framework.GetBundleContext();
		if (!context) {
			XACCLogger::instance()->error(
					"Invalid XACC Framework plugin context.");
		}

		// Get the paths/files we'll be searching
		const std::string xaccLibDir = std::string(XACC_INSTALL_DIR) + std::string("/lib");
		const std::string xaccPluginPath = std::getenv("HOME")
					+ std::string("/.xacc/plugins");
		std::stringstream s;
		s << std::getenv("HOME") << "/.xacc/.internal_plugins";
		const std::string internalFileStr = s.str();

		// Search in the .internal_plugins file first
		if (boost::filesystem::exists(internalFileStr)) {
			std::ifstream internalFile(internalFileStr);
			std::string contents((std::istreambuf_iterator<char>(internalFile)), std::istreambuf_iterator<char>());
			std::vector<std::string> split;
			boost::split(split, contents, boost::is_any_of("\n"));
			for (auto& s : split) {
				if (!boost::contains(s, "#") && boost::filesystem::exists(s)) {

					try {
						context.InstallBundles(s);
						boost::filesystem::path p(s);
						auto name = p.filename().string();
						boost::replace_all(name, "lib", "");
						boost::replace_all(name, ".so", "");
						boost::replace_all(name, ".dy", "");
						installed.insert({name, p.string()});
						XACCLogger::instance()->enqueueLog(
								"Installed base plugin " + name);
					} catch(std::exception& e) {
						// do nothing if we failed
					}
				}
			}
		}

		// If not found, see if we have access to the installation directory
		if (!installed.count("xacc-quantum-gate")
				|| !installed.count("xacc-quantum-aqc")) {

			if (boost::filesystem::exists(xaccLibDir)) {
				XACCLogger::instance()->enqueueLog(
						"Searching XACC Library Directory: " + xaccLibDir);
				for (auto &entry : boost::filesystem::directory_iterator(
						xaccLibDir)) {
					// We want the gate and aqc bundles that come with XACC
					if (boost::contains(entry.path().filename().string(),
							"libxacc-quantum")) {
						auto name = entry.path().filename().string();
						boost::replace_all(name, "lib", "");
						boost::replace_all(name, ".so", "");
						boost::replace_all(name, ".dy", "");

						if (installed.find(name) == installed.end()) {
							context.InstallBundles(entry.path().string());
							XACCLogger::instance()->enqueueLog(
									"Installed base plugin " + name);
							installed.insert({name, entry.path().string()});
						}
					}
				}
			}
		}

		// If we still don't have them, throw an error
		if (!installed.count("xacc-quantum-gate")
						|| !installed.count("xacc-quantum-aqc")) {
			throw std::runtime_error("Could not find required internal plugins.");
		}

		// Add external plugins...
		boost::filesystem::directory_iterator end_itr;
		if (boost::filesystem::exists(xaccPluginPath)) {
			for (auto& entry : boost::filesystem::directory_iterator(
					xaccPluginPath)) {
				auto p = entry.path();
				if (boost::filesystem::is_directory(p)) {
					for (auto& subentry : boost::filesystem::directory_iterator(
							p)) {
						auto name = subentry.path().filename().string();
						boost::replace_all(name, "lib", "");
						boost::replace_all(name, ".so", "");
						boost::replace_all(name, ".dy", "");
						if (installed.find(name) == installed.end()) {

							context.InstallBundles(subentry.path().string());
							XACCLogger::instance()->enqueueLog(
									"Installed Plugin " + name);
							installed.insert(
									{ name, subentry.path().string() });
						}
					}
				}

			}
		}

		XACCLogger::instance()->enqueueLog(
				"Starting the C++ Microservices Framework.");
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

