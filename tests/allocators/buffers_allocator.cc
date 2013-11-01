/*! @file buffers_allocator.cc
 *  @brief Tests for sound_feature_extraction::MemoryAllocation::BuffersAllocator.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/allocators/buffers_allocator.h"
#include <gtest/gtest.h>
#include <memory>

using sound_feature_extraction::MemoryAllocation::Node;
using sound_feature_extraction::MemoryAllocation::BuffersAllocator;

class BuffersAllocatorTest
    : public BuffersAllocator, public testing::Test {
 public:
  virtual size_t Solve(Node*) const noexcept override {
   return 0;
  }

  virtual void SetUp() {
    int data;
    int *item = &data;
    Root = std::make_shared<Node>(1, nullptr, item++);
    Node* node = Root.get(), *prev = node;
    node->Address = 0;

    node->Children.push_back(Node(1, node, item++));
    node->Children[node->Children.size() - 1].Address = 1;
    node->Children.push_back(Node(2, node, item++));
    node->Children[node->Children.size() - 1].Address = 2;
    prev->Next = &node->Children[0];
    node->Children[node->Children.size() - 2].Next =
        &node->Children[node->Children.size() - 1];

    prev = &node->Children[node->Children.size() - 1];
    node = &node->Children[1];

    node->Children.push_back(Node(3, node, item++));
    node->Children[node->Children.size() - 1].Address = 4;

    node->Children.push_back(Node(2, node, item++));
    node->Children[node->Children.size() - 1].Address = 7;

    node->Children.push_back(Node(4, node, item++));
    node->Children[node->Children.size() - 1].Address = 9;

    prev->Next = &node->Children[0];
    node->Children[node->Children.size() - 2].Next =
        &node->Children[node->Children.size() - 1];
    node->Children[node->Children.size() - 3].Next =
        &node->Children[node->Children.size() - 2];

    prev = &node->Children[node->Children.size() - 1];
    node = &node->Children[0];

    node->Children.push_back(Node(1, node, item++));
    node->Children[node->Children.size() - 1].Address = 0;

    node->Children.push_back(Node(2, node, item++));
    node->Children[node->Children.size() - 1].Address = 2;
    prev->Next = &node->Children[0];
    node->Children[node->Children.size() - 2].Next =
        &node->Children[node->Children.size() - 1];

    prev = &node->Children[node->Children.size() - 1];
    node = &node->Parent->Children[1];

    node->Children.push_back(Node(1, node, item++));
    node->Children[node->Children.size() - 1].Address = 13;
    prev->Next = &node->Children[0];

    prev = &node->Children[node->Children.size() - 1];
    node = &node->Children[0];

    node->Children.push_back(Node(1, node, item++));
    node->Children[node->Children.size() - 1].Address = 4;
    prev->Next = &node->Children[0];
    node->Children[0].Next = nullptr;
    Root->Dump("/tmp/buffers_allocator_test.dot");
  }

  std::shared_ptr<Node> Root;
};

TEST_F(BuffersAllocatorTest, ValidateCorrect) {
  ASSERT_TRUE(Validate(*Root));
}

TEST_F(BuffersAllocatorTest, ValidateLeavesIntersect) {
  auto backup = Root->Children[1].Children[1].Children[0].Children[0].Address;
  Root->Children[1].Children[1].Children[0].Children[0].Address = 0;
  ASSERT_FALSE(Validate(*Root));
  Root->Children[1].Children[1].Children[0].Children[0].Address = backup;
}

TEST_F(BuffersAllocatorTest, ValidateIntersectWithParent) {
  auto backup = Root->Children[1].Children[1].Children[0].Children[0].Address;
  Root->Children[1].Children[1].Children[0].Children[0].Address = 13;
  ASSERT_FALSE(Validate(*Root));
  Root->Children[1].Children[1].Children[0].Children[0].Address = backup;
}

TEST_F(BuffersAllocatorTest, ValidateIntersect) {
  auto backup = Root->Children[2].Address;
  Root->Children[1].Children[2].Address = 3;
  ASSERT_FALSE(Validate(*Root));
  Root->Children[1].Children[2].Address = backup;
}

TEST_F(BuffersAllocatorTest, ValidateAddress) {
  auto backup = Root->Children[2].Address;
  Root->Children[1].Children[2].Address = Node::UNINITIALIZED_ADDRESS;
  ASSERT_FALSE(Validate(*Root));
  Root->Children[1].Children[2].Address = backup;
}

TEST_F(BuffersAllocatorTest, ValidateDuplicateNode) {
  auto backup = Root->Children[2].Next;
  Root->Children[1].Children[2].Next = &Root->Children[1];
  ASSERT_FALSE(Validate(*Root));
  Root->Children[1].Children[2].Next = backup;
}

TEST_F(BuffersAllocatorTest, ValidateDuplicateItem) {
  auto backup = Root->Children[2].Item;
  Root->Children[1].Children[2].Item = Root->Children[1].Item;
  ASSERT_FALSE(Validate(*Root));
  Root->Children[1].Children[2].Item = backup;
}

TEST_F(BuffersAllocatorTest, ValidateIncomplete) {
  ASSERT_EQ(10U, NodesCount(*Root));
  auto backup = Root->Children[1].Next;
  Root->Children[1].Children[1].Next = nullptr;
  ASSERT_FALSE(Validate(*Root));
  Root->Children[1].Children[1].Next = backup;
}

TEST_F(BuffersAllocatorTest, ValidateZeroSize) {
  auto backup = Root->Children[2].Size;
  Root->Children[1].Children[2].Size = 0;
  Root->Children[1].Children[2].Address = 0;
  ASSERT_TRUE(Validate(*Root));
  Root->Children[1].Children[2].Size = backup;
}

#include "tests/google/src/gtest_main.cc"
