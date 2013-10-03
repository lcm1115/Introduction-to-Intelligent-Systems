#include "id3.h"

#include <cmath>
#include <iostream>

using std::map;
using std::string;
using std::vector;

using namespace std;

namespace id3 {

double entropy(vector<node>* data, string value) {
    map<string, int> value_count;
    double set_entropy = 0.0;

    // Get occurrences of each value.
    value_count = count_occurrences(data, value);

    // Compute entropy.
    for (map<string, int>::iterator it = value_count.begin();
         it != value_count.end(); ++it) {
        double p = (double) it->second / data->size();
        set_entropy += -p * lg(p);
    }

    return set_entropy;
}

double subset_entropy(
        vector<node>* data, string key, string split_value, string tar_val) {
    vector<node> subset;

    // Split the set.
    for (vector<node>::iterator it = data->begin(); it != data->end(); ++it) {
        if (it->values[key].compare(split_value) == 0) {
            subset.push_back(*it);
        }
    }

    // Compute entropy of subset.
    return entropy(&subset, tar_val);
}

double info_gain(vector<node>* data, string value, string tar_val) {
    map<string, int> value_count;
    double set_entropy = 0.0;
    double gain = 0.0;

    // Get occurrences of each value.
    value_count = count_occurrences(data, value);

    // Compute the entropy of each subset (when split by 'value').
    for (map<string, int>::iterator it = value_count.begin();
         it != value_count.end(); ++it) {
        double p = (double) it->second / data->size();
        double sub_entropy = subset_entropy(data, value, it->first, tar_val);
        set_entropy += p * sub_entropy;
    }

    // Compute the difference in entropy (gain).
    gain = entropy(data, tar_val) - set_entropy;

    return gain;
}

map<string, int> count_occurrences(vector<node>* data, string key) {
    map<string, int> value_count;
   
    // Iterate across each value and count how many times it appears.
    for (vector<node>::iterator it = data->begin(); it != data->end(); ++it) {
        if (value_count.find(it->values[key]) != value_count.end()) {
            ++value_count[it->values[key]];
        } else {
            value_count[it->values[key]] = 1;
        }
    }

    return value_count;
}

double lg(double x) {
    return log(x) / log(2);
}

}  // namespace id3
