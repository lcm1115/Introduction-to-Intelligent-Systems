#include "c45.h"
#include "decision.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using decision::lg;
using decision::string_to_tokens;
using std::ifstream;
using std::map;
using std::sort;
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
        for (vector<node>::const_iterator it = data.begin();
             it != data.end(); ++it) {
            if (it->cont_values.at(attr) <= partition) {
                ++count[0];
            } else {
                ++count[1];
            }
        }
    }

    double cont_entropy(
            const vector<node>& data, const string& attr, double partition) {
        int count[] = { 0, 0 };
        partition_sizes(count, data, attr, partition);
        double set_entropy = 0.0;

        double p;
        if (count[0] > 0) {
            p = (double) count[0] / data.size();
            set_entropy += -p * lg(p);
        }
        if (count[1] > 0) {
            p = (double) count[1] / data.size();
            set_entropy += -p * lg(p);
        }

        return set_entropy;
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

    double ideal_partition(const vector<node>& data,
                           const string& attr) {
        vector<double> values(data.size());
        for (vector<node>::const_iterator it = data.begin();
             it != data.end(); ++it) {
            values.push_back(it->cont_values.at(attr));
        }
        double max_ent = 0;
        double partition = values.at(0);

        for (int i = 0; i < values.size() - 1; ++i) {
            double ent = cont_entropy(data, attr, values.at(i));
            if (ent > max_ent) {
                max_ent = ent;
                partition = values.at(i);
            }
        }

        return partition;
    }
}  // namespace c45

using namespace c45;

int main(int argc, char** argv) {
    vector<node> nodes = read_nodes_from_filepath(argv[1]);
}
