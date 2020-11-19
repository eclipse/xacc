/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 * Anthony Santana, Alexander J. McCaskey - initial API and implementation
 ******************************************************************************/

#include "qaoa_from_file.hpp"

// TODO:
// 1. Run multiple BFGS loops and return the best answer (with different starting points)
// 2. Specify how many or submit a list of which graphs you actually want to run
// 3. Add to optimizer section of JSON: "initialize": random, or eventually
// Fourier, warm starts, etc.

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

// Function returns the time in format: YYYY-MM-DD.HH.MM.SS
std::string get_timestamp() {
    auto now = std::time(nullptr);
    char buf[sizeof("YYYY-MM-DD  HH:MM:SS")];
    auto time = std::string(buf,buf + std::strftime(buf,sizeof(buf),"%F  %T",std::gmtime(&now)));
    time.insert(11, ".");
    std::replace(time.begin(), time.end(), ':', '.');
    time.erase(remove(time.begin(), time.end(), ' '), time.end());
    return time;
}

// Function takes a JSON configuration file as input and sets all parameters according to file
void qaoa_from_file::read_json() {
    if (configs.empty()) {
        std::ifstream t(m_config_file);
        std::string config_str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());\
        configs = json::parse(config_str); 
    }
    m_graph_file = configs["graph"]; //.get<std::string>();
    m_out_file = configs.count("outputfile") ? configs["outputfile"].get<bool>() : false;
    m_acc_name = configs.count("accelerator") ? configs["xacc"]["accelerator"].get<std::string>(): "qpp";
    m_opt_name = configs["xacc"].count("optimizer") ? configs["xacc"]["optimizer"].get<std::string>() : "nlopt";
    m_opt_algo = 
        configs["optimizer-params"].count("algorithm") ? configs["optimizer-params"]["algorithm"].get<std::string>() : "l-bfgs";
    m_step_size = 
        configs["optimizer-params"].count("stepsize") ? configs["optimizer-params"]["stepsize"].get<float>() : 0.1;
    m_steps = configs["qaoa-params"].count("p") ? configs["qaoa-params"]["p"].get<int>() : 1;
}


xacc::HeterogeneousMap qaoa_from_file::set_optimizer() {
    auto optimizerParams = configs["optimizer-params"];
    xacc::HeterogeneousMap m_options;
    std::vector<std::string> keys;
    if (m_opt_name == "nlopt"){
        keys = {"ftol", "maxeval", "algorithm", "maximize"};
    } else {
        std::cout << "Using minimization from " << m_opt_name << ". Adjust output results accordingly.\n";
        keys = {"step-size", "max-iter", "algorithm"};
    }
    m_options.insert("initial-parameters", random_vector(-2., 2., 2*m_steps));

    if (optimizerParams.count(keys[0])) {
        std::stringstream key;
        key << m_opt_name << "-" << keys[0];
        m_options.insert(key.str(), m_step_size);
    } else {
        std::stringstream key;
        key << m_opt_name << "-" << keys[0];
        m_options.insert(key.str(), m_step_size);
    }
    if (optimizerParams.count(keys[1])) {
        std::stringstream key;
        key << m_opt_name << "-" << keys[1];
        int val = optimizerParams[keys[1]].get<int>();
        m_options.insert(key.str(), val);
    } else {
        std::stringstream key;
        key << m_opt_name << "-" << keys[1];
        m_options.insert(key.str(), m_max_iters);
    }
    if (optimizerParams.count(keys[2])) {
        std::stringstream key;
        key << m_opt_name << "-optimizer";
        std::string val = optimizerParams[keys[2]].get<std::string>();
        m_options.insert(key.str(), val);
    } else {
        std::stringstream key;
        key << m_opt_name << "-optimizer";
        m_options.insert(key.str(), m_opt_algo);
    }
    if (m_opt_name == "nlopt"){
        if (optimizerParams.count(keys[3])) {
            bool val = optimizerParams[keys[3]].get<bool>();
            m_options.insert(keys[3], val);
        } else {
            m_options.insert("maximize", true);
        }
    }
    return m_options;
}


// Function takes a graph file, or file of Pauli Observables, as an input and outputs the corresponding Hamiltonian
void qaoa_from_file::read_hamiltonian(const std::string& graphFile, xacc::quantum::PauliOperator& H) {
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


void qaoa_from_file::execute() {
    if (m_in_config == true){
        read_json();
    }

    // Define the graph from H_file
    xacc::quantum::PauliOperator H;
    read_hamiltonian(m_graph_file, H);
    std::cout << H.toString() << "\n";
    xacc::Observable* obs = &H;

    // Target the user-specified backend
    auto acc = xacc::getAccelerator(m_acc_name);
    
    // Define the gradient
    auto gradient = xacc::getService<xacc::AlgorithmGradientStrategy>("central");
    gradient->initialize({{"step", .1}, {"observable", xacc::as_shared_ptr(obs)}});

    // Configuring the optimizer parameters and calling optimizer
    xacc::HeterogeneousMap m_options = set_optimizer();
    auto optimizer = xacc::getOptimizer(m_opt_name, m_options);

    // turn on verbose output
    xacc::set_verbose(true);
    // Initialize QAOA
    auto qaoa = xacc::getAlgorithm("QAOA", {{"accelerator", acc},
                                            {"optimizer", optimizer},
                                            {"observable", obs},
                                            {"steps", m_steps},
                                            {"gradient_strategy", gradient},
                                            {"parameter-scheme", "Standard"}});

    // Allocate necessary amount of qubits and execute
    auto buffer = xacc::qalloc(H.nBits());
    qaoa->execute(buffer);

    // Print out results to terminal
    std::cout << "Min Val: " << (*buffer)["opt-val"].as<double>() << "\n";
    std::cout << "Opt vals: ";
    for (auto param : (*buffer)["opt-params"].as<std::vector<double>>()) {
        std::cout << param << " ";
    }
    std::cout << "\n";

    // Compute and return the State Vector
    auto state_vector_calc = 
        xacc::getAccelerator("aer", {{"sim-type", "statevector"}});
    auto qaoa_statevector = xacc::getAlgorithm("QAOA", {{"accelerator", state_vector_calc},
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

    // Flag to return output files or not
    if (m_out_file == true){
        // Create a directory based on input config file name and 
        // the current date (YYYY-MM-DD.HH.SS) to hold output files
        std::stringstream dirName;
        if (m_config_file.empty()){
            dirName << get_timestamp();
        } else {
            size_t lastindex = m_config_file.find_last_of("."); 
            std::string filename = m_config_file.substr(0, lastindex); 
            dirName << filename << get_timestamp();
        }
        mkdir(dirName.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH);

        // Writing expectation value and optimal parameters to file
        std::stringstream sst;
        sst << dirName.str() << "/QAOA_dat.txt";
        std::ofstream file(sst.str());
        file << (*buffer)["opt-val"].as<double>() << "              ";
        for (auto param : (*buffer)["opt-params"].as<std::vector<double>>()) {
            file << param << "   ";
        }

        // Writing state vector to file in format:
        // Line 1: real components
        // Line 2: imaginary components
        std::stringstream sv;
        sv << dirName.str() << "/statevector.txt";
        std::ofstream statefile(sv.str());
        for (auto [real, imag] : state_vector) {
            statefile << real << "    ";
        }
        statefile << "\n";
        for (auto [real, imag] : state_vector) {
            statefile << imag << "    ";
        }
    }
} 