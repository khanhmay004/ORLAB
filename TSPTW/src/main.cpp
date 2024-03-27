#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include "../include/instance.h"
#include <ilconcert/ilosys.h>
#include <cmath>

using namespace std;
stringstream name;

using namespace std;

int main() {
    IloEnv env;
    try {
        string filename = "C://Users//drnan//CLionProjects//tsptw_1//data//n20w20.001.txt";
        Instance data(filename);
        int n = data.n;
        IloModel model(env);

        // Decision variables
        IloArray<IloBoolVarArray> x(env, n); //x[i][j] = 1 -> directed tour from i to j, and otherwise
        IloNumVarArray t(env, n, 0, IloInfinity, ILOFLOAT); //For time
        IloNumVarArray u(env, n, 0, IloInfinity,  ILOINT); //For SEC constraint

        model.setName("TravellingSalesmanProblemWithTimeframe");

        for (int i = 0; i < n; i++) {
            x[i] = IloBoolVarArray(env, n);
            for (int j = 0; j < n; j++) {
                name<< "x_"<< i<< "."<< j;
                x[i][j] = IloBoolVar(env,name.str().c_str());
                name.str("");
            }
            u[i].setName(("U_" + to_string(i)).c_str());
            t[i].setName(("T_" + to_string(i)).c_str());
        }

        // Objective Function: Minimize total distance
        IloExpr obj(env);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    obj += data.distances[i][j] * x[i][j];
                }
            }
        }
        model.add(IloMinimize(env, obj));
        obj.end();

        // Constraints
        // Each node is visited exactly once
        for (int i = 0; i < n; i++) {
            IloExpr expr(env);
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    expr += x[i][j];
                }
            }
            model.add(expr == 1);
            expr.end();
        }
        //Come-in constraint
        for (int j = 0; j < n; j++) {
            IloExpr expr(env);
            for (int i = 0; i < n; i++) {
                if (i != j) {
                    expr += x[i][j];
                }
            }
            model.add(expr == 1);
            expr.end();
        }


        // MTZ constraints
        // Sub-tour constraints
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (j != 0) {
                    model.add(u[i] + x[i][j] <= u[j] + (n - 1) * (1 - x[i][j]));
                }
            }
        }

        model.add(u[0] == 0);

        // Time window constraints
        const int bigNum = 10000;
        for (int i = 0; i < n; ++i) {
            for (int j = 1; j < n; ++j) {
                if (i != j) {
                    model.add(t[j] >= t[i] + data.distances[i][j] - bigNum * (1 -x[i][j]));
                }
            }
        }
        for (int i = 1; i < n; i++) { // Start from 1 assuming 0 is the depot
            model.add(t[i] >= data.timeWindows[i].first);
            model.add(t[i] <= data.timeWindows[i].second);
        }

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::TimeLimit, 100);
        if (cplex.solve()) {
            string outputPath = filename + ".solution.txt";
            ofstream outFile(outputPath);
            outFile << "Total Distance: " << cplex.getObjValue() << endl;
            outFile << "Route:" << endl;
            vector<int> route;
            route.push_back(0);
            int current = 0;
            while (route.size() < n) {
                for (int j = 0; j < n; j++) {
                    if (j != current && cplex.getValue(x[current][j]) > 0.5) {
                        route.push_back(j);
                        current = j;
                        break;
                    }
                }
            }
            for (int i : route) {
                outFile << i << " ";
            }
            outFile.close();
            cplex.exportModel("model.lp");
        } else {
            cout << "No solution found" << endl;
        }
    } catch (IloException& e) {
        cerr << "CPLEX Exception: " << e << endl;
    } catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
    env.end();
    return 0;
}


