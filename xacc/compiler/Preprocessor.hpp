#ifndef XACC_COMPILER_PREPROCESSOR_HPP_
#define XACC_COMPILER_PREPROCESSOR_HPP_

#include "Compiler.hpp"

namespace xacc {

/**
 *
 */
class Preprocessor : public OptionsProvider {
public:

	Preprocessor() {}

	/**
	 * This method is to be implemented by subclasses to take in a
	 * kernel source string and process it in an isomorphic manner, and
	 * returns the processed source code.
	 *
	 * @param src The unprocessed kernel source code
	 * @param compiler The compiler being used to compile the code
	 * @param accelerator The Accelerator this code will be run on
	 *
	 * @return processedSrc The processed kernel source code
	 */
	virtual const std::string process(const std::string& source,
			std::shared_ptr<Compiler> compiler,
			std::shared_ptr<Accelerator> accelerator) = 0;

	/**
	 * Return the name of this Preprocessor
	 *
	 * @return name The name of this preprocessor
	 */
	virtual const std::string getName() = 0;

	/**
	 * Return an empty options_description, this is for
	 * subclasses to implement.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		return std::make_shared<options_description>();
	}

	~Preprocessor() {}
};

/**
 * Preprocessor Registry is just an alias for a
 * Registry of Preprocessors.
 */
using PreprocessorRegistry = Registry<Preprocessor>;

/**
 * RegisterPreprocessor is a convenience class for
 * registering custom derived Preprocessor classes.
 *
 * Creators of Preprocessor subclasses create an instance
 * of this class with their Preprocessor subclass as the template
 * parameter to register their Preprocessor with XACC. This instance
 * must be created in the CPP implementation file for the Preprocessor
 * and at global scope.
 */
template<typename T>
class RegisterPreprocessor {
public:
	RegisterPreprocessor(const std::string& name) {
		PreprocessorRegistry::instance()->add(name,
				(std::function<std::shared_ptr<xacc::Preprocessor>()>) ([]() {
					std::cout << "I AM TRYING TO CREATE THIS THING\n";
					return std::make_shared<T>();
				}));
	}
};

#define RegisterPreprocessor(TYPE) BOOST_DLL_ALIAS(TYPE::registerPreprocessor, registerPreprocessor)

}

#endif
