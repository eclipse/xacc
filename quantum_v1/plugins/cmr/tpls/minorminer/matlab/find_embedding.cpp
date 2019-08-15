#include <mex.h>
#include <random>
#include <set>
#include <string>

#include "embedding_problem.hpp"
#include "find_embedding.hpp"
#include "graph.hpp"
#include "util.hpp"

extern "C" bool utIsInterruptPending();
extern "C" void utSetInterruptPending(bool);

namespace {

template <class T>
void parseScalar(const mxArray* a, const char* errorMsg, T& t) {
    if (!a || mxIsEmpty(a) || mxIsSparse(a) || !mxIsDouble(a) || mxIsComplex(a) || mxGetNumberOfDimensions(a) != 2 ||
        mxGetNumberOfElements(a) != 1)
        throw find_embedding::FindEmbeddingException(errorMsg);

    if (mxIsNaN(static_cast<double>(*mxGetPr(a)))) throw find_embedding::FindEmbeddingException(errorMsg);

    t = static_cast<T>(*mxGetPr(a));
}

void parseMatrix(const mxArray* a, const char* errorMsg, graph::input_graph& g, int& aSize) {
    if (!a) throw find_embedding::FindEmbeddingException(errorMsg);

    int numRows = static_cast<int>(mxGetM(a));
    int numCols = static_cast<int>(mxGetN(a));

    if (mxGetNumberOfDimensions(a) != 2 || !mxIsDouble(a) || mxIsComplex(a) || numRows != numCols)
        throw find_embedding::FindEmbeddingException(errorMsg);

    aSize = numRows;

    const double* data = mxGetPr(a);

    if (mxIsSparse(a)) {
        const mwIndex* jc = mxGetJc(a);
        const mwIndex* ir = mxGetIr(a);
        for (int col = 0; col < numCols; ++col)
            for (mwIndex index = jc[col]; index < jc[col + 1]; ++index, ++data) {
                int row = static_cast<int>(ir[index]);
                // ignore the 0 entries
                if (*data != 0.0) g.push_back(row, col);
            }
    } else {
        for (int col = 0; col < numCols; ++col)
            for (int row = 0; row < numRows; ++row, ++data) {
                // ignore the 0 entries
                if (*data != 0.0) g.push_back(row, col);
            }
    }
}

void parseBoolean(const mxArray* a, const char* errorMsg, bool& b) {
    if (!a || !mxIsLogicalScalar(a) || mxIsEmpty(a)) throw find_embedding::FindEmbeddingException(errorMsg);

    b = mxIsLogicalScalarTrue(a);
}

void parseChainArray(const mxArray* a, const char* errorMsg, std::map<int, std::vector<int> >& b) {
    if (!a || !mxIsCell(a) || mxGetNumberOfDimensions(a) != 2) throw find_embedding::FindEmbeddingException(errorMsg);

    const mwSize* dims = mxGetDimensions(a);

    if (dims[0] == 0 || dims[1] == 0) return;
    if (dims[0] != 1 || dims[1] < 0) throw find_embedding::FindEmbeddingException(errorMsg);

    for (int i = dims[1]; i--;) {
        const mxArray* ichain = mxGetCell(a, i);
        if (!ichain) continue;
        if (mxGetNumberOfDimensions(ichain) != 2 || !mxIsDouble(ichain) || mxIsComplex(ichain))
            throw find_embedding::FindEmbeddingException(errorMsg);

        const mwSize* cdims = mxGetDimensions(ichain);

        if (cdims[0] == 0 || dims[1] == 0) continue;
        if (cdims[0] > 1) throw find_embedding::FindEmbeddingException(errorMsg);

        const double* data = mxGetPr(ichain);
        std::vector<int> ochain(data, data + cdims[1]);
        b.emplace(i, ochain);
    }
}

class LocalInteractionMATLAB : public find_embedding::LocalInteraction {
  private:
    virtual void displayOutputImpl(const std::string& msg) const {
        mexPrintf("%s", msg.c_str());
        mexEvalString("drawnow;");
    }

    virtual bool cancelledImpl() const {
        if (utIsInterruptPending()) {
            utSetInterruptPending(false);
            return true;
        }

        return false;
    }
};

void checkFindEmbeddingParameters(const mxArray* paramsArray,
                                  find_embedding::optional_parameters& findEmbeddingExternalParams) {
    // mxIsEmpty(paramsArray) handles the case when paramsArray is: struct('a', {}), since isempty(struct('a', {})) is
    // true !!!
    // mxGetM(paramsArray) != 1 || mxGetN(paramsArray) != 1 handles the case when paramsArray is: struct('a', {1 1}) or
    // struct('a', {1; 1}),
    // since size(struct('a', {1 1})) is 1 by 2 and size(struct('a', {1; 1})) is 2 by 1 !!!
    if (!mxIsStruct(paramsArray) || mxIsEmpty(paramsArray) || mxGetNumberOfDimensions(paramsArray) != 2 ||
        mxGetM(paramsArray) != 1 || mxGetN(paramsArray) != 1)
        throw find_embedding::FindEmbeddingException(
                "find_embeddingmex parameters must be a non-empty 1 by 1 structure");

    std::set<std::string> paramsNameSet;
    paramsNameSet.insert("max_no_improvement");
    paramsNameSet.insert("random_seed");
    paramsNameSet.insert("timeout");
    paramsNameSet.insert("tries");
    paramsNameSet.insert("verbose");
    paramsNameSet.insert("inner_rounds");
    paramsNameSet.insert("max_fill");
    paramsNameSet.insert("return_overlap");
    paramsNameSet.insert("chainlength_patience");
    paramsNameSet.insert("skip_initialization");
    paramsNameSet.insert("fixed_chains");
    paramsNameSet.insert("initial_chains");
    paramsNameSet.insert("restrict_chains");
    paramsNameSet.insert("threads");

    int numFields = mxGetNumberOfFields(paramsArray);
    for (int i = 0; i < numFields; ++i) {
        std::string str = mxGetFieldNameByNumber(paramsArray, i);
        if (paramsNameSet.find(str) == paramsNameSet.end())
            throw find_embedding::FindEmbeddingException(std::string(1, '\'') + str +
                                                         "' is not a valid parameter for findEmbedding");
    }

    const mxArray* fieldValueArray;

    fieldValueArray = mxGetField(paramsArray, 0, "skip_initialization");
    if (fieldValueArray)
        parseBoolean(fieldValueArray, "skip_initialization must be a boolean value",
                     findEmbeddingExternalParams.skip_initialization);

    fieldValueArray = mxGetField(paramsArray, 0, "max_no_improvement");
    if (fieldValueArray)
        parseScalar<int>(fieldValueArray, "max_no_improvement must be an integer >= 0",
                         findEmbeddingExternalParams.max_no_improvement);

    fieldValueArray = mxGetField(paramsArray, 0, "random_seed");
    if (fieldValueArray) {
        unsigned int seed;
        parseScalar<unsigned int>(fieldValueArray, "random_seed must be an integer", seed);
        findEmbeddingExternalParams.seed(seed);
    }

    fieldValueArray = mxGetField(paramsArray, 0, "timeout");
    if (fieldValueArray)
        parseScalar<double>(fieldValueArray, "timeout parameter must be a number >= 0.0",
                            findEmbeddingExternalParams.timeout);

    fieldValueArray = mxGetField(paramsArray, 0, "tries");
    if (fieldValueArray)
        parseScalar<int>(fieldValueArray, "tries parameter must be an integer >= 0", findEmbeddingExternalParams.tries);

    fieldValueArray = mxGetField(paramsArray, 0, "inner_rounds");
    if (fieldValueArray)
        parseScalar<int>(fieldValueArray, "inner_rounds parameter must be an integer >= 0",
                         findEmbeddingExternalParams.inner_rounds);

    fieldValueArray = mxGetField(paramsArray, 0, "max_fill");
    if (fieldValueArray)
        parseScalar<int>(fieldValueArray, "max_fill parameter must be an integer >= 0",
                         findEmbeddingExternalParams.max_fill);

    fieldValueArray = mxGetField(paramsArray, 0, "threads");
    if (fieldValueArray)
        parseScalar<int>(fieldValueArray, "threads parameter must be an integer >= 0",
                         findEmbeddingExternalParams.threads);

    fieldValueArray = mxGetField(paramsArray, 0, "chainlength_patience");
    if (fieldValueArray)
        parseScalar<int>(fieldValueArray, "chainlength_patience parameter must be an integer >= 0",
                         findEmbeddingExternalParams.chainlength_patience);

    fieldValueArray = mxGetField(paramsArray, 0, "verbose");
    if (fieldValueArray)
        parseScalar(fieldValueArray, "verbose parameter must be an integer >= 0", findEmbeddingExternalParams.verbose);

    fieldValueArray = mxGetField(paramsArray, 0, "return_overlap");
    if (fieldValueArray)
        parseBoolean(fieldValueArray, "return_overlap parameter must be a boolean value",
                     findEmbeddingExternalParams.return_overlap);

    fieldValueArray = mxGetField(paramsArray, 0, "initial_chains");
    if (fieldValueArray)
        parseChainArray(fieldValueArray, "initial_chains parameter must be a cell array of matrices",
                        findEmbeddingExternalParams.initial_chains);

    fieldValueArray = mxGetField(paramsArray, 0, "fixed_chains");
    if (fieldValueArray)
        parseChainArray(fieldValueArray, "fixed_chains parameter must be a cell array of matrices",
                        findEmbeddingExternalParams.fixed_chains);

    fieldValueArray = mxGetField(paramsArray, 0, "restrict_chains");
    if (fieldValueArray)
        parseChainArray(fieldValueArray, "restrict_chains parameter must be a cell array of matrices",
                        findEmbeddingExternalParams.restrict_chains);
}
}

// problem, A, params
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    if (nrhs != 3) mexErrMsgTxt("Wrong number of arguments");

    if (nlhs > 2) mexErrMsgTxt("Too many outputs");

    std::vector<std::vector<int> > chains;
    int result = 0;
    find_embedding::optional_parameters findEmbeddingExternalParams;
    try {
        graph::input_graph Qg;
        graph::input_graph Ag;
        int QSize;
        parseMatrix(prhs[0], "parameter must be a full or sparse square matrix", Qg, QSize);

        std::map<std::pair<int, int>, int> AMap;
        int ASize;
        parseMatrix(prhs[1], "parameter must be a full or sparse square matrix", Ag, ASize);

        findEmbeddingExternalParams.localInteractionPtr.reset(new LocalInteractionMATLAB());
        checkFindEmbeddingParameters(prhs[2], findEmbeddingExternalParams);

        result = find_embedding::findEmbedding(Qg, Ag, findEmbeddingExternalParams, chains);
    } catch (const find_embedding::FindEmbeddingException& e) {
        mexErrMsgTxt(e.what().c_str());
    } catch (...) {
        mexErrMsgTxt("unknown error");
    }

    plhs[0] = mxCreateCellMatrix(chains.size() ? 1 : 0, chains.size());
    for (unsigned int i = chains.size(); i--;) {
        mxArray* cellPart = mxCreateDoubleMatrix(1, chains[i].size(), mxREAL);
        double* data = mxGetPr(cellPart);
        for (unsigned int j = chains[i].size(); j--;) data[j] = chains[i][j];
        mxSetCell(plhs[0], i, cellPart);
    }
    if (nlhs > 1) {
        plhs[1] = mxCreateLogicalScalar(result);
    }
}
