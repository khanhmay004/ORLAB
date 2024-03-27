#include "../include/instance.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Instance::Instance(const std::string& filename) {
    readData(filename);
}

void Instance::readData(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        exit(1);
    }
    file >> n; // Number of nodes including the depot
    distances.resize(n, vector<float>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            file >> distances[i][j];
        }
    }
    timeWindows.resize(n);
    for (int i = 0; i < n; ++i) {
        file >> timeWindows[i].first >> timeWindows[i].second;
    }
    file.close();
}

