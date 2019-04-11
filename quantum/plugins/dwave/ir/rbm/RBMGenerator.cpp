#include "RBMGenerator.hpp"
#include "XACC.hpp"
#include "DWQMI.hpp"
#include "DWFunction.hpp"
#include <typeinfo>

using namespace xacc;

namespace xacc {
namespace rbm {


std::shared_ptr<Function> RBMGenerator::generate(
                        std::map<std::string, InstructionParameter>& parameters) {

    if (!parameters.count("visible-units") && !parameters.count("visible_units")) {
        xacc::error("Invalid mapping of parameters for RBM generator, missing visible units key.");
    }

    if (!parameters.count("hidden-units") && !parameters.count("hidden_units")) {
        xacc::error("Invalid mapping of parameters for RBM generator, missing hidden units key.");
    }

    bool hasUnderscore = false;

    if (parameters.count("visible_units")) {
        hasUnderscore = true;
        if (!parameters.count("hidden_units")) xacc::error("RBM Generator missing hidden_units key.");
    }

    if (parameters.count("hidden_units")) {
        hasUnderscore = true;
        if (!parameters.count("visible_units")) xacc::error("RBM Generator missing visible_units key.");
    }

    std::vector<InstructionParameter> params;
    if (hasUnderscore) {
        params.push_back(parameters["visible_units"]);
        params.push_back(parameters["hidden_units"]);
    } else {
        params.push_back(parameters["visible-units"]);
        params.push_back(parameters["hidden-units"]);
    }

    return generate(nullptr, params);
}

std::shared_ptr<Function> RBMGenerator::generate(
		                        std::shared_ptr<AcceleratorBuffer> buffer,
		                        std::vector<InstructionParameter> parameters) {

    xacc::info("Running RBM IRGenerator.");

    int n_visible = parameters[1].as<int>();
    int n_hidden = parameters[0].as<int>();

    auto kernel = std::make_shared<xacc::quantum::DWFunction>("rbm_function");

    for (int i = 0; i < n_visible; i++){
        std::string paramName = "v" + std::to_string(i);
        xacc::InstructionParameter visParam(paramName);
        auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, i, visParam);
        kernel->addInstruction(qmi);
    }

    for (int i = n_visible; i < n_visible + n_hidden; i++){
        std::string paramName = "h" + std::to_string(i);
        xacc::InstructionParameter hidParam(paramName);
        auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, i, hidParam);
        kernel->addInstruction(qmi);
    }
    for (int i = 0; i < n_visible; i++){
        for (int j = n_visible; j < n_visible + n_hidden; j++){
            std::string paramName = "w" + std::to_string(i) + std::to_string(j);
            xacc::InstructionParameter wParam(paramName);
            auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, j, wParam);
            kernel->addInstruction(qmi);
        }
    }
    return kernel;
}
}
}