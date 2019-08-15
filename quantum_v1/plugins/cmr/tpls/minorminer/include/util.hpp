#pragma once
#include <algorithm>
#include <chrono>
#include <mutex>
#include <random>
#include <thread>
#include <unordered_map>
#include "debug.hpp"
#include "pairing_queue.hpp"

namespace find_embedding {
// Import some things from the std library
using std::default_random_engine;
using std::vector;
using std::string;
using std::shared_ptr;
using std::map;
using std::unordered_map;
using std::pair;
using std::numeric_limits;
using std::uniform_int_distribution;
using std::min;
using std::max;
using std::thread;
using std::mutex;
using std::chrono::duration;
using std::chrono::duration_cast;

// Select some default structures and types
using distance_t = long long int;
constexpr distance_t max_distance = numeric_limits<distance_t>::max();
using RANDOM = default_random_engine;
using clock = std::chrono::high_resolution_clock;
using pairing_queue::pairing_queue_fast_reset;
using distance_queue = pairing_queue::pairing_queue_fast_reset<distance_t>;
using int_queue = pairing_queue::pairing_queue_fast_reset<int64_t>;

//! Interface for communication between the library and various bindings.
//!
//! Any bindings of this library need to provide a concrete subclass.
class LocalInteraction {
  public:
    virtual ~LocalInteraction() {}
    //! Print a message through the local output method
    void displayOutput(const string& msg) const { displayOutputImpl(msg); }

    //! Check if someone is trying to cancel the embedding process
    bool cancelled(const clock::time_point stoptime) const {
        if (cancelledImpl()) {
            displayOutput("caught interrupt; embedding cancelled\n");
            return true;
        }
        if (timedOutImpl(stoptime)) {
            displayOutput("embedding timed out\n");
            return true;
        }
        return false;
    }

  private:
    //! Print the string to a binding specified sink
    virtual void displayOutputImpl(const string&) const = 0;

    //! Check if the embedding process has timed out.
    virtual bool timedOutImpl(const clock::time_point stoptime) const { return clock::now() >= stoptime; }

    //! Check if someone has tried to cancel the embedding process
    virtual bool cancelledImpl() const = 0;
};

typedef shared_ptr<LocalInteraction> LocalInteractionPtr;

//! Set of parameters used to control the embedding process.
class optional_parameters {
  public:
    //! actually not controlled by user, not initialized here, but initialized in Python, MATLAB, C wrappers level
    LocalInteractionPtr localInteractionPtr;
    int max_no_improvement = 10;
    RANDOM rng;
    //! Number of seconds before the process unconditionally stops
    double timeout = 1000;
    int tries = 10;
    int verbose = 0;
    int inner_rounds = numeric_limits<int>::max();
    int max_fill = numeric_limits<int>::max();
    bool return_overlap = false;
    int chainlength_patience = 2;
    int threads = 1;
    bool skip_initialization = false;
    map<int, vector<int> > fixed_chains;
    map<int, vector<int> > initial_chains;
    map<int, vector<int> > restrict_chains;

    template <typename... Args>
    void printx(const char* format, Args... args) const {
        char buffer[1024];
        snprintf(buffer, 1024, format, args...);
        localInteractionPtr->displayOutput(buffer);
    }

    template <typename... Args>
    void error(const char* format, Args... args) const {
        if (verbose >= 0) printx(format, args...);
    }

    template <typename... Args>
    void major_info(const char* format, Args... args) const {
        if (verbose >= 1) printx(format, args...);
    }

    template <typename... Args>
    void minor_info(const char* format, Args... args) const {
        if (verbose >= 2) printx(format, args...);
    }

    template <typename... Args>
    void extra_info(const char* format, Args... args) const {
        if (verbose >= 3) printx(format, args...);
    }

    template <typename... Args>
    void debug(const char* format, Args... args) const {
#ifdef CPPDEBUG
        if (verbose >= 4) printx(format, args...);
#endif
    }

    optional_parameters() : localInteractionPtr(), rng() {}
    void seed(unsigned int randomSeed) { rng.seed(randomSeed); }
};

class FindEmbeddingException {
  public:
    FindEmbeddingException(const string& m = "find embedding exception") : message(m) {}
    const string& what() const { return message; }

  private:
    string message;
};

class ProblemCancelledException : public FindEmbeddingException {
  public:
    ProblemCancelledException(const string& m = "problem cancelled exception") : FindEmbeddingException(m) {}
};

//! Fill output with the index of all of the minimum and equal values in input
template <typename T>
void collectMinima(const vector<T>& input, vector<int>& output) {
    output.clear();
    auto lowest_value = input[0];
    int index = 0;
    for (auto& y : input) {
        if (y == lowest_value) {
            output.push_back(index);
        } else if (y < lowest_value) {
            output.clear();
            output.push_back(index);
            lowest_value = y;
        }
        index++;
    }
}
}
