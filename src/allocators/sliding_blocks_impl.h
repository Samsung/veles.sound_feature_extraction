/*! @file sliding_blocks_impl.h
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

#ifndef SRC_ALLOCATORS_SLIDING_BLOCKS_IMPL_H_
#define SRC_ALLOCATORS_SLIDING_BLOCKS_IMPL_H_

#include <memory>
#include <set>
#include <vector>
#include "src/allocators/buffers_allocator.h"
#include "src/logger.h"

namespace sound_feature_extraction {
namespace MemoryAllocation {

struct TraversalTreeNode {
  explicit TraversalTreeNode(std::weak_ptr<TraversalTreeNode> parent)
      : Parent(parent) {
  }

  std::vector<Node*> Chain;
  std::vector<std::shared_ptr<TraversalTreeNode>> Children;
  std::weak_ptr<TraversalTreeNode> Parent;
};

struct Block {
  Block()
      : X(0), Width(0), Y(0), Height(0), BoundNode(nullptr),
        NextNode(nullptr) {
  }

  Block(int x, int width, size_t y, size_t height, Node* node, Node* next)
      : X(x), Width(width), Y(y), Height(height), BoundNode(node),
        NextNode(next) {
  }

  bool operator<(const Block& other) const {
    return X < other.X;
  }

  int X;
  int Width;
  size_t Y;
  size_t Height;

  Node* BoundNode;
  Node* NextNode;
};

struct Hole {
  Hole() : X(0), Y(0), Height(0) {
  }

  Hole(int x, size_t y, size_t height)
      : X(x), Y(y), Height(height) {
  }

  bool operator<(const Hole& other) const {
    if (Y < other.Y) {
      return true;
    } else if (Y == other.Y) {
      return X < other.X;
    } else {
      return false;
    }
  }

  int X;
  size_t Y;
  size_t Height;
};

class SlidingBlocksImpl : public Logger {
 public:
  SlidingBlocksImpl()
      : Logger("SlidingBlocksAllocator", EINA_COLOR_LIGHTBLUE) {
  }

  size_t Solve(Node* root) noexcept;

 private:
  static void SimplifyTreeRecursive(
      std::shared_ptr<TraversalTreeNode> treeNode, Node* node) noexcept;
  static void EliminateLeafBranchesRecursive(
      std::shared_ptr<TraversalTreeNode> treeNode) noexcept;
  static void GenerateTreeTraversalVariants(
      const std::shared_ptr<TraversalTreeNode>& node,
      std::vector<std::vector<Node*>>* variants) noexcept;
  static void AddTraversalVariantsRecursive(
      std::vector<Node*> current, size_t index,
      const std::vector<int>& permutation,
      const std::vector<std::vector<std::vector<Node*>>>& inner,
      std::vector<std::vector<Node*>>* variants) noexcept;
  static std::set<Block> GetProblemForTraversalVariant(
      const std::vector<Node*>& variant) noexcept;
  static void GreedySolve(std::vector<Block>* blocks,
                          std::set<Block>* blocksSet = nullptr) noexcept;
  static size_t CalculateBlocksSolutionPeakHeight(
      const std::vector<Block>& blocks) noexcept;
  static void SaveBlocksSolution(const std::vector<Block>& blocks) noexcept;
};

}  // namespace MemoryAllocation
}  // namespace sound_feature_extraction
#endif  // SRC_ALLOCATORS_SLIDING_BLOCKS_IMPL_H_
