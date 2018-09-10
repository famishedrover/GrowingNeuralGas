//
//  main.cpp
//  clustering_cpp
//
//  Created by Mudit Verma on 08/09/2018.
//  Copyright © 2018 Mudit Verma. All rights reserved.
//

#include <iostream>
#include "graph.hpp"
#include "csv_reader.h"

using namespace std;

int main(int argc, const char * argv[]) {
//    example with 3d data
    string path = "/Users/muditverma/Desktop/IIITD/IOT/datasets/sample.txt";
    cin>>path;
    vector<vector<float> > data = read_csv(path);
    data_head(data);
    
    Graph graph = Graph(true);
    graph.train(data,50);
    cout<<"Number of Connected Components:"<<graph.connectedComponents();
    
    
    
//    graph.testerFunc(data);
    
    
    cout<<"\n";
    
}
