#ifndef DECISION_H
#define DECISION_H

#include <map>
#include <string>
#include <vector>

namespace decision {

// Tokenizes a string and returns it as a vector of strings.
// 'str' the string being tokenized
// 'delim' the delimiter by which to split the string
// Returns a vector that contains the tokens of the string.
std::vector<std::string> string_to_tokens(const std::string& str, char delim);

// Computes the log (base 2) of a given value.
// 'x' is the value for which the log is being computed.
// Returns the log (base 2) of x.
double lg(double x);

}  // namespace decision

#endif  // DECISION_H
