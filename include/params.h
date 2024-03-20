#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <algorithm>

using namespace std;
//namespace fs = filesystem;

class Params
{
public:
    string pathToInstance;
    bool isRoundingInteger;
	ifstream fin;
public:

	vector< vector<double> > dist_mtx;
	int nbVertices;		
public:
    Params(){};
    Params(string pathToInstance, bool isRoundingInteger);
    void print_info();
public:
    void read_distance_data_upper_diag_matrix();
    void read_distance_data_upper_matrix();
    void read_distance_data_lower_diag_matrix();
    void read_distance_data_geo_coord_matrix();
    void read_distance_data_att_coord_matrix();
    void read_distance_data_euc2D_coord_matrix();
    void read_distance_data_full_matrix();
    void read_distance_data_ceil2D_coord_matrix();
	void read_distance_data_lower_matrix();
};