//
// Created by pta on 15/08/2018.
//

#ifndef IRP_PLFUNCTION_H
#define IRP_PLFUNCTION_H

#include "memory"
#include <algorithm>
#include <string>
#include <vector>
#include "LinearPiece.h"
#include "Params.h"

using namespace std;

class PLFunction
{
private:
    Params *params;

public:
    vector<std::shared_ptr<LinearPiece>> pieces;
    int nbPieces;

    std::shared_ptr<LinearPiece> minimalPiece;
    double maxValue, minValue;

    std::shared_ptr<LinearPiece> pieceAt0;
    double valueAt0;

    PLFunction(Params *params);

    // initialize a PL function from arc profile
    PLFunction(Params *params, vector<Insertion> insertions, int day, int client);
    PLFunction(Params *params, vector<Insertion> insertions, int day, int client,bool st);
    PLFunction(Params *params, vector<Insertion> insertions, int day, int client,bool st,int daily);

    // initialize a PLFunction from list of pieces
    PLFunction(Params *params, vector<shared_ptr<LinearPiece>> pieces);

    PLFunction(PLFunction *plf);

    double cost(double x);
    void update_minValue(double &rep);
    double calculateDemandFromCost(int day, int client, double detour, double cost, double freeload);
    void clear();

    // get piece that fits with time t
    std::shared_ptr<LinearPiece> getPiece(double t);

    std::shared_ptr<LinearPiece> getMinimalPiece(int client, double &minAt, double &minValue);
    
    std::shared_ptr<LinearPiece> getMinimalPiece_stockout(int client, double &minAt, double &minValue);
    //    std::shared_ptr<LinearPiece> copyPiece(std::shared_ptr<LinearPiece> source);

    // convert pieces from linked list to vector
    //    void updatePieceVector();

    // check intersection of two pieces
    bool intersect(shared_ptr<LinearPiece> lp1, shared_ptr<LinearPiece> lp2, double &x, double &y);

    void append(shared_ptr<LinearPiece> lp);

    //    PLFunction *superposition(PLFunction &rhs);

    void shiftLeft(double x_axis);
    void addHolding(double InventoryCost,double daily);
    void addHoldingf(double InventoryCost);
    void addStockout(double stockoutCost,double daily);
    void addStockoutf(double stockoutCost);
    void moveUp(double y_axis);
    std::shared_ptr<PLFunction> update0(double min0);
    void reflection(double x_axis);

    std::shared_ptr<PLFunction> getInBound(double lb, double ub, bool updateValueAt0);

    double calculateCost(int day, int client, double detour, double demand, double freeload);
    double calculateCost_stockout(int day, int client, double detour, double replenishment, double freeload); 
    double calculateCost_holding(int day, int client, double detour, double replenishment, double freeload); 

    bool testSuperposition();

    void print();

    ~PLFunction();
};

#endif // IRP_PLFUNCTION_H
