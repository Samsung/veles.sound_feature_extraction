/*! @file sliding_blocks_allocator.cc
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


#include "src/allocators/sliding_blocks_allocator.h"
#include <gtest/gtest.h>
#include <memory>

using sound_feature_extraction::memory_allocation::Node;
using sound_feature_extraction::memory_allocation::BuffersAllocator;
using sound_feature_extraction::memory_allocation::SlidingBlocksAllocator;

TEST(SlidingBlocksAllocator, Solve) {
  int data;
  int *item = &data;
  auto root = std::make_shared<Node>(1, nullptr, item++);
  Node* node = root.get();
  node->Children.push_back(Node(1, node, item++));

  node = &node->Children[0];
  node->Children.push_back(Node(1, node, item++));

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


  SlidingBlocksAllocator alloc;
  alloc.Solve(root.get());
  root->Dump("/tmp/sliding_blocks_allocator_test.dot");
  ASSERT_TRUE(alloc.Validate(*root));
}

#include "tests/google/src/gtest_main.cc"
