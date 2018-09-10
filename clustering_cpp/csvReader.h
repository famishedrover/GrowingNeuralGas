//
//  csvReader.h
//  clustering_cpp
//
//  Created by Mudit Verma on 10/09/2018.
//  Copyright © 2018 Mudit Verma. All rights reserved.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>


using namespace std;
#ifndef csvReader_h
#define csvReader_h

vector<vector<float> > readCsv(string path){
    
    ifstream csv(path);
    string line;
    vector<vector<float> > items;
    
    if (csv.is_open()) {
        for (string row_line; getline(csv, row_line);){
            items.emplace_back();
            istringstream row_stream(row_line);
            for(string column; getline(row_stream, column, ',');)
                items.back().push_back(strtof((column).c_str(),0));
        }
    }
    else {
        cout << "Unable to open file";
    }
    return items;
}

void data_head(vector<vector<float>>data,int k=5){
    int counter = 0;
    for(auto x:data){
        for(auto y:x){
            cout<<y<<" ";
        }
        cout<<endl;
        counter ++;
        if(counter == k)
            break;
    }
}


#endif /* csvReader_h */
