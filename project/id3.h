#include <map>
#include <string>
#include <vector>

namespace id3 {

struct node {
    std::map<std::string, std::string> values;
    bool positive;
};

// Computes the entropy for a given set based on a value.
// 'data' must be non-NULL and is a vector containing all nodes.
// 'value' is the value on which entropy is being computed.
// Returns a double which represents the entropy of the set.
double entropy(std::vector<node>* data, std::string value);

// Computes the entropy of a subset (split by a value) of a given set of data.
// 'data' must be non-NULL and is a vector containing all nodes.
// 'key' is the key in which we are searching for the value.
// 'split_value' is the value by which the set is being split.
// 'tar_val' is the target value for the decision tree.
// Returns a double which represents the entropy of the subset generated by
// splitting the set on a value.
double subset_entropy(std::vector<node>* data,
                      std::string value,
                      std::string split_value,
                      std::string tar_val);

// Computes the information gained from splitting the data set on a specific
// value.
// 'data' must be non-NULL and is a vector containing all nodes.
// 'split_value' is the value by which the set is being split.
// 'tar_val' is the target value for the decision tree.
// Returns a double which represents the difference in current entropy and the
// entropy from splitting the set.
double info_gain(
        std::vector<node>* data, std::string split_value, std::string tar_val);

// Counts how many times each value appears for a given key in the list of
// nodes.
// 'data' must be non-NULL and is a vector containing all nodes.
// 'key' is the key whose values are being counted.
// Returns a map with <value, count> pairings.
std::map<std::string, int> count_occurrences(
        std::vector<node>* data, std::string key);

// Computes the log (base 2) of a given value.
// 'x' is the value for which the log is being computed.
// Returns the log (base 2) of x.
double lg(double x);

}  // namespace id3
