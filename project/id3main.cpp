#include "constructtree.h"
#include "id3.h"

#include <iostream>

using id3::construct_tree;
using id3::node;
using id3::tree_to_string;
using id3::read_nodes_from_filepath;
using id3::vertex;
using std::cout;
using std::string;
using std::vector;

int main(int argc, char** argv) {
    vector<node> nodes = read_nodes_from_filepath(argv[1]);
    vector<string> values;
    string tar_val = argv[2];
    vertex root = construct_tree(&nodes, tar_val);
    cout << tree_to_string(root);
}
