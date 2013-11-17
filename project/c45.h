#include <map>
#include <string>
#include <vector>

namespace c45 {

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
// 'partition' is the partition threshold for the splitting
// Returns a double that represents the entropy of the set.
double cont_entropy(const std::vector<node>& data,
                    const std::string& attr,
                    double partition);

// Determines the ideal partition for a set for a continuous attribute.
// 'data' is the list of nodes
// 'attr' is the continuous attribute for which a partition is being found
// Returns a double representing the value on which the set should be
// partitioned.
double ideal_partition(
        const std::vector<node>& data, const std::string& attr);

}  // namespace c45
