#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>
#include <ilconcert/ilosys.h>
#include <cmath>
#include "../include/params.h"

ILOSTLBEGIN;
typedef IloArray<IloNumVarArray> NumVarMatrix;

// Tim tat ca cac subtour tu tour hien co: nextNode. nextNode[i] = j co nghia tham node j sau node i

vector<vector<int>> findSubTours(vector<int> nextNode) {
    int n = nextNode.size();
    vector<int> visited(n, 0);
    vector<vector<int>> subTours;
    for(int i = 0; i < n; i++) {
        if(visited[i])
            continue;
        int currentNode = i;
        vector<int> currentTour;
        do {
            currentTour.push_back(currentNode);
            visited[currentNode] = 1;
            currentNode = nextNode[currentNode];
        } while(currentNode != i);
        subTours.push_back(currentTour);
    }
    return subTours;
}

int main(){
    IloEnv env;
    try{
        // Import data
        Params param("C:\\Users\\drnan\\CLionProjects\\TSP\\data\\berlin52.tsp\\berlin52.tsp", true);
        int n = param.nbVertices; //n

        //Distance between each city
        IloNumArray2 dist(env, n);

        for (int i = 0; i < n; i++) {
            dist[i] = IloNumArray(env, n);
            for (int j = 0; j < n; j++) {
                dist[i][j] = param.dist_mtx[i][j];
            }
        }
        IloModel model(env);
        // Decision variable
        // x[i][j]= 1 if place j is visited immediately after i, 0 otherwise
        NumVarMatrix x(env, n);
        for (int i = 0; i < n; i++) {
            x[i] = IloNumVarArray(env, n, 0, 1, ILOINT);
        }

        //Objective functions (minimize the total distance)
        IloExpr obj_expr(env);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                obj_expr += dist[i][j] * x[i][j];
            }
        }
        IloObjective obj = IloMinimize(env, obj_expr);
        model.add(obj);


        // Constraints:
        // Go-to constraint
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
        // Subtour-elimination constraint

        IloCplex cplex(model);
        IloExpr expr(env);

        int countCons=1; // number of constraint additions.
        while(countCons++) {
            if(!cplex.solve()) {
                cout<< "Failed to optimize model with this data file!";
                break;
            }
            vector<int> nextNode(n, -1); //Current found solution
            for(int i = 0; i < n; i++) {
                for(int j = 0; j < n; j++) {
                    if(i != j && cplex.getValue(x[i][j]) > 0.5) {
                        nextNode[i] = j;
                        break;
                    }
                }
            }
            //Find subtours of current solution
            vector<vector<int>> subTours = findSubTours(nextNode);
            // Truong hop toi uu: Chi co 1 subtour hay day chinh la subtour toi uu nhat -> break vong lap
            if(subTours.size() == 1)
                break;
            //Subtour elimination for each subtour identified
            int numOfSubTour = subTours.size();
            for(int i = 0; i < numOfSubTour; i++) {
                int numOfNodes = subTours[i].size();
                for(int j = 0; j < numOfNodes; j++) {
                    for(int k = 0; k < numOfNodes; k++) {
                        if(subTours[i][j] != subTours[i][k]) {
                            expr += x[subTours[i][j]][subTours[i][k]];
                        }
                    }
                }
                IloRange DFJ(env, -IloInfinity, expr, numOfNodes - 1);
                model.add(DFJ);
                expr.clear(); //clear cho vong lap toi
            }
        }
        expr.end();
        // Print solution:
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

    catch (IloException& e) {
        cerr << "Conver exception caught: " << e << endl; // No solution exists
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
    env.end();
    return 0;
}