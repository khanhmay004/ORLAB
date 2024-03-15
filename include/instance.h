//
// Created by mei on 3/6/2024.
//

#ifndef UMV_FSTSP_INSTANCE_H
#define UMV_FSTSP_INSTANCE_H
#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <vector>
#include <utility>
using namespace std;
class TSP_Instance {
public:
    TSP_Instance(const string &filename);

    int get_dimension() const;
    vector<pair<double, double>> get_node_coordinates() const;
    int dimension;
    vector<pair<double, double>> node_coordinates;
};

#endif // INSTANCE_H

#endif //UMV_FSTSP_INSTANCE_H
