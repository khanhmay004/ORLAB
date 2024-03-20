#include "../include/params.h"
#include <fstream>
#include <sstream>
#include <filesystem>

// InstanceCVRPLIB.cpp | HGS-Thibaut
Params::Params(string pathToInstance, bool isRoundingInteger):pathToInstance(pathToInstance), isRoundingInteger(isRoundingInteger){
    string content, content2, content3, content4;
    double serviceTimeData = 0.;
    // Read INPUT dataset
    fin = ifstream(pathToInstance);
    if (fin.is_open()) {
        for (fin >> content; content != "NODE_COORD_SECTION" && content != "NODE_COORD_SECTION:" && content != "EDGE_WEIGHT_SECTION" && content != "EDGE_WEIGHT_SECTION:"; fin >> content) {
            transform(content.begin(), content.end(), content.begin(), ::toupper);
            if(content[(int)content.size()-1] != ':') fin>>content2;
            if (content == "NAME" || content == "NAME:" || content == "TYPE" || content == "TYPE:" || content == "COMMENT" || content == "COMMENT:"){
                getline(fin, content2);
            }else if (content == "DIMENSION" || content == "DIMENSION:") {
                fin >> nbVertices;
            }  
            else if (content == "EDGE_WEIGHT_TYPE"  || content == "EDGE_WEIGHT_TYPE:"){
                fin >> content3;
            }
            else if (content == "SERVICE_TIME" || content == "SERVICE_TIME:"){
                fin >> serviceTimeData;
            }
            else if (content == "EDGE_WEIGHT_FORMAT" || content == "EDGE_WEIGHT_FORMAT:"){
                fin >> content4;
            }
            else if (content == "DISPLAY_DATA_TYPE" || content == "DISPLAY_DATA_TYPE:"){
                fin >> content2;
            }else if (content == "NODE_COORD_TYPE" || content == "NODE_COORD_TYPE:"){
                fin >> content2;
            }else
                throw string("Unexpected data in input file: " + content);
        }
        if (nbVertices <= 0) throw string("Number of nodes is undefined");
        //if (vehicleCapacity == 1.e30) throw string("Vehicle capacity is undefined");
        dist_mtx = vector<vector<double> >(nbVertices, vector<double>(nbVertices, 0));

        if(content3 == "EUC_2D"){
            read_distance_data_euc2D_coord_matrix();
        }else if(content3 == "ATT"){
            read_distance_data_att_coord_matrix();
        }else if(content3 == "CEIL_2D"){
            read_distance_data_ceil2D_coord_matrix();    
        }else if(content3 == "GEO"){
            read_distance_data_geo_coord_matrix();
        }else if(content3 == "EXPLICIT"){
            if(content4 == "LOWER_DIAG_ROW"){
                read_distance_data_lower_diag_matrix();
            }else if(content4 == "UPPER_DIAG_ROW"){
                read_distance_data_upper_diag_matrix();
            }else if(content4 == "FULL_MATRIX"){
                read_distance_data_full_matrix();
            }else if(content4 == "UPPER_ROW"){
                read_distance_data_upper_matrix();
            }else if(content4 == "LOWER_ROW"){
                read_distance_data_lower_matrix();
            }else{
                throw string("Type of data is not supported");
            }
        }else{
            throw string("Type of data is not supported");
        }

        if(isRoundingInteger){
            for(int i = 0; i < nbVertices; i++){
                for(int j = 0; j < nbVertices; j++){
                    dist_mtx[i][j] = round(dist_mtx[i][j]);
                }
            }
        }
    } else
        throw string("Impossible to open instance file: " + pathToInstance);
}

void Params::print_info(){
    //cout << "Instance: " << fs::path(pathToInstance).stem() << endl;
    cout << "Number of vertices: " << nbVertices << endl;
    cout << "Distance matrix: \n";
    for (int i = 0; i < nbVertices; i++){
        for (int j = 0; j < nbVertices; j++){
            cout << dist_mtx[i][j] << " ";
        }
        cout << "\n";
    }
}

void Params::read_distance_data_upper_diag_matrix() {
    for (int i = 0; i < nbVertices; i++) {
        for (int j = i; j < nbVertices; j++) {
            fin >> dist_mtx[i][j];
            dist_mtx[j][i] = dist_mtx[i][j];
        }
    }
}

void Params::read_distance_data_upper_matrix() {
    for (int i = 0; i < nbVertices-1; i++) {
        for (int j = i + 1; j < nbVertices; j++) {
            fin >> dist_mtx[i][j];
            dist_mtx[j][i] = dist_mtx[i][j];
        }
    }
}

void Params::read_distance_data_lower_diag_matrix() {
    double tmp_number;
    for (int i = 0; i < nbVertices; i++) {
        for (int j = 0; j <= i; j++) {
            fin >> tmp_number;
            dist_mtx[i][j] = dist_mtx[j][i] = tmp_number;
        }
    }
}

void Params::read_distance_data_lower_matrix() {
    double tmp_number;
    for (int i = 1; i < nbVertices; i++) {
        for (int j = 0; j < i; j++) {
            fin >> tmp_number;
            dist_mtx[i][j] = dist_mtx[j][i] = tmp_number;
        }
    }
}


void Params::read_distance_data_geo_coord_matrix() {
    vector<double> x = vector<double>(nbVertices, 0);
    vector<double> y = vector<double>(nbVertices, 0);
    vector<double> latitude = vector<double>(nbVertices, 0);
    vector<double> longitude = vector<double>(nbVertices, 0);
    int tmp;
    for (int i = 0; i < nbVertices; i++) {
        fin >> tmp;
        fin >> x[i];
        fin >> y[i];
    }
    double PI = 3.141592, RRR = 6378.388;
    for (int i = 0; i < nbVertices; i++){
        double deg, min;
        deg = (int)(x[i]);
        min = x[i] - deg;
        latitude[i] = PI * (deg + 5.0 * min / 3.0 ) / 180.0;
        deg = (int)(y[i]);
        min = y[i] - deg;
        longitude[i] = PI * (deg + 5.0 * min / 3.0 ) / 180.0;
    }
    for (int i  = 0; i < nbVertices; i++){
        for (int j = 0; j < nbVertices; j++){
            double q1 = cos( longitude[i] - longitude[j]);
            double q2 = cos( latitude[i] - latitude[j]);
            double q3 = cos( latitude[i] + latitude[j]);
            if(i != j){
                dist_mtx[i][j] = (int)  ( RRR * acos( 0.5*((1.0+q1)*q2 - (1.0-q1)*q3) ) + 1.0);
            }
        }
    }
}

void Params::read_distance_data_att_coord_matrix() {
    vector<double> x(nbVertices, 0);
    vector<double> y(nbVertices, 0);
    for (int i = 0; i < nbVertices; i++) {
        int tmp; fin >> tmp;
        fin >> x[i];
        fin >> y[i];
    }
    for (int i  = 0; i < nbVertices; i++){
        for (int j = 0; j < nbVertices; j++){
            double xd = x[i] - x[j];
            double yd = y[i] - y[j];
            double rij = sqrt((xd*xd + yd*yd) / 10.0);
            double tij = (int)(rij);
            if(tij < rij){
                dist_mtx[i][j] = tij + 1;
            }
            else{
                dist_mtx[i][j] = tij;
            }
        }
    }
}

void Params::read_distance_data_euc2D_coord_matrix() {
    vector<double> x(nbVertices);
    vector<double> y(nbVertices);
    int node_number;
    for (int i = 0; i < nbVertices; i++) {
        fin >> node_number >> x[i] >> y[i];
        if (node_number != i + 1) throw string("The node numbering is not in order.");
    }
    for (int i  = 0; i < nbVertices; i++){
        for (int j = 0; j < nbVertices; j++){
            double xd = x[i] - x[j];
            double yd = y[i] - y[j];
            dist_mtx[i][j] = (int)(sqrt(xd*xd + yd*yd)+0.5);
        }
    }
}

void Params::read_distance_data_ceil2D_coord_matrix() {
    vector<double> x(nbVertices, 0);
    vector<double> y(nbVertices, 0);
    for (int i = 0; i < nbVertices; i++) {
        int tmp; fin >> tmp;
        fin >> x[i];
        fin >> y[i];
    }
    for (int i  = 0; i < nbVertices; i++){
        for (int j = 0; j < nbVertices; j++){
            double xd = x[i] - x[j];
            double yd = y[i] - y[j];
            double zij = ceil(sqrt(xd*xd + yd*yd));
            dist_mtx[i][j] = zij;
        }
    }
}

void Params::read_distance_data_full_matrix() {
    for(int i = 0; i < nbVertices; i++){
        for(int j = 0; j < nbVertices; j++){
            fin >> dist_mtx[i][j];
        }
    }
}