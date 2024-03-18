#include "../include/instance.h"
#include <fstream>

#include <sstream>

using namespace std;
TSP_Instance::TSP_Instance(const string &filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.find("DIMENSION:") != string::npos) {
            istringstream iss(line.substr(line.find(":") + 1));
            iss >> dimension;
        } else if (line == "NODE_COORD_SECTION") {
            while (getline(file, line) && line != "EOF") {
                istringstream iss(line);
                int id;
                double x, y;
                iss >> id >> x >> y;
                node_coordinates.push_back({x, y});
            }
        }
    }
}

int TSP_Instance::get_dimension() const {
    return dimension;
}

std::vector<std::pair<double, double>> TSP_Instance::get_node_coordinates() const {
    return node_coordinates;
}
