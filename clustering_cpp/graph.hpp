//
//  graph.hpp
//  clustering_cpp
//
//  Created by Mudit Verma on 09/09/2018.
//  Copyright © 2018 Mudit Verma. All rights reserved.
//

#ifndef graph_hpp
#define graph_hpp

#include <stdio.h>
#include "node.hpp"
//#include "data.hpp"
#include <map>
#include <set>

class Graph{
//    number of features in the node
    int n_inputs;
//    start nodes
    int n_start_nodes;
//    learning rate
    float step;
//    learning rate for neurons connected via edge
    float neighbour_step;
    int max_edge_age;
    int n_iter_before_neuron_added;
    float error_decay_rate;
    float after_split_error_decay_rate;
    int max_nodes;
    float min_distance_for_update;
    int n_updates;
    map<Node*,set<Node*> > edges_per_node;
    map<string,int> edges;
    
    map<Node*,long long> mappings;
    
    vector<vector<float> >gatherNodeWeights();
    vector<float> gatherNodeErrors();
    void closestTwo(vector<float> distance,int *s1, int *s2);
    void printParams();
    void init_nodes(vector<vector<float> >);
    void printGraph();
    void dfsUtil(Node* v, map<Node*,bool> &visited,long long count);
    void add_edge(Node* x,Node* y);
    void remove_edge(Node* x,Node* y);
    void add_node(Node* x);
    void remove_node(Node* x);
    float train_epoch(vector<vector<float> > &train_data);
    
public:

    Graph();
    Graph(bool);
    int n_nodes();
    void resetParams();
    void train(vector<vector<float> > &train_data,int epochs);
    vector<Node*> nodesList();
    long long connectedComponents();
    long long predict(vector<float> sample,int k);
    void testerFunc(vector<vector<float> > &train_data);

};


#endif /* graph_hpp */
