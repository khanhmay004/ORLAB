#include <ilcplex/ilocplex.h>
#include <iostream>

ILOSTLBEGIN

int main()
{
    IloEnv env;
    try
    {
        IloModel model(env);

        // Decision variables
        IloNumVarArray X(env, 4, 0.0, IloInfinity);

        // Objective function
        IloExpr objExpr(env);
        objExpr = 3 * X[0] - 11 * X[1] + 5 * X[2] + X[3];
        model.add(IloMinimize(env, objExpr));

        // Constraints
        IloExpr expr1(env);
        expr1 = X[0] + 5 * X[1] - 3 * X[2] + 6 * X[3];
        model.add(expr1 <= 7);
        expr1.end();

        IloExpr expr2(env);
        expr2 = -X[0] + X[1] + X[2] - 2 * X[3];
        model.add(expr2 >= 3);
        expr2.end();

        IloCplex cplex(model);

        cplex.solve();

        cout << "Solution status: " << cplex.getStatus() << endl;
        cout << "Objective value: " << cplex.getObjValue() << endl;
        for (int i = 0; i < 4; ++i)
        {
            cout << "X[" << i << "] = " << cplex.getValue(X[i]) << endl;
        }
    }
    catch (IloException &e)
    {
        cerr << "Error: " << e << endl;
    }
    catch (...)
    {
        cerr << "Unknown error occurred." << endl;
    }

    env.end();
    return 0;
}
