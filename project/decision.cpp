#include "decision.h"

#include <cstring>
#include <cmath>
#include <sstream>

using std::getline;
using std::map;
using std::stringstream;
using std::string;
using std::vector;

using namespace std;

namespace decision {

vector<string> string_to_tokens(const string& str, char delim) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    // Read each token and add it to vector.
    while (getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

double lg(double x) {
    return log(x) / log(2);
}

}  // namespace decision
