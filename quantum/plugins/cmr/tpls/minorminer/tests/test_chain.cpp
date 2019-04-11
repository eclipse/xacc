#include <vector>
#include "chain.hpp"
#include "gtest/gtest.h"
#include "util.hpp"
using std::vector;

struct embedding {
    std::vector<int> qubit_weights;
    std::vector<find_embedding::chain> var_embedding;
    embedding(int num_qubits, int num_vars) : qubit_weights(num_qubits, 0) {
        for (int v = 0; v < num_vars; v++) var_embedding.emplace_back(qubit_weights, v);
    }
};

class embedding_problem_t {
  public:
    inline bool accepts_qubit(int v, int q) { return true; }
};

//
TEST(chain, construction_empty) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(5, 0);
    find_embedding::chain c(weight, 0);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.get_link(0), -1);
}

TEST(chain, construction_root) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(5, 0);
    find_embedding::chain c(weight, 0);
    c.set_root(0);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 1);
}

TEST(chain, trim_root_bounce) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(5, 0);
    find_embedding::chain c(weight, 0);
    c.set_root(0);
    c.trim_leaf(0);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 1);
}

TEST(chain, trim_root_branch_bounce) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(5, 0);
    find_embedding::chain c(weight, 0);
    c.set_root(0);
    c.trim_branch(0);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 1);
}

TEST(chain, add_leaves_path) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(5, 0);
    find_embedding::chain c(weight, 0);
    c.set_root(0);
    c.add_leaf(1, 0);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 2);
    c.add_leaf(2, 1);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 3);
    c.add_leaf(3, 2);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 4);
    c.add_leaf(4, 3);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 5);
}

TEST(chain, trim_branch) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(5, 0);
    find_embedding::chain c(weight, 0);
    c.set_root(0);
    c.add_leaf(1, 0);
    c.add_leaf(2, 1);
    c.add_leaf(3, 2);
    c.add_leaf(4, 3);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 5);
    c.trim_branch(4);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(c.size(), 1);
}

TEST(chain, linkpath) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(50, 0);
    find_embedding::chain c(weight, 0);
    find_embedding::chain d(weight, 1);
    find_embedding::chain e(weight, 2);
    std::vector<int> parents(50, -1);
    c.set_root(0);

    d.set_root(5);
    parents[0] = 1;
    parents[1] = 2;
    parents[2] = 3;
    parents[3] = 5;
    c.link_path(d, 0, parents);

    e.set_root(10);
    parents[0] = 2;
    parents[2] = 1;
    parents[1] = 3;
    parents[3] = 4;
    parents[4] = 10;
    c.link_path(e, 0, parents);

    ASSERT_EQ(c.size(), 5);
    ASSERT_EQ(d.size(), 1);
    ASSERT_EQ(e.size(), 1);
}

TEST(chain, linkpath_overlap) {
    std::mt19937_64 rng(0);
    std::vector<int> weight(1, 0);
    find_embedding::chain c(weight, 0);
    find_embedding::chain d(weight, 1);
    std::vector<int> parents(1, 0);
    d = c = std::vector<int>{0};
    c.link_path(d, 0, parents);

    ASSERT_EQ(c.get_link(1), 0);
    ASSERT_EQ(d.get_link(0), 0);
}

TEST(chain, linkpathandsteal) {
    embedding_problem_t mock;
    std::mt19937_64 rng(0);
    std::vector<int> weight(50, 0);
    find_embedding::chain c(weight, 0);
    find_embedding::chain d(weight, 1);
    find_embedding::chain e(weight, 2);
    std::vector<int> parents(50, -1);
    c.set_root(0);

    d.set_root(5);
    parents[0] = 1;
    parents[1] = 2;
    parents[2] = 3;
    parents[3] = 5;
    c.link_path(d, 0, parents);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(d.run_diagnostic(), 0);

    e.set_root(10);
    parents[0] = 2;
    parents[2] = 1;
    parents[1] = 3;
    parents[3] = 4;
    parents[4] = 10;  // to be stolen
    c.link_path(e, 0, parents);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(e.run_diagnostic(), 0);

    ASSERT_EQ(c.size(), 5);
    ASSERT_EQ(d.size(), 1);
    ASSERT_EQ(e.size(), 1);

    d.steal(c, mock, 0);
    e.steal(c, mock, 0);
    ASSERT_EQ(c.run_diagnostic(), 0);
    ASSERT_EQ(d.run_diagnostic(), 0);
    ASSERT_EQ(e.run_diagnostic(), 0);

    ASSERT_EQ(c.size(), 4);
    ASSERT_EQ(d.size(), 1);
    ASSERT_EQ(e.size(), 2);
}

TEST(chain, balancechains) {
    embedding_problem_t mock;
    std::mt19937_64 rng(0);
    std::vector<int> weight(50, 0);
    find_embedding::chain c(weight, 0);
    find_embedding::chain d(weight, 1);
    std::vector<int> parents(50, -1);
    for (int i = 0; i < 50; i++) parents[i] = i - 1;
    c.set_root(0);
    d.set_root(49);
    d.link_path(c, 49, parents);

    ASSERT_EQ(c.get_link(d.label), 0);
    ASSERT_EQ(d.get_link(c.label), 1);

    c.steal(d, mock, 25);

    ASSERT_EQ(c.size(), 25);
    ASSERT_EQ(d.size(), 25);
}

TEST(chain, adoption) {
    std::vector<int> weight(50, 0);
    find_embedding::chain c(weight, 0);
    c = vector<int>{0, 1, 2};
    c.adopt(0, 1);
    c.adopt(0, 2);
    ASSERT_EQ(c.size(), 3);
    ASSERT_EQ(c.refcount(0), 3);
    ASSERT_EQ(c.refcount(1), 0);
    ASSERT_EQ(c.refcount(2), 0);
    ASSERT_EQ(c.parent(0), 0);
    ASSERT_EQ(c.parent(1), 0);
    ASSERT_EQ(c.parent(2), 0);
}

TEST(chain, copying) {
    std::vector<int> weight(50, 0);
    find_embedding::chain d(weight, 0);
    d = vector<int>{0, 1, 2};
    d.adopt(0, 1);
    d.adopt(0, 2);
    find_embedding::chain c(weight, 0);
    c = d;
    ASSERT_EQ(c.size(), 3);
    ASSERT_EQ(c.refcount(0), 3);
    ASSERT_EQ(c.refcount(1), 0);
    ASSERT_EQ(c.refcount(2), 0);
    ASSERT_EQ(c.parent(0), 0);
    ASSERT_EQ(c.parent(1), 0);
    ASSERT_EQ(c.parent(2), 0);
}

TEST(chain, clear) {
    std::vector<int> dweight(3, 0);
    std::vector<int> cweight(3, 0);
    std::vector<int> zero(3, 0);
    std::vector<int> one(3, 1);
    find_embedding::chain d(dweight, 0);
    find_embedding::chain c(cweight, 0);
    d = std::vector<int>{0, 1, 2};

    c = d;
    ASSERT_EQ(c.size(), 3);
    ASSERT_EQ(d.size(), 3);
    ASSERT_EQ(dweight, one);
    ASSERT_EQ(cweight, one);
    c.clear();
    ASSERT_EQ(dweight, one);
    ASSERT_EQ(cweight, zero);
    ASSERT_EQ(c.size(), 0);
    ASSERT_EQ(d.size(), 3);
}

TEST(chain, steal_gc) {
    embedding_problem_t mock;
    std::mt19937_64 rng(0);
    std::vector<int> weight(50, 0);
    find_embedding::chain c(weight, 0);
    find_embedding::chain d(weight, 1);
    find_embedding::chain e(weight, 2);
    std::vector<int> parents(50, -1);
    for (int i = 0; i < 50; i++) parents[i] = i - 1;
    c.set_root(0);
    d.set_root(49);
    d.link_path(c, 49, parents);

    e.set_root(49);
    for (int i = 49; i;) i = parents[i] = i / 2;
    e.link_path(c, 49, parents);

    ASSERT_EQ(c.size(), 1);
    ASSERT_EQ(d.size(), 49);
    ASSERT_EQ(e.size(), 6);

    d.steal(c, mock);
    e.steal(c, mock);

    ASSERT_EQ(c.size(), 1);
    ASSERT_EQ(d.size(), 49);
    ASSERT_EQ(e.size(), 6);

    c.steal(e, mock);
    c.steal(d, mock);

    ASSERT_EQ(c.size(), 30);
    ASSERT_EQ(d.size(), 1);
    ASSERT_EQ(e.size(), 1);

    e.steal(c, mock);
    d.steal(c, mock);

    ASSERT_EQ(c.size(), 6);
    ASSERT_EQ(d.size(), 25);
    ASSERT_EQ(e.size(), 1);
}
