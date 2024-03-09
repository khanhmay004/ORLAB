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
    IloEnv env;                            // environment object
    IloModel model(env);                   // model object
    IloNumVarArray x(env, numItems, 0, 1); // variables x_i indicating whether to include item i in the knapsack

    // Add objective function
    IloExpr objective(env);
    for (int i = 0; i < numItems; ++i)
    {
        objective += values[i] * x[i];
    }
    model.add(IloMaximize(env, objective));
    objective.end();

    // Add capacity constraint
    IloExpr capacityExpr(env);
    for (int i = 0; i < numItems; ++i)
    {
        capacityExpr += weights[i] * x[i];
    }
    model.add(capacityExpr <= capacity);
    capacityExpr.end();

    // Model Solution
    IloCplex cplex(model);
    cplex.setOut(env.getNullStream()); // Suppress output

    // Solve
    if (!cplex.solve())
    {
        cout << "No solution found" << endl;
        env.end();
        return 1;
    }

    // Printing the Solution
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
