//
// Created by pta on 16/08/2018.
//

#ifndef IRP_LOTSIZINGSOLVER_H
#define IRP_LOTSIZINGSOLVER_H

#include "memory"
#include "PLFunction.h"
#include "LinearPiece.h"
#include "Params.h"

using namespace std;

class LotSizingSolver
{
private:
    Params *params;

public:
    vector<vector<Insertion>> insertions;
    vector<std::shared_ptr<PLFunction>> C;
    vector<std::shared_ptr<PLFunction>> F;
    vector<double> I;
    vector<double> quantities;
    vector<std::shared_ptr<Insertion>> breakpoints;
    vector<std::shared_ptr<LinearPiece>> exceptionalPieces;

    int client;
    int horizon;
    double objective;

    bool traces;

    LotSizingSolver(Params *params, vector<vector<Insertion>> insertions, int client);

    std::shared_ptr<PLFunction> copyPLFunction(std::shared_ptr<PLFunction> source);

    vector<double> getBreakpoints(std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2);

    std::shared_ptr<PLFunction> min(std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2);

    std::shared_ptr<PLFunction> supperposition(std::shared_ptr<LinearPiece> fromPieceC, std::shared_ptr<LinearPiece>
                                                                                            fromPieceF);

    std::shared_ptr<PLFunction> supperposition(std::shared_ptr<PLFunction> fromC, std::shared_ptr<PLFunction> fromF);

    bool backtracking();

    void solveEquationSystem(std::shared_ptr<LinearPiece> C, std::shared_ptr<LinearPiece> fromC,
                             std::shared_ptr<LinearPiece> fromF, double I, double demand, double &fromI, double &quantity, double maxClientInventory);
    bool solve();

    ~LotSizingSolver();
};

#endif // IRP_LOTSIZINGSOLVER_H
