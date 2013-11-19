/*! @file buffers_allocator.cc
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

#include "src/allocators/buffers_allocator.h"
#include <fstream>  // NOLINT(*)
#include <functional>
#include <map>
#include <set>

namespace sound_feature_extraction {
namespace memory_allocation {

void Node::Dump(const std::string& dotFileName) const {
  std::ofstream fw;
  fw.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fw.open(dotFileName);
  fw << "digraph BuffersAllocator {" << std::endl;
  int index = 1;
  std::map<const Node*, int> indices;
  std::function<int(const Node&)> write_node;
  write_node = [&](const Node& node) {
    int myindex = index++;
    indices[&node] = myindex;
    fw << "\t" << std::to_string(myindex) << " [";
    if (node.Children.size() == 0) {
      fw << "style=\"filled\", fillcolor=\"green\", ";
    }
    fw << "label=<[" <<
        std::to_string(node.Address) << ", " <<
        std::to_string(node.Address + node.Size) << "]>]" << std::endl;
    std::vector<int> myChildrenIndices;
    for (size_t i = 0; i < node.Children.size(); i++) {
      myChildrenIndices.push_back(write_node(node.Children[i]));
    }
    if (!myChildrenIndices.empty()) {
      fw << "\t" << std::to_string(myindex) << " -> {";
      for (auto i : myChildrenIndices) {
        fw << std::to_string(i) << "; ";
      }
      fw.seekp(-2, std::ios::cur);
      fw << "}" << std::endl;
    }
    return myindex;
  };
  write_node(*this);
  fw << std::endl;
  auto node = this;
  auto next = this->Next;
  while (next != nullptr) {
    fw << "\t" << std::to_string(indices[node]) << " -> " <<
        std::to_string(indices[next]) << " [color=\"red\"]" << std::endl;
    node = next;
    next = next->Next;
  }
  fw << "}" << std::endl;
}

BuffersAllocator::BuffersAllocator()
    : Logger("Allocator", EINA_COLOR_LIGHTRED) {
}

bool BuffersAllocator::Validate(const Node& root) const {
  const Node* node = &root;
  std::vector<const Node*> leaves;
  std::set<const void*> items;
  std::set<const Node*> visitedNodes;
  while (node != nullptr) {
    if (visitedNodes.find(node) != visitedNodes.end()) {
      ERR("Duplicate node");
      return false;
    }
    if (node->Item == nullptr) {
      ERR("Attached Item is null.");
      return false;
    }
    if (items.find(node->Item) != items.end()) {
      ERR("Duplicate item.");
      return false;
    }
    items.insert(node->Item);
    if (node->Address == Node::UNINITIALIZED_ADDRESS) {
      ERR("Address is not initialized.");
      return false;
    }
    const Node* parent = node->Parent;
    if (parent != nullptr) {
      if (NodesOverlap(*node, *parent)) {
        ERR("Memory regions overlap on dependent nodes.");
        return false;
      }
      const Node* archParent = parent->Parent;
      while (archParent != nullptr) {
        if (NodesOverlap(*node, *archParent)) {
          size_t visitedChildrenCount = 0;
          for (size_t i = 0; i < archParent->Children.size(); i++) {
            if (visitedNodes.find(&archParent->Children[i]) !=
                visitedNodes.end()) {
              visitedChildrenCount++;
            }
          }
          if (visitedChildrenCount < archParent->Children.size() - 1) {
            ERR("Memory regions overlap on dependent nodes.");
            return false;
          }
        }
        archParent = archParent->Parent;
      }
    }
    for (auto leaf : leaves) {
      if (NodesOverlap(*node, *leaf)) {
        ERR("Memory regions overlap with leaves.");
        return false;
      }
    }
    if (node->Children.size() == 0) {
      leaves.push_back(node);
    }
    visitedNodes.insert(node);
    node = node->Next;
  }
  if (visitedNodes.size() < NodesCount(root)) {
    ERR("Not all nodes are linked.");
    return false;
  }
  return true;
}

size_t BuffersAllocator::NodesCount(const Node& root) const noexcept {
  size_t sum = 1;
  for (auto child : root.Children) {
    sum += NodesCount(child);
  }
  return sum;
}

bool BuffersAllocator::NodesOverlap(const Node& n1, const Node& n2) noexcept {
  if (n1.Size * n2.Size == 0) {
    return false;
  }
  return SegmentsOverlap(n1.Address, n1.Address + n1.Size,
                         n2.Address, n2.Address + n2.Size);
}

bool BuffersAllocator::SegmentsOverlap(size_t a1, size_t a2,
                                       size_t b1, size_t b2) noexcept {
  return (a1 > b1 && a1 < b2) || (a2 > b1 && a2 < b2) || (a1 <= b1 && a2 >= b2);
}

}  // namespace sound_feature_extraction
}  // namespace memory_allocation
