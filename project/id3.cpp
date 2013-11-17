#include "decision.h"
#include "id3.h"

#include <cmath>
#include <cstring>
#include <fstream>

using decision::lg;
using decision::string_to_tokens;
using std::ifstream;
using std::map;
using std::string;
using std::vector;

namespace id3 {

string tree_to_string(const vertex& root) {
    string tree_string;
    vector<string> paths;
    vector<string> base;

    // Build all paths.
    tree_to_paths(root, base, &paths);
    
    for (vector<string>::const_iterator it = paths.begin();
         it != paths.end(); ++it) {
        tree_string += "If " + *it + "\n";
    }

    return tree_string;
}

void tree_to_paths(const vertex& root,
                   const vector<string>& values,
                   vector<string>* const paths) {
    // If this vertex is not terminal, recurse.
    if (!root.terminal) {
        for (map<string, vertex>::const_iterator it = root.children.begin();
             it != root.children.end(); ++it) {
             vector<string> next = values;
             next.push_back(root.key + " = " + it->first);
             tree_to_paths(it->second, next, paths);
        }
    } else {
        // Construct string and insert it into vector.
        string path;
        for (vector<string>::const_iterator it = values.begin();
             it != values.end(); ++it) {
            if (it != values.begin()) {
                path += "and\t";
            }
            path += *it + "\t";
        }

        if (root.positive) {
            path += "then\tTrue";
        } else {
            path += "then\tFalse";
        }
        paths->push_back(path);
    }
}

vector<node> read_nodes_from_filepath(const string& filepath) {
    vector<node> nodes;
    vector<string> values;
    ifstream input_file(filepath.c_str());
    string line;

    // Read first line (value names)
    getline(input_file, line);
    values = string_to_tokens(line, ' ');

    // Read all other lines (nodes).
    while (getline(input_file, line)) {
        node new_node;

        // Get tokens, then add them to corresponding key value for node.
        vector<string> tmp_values = string_to_tokens(line, ' ');
        for (int i = 0; i < tmp_values.size(); ++i) {
            new_node.values[values.at(i)] = tmp_values.at(i);
        }
        // Determine if node is positive.
        new_node.positive =
            strcmp("0", tmp_values.at(tmp_values.size() - 1).c_str());
        nodes.push_back(new_node);
    }

    input_file.close();
    return nodes;
}

double entropy(const vector<node>& data, const string& value) {
    map<string, int> value_count;
    double set_entropy = 0.0;

    // Get occurrences of each value.
    value_count = count_occurrences(data, value);

    // Compute entropy.
    for (map<string, int>::iterator it = value_count.begin();
         it != value_count.end(); ++it) {
        double p = (double) it->second / data.size();
        set_entropy += -p * lg(p);
    }

    return set_entropy;
}

double subset_entropy(const vector<node>& data,
                      const string& key,
                      const string& split_value,
                      const string& tar_val) {
    vector<node> subset;

    // Split the set.
    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        if (it->values.at(key).compare(split_value) == 0) {
            subset.push_back(*it);
        }
    }

    // Compute entropy of subset.
    return entropy(subset, tar_val);
}

double info_gain(const vector<node>& data,
                 const string& split_value,
                 const string& tar_val) {
    map<string, int> value_count;
    double set_entropy = 0.0;
    double gain = 0.0;

    // Get occurrences of each value.
    value_count = count_occurrences(data, split_value);

    // Compute the entropy of each subset (when split by 'value').
    for (map<string, int>::iterator it = value_count.begin();
         it != value_count.end(); ++it) {
        double p = (double) it->second / data.size();
        double sub_entropy =
            subset_entropy(data, split_value, it->first, tar_val);
        set_entropy += p * sub_entropy;
    }

    // Compute the difference in entropy (gain).
    gain = entropy(data, tar_val) - set_entropy;

    return gain;
}

map<string, int> count_occurrences(
        const vector<node>& data, const string& key) {
    map<string, int> value_count;
   
    // Iterate across each value and count how many times it appears.
    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        if (value_count.find(it->values.at(key)) != value_count.end()) {
            ++value_count[it->values.at(key)];
        } else {
            value_count[it->values.at(key)] = 1;
        }
    }

    return value_count;
}

vertex construct_tree(const vector<node>& data, const string& tar_val) {
    vertex root;
    root.terminal = false;
    
    // First, check to see if the vertex contains only positive or negative
    // nodes.
    int positive_nodes = 0;
    for(vector<node>::const_iterator it = data.begin();
        it != data.end(); ++it) {
        positive_nodes += it->positive;
    }

    if (positive_nodes == data.size()) {
        root.terminal = true;
        root.positive = true;
        return root;
    } else if(positive_nodes == 0) {
        root.terminal = true;
        root.positive = false;
        return root;
    }

    // Calculate gain for each possible split
    map<string,double> gain;
    for(map<string,string>::const_iterator it = data.begin()->values.begin();
        it != data.begin()->values.end(); ++it){
        if (strcmp(it->first.c_str(), tar_val.c_str()) != 0) {
            gain[it->first] = info_gain(data, it->first, tar_val);
        }
    }
    double max_gain = 0;
    string split_value;
    for(map<string, double>::iterator it = gain.begin();
        it != gain.end(); ++it) {
        if(it->second > max_gain){
            split_value = it->first;
            max_gain = it->second;
        }
    }

    // If no split produces any information gain, the node is terminal and the
    // most common +/- value is used.
    if(max_gain == 0) {
        root.terminal = true;
        root.positive = (round(positive_nodes/data.size()) > 0);
        return root;
    }

    // If nothing has gone wrong up to this point, we assign the vertex's key
    // and recursively assign child vertices.
    root.key = split_value;
    map<string,vector<node> > children = split_nodes(data, split_value);
    for(map< string,vector<node> >::iterator it = children.begin();
        it != children.end(); ++it) {
        root.children[it->first] = construct_tree(it->second , tar_val);
    }

    return root;
}

map<string, vector<node> > split_nodes(
        const vector<node>& data, const string& key) {
    map<string, vector<node> > split;
    for(vector<node>::const_iterator it = data.begin();
        it != data.end(); ++it) {
        // If the key's value at the current node has not yet been seen, create
        // a new vector and push the current node to it. Else push it to the
        // existing vector.
        if(split.find(it->values.at(key)) == split.end()) {
            vector<node> new_value;
            new_value.push_back(*it);
            split[it->values.at(key)] = new_value;
        } else{
            split[it->values.at(key)].push_back(*it);
        }
    }

    return split;
}

}  // namespace id3
