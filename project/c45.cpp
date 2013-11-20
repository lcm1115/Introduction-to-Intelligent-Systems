#include "c45.h"
#include "decision.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using decision::lg;
using decision::string_to_tokens;
using std::ifstream;
using std::map;
using std::string;
using std::vector;

namespace c45 {

vector<node> read_nodes_from_filepath(const string& filepath) {
    vector<node> nodes;
    vector<string> values;
    map<string, char> value_types;
    vector<string> tokens;
    ifstream input_file(filepath.c_str());
    string line;

    // Get value types.
    getline(input_file, line);
    tokens = string_to_tokens(line, ' ');

    // Determine which values are continuous and which are discrete.
    for (int i = 0; i < tokens.size(); ++i) {
        int split_index = tokens.at(i).find(':');
        string value_name = tokens.at(i).substr(0, split_index);
        char value_type = tokens.at(i).at(split_index + 1);
        value_types[value_name] = value_type;
        values.push_back(value_name);
    }

    while (getline(input_file, line)) {
        node new_node;

        vector<string> tmp_values = string_to_tokens(line, ' ');

        for (int i = 0; i < tmp_values.size(); ++i) {
            // Add value to corresponding value list depending on its type.
            if (value_types[values.at(i)] == 'D') {
                new_node.string_values[values.at(i)] = tmp_values.at(i);
            } else {
                new_node.cont_values[values.at(i)] =
                    atof(tmp_values.at(i).c_str());
            }
        }

        nodes.push_back(new_node);
    }

    input_file.close();
    return nodes;
}

void partition_sizes(int count[],
                     const vector<node>& data,
                     const string& attr,
                     double partition) {
    count[0] = 0;
    count[1] = 0;
    // Count size of each partition.
    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        if (it->cont_values.at(attr) <= partition) {
            ++count[0];
        } else {
            ++count[1];
        }
    }
}

vector<node> cont_split(const vector<node>& data,
                        const string& attr,
                        double partition,
                        bool gt) {
    vector<node> split;

    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        if (gt && it->cont_values.at(attr) > partition) {
            split.push_back(*it);
        } else if (!gt && it->cont_values.at(attr) <= partition) {
            split.push_back(*it);
        }
    }

    return split;
}

map<string, int> count_occurrences(
        const vector<node>& data, const string& key) {
    map<string, int> value_count;
   
    // Iterate across each value and count how many times it appears.
    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        if (value_count.find(
                    it->string_values.at(key)) != value_count.end()) {
            ++value_count[it->string_values.at(key)];
        } else {
            value_count[it->string_values.at(key)] = 1;
        }
    }

    return value_count;
}

double cont_entropy(const vector<node>& data,
                    const string& attr,
                    const string& tar_attr,
                    double partition) {
    map<string, int[2]> count_map;

    // Count how many of each entry there are that are less than or equal to
    // the partition with a specific value for tar_attr, and those that are
    // greater than the partition with a specific value for tar_attr.
    for (int j = 0; j < data.size(); ++j) {
        string value = data.at(j).string_values.at(tar_attr);
        double cont = data.at(j).cont_values.at(attr);
        if (count_map.find(value) == count_map.end()) {
            count_map[value][0] = 0;
            count_map[value][1] = 0;
        }
        if (cont <= partition) {
            ++count_map[value][0];
        } else {
            ++count_map[value][1];
        }
    }

    // Compute entropy for continuous value
    double ent = 0;
    for (map<string, int[2]>::const_iterator it = count_map.begin();
         it != count_map.end(); ++it) {
        for (int i = 0; i < 2; ++i) {
            double p = (double) it->second[i] / data.size();
            ent += -p * lg(p);
        }
    }

    return ent;
}

double ideal_partition(const vector<node>& data,
                       const string& attr,
                       const string& tar_attr) {
    vector<double> values(data.size());
    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        values.push_back(it->cont_values.at(attr));
    }
    double max_ent = 0;
    double partition = values.at(0);

    // Find the partition that best splits the data.
    for (int i = 0; i < values.size() - 1; ++i) {
        double part = values.at(i);
        double ent = cont_entropy(data, attr, tar_attr, part);
        if (ent > max_ent) {
            max_ent = ent;
            partition = part;
        }
    }

    return partition;
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
        if (it->string_values.at(key).compare(split_value) == 0) {
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

}  // namespace c45
