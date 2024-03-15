#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

typedef std::pair<double, double> Point;

// Function to calculate Euclidean distance and round up to the next integer
int euclidean_distance_rounded(const Point& a, const Point& b) {
    double xd = a.first - b.first;
    double yd = a.second - b.second;
    return static_cast<int>(std::ceil(std::sqrt(xd * xd + yd * yd)));
}

int main() {
    std::ifstream file("berlin52.tsp");
    std::string line;
    std::vector<Point> nodes;
    bool is_node_coord_section = false;

    while (std::getline(file, line)) {
        if (line == "NODE_COORD_SECTION") {
            is_node_coord_section = true;
            continue;
        } else if (line == "EOF") {
            break;
        }

        if (is_node_coord_section) {
            std::istringstream iss(line);
            double x, y;
            iss >> x >> y;
            nodes.push_back({x, y});
        }
    }

    // Calculate distances between each pair of points
    int n = nodes.size();
    std::vector<std::vector<int>> distances(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            distances[i][j] = euclidean_distance_rounded(nodes[i], nodes[j]);
        }
    }

    // Output distances
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << distances[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
