#include "graph.hpp"
#include "gtest/gtest.h"

// Construct an empty queue
TEST(pairing_queue, construction) {
    pairing_queue::pairing_queue<int> queue(10);

    int key, value;

    auto has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}

// Construct an empty queue
TEST(pairing_queue, construction_reset) {
    pairing_queue::pairing_queue<int> queue(10);
    queue.reset();

    int key, value;

    auto has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}

// Construct an empty queue and reset it to a given value
TEST(pairing_queue, insert_single) {
    pairing_queue::pairing_queue<int> queue(10);
    queue.reset();

    queue.set_value(5, 0);

    int key, value;
    auto has_result = queue.pop_min(key, value);
    EXPECT_EQ(value, 0);
    EXPECT_EQ(key, 5);
    EXPECT_TRUE(has_result);
    EXPECT_TRUE(queue.empty());
}

TEST(pairing_queue, insert_two) {
    pairing_queue::pairing_queue<int> queue(10);
    queue.reset();

    queue.set_value(5, 10);
    queue.set_value(8, 6);

    int key, value;
    auto has_result = queue.pop_min(key, value);
    ASSERT_TRUE(has_result);
    EXPECT_EQ(value, 6);
    EXPECT_EQ(key, 8);
    ASSERT_FALSE(queue.empty());

    has_result = queue.pop_min(key, value);
    ASSERT_TRUE(has_result);
    EXPECT_EQ(value, 10);
    EXPECT_EQ(key, 5);
    ASSERT_TRUE(queue.empty());
}

// Construct an empty queue and reset it to a given value
TEST(pairing_queue, insert_reverse) {
    pairing_queue::pairing_queue<int> queue(10);
    queue.reset();

    for (int ii = 0; ii < 10; ii++) {
        queue.set_value(ii, 10 - ii);
    }

    for (int ii = 0; ii < 9; ii++) {
        int key, value;
        auto has_result = queue.pop_min(key, value);
        EXPECT_EQ(value, ii + 1);
        EXPECT_EQ(key, 9 - ii);
        EXPECT_TRUE(has_result);
        EXPECT_FALSE(queue.empty());
    }

    int key, value;
    auto has_result = queue.pop_min(key, value);
    EXPECT_EQ(key, 0);
    EXPECT_EQ(value, 10);
    EXPECT_TRUE(has_result);
    EXPECT_TRUE(queue.empty());

    has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}

// Construct an empty queue and reset it to a given value
TEST(pairing_queue, insert_forward) {
    pairing_queue::pairing_queue<int> queue(10);
    queue.reset();

    for (int ii = 0; ii < 10; ii++) {
        queue.set_value(ii, ii);
    }

    for (int ii = 0; ii < 9; ii++) {
        int key, value;
        auto has_result = queue.pop_min(key, value);
        EXPECT_EQ(value, ii);
        EXPECT_EQ(key, ii);
        EXPECT_TRUE(has_result);
        EXPECT_FALSE(queue.empty());
    }

    int key, value;
    auto has_result = queue.pop_min(key, value);
    EXPECT_EQ(key, 9);
    EXPECT_EQ(value, 9);
    EXPECT_TRUE(has_result);
    EXPECT_TRUE(queue.empty());

    has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}

// Construct an empty queue
TEST(pairing_queue_fast_reset, construction) {
    pairing_queue::pairing_queue_fast_reset<int> queue(10);

    int key, value;

    auto has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}

// Construct an empty queue
TEST(pairing_queue_fast_reset, construction_reset) {
    pairing_queue::pairing_queue_fast_reset<int> queue(10);
    queue.reset();

    int key, value;

    auto has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}

// Construct an empty queue and reset it to a given value
TEST(pairing_queue_fast_reset, insert_single) {
    pairing_queue::pairing_queue_fast_reset<int> queue(10);
    queue.reset();

    queue.set_value(5, 0);

    int key, value;
    auto has_result = queue.pop_min(key, value);
    EXPECT_EQ(value, 0);
    EXPECT_EQ(key, 5);
    EXPECT_TRUE(has_result);
    EXPECT_TRUE(queue.empty());
}

TEST(pairing_queue_fast_reset, insert_two) {
    pairing_queue::pairing_queue_fast_reset<int> queue(10);
    queue.reset();

    queue.set_value(5, 10);
    queue.set_value(8, 6);

    int key, value;
    auto has_result = queue.pop_min(key, value);
    ASSERT_TRUE(has_result);
    EXPECT_EQ(value, 6);
    EXPECT_EQ(key, 8);
    ASSERT_FALSE(queue.empty());

    has_result = queue.pop_min(key, value);
    ASSERT_TRUE(has_result);
    EXPECT_EQ(value, 10);
    EXPECT_EQ(key, 5);
    ASSERT_TRUE(queue.empty());
}

// Construct an empty queue and reset it to a given value
TEST(pairing_queue_fast_reset, insert_reverse) {
    pairing_queue::pairing_queue_fast_reset<int> queue(10);
    queue.reset();

    for (int ii = 0; ii < 10; ii++) {
        queue.set_value(ii, 10 - ii);
    }

    for (int ii = 0; ii < 9; ii++) {
        int key, value;
        auto has_result = queue.pop_min(key, value);
        EXPECT_EQ(value, ii + 1);
        EXPECT_EQ(key, 9 - ii);
        EXPECT_TRUE(has_result);
        EXPECT_FALSE(queue.empty());
    }

    int key, value;
    auto has_result = queue.pop_min(key, value);
    EXPECT_EQ(key, 0);
    EXPECT_EQ(value, 10);
    EXPECT_TRUE(has_result);
    EXPECT_TRUE(queue.empty());

    has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}

// Construct an empty queue and reset it to a given value
TEST(pairing_queue_fast_reset, insert_forward) {
    pairing_queue::pairing_queue_fast_reset<int> queue(10);
    queue.reset();

    for (int ii = 0; ii < 10; ii++) {
        queue.set_value(ii, ii);
    }

    for (int ii = 0; ii < 9; ii++) {
        int key, value;
        auto has_result = queue.pop_min(key, value);
        EXPECT_EQ(value, ii);
        EXPECT_EQ(key, ii);
        EXPECT_TRUE(has_result);
        EXPECT_FALSE(queue.empty());
    }

    int key, value;
    auto has_result = queue.pop_min(key, value);
    EXPECT_EQ(key, 9);
    EXPECT_EQ(value, 9);
    EXPECT_TRUE(has_result);
    EXPECT_TRUE(queue.empty());

    has_result = queue.pop_min(key, value);
    EXPECT_FALSE(has_result);
    EXPECT_TRUE(queue.empty());
}
