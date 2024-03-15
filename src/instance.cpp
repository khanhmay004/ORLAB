#include "../include/instance.h"
#include <fstream>

#include <sstream>

using namespace std;
TSP_Instance::TSP_Instance(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("DIMENSION:") != std::string::npos) {
            std::istringstream iss(line.substr(line.find(":") + 1));
            iss >> dimension;
        } else if (line == "NODE_COORD_SECTION") {
            while (std::getline(file, line) && line != "EOF") {
                std::istringstream iss(line);
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
