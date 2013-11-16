#include <map>
#include <string>
#include <vector>

namespace c45 {
    struct node {
        std::map<std::string, std::string> string_values;
        std::map<std::string, double> cont_values;
    };

    double lg(double x);

    double cont_entropy(const std::vector<node>& data,
                        const std::string& attr,
                        double partition);

    double ideal_partition(
            const std::vector<node>& data, const std::string& attr);
}  // namespace c45
