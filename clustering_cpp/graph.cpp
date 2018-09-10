//
//  graph.cpp
//  clustering_cpp
//
//  Created by Mudit Verma on 09/09/2018.
//  Copyright © 2018 Mudit Verma. All rights reserved.
//

#include "graph.hpp"
#include "math.h"
#include <utility>
#include <iostream>
#include <ctime>
#include <random>
#include <assert.h>
#include <algorithm>

using namespace std;
//#include "data.hpp"

void printNodes(vector<Node*> nodes){
    cout<<"Number of Nodes:"<<nodes.size()<<"\n";
    for(auto node:nodes){
        cout<<node->node_id<<endl;
    }
}

void Graph::printParams(){
    cout<<"n_start_nodes="<<n_start_nodes<<endl;
    cout<<"step="<<step<<endl;
    cout<<"neighbour_step="<<neighbour_step<<endl;
    cout<<"max_edge_age="<<max_edge_age<<endl;
    cout<<"n_iter_before_neuron_added="<<n_iter_before_neuron_added<<endl;
    cout<<"error_decay_rate="<<error_decay_rate<<endl;
    cout<<"after_split_error_decay_rate="<<after_split_error_decay_rate<<endl;
    cout<<"max_nodes="<<max_nodes<<endl;
    cout<<"min_distance_for_update="<<min_distance_for_update<<endl;
    cout<<"n_updates="<<n_updates<<endl;
}

void Graph::resetParams(){
//    n_inputs=3;
    n_start_nodes=2;
    step=0.2;
    neighbour_step=0.05;
    max_edge_age=100;
    n_iter_before_neuron_added=1000;
    error_decay_rate=0.995;
    after_split_error_decay_rate=0.5;
    max_nodes=1000;
    min_distance_for_update=0;
    n_updates=0;
}

void Graph::init_nodes(vector<vector<float> > train_data){
    assert(train_data.size()>0);
    assert(train_data[0].size()==n_inputs);
    mt19937 rng;
    rng.seed(1);
    uniform_int_distribution<mt19937::result_type> dist(0,(train_data.size()-1)%294967295);
    
    for(int i=0;i<n_start_nodes;i++){
        int idx = dist(rng);
        Node* node = new Node(n_inputs);
        node->setWeights(train_data[idx]);
        
        this->add_node(node);
    }
}

Graph::Graph(){
    this->resetParams();
}

Graph::Graph(bool showParams){
    this->resetParams();
    if (showParams){
        this->printParams();
    }
}


template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {
    
    // initialize original index locations
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);
    
    // sort indexes based on comparing values in v
    sort(idx.begin(), idx.end(),
         [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
    
    return idx;
}



void vec_scal_prod(vector<float>& w, float s){
    for(auto it=w.begin();it!=w.end();it++){
        *it = *it*s;
    }
}

vector<float> vec_add(vector<float>x, vector<float>y){
    vector<float> result;
    int t = static_cast<int> (x.size());
    for(int i=0;i<t;i++){
        result.push_back(x[i]+y[i]);
    }
    return result;
}

vector<float> vec_diff(vector<float>x, vector<float>y){
//    x-y
// length of x and y should be same.
    vector<float> result;
    int t = static_cast<int> (x.size());
    for(int i=0;i<t;i++){
        result.push_back(x[i]-y[i]);
    }
    return result;
}

float L2norm(vector<float>& v){
    float result=0;
    for(auto i:v){
        result += i*i;
    }
    return (float)sqrt(result);
}

vector<float> vector_norm(vector<vector<float> >& weights, vector<float>& sample){
//    ||x-y||
    vector<float> result;
    vector<float> dist;
    for(auto nodew:weights){
        dist = vec_diff(nodew, sample);
        result.push_back(L2norm(dist));
    }
    return result;
}

vector<vector<float> > Graph::gatherNodeWeights(){
    vector<vector<float> > result;
    vector<Node*> nodes = nodesList();
//    cout<<"Nodes :"<<nodes.size()<<" ";
    for(auto node:nodes){
//        cout<<" Address"<<node->node_id;
        result.push_back(node->weights());
    }
    return result;
}

void Graph::closestTwo(vector<float>distance,int *s1, int *s2){
    float min = INT_MAX;
    float min2 = INT_MAX;
    int idx= -1;
    int idx2= -1;
    int i=0;
    for(auto d:distance){
        if(d<min){
            min2 = min;
            min = d;
            idx2 = idx;
            idx = i;
        }
        else if(d < min2){
            min2 = d;
            idx2 = i;
        }
        i++;
    }
    
    *s1=idx; *s2=idx2;
    
//    s1 = this->nodes[idx];
//    s2 = this->nodes[idx2];
}

inline string make_id(Node* x,Node* y){
    if(x->node_id<y->node_id)
        return x->node_id+y->node_id;
    else
        return y->node_id+x->node_id;
}

int  Graph::n_nodes(){
    return static_cast<int>(edges_per_node.size());
}

void Graph::add_edge(Node* x, Node* y){
    edges_per_node[x].insert(y);
    edges_per_node[y].insert(x);
    
    edges[make_id(x, y)] = 0;
}

void Graph::remove_edge(Node* x, Node* y){
    vector<Node*> v = nodesList();
    vector<Node*>::iterator it;
    vector<Node*>::iterator it2;
    it = std::find (v.begin(), v.end(), x);
    it2 = std::find (v.begin(), v.end(), y);
    if(it != v.end() && it2 != v.end()){
        edges_per_node[x].erase(x);
        edges_per_node[y].erase(y);
        string edge_id = make_id(x,y);
        edges.erase(edge_id);
    }
    else{
        cout<<"Skipped : Invalid Access to remove edge b/w: "<<x->node_id<<" and "<<y->node_id;
    }
}

void Graph::add_node(Node* x){
    set<Node*> s;
    edges_per_node[x] = s;
}
void Graph::remove_node(Node* x){
    auto it = edges_per_node.find(x);
    if(it != edges_per_node.end()){
        edges_per_node.erase(it);
        map<Node*, set<Node*> >::iterator i;
        
        for(i = edges_per_node.begin(); i != edges_per_node.end(); i++){
            i->second.erase(x);
        }
    }
    else
        cout<<"\nSkipped : Invalid Access to remove node:"<<x->node_id;
}

vector<Node*> Graph::nodesList(){
    vector<Node*> v;
    for(auto it = edges_per_node.begin(); it != edges_per_node.end(); ++it) {
        v.push_back(it->first);
    }
    return v;
}

vector<float> Graph::gatherNodeErrors(){
    vector<float> result;
    vector<Node*> tmpList = nodesList();
    for(vector<Node*>::iterator node=tmpList.begin();node!= tmpList.end();node++){
        if (*node == NULL )
            cout<<"NULL somehow";
        else
            result.push_back((*node)->error);
    }
    return result;
}

void Graph::dfsUtil(Node* v, map<Node*,bool> &visited,long long count)
{
    // Mark the current node as visited and print it
    visited[v] = true;
    mappings[v] = count;
    // Recur for all the vertices
    // adjacent to this vertex
    vector<Node>::iterator i;

    for(auto i:edges_per_node[v]){
        if(!visited[i])
            dfsUtil(i, visited,count);
    }
}


long long Graph::connectedComponents()
{
    // Mark all the vertices as not visited
//    bool *visited = new bool[V];
    map<Node*,bool> visited;
    vector<Node*> nodesL = nodesList();
    for(auto nn:nodesL){
        visited[nn] = false;
    }
    
    long long count=0;
    for (auto nn:nodesL)
    {
        if (visited[nn] == false)
        {
            mappings[nn] = count;
            dfsUtil(nn,visited,count);
            count++;
        }
    }
    return count;
}


long long Graph::predict(vector<float> sample, int k=1){
//    use knn method 1. find distance.
//    2. sort
//    find max k
//    vote
    auto weights = gatherNodeWeights();
    auto nodes = nodesList();
    
    vector<float> distance = vector_norm(weights,sample);
    
    assert(k<= weights.size());
    
    int counter=0;
    map<long long,long long> votes;
    for(auto i:sort_indexes(distance)){
        long long vo = mappings[nodes[i]];
        if(votes.find(vo) != votes.end())
            votes[vo]++;
        else
            votes[vo] = 1;
        
        counter++;
        if(counter == k)
            break;
    }
    
    long long _max = LONG_MIN;
    long long cluster = -1;
    for(map<long long,long long>::iterator i=votes.begin();i!=votes.end();i++){
        if((i->second) > _max){
            _max = (i->second);
            cluster = i->first;
        }
    }
    
    return cluster;
}

void Graph::train(vector<vector<float> > &train_data, int epochs=100){
//    return ;
    
    assert(train_data.size()<=0);
    
    this->n_inputs = <static_cast<int>(train_data[0].size());
    
    vector<Node*> currNodes = nodesList();
    if (currNodes.size()==0)
        init_nodes(train_data);

    float error;


        for (int ep=0;ep<epochs;ep++){
            cout<<"Epoch:"<<ep<<"\t";
            clock_t begin = clock();

            error = train_epoch(train_data);

            clock_t end = clock();
            cout<<"CurrentError:"<<error<<"\tTime:"<<double(end - begin) / CLOCKS_PER_SEC<<"\n";

        }


}

void Graph::printGraph(){
    cout<<"\nNumber of Nodes: "<<nodesList().size()<<endl;
    for(map<Node*,set<Node*> >::const_iterator it = edges_per_node.begin();
        it != edges_per_node.end(); ++it)
    {
        cout<<it->first->node_id<<"->";
        for(auto sit:it->second){
            cout<<sit->node_id<<",";
        }
        cout<<endl;
    }
}

void Graph::testerFunc(vector<vector<float> > &train_data){
    
    vector<Node*> currNodes = nodesList();
    if (currNodes.size()==0)
        init_nodes(train_data);
    
    currNodes.clear();
    currNodes = nodesList();
    printNodes(currNodes);
    
    Node* node = new Node(this->n_inputs);
    add_node(node);
    
    Node* node2 = new Node(this->n_inputs);
    add_node(node);
    
    Node* node3 = new Node(n_inputs);
    
    cout<<"WORK-----\n";
    add_edge(node, node2);
    printGraph();
    remove_node(node);
    cout<<"\nRemoved Node"<<node->node_id;
    printGraph();
    
    remove_edge(node2, node);
    printGraph();
    
    cout<<"Trying to remove a node which has not been added \n";
    remove_node(node3);
    
//    currNodes = nodesList();
//    printNodes(currNodes);
    
}


float Graph::train_epoch(vector<vector<float> > &train_data){
    vector<vector<float> > weights = gatherNodeWeights();
    
    float total_error = 0;
    bool did_update = false;
    long long n_samples = train_data.size();
    
    vector<Node*> nodes = this->nodesList();
    
    for(auto sample:train_data) {
//        find the closest two nodes to the sample.
        vector<float> distance = vector_norm(weights,sample);
//        closest s1 and s2
        
        Node *s1,*s2;
        
        int s1arg = -1, s2arg=-1;
        closestTwo(distance,&s1arg,&s2arg);
        
        s1 = nodes[s1arg];
        s2 = nodes[s2arg];
    
        total_error += distance[s1arg];
        if (distance[s1arg] <  min_distance_for_update)
            continue;
        
        n_updates++;
        did_update = true;
        
        s1->error += distance[s1arg];
        
//        s1.weights += step*(sample-weights)
        vector<float> tmp = vec_diff(sample,s1->weights());
        vec_scal_prod(tmp,step);
        tmp = vec_add(s1->weights(), tmp);
        s1->setWeights(tmp);
        
        this->add_edge(s1,s2);
        
        
        for(auto to_neuron = edges_per_node[s1].begin(); to_neuron!= edges_per_node[s1].end();to_neuron++){
            
            string edge_id = make_id(*to_neuron,s1);
            int age = edges[edge_id];
            
            if(age>=max_edge_age){
                remove_edge(*to_neuron,s1);
                if(edges_per_node[*to_neuron].size()==0)
                    remove_node(*to_neuron);
            }
            else{
                this->edges[edge_id] += 1;

                vector<float> tmp = vec_diff(sample,(*to_neuron)->weights());
                vec_scal_prod(tmp,step);
                tmp = vec_add((*to_neuron)->weights(), tmp);
                (*to_neuron)->setWeights(tmp);
            }
            
            bool time_to_add_new_neuron = (n_updates % n_iter_before_neuron_added ==0 )&&(this->n_nodes()<this->max_nodes);
            
            if (time_to_add_new_neuron){
//                find the largest error neuron.
//                printGraph();
                vector<float> nodeErrors = this->gatherNodeErrors();
                
                
                float _max = -1;
                int _maxIdx = INT_MIN, cnt=0;
                for(auto i:nodeErrors){
                    if(_max<i){
                        _maxIdx = cnt;
                        _max = i;
                    }
                    cnt++;
                }
                
                Node* largest_err_neuron = nodes[_maxIdx];
//                Node* largest_err_neuron = NULL;
                
                
                
//              find the neighbour with largest error to this largest_err_neuron
                set<Node*> neighbours2largestErrorNode = edges_per_node[largest_err_neuron];
                if (neighbours2largestErrorNode.size() !=0){
                    _max = INT_MIN; // reusing _max from above
                    Node* neighbour_neuron = NULL;
                    for(auto node:neighbours2largestErrorNode){
                        if(node->error>_max){
                            _max = node->error;
                            neighbour_neuron = node;
                        }
                    }

    //              update largest_err_neuron and neighbour_neuron error values
                    largest_err_neuron->error *= after_split_error_decay_rate;
        
                    neighbour_neuron->error *= after_split_error_decay_rate;

    //              calculate weight for new node.
                    vector<float> new_weight = vec_add(largest_err_neuron->weights(), neighbour_neuron->weights());
                    Node* newNeuron = new Node(this->n_inputs);
                    newNeuron->setWeights(new_weight);
                    
                    remove_edge(neighbour_neuron, largest_err_neuron);
        
                    add_node(newNeuron);
        
                    add_edge(largest_err_neuron, newNeuron);
                    add_edge(neighbour_neuron, newNeuron);
                }
                else{
//                    no neighbours exists to the largetst error neuron.
//                    this is not handled in the python implementattion.
//                    if such a case occurs, then we simple add a new neuron
//                    at w/2 position to the largestErrorNeuron and add an edge
//                    between the two
                    
//                   Node* newNeuron = new Node(this->n_inputs);
                    cout<<"No Neighbour Case";
                    
                    
                }
            }
            nodes.clear();
            nodes = nodesList();
            //     decay error for each node in graph.
            for(auto  xnode=nodes.begin();xnode!=nodes.end();xnode++){
                (*xnode)->error *= error_decay_rate;
            }
            
        }
    }
    
    if(!did_update && min_distance_for_update !=0 && n_samples>1){
        throw  invalid_argument("Distance between every data sample and neurons, closest to them, is less than"+to_string(min_distance_for_update));
    }
    
    return total_error/n_samples;
}


// TODO
//Test on real data
//predict function
//get Clusters









