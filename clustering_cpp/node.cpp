//
//  node.cpp
//  clustering_cpp
//
//  Created by Mudit Verma on 08/09/2018.
//  Copyright © 2018 Mudit Verma. All rights reserved.
//

#include "node.hpp"
#include <sstream>
#include <assert.h>
#include <iostream>

using namespace std;

string Node::address(){
    std::ostringstream address;
    address << (void const *)this;
    std:string name = address.str();
    return name;
}

Node::Node(int n){
    std::ostringstream address;
    address << (void const *)this;
    node_id = address.str();
    
    this->n = n;
    this->random_init();
}

void Node::random_init(){
//    some kind of initializations to NodeWeights
    this->error = 0;
    
    for(int i=0;i<this->n;i++)
        NodeWeights.push_back(0);
}

vector<float> Node::weights(){
    return NodeWeights;
}

void Node::setWeights(vector<float> w){
    assert(w.size() == NodeWeights.size());
    for(int i=0;i<NodeWeights.size();i++){
        NodeWeights[i] = w[i];
    }
}
