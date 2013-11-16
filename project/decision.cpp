#include "decision.h"

#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

using std::getline;
using std::ifstream;
using std::map;
using std::stringstream;
using std::string;
using std::vector;

using namespace std;

namespace decision {

vector<string> string_to_tokens(const string& str, char delim) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    // Read each token and add it to vector.
    while (getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
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

double lg(double x) {
    return log(x) / log(2);
}

}  // namespace decision
