#include <cmath>
#include <cstring>
#include <iostream>

#include "constructtree.h"

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

namespace id3 {

void print_tree(const vertex& root) {
    vector<string> paths;
    vector<string> base;

    // Build all paths.
    tree_to_paths(root, base, &paths);
    
    for (vector<string>::const_iterator it = paths.begin();
         it != paths.end(); ++it) {
        cout << "If " << *it << endl;
    }
}

void tree_to_paths(const vertex& root,
                   const vector<string>& values,
                   vector<string>* paths) {
    // If this vertex is not terminal, recurse.
    if (!root.terminal) {
        for (map<string, vertex>::const_iterator it = root.children.begin();
             it != root.children.end(); ++it) {
             vector<string> next = values;
             next.push_back(root.key + " = " + it->first);
             tree_to_paths(it->second, next, paths);
        }
    } else {
        // Construct string and insert it into vector.
        string path;
        for (vector<string>::const_iterator it = values.begin();
             it != values.end(); ++it) {
            if (it != values.begin()) {
                path += "and\t";
            }
            path += *it + "\t";
        }

        if (root.positive) {
            path += "then\tTrue";
        } else {
            path += "then\tFalse";
        }
        paths->push_back(path);
    }
}

vertex construct_tree(vector<node>* data, const string& tar_val){
    vertex root;
    root.terminal = false;
    
    // First, check to see if the vertex contains only positive or negative nodes
    int positive_nodes = 0;
    for(vector<node>::iterator it = data->begin(); it != data->end(); ++it) {
        positive_nodes += it->positive;
    }

    if (positive_nodes == data->size()) {
        root.terminal = true;
        root.positive = true;
        return root;
    } else if(positive_nodes == 0) {
        root.terminal = true;
        root.positive = false;
        return root;
    }

    // Calculate gain for each possible split
    map<string,double> gain;
    for(map<string,string>::iterator it = data->begin()->values.begin(); 
        it != data->begin()->values.end(); ++it){
        if (strcmp(it->first.c_str(), tar_val.c_str()) != 0) {
            gain[it->first] = info_gain(data,it->first,tar_val);
        }
    }
    double max_gain = 0;
    string split_value;
    for(map<string,double>::iterator it = gain.begin(); it != gain.end(); ++it) {
        if(it->second > max_gain){
            split_value = it->first;
            max_gain = it->second;
        }
    }

    // If no split produces any information gain, the node is terminal and the most common +/- value
    // is used
    if(max_gain == 0) {
        root.terminal = true;
        root.positive = (round(positive_nodes/data->size()) > 0);
        return root;
    }

    // If nothing has gone wrong up to this point, we assign the vertex's key and recursively assign
    // child vertices.
    root.key = split_value;
    map<string,vector<node> > children = split_nodes(data, split_value);
    for(map< string,vector<node> >::iterator it = children.begin();
        it != children.end(); ++it) {
        root.children[it->first] = construct_tree( &(it->second) , tar_val);
    }

    return root;
}

map<string, vector<node> > split_nodes(vector<node>* data, const string& key) {
    map<string, vector<node> > split;
    for(vector<node>::iterator it = data->begin();
        it != data->end(); ++it) {

        // If the key's value at the current node has not yet been seen, create a new vector and
        // push the current node to it. Else push it to the existing vector.
        if(split.find(it->values[key]) == split.end()) {
            vector<node> new_value;
            new_value.push_back(*it);
            split[it->values[key]] = new_value;
        } else{
            split[it->values[key]].push_back(*it);
        }
    }

    return split;
}

}  // namespace id3
