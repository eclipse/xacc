#include "GateFunction.hpp"

namespace xacc {
namespace quantum {


void GateFunction::mapBits(std::vector<int> bitMap) {
		for (auto i : instructions) {
			i->mapBits(bitMap);
		}
}

const int GateFunction::nInstructions() {
		return instructions.size();
}

std::list<InstPtr> GateFunction::getInstructions() {
		return instructions;
}

void GateFunction::removeInstruction(const int idx) {
    auto instruction = getInstruction(idx);
    if (instruction->isParameterized()){
        auto iparam = instruction->getParameter(0);
        for (auto i : instructions){
            if (i->isParameterized() && i->getParameter(0) == iparam){
            } else {
                parameters.erase(std::remove(parameters.begin(), parameters.end(), iparam), parameters.end());
            }
        }
    }
		instructions.remove(getInstruction(idx));
}

const std::string GateFunction::name() const {
		return functionName;
}

const std::vector<int> GateFunction::bits() {
		return std::vector<int> { };
	}

const std::string GateFunction::getTag() {
    return tag;
}

void GateFunction::addInstruction(InstPtr instruction) {
        if (instruction->isParameterized() && instruction->nParameters() <= 1){
            xacc::InstructionParameter param = instruction->getParameter(0);
            if (param.which() == 3){
                if (std::find(parameters.begin(), parameters.end(), param) != parameters.end()) { }
                else { 
                    parameters.push_back(param);
                }
            }
        }
		instructions.push_back(instruction);
}
    
void GateFunction::replaceInstruction(const int idx, InstPtr replacingInst) {
		std::replace(instructions.begin(), instructions.end(),
				getInstruction(idx), replacingInst);
}

void GateFunction::insertInstruction(const int idx, InstPtr newInst) {
		auto iter = std::next(instructions.begin(), idx);
		instructions.insert(iter, newInst);
}

InstPtr GateFunction::getInstruction(const int idx) {
		InstPtr i;
		if (instructions.size() > idx) {
			i = *std::next(instructions.begin(), idx);
		} else {
			xacc::error("GateFunction getInstruction invalid instruction index - " + std::to_string(idx) + ".");
		}
		return i;
}

void GateFunction::setParameter(const int idx, InstructionParameter& p) {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested.");
		}

		parameters[idx] = p;
}

InstructionParameter GateFunction::getParameter(const int idx) const {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested.");
		}

		return parameters[idx];
}

void GateFunction::addParameter(InstructionParameter instParam) {
		parameters.push_back(instParam);
}

std::vector<InstructionParameter> GateFunction::getParameters() {
		return parameters;
}

bool GateFunction::isParameterized() {
		return nParameters() > 0;
}

const int GateFunction::nParameters() {
		return parameters.size();
}

const std::string GateFunction::toString(const std::string& bufferVarName) {
		std::string retStr = "";
		for (auto i : instructions) {
			retStr += i->toString(bufferVarName) + "\n";
		}
		return retStr;
}


std::shared_ptr<Function> GateFunction::operator()(const Eigen::VectorXd& params){
		if (params.size() != nParameters()) {
			xacc::error("Invalid GateFunction evaluation: number "
					"of parameters don't match. " + std::to_string(params.size()) +
					", " + std::to_string(nParameters()));
		}
       
        Eigen::VectorXd p = params;
        symbol_table_t symbol_table;
		symbol_table.add_constants();
		std::vector<std::string> variableNames;
        std::vector<double> values;
		for (int i = 0; i < params.size(); i++) {
            auto var = boost::get<std::string>(getParameter(i));
			variableNames.push_back(var);
            symbol_table.add_variable(var, p(i));
		}

        auto compileExpression = [&](InstructionParameter& p) -> double {
            	auto expression = boost::get<std::string>(p);
				expression_t expr;
				expr.register_symbol_table(symbol_table);
				parser_t parser;
				parser.compile(expression, expr);
                return expr.value();
        };
  
		auto gateRegistry = xacc::getService<IRProvider>("gate");
		auto evaluatedFunction = std::make_shared<GateFunction>("evaled_"+name());

		// Walk the IR Tree, handle functions and instructions differently
		for (auto inst : getInstructions()) {
			if (inst->isComposite()) {
				// If a Function, call this method recursively
				auto evaled =
						std::dynamic_pointer_cast<Function>(inst)->operator()(
								params);
				evaluatedFunction->addInstruction(evaled);
			} else {
				// If a concrete GateInstruction, then check that it
				// is parameterized and that it has a string parameter
				if (inst->isParameterized()
						&& inst->getParameter(0).which() == 3) {
                    InstructionParameter p = inst->getParameter(0);
                    std::stringstream s;
                    s << p;
                    auto val = compileExpression(p);
					InstructionParameter pnew(val);
					auto updatedInst = gateRegistry->createInstruction(inst->name(), inst->bits());
                    updatedInst->setParameter(0, pnew);
					evaluatedFunction->addInstruction(updatedInst);
				} else {
					evaluatedFunction->addInstruction(inst);
				}
			}
		}
		return evaluatedFunction;
	}
    
    
}
}