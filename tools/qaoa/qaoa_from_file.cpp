#include <string>
#include <fstream>
#include <streambuf>
#include <random>
#include "PauliOperator.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "json.hpp"

using json = nlohmann::json;


// TODO:
// 1. Run multiple BFGS loops and return the best answer (with different starting points)
// 2. Specify how many or submit a list of which graphs you actually want to run

// Generate random vector 
auto random_vector(const double l_range, const double r_range, const std::size_t size) {
    // Generate a random initial parameter set
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    std::uniform_real_distribution<double> dist{l_range, r_range};
    auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };
    std::vector<double> vec(size);
    std::generate(vec.begin(), vec.end(), gen);
    return vec;
}

class qaoa_from_file {
    protected:
        std::string m_config_file;
        std::string m_graph_file;
        std::string m_out_file;
        std::string m_acc_name;
        std::string m_opt_name;
        std::string m_in_config;
        int m_steps;

        // Function takes a JSON configuration file as input and sets all parameters according to file
        void read_json() {
            std::ifstream t(m_config_file);
            std::string config_str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
            json configs = json::parse(config_str); 
            m_graph_file = configs["graph"].get<std::string>();
            m_out_file = configs["outputfile"];
            m_acc_name = configs["xacc"]["accelerator"];
            m_opt_name = configs["xacc"]["optimizer"];
            m_steps = configs["qaoa-params"]["p"];//.get<int>();
        }

        // Function takes a graph file, or file of Pauli Observables, as an input and outputs the corresponding Hamiltonian
        void read_hamiltonian(const std::string& graphFile, xacc::quantum::PauliOperator& H) {
            std::ifstream H_file(graphFile);
            std::string str((std::istreambuf_iterator<char>(H_file)),
                                  std::istreambuf_iterator<char>());

            int nbOrder = 0;
            int i = 0;
            std::vector<std::pair<int,int>> edges;
            auto lines = xacc::split(str, '\n');
            for (auto line : lines) {
                if (line.find("Graph") != std::string::npos) {
                    auto elements = xacc::split(line, ' ');
                    auto order_str = elements.back();
                    nbOrder = std::stoi(order_str.substr(0,order_str.length()-1));
                } else {
                    if (nbOrder == 0) {
                        xacc::error("Invalid graph file syntax.");
                    }
                    int j_counter = i+1;
                    for(int j = 0; j < line.length(); j++){
                        if (line[j] == '1') {
                            // Outputting edge pairs to terminal for verification:
                            std::cout << "Edge at: " << i << ", " << j_counter << "\n";
                            edges.push_back(std::make_pair(i,j_counter));
                        }
                        j_counter++;
                    }
                    i++;
                }
            }
            for (auto [vi, vj] : edges) {
                H += 0.5 * (xacc::quantum::PauliOperator(1.) - xacc::quantum::PauliOperator({{vi, "Z"}, {vj, "Z"}}));
            }
        }

    public:
        qaoa_from_file(const std::string& configFile, const std::string& graphFile, const std::string& outFile, const std::string& optName, const std::string& accName, const std::string& inConfig, const int& nbSteps) : m_config_file(configFile), m_graph_file(graphFile), m_out_file(outFile), m_opt_name(optName), m_acc_name(accName), m_in_config(inConfig), m_steps(nbSteps) {}
        
        void execute() {

            if (m_in_config == "true"){
                read_json();
            }

            // Define the graph from H_file
            xacc::quantum::PauliOperator H;
            read_hamiltonian(m_graph_file, H);
            std::cout << H.toString() << "\n";
            xacc::Observable* obs = &H;

            // Target the user-specified backend
            auto acc = xacc::getAccelerator(m_acc_name);

            if (m_opt_name != "nlopt")
            {
                std::cout << "Optimization currently only supports nlopt.\n";
            }

            // Define the gradient for L-BFGS
            auto gradient = xacc::getService<xacc::AlgorithmGradientStrategy>("central");
            gradient->initialize({{"step", .1}, {"observable", xacc::as_shared_ptr(obs)}});

            // Get the L-BFGS Optimizer from NLOPT
            auto optimizer = xacc::getOptimizer(m_opt_name, 
                                    {{"nlopt-optimizer", "l-bfgs"}, {"maximize", true},{"initial-parameters", random_vector(-2., 2., 2*m_steps)}});

            // turn on verbose output
            xacc::set_verbose(true);

            // Initialize QAOA
            auto qaoa = xacc::getAlgorithm("QAOA", {{"accelerator", acc},
                                                    {"optimizer", optimizer},
                                                    {"observable", obs},
                                                    {"steps", m_steps},
                                                    {"gradient_strategy", gradient},
                                                    {"parameter-scheme", "Standard"}});

            // Allocate some qubits and execute
            auto buffer = xacc::qalloc(H.nBits());
            qaoa->execute(buffer);

            // Print out the results (will eventually take more args to be able to print out)
            std::cout << "Min Val: " << (*buffer)["opt-val"].as<double>() << "\n";
            std::cout << "Opt vals: ";
            for (auto param : (*buffer)["opt-params"].as<std::vector<double>>()) {
                std::cout << param << " ";
            }
            std::cout << "\n";

            // Compute and return the State Vector
            auto state_vector_calc = 
                xacc::getAccelerator("aer", {{"sim-type", "statevector"}});
            auto qaoa_statevector =
                xacc::getAlgorithm("QAOA", {{"accelerator", state_vector_calc},
                                            {"observable", obs},
                                            {"optimizer", optimizer},
                                            {"steps", m_steps},
                                            {"parameter-scheme", "Standard"}});
            auto state_vector_buffer = xacc::qalloc(H.nBits());
            // turn off verbose output
            xacc::set_verbose(false);
            auto result = qaoa_statevector->execute(
                                        state_vector_buffer,
                                        (*buffer)["opt-params"].as<std::vector<double>>())[0];
            auto state_vector = (*state_vector_buffer->getChildren()[1])["state"]
                                    .as<std::vector<std::pair<double, double>>>();
            std::cout << "\n" << "STATE VECTOR: [REAL, IM] " << "\n";
            for (auto [real, imag] : state_vector) {
                std::cout << real << ", " << imag << "\n";
            }

        if (m_out_file == "true"){
            std::stringstream ss;
            ss << "qaoa_output_n" << H.nBits() << "_p" << m_steps << ".txt";
            std::ofstream file(ss.str());
            file << "Min Val: " << (*buffer)["opt-val"].as<double>() << "\n";
            file << "Opt vals: ";
            for (auto param : (*buffer)["opt-params"].as<std::vector<double>>()) {
                file << param << " ";
            }
            file << "\n";
            file << "\n";
            buffer->print(file);
        }
    } 
};

int main(int argc, char **argv)
{
    // Default settings if user doesn't specify:
    int nbSteps = 1; 
    std::string accName = "qpp";
    std::string optName = "nlopt";
    std::string graphFile = "temp";
    std::string configFile = "temp";
    std::string inConfig = "false";
    std::string outFile = "false";

    std::vector<std::string> arguments(argv + 1, argv + argc);
    for (int i = 0; i < arguments.size(); i++) {
        if (arguments[i] == "-c"){
            configFile = arguments[i+1];
            inConfig = "true";
            break;
        }
        if (arguments[i] == "-i") {
           graphFile = arguments[i+1];
        }   
        if (arguments[i] == "-o") {
           outFile = arguments[i+1];
        }   
        if (arguments[i] == "-p") {
            nbSteps = std::stoi(arguments[i+1]);
        }
        if (arguments[i] == "-qpu") {
            accName = arguments[i+1];
        }
        if (arguments[i] == "-opt") {
            optName = arguments[i+1];
        }
    }
    xacc::Initialize(argc, argv);

    // Instantiate class
    qaoa_from_file QAOA(configFile, graphFile, outFile, optName, accName, inConfig, nbSteps);

    // Execute the QAOA algorithm and return results
    QAOA.execute();
}