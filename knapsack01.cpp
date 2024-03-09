#include <iostream>
#include <ilcplex/ilocplex.h>

using namespace std;

int main()
{
    // Data
    const int numItems = 5;
    const int capacity = 12;
    int weights[numItems] = {2, 3, 4, 5, 6};
    int values[numItems] = {20, 10, 40, 50, 25};

    // Model Definition
    IloEnv env;                           
    IloModel model(env);                  
    IloNumVarArray x(env, numItems, 0, 1); 

  
    IloExpr objective(env);
    for (int i = 0; i < numItems; ++i)
    {
        objective += values[i] * x[i];
    }
    model.add(IloMaximize(env, objective));
    objective.end();

    // constraint
    IloExpr capacityExpr(env);
    for (int i = 0; i < numItems; ++i)
    {
        capacityExpr += weights[i] * x[i];
    }
    model.add(capacityExpr <= capacity);
    capacityExpr.end();

    IloCplex cplex(model);
    cplex.setOut(env.getNullStream()); // Suppress output

    if (!cplex.solve())
    {
        cout << "No solution found" << endl;
        env.end();
        return 1;
    }

    cout << "Objective value: " << cplex.getObjValue() << endl;
    cout << "Selected items:" << endl;
    for (int i = 0; i < numItems; ++i)
    {
        if (cplex.getValue(x[i]) > 0.5)
        {
            cout << "Item " << i + 1 << endl;
        }
    }

    // Closing the Model
    env.end();
    return 0;
}
