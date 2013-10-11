#include "constructtree.h"
#include "id3.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

using id3::construct_tree;
using id3::entropy;
using id3::info_gain;
using id3::node;
using id3::print_tree;
using id3::read_nodes_from_filepath;
using id3::split_nodes;
using id3::subset_entropy;
using id3::vertex;
using std::cin;
using std::cout;
using std::getline;
using std::endl;
using std::ifstream;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

int main(int argc, char** argv) {
    vector<node> nodes = read_nodes_from_filepath(argv[1]);
    vector<string> values;
    string tar_val = argv[2];
    vertex root = construct_tree(&nodes, tar_val);
    print_tree(root);
}
