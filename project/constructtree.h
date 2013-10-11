#include <map>
#include <string>
#include <vector>

#include "id3.h"

namespace id3{

struct vertex{
	std::string key; // If the vertex is not terminal, it splits along this key
	bool terminal;
	bool positive;
	std::map<std::string,vertex> children; // If the vertex is not terminal, these are the immediate offspring
};

// Constructs a decision tree based on the given data
vertex construct_tree(std::vector<node>* data);

// Splits the data set along the given value
std::map< std::string, std::vector<node> > split_nodes(std::vector<node>* data, std::string key);

}