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

class Instance {
public:
    int n; // Number of nodes, including the depot
    vector<vector<float>> distances;
    vector<pair<int, int>> timeWindows;

    Instance(const string& filename);
    // Load instance data from file
    void readData(const string& filename);
};


#endif // INSTANCE_H

#endif //UMV_FSTSP_INSTANCE_H
