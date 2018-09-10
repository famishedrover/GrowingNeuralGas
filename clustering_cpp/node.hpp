//
//  node.hpp
//  clustering_cpp
//
//  Created by Mudit Verma on 08/09/2018.
//  Copyright © 2018 Mudit Verma. All rights reserved.
//

#ifndef node_hpp
#define node_hpp

#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

class Node{
//    all the weights are floats
    int n;
    
//    node id is the address of the node itself
    
    vector<float> NodeWeights;
    
public:
    string node_id;
    float error;
    
    Node(int n);
    string address();
    void random_init();
    vector<float> weights();
    void setWeights(vector<float> w);
};


#endif /* node_hpp */
