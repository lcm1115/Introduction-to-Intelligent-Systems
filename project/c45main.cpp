#include "c45.h"
#include "decision.h"

#include <cstdio>
#include <map>
#include <string>
#include <vector>

using decision::string_to_tokens;
using std::pair;
using std::string;
using std::vector;

using namespace c45;

int main(int argc, char** argv) {
    vector<node> nodes = read_nodes_from_filepath(argv[1]);
    vertex root = construct_tree(nodes, argv[2]);
    print(root, vector<string>());
}
