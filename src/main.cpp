#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>
#include "../include/instance.h"
#include <ilconcert/ilosys.h>
#include <cmath>
#include "../include/params.h"

using namespace std;
ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<IloExprArray> ExprMatrix;

// Calculate Euclidean distance
long long euclid_distance(pair<double, double> a, pair<double, double> b) {
    double xd = a.first - b.first;
    double yd = a.second - b.second;
    return sqrt(xd * xd + yd * yd);
}

int main() {
    IloEnv env;
    try {
        // Import data
        Params param("C:\\Users\\drnan\\CLionProjects\\TSP\\data\\berlin52.tsp\\berlin52.tsp", true);
        TSP_Instance instance("C:\\Users\\drnan\\CLionProjects\\TSP\\data\\berlin52.tsp\\berlin52.tsp");
        int n = param.nbVertices;

        // Distance matrix: Khoanq cach giua cac nodes tinh theo euclid, idst[i][i] = 0
        IloNumArray2 dist(env, n);

        for (int i = 0; i < n; i++) {
            dist[i] = IloNumArray(env, n);
            for (int j = 0; j < n; j++) {
                dist[i][j] = param.dist_mtx[i][j];
            }
        }

        // Model
        IloModel model(env);

        // Decision variables: binary 1, 0
        NumVarMatrix x(env, n);
        for (int i = 0; i < n; i++) {
            x[i] = IloNumVarArray(env, n, 0, 1, ILOINT);
        }

        //Rank variable ui
        IloNumVarArray u(env, n, 0, IloInfinity);

        // Objective functions (minimize the total distance)
        IloExpr obj_expr(env);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                obj_expr += dist[i][j] * x[i][j];
            }
        }
        IloObjective obj = IloMinimize(env, obj_expr);
        model.add(obj);

        // Constraints


        //Go-to constraint
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

        // Sub-tour constraints

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (j != 0 && i != j) {
                    model.add(u[i] + x[i][j] <= u[j] + (n - 1) * (1 - x[i][j]));
                }
            }
        }

        model.add(u[0] == 0);



        // Solve the model
        IloCplex cplex(model);
        cplex.solve();

        // Print
        cout << "Optimal Solution: " << cplex.getObjValue() << endl;
        cout << "Route:" << endl;

        vector<int> route;
        route.push_back(0);
        int current = 0;
        while (route.size() <= n) {
            for (int j = 0; j <= n; j++) {
                if (j != current && cplex.getValue(x[current][j]) > 0.5) {
                    route.push_back(j);
                    current = j;
                    break;
                }
            }
        }
        for (int i : route) {
            cout << i + 1 << " ";
        }
    }
    catch (IloException& ex) {
        cerr << "Error: " << ex << endl;
    }
    env.end();
    return 0;
}
