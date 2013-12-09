#include "c45.h"
#include "decision.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

using decision::lg;
using decision::string_to_tokens;
using std::ifstream;
using std::map;
using std::pair;
using std::sort;
using std::ostringstream;
using std::string;
using std::vector;

namespace c45 {

void print(vertex root, vector<string> path) {
    if (!root.terminal) {
        for (auto it : root.children) {
            vector<string> next = path;
            next.push_back(it.first.c_str());
            print(it.second, next);
        }
    } else {
        for (auto it : path) {
            printf("%s\t->\t", it.c_str());
        }
        printf("%s\n", root.key.c_str());
    }
}

void find_prune_candidates(vertex* root, vector<vertex*>* candidates) {
    vector<vertex*> children;
    for (map<string, vertex>::iterator it = root->children.begin();
         it != root->children.end(); ++it) {
        if (it->second.children.size() != 0) {
            children.push_back(&(it->second));
        }
    }

    if (children.size() == 0) {
        candidates->push_back(root);
    } else {
        for (vector<vertex*>::iterator it = children.begin();
             it != children.end(); ++it) {
            find_prune_candidates(*it, candidates);
        }
    }
}

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
        int total = it->second[0] + it->second[1];
        for (int i = 0; i < 2; ++i) {
            double p = (double) it->second[i] / total;
            if (p > 0) {
                ent += (double) total / data.size() * -p * lg(p);
            }
        }
    }

    return ent;
}

pair<double, double> partition_gain(const vector<node>& data,
                                    const string& attr,
                                    const string& tar_attr) {
    vector<double> values;
    double current_entropy = entropy(data, tar_attr);
    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        values.push_back(it->cont_values.at(attr));
    }

    double max_gain = 0.0;
    double ideal_part = 0.0;
    sort(values.begin(), values.end());

    // Find the partition that best splits the data.
    for (int i = 2; i < values.size() - 2; ++i) {
        double part = values.at(i);
        double ent = cont_entropy(data, attr, tar_attr, part);
        if (current_entropy - ent > max_gain) {
            max_gain = current_entropy - ent;
            ideal_part = part;
        }
    }

    return pair<double, double>(max_gain, ideal_part);
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

vertex construct_tree(const vector<node>& data, const string& tar_val) {
    vertex root;
    root.terminal = false;

    map<string, int> value_counts;
    int entries = 0;
    for (vector<node>::const_iterator it = data.begin();
         it != data.end(); ++it) {
        if (value_counts.find(it->string_values.at(tar_val)) !=
            value_counts.end()) {
            value_counts[tar_val] = 0;
            ++entries;
        }
        ++value_counts[tar_val];
    }

    if (entries == 1) {
        root.key = data.at(0).string_values.at(tar_val);
        root.terminal = true;
        return root;
    }

    double max_gain = 0;
    double ideal_part = 0;
    string split_value;
    int split_type = 0;
    for (map<string, string>::const_iterator it =
         data.begin()->string_values.begin();
         it != data.begin()->string_values.end(); ++it) {
        if (strcmp(it->first.c_str(), tar_val.c_str()) != 0) {
            double gain = info_gain(data, it->first, tar_val);
            if (gain > max_gain) {
                max_gain = gain;
                split_value = it->first;
            }
        }
    }

    if (max_gain == 0) {
        root.key = data.at(0).string_values.at(tar_val);
        root.terminal = true;
        return root;
    }

    for (map<string, double>::const_iterator it =
         data.begin()->cont_values.begin();
         it != data.begin()->cont_values.end(); ++it) {
        pair<double, double> gain = partition_gain(data, it->first, tar_val);
        if (gain.first > max_gain) {
            max_gain = gain.first;
            ideal_part = gain.second;
            split_value = it->first;
            split_type = 1;
        }
    }

    root.key = split_value;
    map<string, vector<node> > children;
    if (split_type == 0) {
        children = split_nodes(data, split_value);
    } else {
        children = split_nodes(data, split_value, ideal_part);
    }

    for (map<string, vector<node> >::iterator it = children.begin();
         it != children.end(); ++it) {
        root.children[it->first] = construct_tree(it->second, tar_val);
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
        string value = it->string_values.at(key);
        string map_key = key + " = " + value;
        if(split.find(map_key) == split.end()) {
            vector<node> new_value;
            new_value.push_back(*it);
            split[map_key] = new_value;
        } else{
            split[map_key].push_back(*it);
        }
    }

    return split;
}

map<string, vector<node> > split_nodes(
        const vector<node>& data, const string& key, double part) {
    map<string, vector<node> > split;
    ostringstream s;
    s << key << " <= " << part;
    string le_key = s.str();
    s.clear();
    s.str(string());
    s << key << " > " << part;
    string gt_key = s.str();
    split[le_key] = vector<node>();
    split[gt_key] = vector<node>();
    for(vector<node>::const_iterator it = data.begin();
        it != data.end(); ++it) {
        // If the key's value at the current node has not yet been seen, create
        // a new vector and push the current node to it. Else push it to the
        // existing vector.
        node to_insert = *it;
        double value = to_insert.cont_values.at(key);
        to_insert.cont_values.erase(key);
        if (value <= part) {
            split[le_key].push_back(to_insert);
        } else {
            split[gt_key].push_back(to_insert);
        }
    }

    return split;
}

}  // namespace c45
