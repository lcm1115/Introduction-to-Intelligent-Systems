#include <map>
#include <string>
#include <vector>

namespace c45 {

struct vertex {
    std::string key;
    std::string tar_val;
    bool terminal;
    std::map<std::string, vertex> children;
};

struct node {
    std::map<std::string, std::string> string_values;
    std::map<std::string, double> cont_values;
};

// Given a filepath, reads all lines from the file and creates a node from each.
// 'filepath' the filepath to the data file that contains node information
// Returns a vector that contains all nodes created from the data file.
std::vector<node> read_nodes_from_filepath(const std::string& filepath);

// Counts the sizes of each partition of a set of nodes.
// 'count' is the array in which the sizes will be stored
// 'data' is the list of nodes
// 'attr' is the continuous attribute on which partitioning occurs
// 'partition' is the threshold for splitting the set
void partition_sizes(int count[],
                     const std::vector<node>& data,
                     const std::string& attr,
                     double partition);

// Determines the entropy of a set when partitioned by a value.
// 'data' is the list of nodes
// 'attr' is the continuous attribute
// 'tar_attr' is the target attribute for the decision tree
// 'partition' is the partition threshold for the splitting
// Returns a double that represents the entropy of the set.
double cont_entropy(const std::vector<node>& data,
                    const std::string& attr,
                    const std::string& tar_attr,
                    double partition);

// Determines the ideal partition for a set for a continuous attribute.
// 'data' is the list of nodes
// 'attr' is the continuous attribute for which a partition is being found
// Returns a double representing the value on which the set should be
// partitioned.
std::pair<double, double> partition_gain(const std::vector<node>& data,
                                         const std::string& attr,
                                         const std::string& tar_attr);

// Computes the entropy for a given set based on a value.
// 'data' must be non-NULL and is a vector containing all nodes.
// 'value' is the value on which entropy is being computed.
// Returns a double which represents the entropy of the set.
double entropy(const std::vector<node>& data, const std::string& value);

// Computes the entropy of a subset (split by a value) of a given set of data.
// 'data' must be non-NULL and is a vector containing all nodes.
// 'key' is the key in which we are searching for the value.
// 'split_value' is the value by which the set is being split.
// 'tar_val' is the target value for the decision tree.
// Returns a double which represents the entropy of the subset generated by
// splitting the set on a value.
double subset_entropy(const std::vector<node>& data,
                      const std::string& value,
                      const std::string& split_value,
                      const std::string& tar_val);

// Computes the information gained from splitting the data set on a specific
// value.
// 'data' must be non-NULL and is a vector containing all nodes.
// 'split_value' is the value by which the set is being split.
// 'tar_val' is the target value for the decision tree.
// Returns a double which represents the difference in current entropy and the
// entropy from splitting the set.
double info_gain(const std::vector<node>& data,
                 const std::string& split_value,
                 const std::string& tar_val);

vertex construct_tree(
        const std::vector<node>& data, const std::string& tar_val);

std::map<std::string, std::vector<node> > split_nodes(
        const std::vector<node>& data, const std::string& key, double part);

// Splits the data set along the given value
std::map<std::string, std::vector<node> > split_nodes(
        const std::vector<node>& data, const std::string& key);

}  // namespace c45
