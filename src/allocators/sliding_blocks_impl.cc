/*! @file sliding_blocks_impl.cc
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

#include "sliding_blocks_impl.h"
#include <assert.h>
#include <algorithm>
#include <map>
#include "src/safe_omp.h"

extern "C" {
  /// @brief Gets the maximal number of threads setting from API.
  /// @note The implementation of this function is in src/api.cc.
  extern int get_omp_transforms_max_threads_num();
}

namespace SoundFeatureExtraction {
namespace MemoryAllocation {

size_t SlidingBlocksImpl::Solve(Node* root) noexcept {
  std::vector<std::vector<Node*>> traversalVariants;
  {
    auto treeRoot = std::make_shared<TraversalTreeNode>(
        std::shared_ptr<TraversalTreeNode>(nullptr));
    DBG("Simplifying the tree...");
    SimplifyTreeRecursive(treeRoot, root);
    EliminateLeafBranchesRecursive(treeRoot);
    DBG("Generating traversal variants...");
    GenerateTreeTraversalVariants(treeRoot, &traversalVariants);
    INF("Solving %zu traversal variants...", traversalVariants.size());
  }
  size_t min_height = 0;
  std::vector<Block> solution;
#ifdef HAVE_OPENMP
  #pragma omp parallel for num_threads(get_omp_transforms_max_threads_num())
#endif
  for (size_t i = 0; i <
#ifndef DEBUG
          traversalVariants.size();
#else
          std::min(traversalVariants.size(), static_cast<size_t>(50000));
#endif
      i++) {
    auto blocks_set = GetProblemForTraversalVariant(traversalVariants[i]);
    std::vector<Block> blocks(blocks_set.begin(), blocks_set.end());
    GreedySolve(&blocks, &blocks_set);
    size_t height = CalculateBlocksSolutionPeakHeight(blocks);
    #pragma omp critical
    {
      if (height < min_height || min_height == 0) {
        min_height = height;
        solution = blocks;
      }
    }
  }
  SaveBlocksSolution(solution);
  return min_height;
}

void SlidingBlocksImpl::SimplifyTreeRecursive(
    std::shared_ptr<TraversalTreeNode> treeNode,
    Node* node) noexcept {
  treeNode->Chain.push_back(node);
  while (node->Children.size() == 1) {
    node = &node->Children[0];
    treeNode->Chain.push_back(node);
  }
  for (size_t i = 0; i < node->Children.size(); i++) {
    treeNode->Children.push_back(
        std::make_shared<TraversalTreeNode>(treeNode));
    SimplifyTreeRecursive(treeNode->Children.back(), &node->Children[i]);
  }
}

void SlidingBlocksImpl::EliminateLeafBranchesRecursive(
    std::shared_ptr<TraversalTreeNode> treeNode) noexcept {
  bool canMergeLeaves = true;
  for (auto& branch : treeNode->Children) {
    if (!branch->Children.empty()) {
      canMergeLeaves = false;
      break;
    }
  }
  if (canMergeLeaves) {
    for (auto& branch : treeNode->Children) {
      treeNode->Chain.insert(treeNode->Chain.end(),
                             branch->Chain.begin(),
                             branch->Chain.end());
    }
    treeNode->Children.clear();
  }
}

constexpr int factorial(int n) {
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

void SlidingBlocksImpl::GenerateTreeTraversalVariants(
    const std::shared_ptr<TraversalTreeNode>& node,
    std::vector<std::vector<Node*>>* variants) noexcept {
  if (node->Children.size() == 0) {
    variants->push_back(node->Chain);
    return;
  }
  assert(node->Children.size() > 1 && "The tree was not simplified");
  std::vector<std::vector<std::vector<Node*>>> innerVariants(
      node->Children.size());
  size_t innerVariantsCount = 1;
  size_t innerVariantSize = 0;
  for (size_t i = 0; i < node->Children.size(); i++) {
    GenerateTreeTraversalVariants(node->Children[i], &innerVariants[i]);
    innerVariantsCount *= innerVariants[i].size();
    innerVariantSize += innerVariants[i][0].size();
  }
  variants->reserve(factorial(node->Children.size()) * innerVariantsCount);
  std::vector<int> permutation(node->Children.size());
  for (int i = 0; i < static_cast<int>(permutation.size()); i++) {
    permutation[i] = i;
  }
  std::vector<Node*> tmpVariant;
  tmpVariant.reserve(node->Chain.size() + innerVariantSize);
  tmpVariant.insert(tmpVariant.end(), node->Chain.begin(), node->Chain.end());
  do {
    AddTraversalVariantsRecursive(tmpVariant, 0, permutation, innerVariants, variants);
  } while (std::next_permutation(permutation.begin(), permutation.end()));
}

void SlidingBlocksImpl::AddTraversalVariantsRecursive(
      std::vector<Node*> current, size_t index,
      const std::vector<int>& permutation,
      const std::vector<std::vector<std::vector<Node*>>>& inner,
      std::vector<std::vector<Node*>>* variants) noexcept {
  for (auto variant : inner[permutation[index]]) {
    current.insert(current.end(), variant.begin(), variant.end());
    if (index == permutation.size() - 1) {
      variants->push_back(current);
    } else {
      AddTraversalVariantsRecursive(current, index + 1, permutation, inner,
                                    variants);
    }
    current.resize(current.size() - variant.size());
  }
}

std::set<Block> SlidingBlocksImpl::GetProblemForTraversalVariant(
    const std::vector<Node*>& variant) noexcept {
  std::set<Block> blocks;
  size_t y = 0;
  std::map<Node*, Block> visitedNodes;
  std::map<Node*, size_t> visitedParentChildren;
  for (size_t i = 0; i < variant.size(); i++) {
    Node* node = variant[i];
    Node* parent = node->Parent;
    if (parent != nullptr && visitedNodes[parent].Width == 0) {
      if (++visitedParentChildren[parent] == parent->Children.size()) {
        auto parentBlock = visitedNodes[parent];
        parentBlock.Width = i + 1 - parentBlock.X;
        blocks.insert(parentBlock);
      }
    }
    Block block(i, 0, y, node->Size, node,
                (i < variant.size() - 1)? variant[i + 1] : nullptr);
    y += node->Size;
    switch (node->Children.size()) {
      case 0:
        block.Width = variant.size() - i;
        blocks.insert(block);
        break;
      case 1:
        block.Width = 2;
        blocks.insert(block);
        break;
      default:
        break;
    }
    visitedNodes.insert(std::make_pair(node, block));
  }
  return blocks;
}

void SlidingBlocksImpl::GreedySolve(
    std::vector<Block>* blocks,
    std::set<Block>* blocksSet) noexcept {
  std::set<Block>* blocks_left = (blocksSet != nullptr)?
      blocksSet : new std::set<Block>(blocks->begin(), blocks->end());
  std::vector<size_t> relief(blocks->size());
  for (size_t i = 0; i < blocks->size(); i++) {
    relief[i] = 0;
  }
  while (!blocks_left->empty()) {
    size_t current_x = blocks_left->begin()->X;
    while (current_x < blocks->size()) {
      auto &block = (*blocks)[current_x];
      size_t max_y = relief[current_x];
      blocks_left->erase(block);
      for (size_t i = current_x; i < current_x + block.Width; i++) {
        max_y = std::max(max_y, relief[i]);
      }
      block.Y = max_y;
      for (size_t i = current_x; i < current_x + block.Width; i++) {
        relief[i] = max_y + block.Height;
      }
      current_x += block.Width;
    }
  }
  if (blocksSet == nullptr) {
    delete blocks_left;
  }
}

size_t SlidingBlocksImpl::CalculateBlocksSolutionPeakHeight(
    const std::vector<Block>& blocks) noexcept {
  size_t max = 0;
  for (auto& block : blocks) {
    max = std::max(block.Y + block.Height, max);
  }
  return max;
}

void SlidingBlocksImpl::SaveBlocksSolution(
    const std::vector<Block>& blocks) noexcept {
  for (auto& block : blocks) {
    block.BoundNode->Next = block.NextNode;
    block.BoundNode->Address = block.Y;
  }
}

}  // namespace MemoryAllocation
}  // namespace SoundFeatureExtraction
