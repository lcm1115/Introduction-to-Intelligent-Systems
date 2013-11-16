#ifndef ID3_H
#define ID3_H

#include "decision.h"

#include <map>
#include <string>
#include <vector>

namespace id3 {

struct vertex {
    // If the vertex is not terminal, it splits along this key
    std::string key;
    bool terminal;
    bool positive;
    // If the vertex is not terminal, these are the immediate offspring
    std::map<std::string, vertex> children;
};

// Given the root node of a tree, prints the all paths to leaves.
// 'root' is the root node of the tree
std::string tree_to_string(const vertex& root);

// Constructs paths from root node to leaves.
// 'root' is the root node of the tree
// 'values' is the list of values leading to the current node
// 'paths' is the vector of possible paths to leaves
void tree_to_paths(const vertex& root,
                   const std::vector<std::string>& values,
                   std::vector<std::string>* const paths);

// Constructs a decision tree based on the given data
vertex construct_tree(
        const std::vector<decision::node>& data, const std::string& tar_val);

// Splits the data set along the given value
std::map<std::string, std::vector<decision::node> > split_nodes(
        const std::vector<decision::node>& data, const std::string& key);

}  // namespace id3

#endif  // ID3_H
