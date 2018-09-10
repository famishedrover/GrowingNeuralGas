//
//  data.hpp
//  clustering_cpp
//
//  Created by Mudit Verma on 09/09/2018.
//  Copyright © 2018 Mudit Verma. All rights reserved.
//

#ifndef data_hpp
#define data_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "csvReader.h"
#include <string>

using namespace std;

class Data{
    
public:
    vector<vector<float> > data;
    
    Data();
//    Data(vector<vector<float> > &p);
//    void data_head(int k);
//    void read_csv(string path);
};

#endif /* data_hpp */
