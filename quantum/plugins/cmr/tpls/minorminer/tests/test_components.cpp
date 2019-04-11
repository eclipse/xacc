#include "graph.hpp"
#include "gtest/gtest.h"

template <class Type>
std::string str(const std::set<Type>& in) {
    std::stringstream ss("{");
    size_t ii = 0;
    for (auto x : in) ss << x << (++ii == in.size() ? "" : ",");
    ss << "}";
    return ss.str();
}

//
TEST(components, construction_empty) {
    std::mt19937_64 rng(0);
    graph::input_graph graph;
    graph::components components(graph, rng, {});
    ASSERT_EQ(components.size(), 0);
}

TEST(components, construction_single) {
    std::mt19937_64 rng(0);
    graph::input_graph graph(1, {}, {});
    graph::components components(graph, rng, {0});
    ASSERT_EQ(components.size(), 0);
}

TEST(components, construction_two_disconnected) {
    std::mt19937_64 rng(0);
    graph::input_graph graph(2, {}, {});
    graph::components components(graph, rng, {0, 0});
    ASSERT_EQ(components.size(), 0);
}

TEST(components, construction_three_disconnected) {
    std::mt19937_64 rng(0);
    graph::input_graph graph(3, {0}, {1});
    graph::components components(graph, rng, {0, 0, 0});

    ASSERT_EQ(components.size(), 1);
    ASSERT_EQ(components.num_reserved(0), 0);
    auto nodes = components.nodes(0);
    ASSERT_EQ(std::set<int>(nodes.begin(), nodes.end()), std::set<int>({0, 1}));
    ASSERT_EQ(components.size(0), 2);

    auto out_graph = components.component_graph(0);
    ASSERT_EQ(out_graph.num_nodes(), 2);
    ASSERT_EQ(out_graph.num_edges(), 1);
}

TEST(components, construction_three_disconnected_again) {
    std::mt19937_64 rng(0);
    graph::input_graph graph(3, {1}, {2});
    graph::components components(graph, rng, {0, 0, 0});

    ASSERT_EQ(components.size(), 1);
    ASSERT_EQ(components.num_reserved(0), 0);
    auto nodes = components.nodes(0);
    ASSERT_EQ(std::set<int>(nodes.begin(), nodes.end()), std::set<int>({1, 2}));
    ASSERT_EQ(components.size(0), 2);

    auto out_graph = components.component_graph(0);
    ASSERT_EQ(out_graph.num_nodes(), 2);
    ASSERT_EQ(out_graph.num_edges(), 1);
}

TEST(components, construction_three_connected) {
    std::mt19937_64 rng(0);
    graph::input_graph graph(3, {0, 1}, {1, 2});
    graph::components components(graph, rng, {0, 0, 0});

    ASSERT_EQ(components.size(), 1);
    ASSERT_EQ(components.num_reserved(0), 0);
    {
        // TODO confirm nodes from this shouldn't be sorted already
        auto local_names = components.nodes(0);
        std::vector<int> global_names;
        components.from_component(0, local_names, global_names);
        ASSERT_EQ(std::set<int>(global_names.begin(), global_names.end()), std::set<int>({0, 1, 2}));
    }
    ASSERT_EQ(components.size(0), 3);

    auto out_graph = components.component_graph(0);
    ASSERT_EQ(out_graph.num_nodes(), 3);
    ASSERT_EQ(out_graph.num_edges(), 2);
}

TEST(components, construction_four_disconnected) {
    std::mt19937_64 rng(0);
    graph::input_graph graph(4, {0, 2}, {1, 3});

    std::set<int> component_a({0, 1});
    std::set<int> component_b({2, 3});
    bool a_seen = false;
    bool b_seen = false;

    graph::components components(graph, rng, {0, 0, 0, 0});

    ASSERT_EQ(components.size(), 2);
    for (int ii = 0; ii < 2; ii++) {
        EXPECT_EQ(components.num_reserved(ii), 0);
        {
            auto nodes = components.nodes(ii);
            std::set<int> sorted_nodes(nodes.begin(), nodes.end());
            EXPECT_TRUE(sorted_nodes == component_b || sorted_nodes == component_a)
                    << str(sorted_nodes) << " expected to be one of " << str(component_a) << " or " << str(component_b);
            a_seen |= component_a == sorted_nodes;
            b_seen |= component_b == sorted_nodes;
        }
        EXPECT_EQ(components.size(ii), 2);
    }
    EXPECT_TRUE(a_seen and b_seen);

    auto out_graph1 = components.component_graph(0);
    ASSERT_EQ(out_graph1.num_nodes(), 2);
    ASSERT_EQ(out_graph1.num_edges(), 1);

    auto out_graph2 = components.component_graph(1);
    ASSERT_EQ(out_graph2.num_nodes(), 2);
    ASSERT_EQ(out_graph2.num_edges(), 1);
}

//
//     // translate nodes from the input graph, to their labels in component c
//     bool into_component(const int c, const vector<int>& nodes_in, vector<int>& nodes_out) const {
//
//     // translate nodes from labels in component c, back to their original input labels
//     void from_component(const int c, const vector<int> &nodes_in, vector<int> &nodes_out) {
