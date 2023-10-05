#ifndef MODELLS_H
#define MODELLS_H

#define ILOSTLBEGIN
#include <ilcplex/ilocplex.h>
#include "Params.h"

class ModelLotSizingPI
{

public:

static bool solveCPLEX(const vector<double> & myA, const vector<double> & myB, const vector<vector<Insertion> > & insertions, 
	double netInventoryCost, double penaltyCoefficient, vector<double>& quantities, vector <int> & breakPoints, double & objective)
{
	int horizon = (int)myA.size();
	IloEnv env;

	// create model
	IloModel lotsizing_model(env);

	//Create quantity variables x
	//number of variables = horizon
	IloNumVarArray x(env, horizon, 0, IloInfinity, ILOFLOAT);
	for (int t = 0; t < horizon; t++)
	{
		char name_x[20];
		sprintf(name_x,"x%d", t);
		x[t].setName(name_x);
	}

	//Create penalty variables w
	//number of variables = horizon
	IloNumVarArray w(env, horizon, 0, IloInfinity, ILOFLOAT);
	for (int t = 0; t < horizon; t++)
	{
		char name_w[20];
		sprintf(name_w,"w%d", t);
		w[t].setName(name_w);
	}

	//Create variables y[t][i] for breakPoints in detour costs at time t
	//number of variables is at most horizon * fleet of vehicle
	IloArray<IloNumVarArray> y(env, horizon);
	for (int t = 0; t < horizon; t++)
	{
		y[t] = IloNumVarArray(env, insertions[t].size(), 0, 1, ILOINT);
		for (int i = 0; i < insertions[t].size(); i++)
		{
			char name_y[20];
			sprintf(name_y,"y%d.%d", t, i);
			y[t][i].setName(name_y);
		}
	}


	/* OBJECTIVE*/
	// Objective is to minimize the sum of the net inventory cost and the detour cost
	//plus the penalty cost
	IloExpr obj(env);
	for (int t = 0; t < horizon; t++)
	{
		for (int i = 0; i <  insertions[t].size(); i++)
		{
			obj +=  insertions[t][i].detour * y[t][i];
		}
		obj += netInventoryCost * (horizon-t) * x[t];
		obj += penaltyCoefficient * w[t];
	}


	lotsizing_model.add(IloMinimize(env, obj));

	//LOWER BOUND A and UPPER BOUND B
	for(int t = 0; t < horizon; t++)
	{
		IloExpr lb(env);
		IloExpr ub(env);
		for (int tt = 0; tt <= t; tt++)
		{
			lb += x[tt];
			ub += x[tt];
		}
		lotsizing_model.add(lb >= myA[t]);
		lotsizing_model.add(ub <= myB[t]);
	}

	//MAXLOAD CONSTRAINTS
	for(int t = 0; t < horizon; t++)
	{
		IloExpr maxl(env);
		for (int i=0; i< insertions[t].size(); i++)
		{
			maxl += insertions[t][i].load * y[t][i];
		}
		maxl += w[t];
		maxl -= x[t];
		lotsizing_model.add(maxl >=0);
	}

	//AT MOST ONE BREAKPOINT PER DAY
	for(int t = 0; t < horizon; t++)
	{
		IloExpr oneBreakPoint(env);
		for (int i=0; i < insertions[t].size(); i++)
		{
			oneBreakPoint +=y[t][i];
		}
		lotsizing_model.add(oneBreakPoint <= 1);
	}

	// THIBAUT -- MODIFICATION (one constraint was missing)
	// qt > 0 => AT LEAST ONE BREAKPOINT MUST BE USED
	for(int t = 0; t < horizon; t++)
	{
		IloExpr oneBreakPointMin(env);
		oneBreakPointMin += x[t];
		for (int i=0; i < insertions[t].size(); i++)
		{
			oneBreakPointMin -= y[t][i] * myB[horizon-1] ;
		}
		lotsizing_model.add(oneBreakPointMin <= 0);
	}

	IloCplex cplex(lotsizing_model);
	//cplex.exportModel("lotsizing.lp");
	cplex.setParam(IloCplex::Threads,1);
	cplex.setParam(IloCplex::MIPDisplay,0);
	//cplex.setParam(IloCplex::TiLim,120);
	cplex.solve();

	int status = cplex.getStatus();
	if ((status == 1)||(status==2))
	{
		//get x quantities
		quantities.resize(horizon);
		for (int t = 0; t < horizon; t++)
			quantities[t] = cplex.getValue(x[t]);

		//get y variables
		for (int t = 0; t < horizon; t++)
		{
			breakPoints[t] = -1 ;
			for (int i = 0; i < insertions[t].size(); i++)
				if (cplex.getValue(y[t][i]) > 0.99) // Should be set to 1 in Cplex
					breakPoints[t] = i ;
		}

		//get objective value
		objective = cplex.getValue(obj);
	}
	else
		return false;

	env.end();
	return true;
}


// computes without any information about the routing solution, the best lot sizing configuration for the customer visits
// TODO: need to take care inventory level for the supplier constraints
static void bestInsertionLotSizing(int client, vector <double> & insertionQuantity, Params * params)
{
	/* Generate the structures of the subproblem */
	vector <double> myA = vector <double> (params->nbDays);
	vector <double> myB = vector <double> (params->nbDays);
	vector < vector < Insertion > > insertions = vector < vector< Insertion > > (params->nbDays);
	double netInventoryCost = params->cli[client].inventoryCost - params->inventoryCostSupplier;
	vector<double> quantities = vector <double> (params->nbDays);
	vector <int> breakpoints = vector <int> (params->nbDays);
	double objective;

	/* Calculate the parameters of the subproblem */
	// Take care, the conventions on the indices are slightly different
	// On the CVRP code we cound the days from 1 to t
	// In the CPLEX model, we count the days from 0 to t-1
	// kept minInventory here even if its always equal to 0.
	myA[0] = params->cli[client].dailyDemand[1] - params->cli[client].startingInventory + params->cli[client].minInventory ; 
	for (int k=2 ; k <= params->nbDays ; k++)
		myA[k-1] = myA[k-2] + params->cli[client].dailyDemand[k] ;

	// for now, the supplier inventory constraints are not counted in myB, so myB = myA + inventory capacity of the customer
	for (int k=1 ; k <= params->nbDays ; k++)
		myB[k-1] = myA[k-1] - params->cli[client].dailyDemand[k] + params->cli[client].maxInventory - params->cli[client].minInventory ;

	// copying the non-dominated insertions obtained from the pre-processing
	for (int k=1 ; k <= params->nbDays ; k++)
		insertions[k-1].push_back(Insertion(0.,params->ordreVehicules[k][0].vehicleCapacity,NULL));

	/* Solve it and get the solution */
	ModelLotSizingPI::solveCPLEX(myA,myB,insertions,netInventoryCost,params->penalityCapa,insertionQuantity,breakpoints,objective);
}

};

#endif
