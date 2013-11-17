#include "c45.h"
#include "decision.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

using decision::lg;
using std::map;
using std::sort;
using std::string;
using std::vector;

namespace c45 {
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
                           const string& attr,
                           const string& target_attr) {
        vector<double> values(data.size());
        for (vector<node>::const_iterator it = data.begin();
             it != data.end(); ++it) {
            values.push_back(it->cont_values.at(attr));
        }
        sort(values.begin(), values.end());
        double max_ent = 0;
        double partition = values.at(0);

        for (int i = 0; i < values.size() - 1; ++i) {
            double part = (values.at(i) + values.at(i + 1)) / 2;
            double ent = cont_entropy(data, attr, part);
            if (ent > max_ent) {
                max_ent = ent;
                partition = part;
            }
        }

        return partition;
    }
}  // namespace c45

using namespace c45;

int main(int argc, char** argv) {
    vector<node> nodes;
    node n1;
    n1.cont_values["TEST"] = .25;
    n1.string_values["TAR"] = "A";
    nodes.push_back(n1);
    node n2;
    n2.cont_values["TEST"] = .4;
    n2.string_values["TAR"] = "B";
    nodes.push_back(n2);
    node n3;
    n3.cont_values["TEST"] = .75;
    n3.string_values["TAR"] = "A";
    nodes.push_back(n3);
    node n4;
    n4.cont_values["TEST"] = .675;
    n4.string_values["TAR"] = "B";
    nodes.push_back(n4);
    node n5;
    n5.cont_values["TEST"] = .1;
    n5.string_values["TAR"] = "A";
    nodes.push_back(n5);
    printf("Test 1: %f\n", cont_entropy(nodes, "TEST", 0.0));
    printf("Test 2: %f\n", cont_entropy(nodes, "TEST", 0.5));
    printf("Test 3: %f\n", cont_entropy(nodes, "TEST", 1.0));
    double ideal = ideal_partition(nodes, "TEST", "TAR");
    printf("Ideal Partition: %f\n", ideal);
    printf("Ideal Entropy: %f\n", cont_entropy(nodes, "TEST", ideal));
}
