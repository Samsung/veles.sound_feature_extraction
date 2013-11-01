/*! @file worst_allocator.cc
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/allocators/worst_allocator.h"
#include <gtest/gtest.h>
#include <memory>

using sound_feature_extraction::MemoryAllocation::Node;
using sound_feature_extraction::MemoryAllocation::WorstAllocator;

class WorstAllocatorTest
    : public WorstAllocator, public testing::Test {
 public:
  virtual void SetUp() {
    int data;
    int *item = &data;
    Root = std::make_shared<Node>(1, nullptr, item++);
    Node* node = Root.get();

    node->Children.push_back(Node(1, node, item++));
    node->Children.push_back(Node(2, node, item++));

    node = &node->Children[1];

    node->Children.push_back(Node(3, node, item++));
    node->Children.push_back(Node(2, node, item++));
    node->Children.push_back(Node(4, node, item++));

    node = &node->Children[0];

    node->Children.push_back(Node(1, node, item++));
    node->Children.push_back(Node(2, node, item++));

    node = &node->Parent->Children[1];

    node->Children.push_back(Node(1, node, item++));

    node = &node->Children[0];

    node->Children.push_back(Node(1, node, item++));
    Root->Dump("/tmp/buffers_allocator_test.dot");
  }

  std::shared_ptr<Node> Root;
};

TEST_F(WorstAllocatorTest, Solve) {
  Solve(Root.get());
  ASSERT_TRUE(Validate(*Root));
  Root->Dump("/tmp/worst_allocator_test.dot");
}

#include "tests/google/src/gtest_main.cc"
