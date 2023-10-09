//
// Created by pta on 15/08/2018.
//

#include "PLFunction.h"

PLFunction::PLFunction(Params *params) : params(params)
{
    nbPieces = 0;
    pieces = vector<shared_ptr<LinearPiece>>();
    minimalPiece = nullptr;
    pieceAt0 = nullptr;

    minValue = MAXCOST;
    maxValue = -MAXCOST;
}

PLFunction::PLFunction(PLFunction *plf)
{
    nbPieces = plf->nbPieces;
    pieces = plf->pieces;
    minimalPiece = plf->minimalPiece;
    pieceAt0 = plf->pieceAt0;
    valueAt0 = plf->valueAt0;
    minValue = plf->minValue;
    maxValue = plf->maxValue;
}

void PLFunction::clear()
{
    nbPieces = 0;
    pieces.clear();
    minimalPiece = nullptr;
    pieceAt0 = nullptr;

    minValue = MAXCOST;
    maxValue = -MAXCOST;
}

PLFunction::PLFunction(Params *params, vector<Insertion> insertions, int day, int client) : params(params)
{
    if (insertions.size() == 0)
    {
        throw std::string(
            "ERROR: can not initialize a PL function with zero element in "
            "insertions!!!");
    }
    nbPieces = 0;
    pieces = vector<shared_ptr<LinearPiece>>();
    minimalPiece = nullptr;
    pieceAt0 = nullptr;

    minValue = MAXCOST;
    maxValue = -MAXCOST;

    //    LinearPiece *tmp = new LinearPiece();

    std::vector<Insertion>::iterator index = insertions.begin();

    double pre_x = 0;
    double pre_y, x, y, a, pre_load, pre_detour;
    Noeud *pre_place = nullptr;

    // loop through periods
    while (index != insertions.end())
    {
        // make a first piece for the first insertion
        if (index == insertions.begin())
        {
            pre_x = 0;
            pre_y = index->detour;
            x = index->load;
            y = calculateCost(day, client, index->detour, x, index->load);
        }
        else
        {
            double current_cost = calculateCost(day, client, index->detour, index->load, index->load);
            std::vector<Insertion>::iterator pre_index = index - 1;
            double pre_insertion_cost_with_current_demand = calculateCost(day, client, pre_index->detour, index->load, pre_index->load);
            bool is_dominated_vehicle = (index->load <= pre_load) || (pre_insertion_cost_with_current_demand <= current_cost);
            if (!is_dominated_vehicle)
            {
                x = pre_load + (index->detour - pre_detour) / params->penalityCapa;
                y = calculateCost(day, client, pre_detour, x, pre_load);

                if (neq(x, pre_x))
                {
                    shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
                    tmp->fromInst = make_shared<Insertion>(pre_index->detour, pre_index->load, pre_index->place);

                    append(tmp);

                    pre_x = x;
                    pre_y = y;
                }
            }

            // second with slop = 0
            x = index->load;
            y = current_cost;
        }

        // make piece
        if (neq(x, pre_x))
        {
            shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
            tmp->fromInst = make_shared<Insertion>(index->detour, index->load, index->place);

            append(tmp);
        }

        pre_x = x;
        pre_y = y;
        pre_load = index->load;
        pre_detour = index->detour;
        pre_place = index->place;

        index++;
    }

    // the last piece
    x = params->cli[client].maxInventory;
    if (x <= pre_x)
    {
        return;
    }

    if (neq(x, pre_x))
    {
        y = calculateCost(day, client, pre_detour, x, pre_load);
        // make piecey
        shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
        tmp->fromInst = make_shared<Insertion>(pre_detour, pre_load, pre_place);

        append(tmp);
    }
}

PLFunction::PLFunction(Params *params, vector<shared_ptr<LinearPiece>> pieces) : params(params)
{
    nbPieces = 0;
    this->pieces = vector<shared_ptr<LinearPiece>>();

    for (int i = 0; i < nbPieces; i++)
    {
        append(pieces[i]);
    }
}

double PLFunction::cost(double x)
{
    if (eq(x, 0))
    {
        return 0;
    }

    shared_ptr<LinearPiece> piece = getPiece(x);

    if (piece == nullptr)
    {
        throw std::string(
            "ERROR: can not find a piece that fit the value of x !!!");
    }

    return piece->cost(x);
}

shared_ptr<LinearPiece> PLFunction::getPiece(double x)
{
    if (gt(x, pieces[nbPieces - 1]->p2->x))
    {
        return nullptr;
    }

    shared_ptr<LinearPiece> fitPiece;
    // binary search to get right piece
    int first = 0;
    int last = nbPieces - 1;
    int middle = (first + last) / 2;
    while (first <= last)
    {
        if (lt(pieces[middle]->p2->x, x))
        {
            if ((middle == nbPieces - 1) || lt(x, pieces[middle + 1]->p2->x))
            {
                fitPiece = pieces[middle + 1];
                break;
            }
            first = middle + 1;
        }
        else if (lt(x, pieces[middle]->p2->x))
        {
            if ((middle == 0) || gt(x, pieces[middle - 1]->p2->x))
            {
                fitPiece = pieces[middle];
                break;
            }
            last = middle;
        }
        else
        {
            fitPiece = pieces[middle];
            break;
        }

        middle = (first + last) / 2;
    }
    return fitPiece;
}

std::shared_ptr<LinearPiece> PLFunction::getMinimalPiece(int client, double &minAt, double &minValue)
{
    if (nbPieces == 0)
        return nullptr;

    shared_ptr<LinearPiece> minPiece = pieces[0];
    minValue = pieces[0]->p1->y;
    minAt = pieces[0]->p1->x;

    if (lt(minPiece->p2->y, minValue))
    {
        minValue = minPiece->p2->y;
        minAt = minPiece->p2->x;
    }

    if (pieceAt0 != nullptr && lt(valueAt0, minValue))
    {
        minPiece = pieceAt0;
        minValue = valueAt0;
        minAt = params->cli[client].minInventory;
    }

    for (int i = 1; i < nbPieces; i++)
    {
        if (gt(minValue, pieces[i]->p2->y))
        {
            minPiece = pieces[i];
            minValue = pieces[i]->p2->y;
            minAt = pieces[i]->p2->x;
        }
    }

    return minPiece;
}

bool PLFunction::intersect(shared_ptr<LinearPiece> lp1, shared_ptr<LinearPiece> lp2, double &x, double &y)
{
    // parallel
    if (eq(lp1->slope, lp2->slope))
    {
        return false;
    }

    // get intersection point
    x = (lp1->p2->y - lp2->p2->y + lp2->slope * lp2->p2->x -
         lp1->slope * lp1->p2->x) /
        (lp2->slope - lp1->slope);
    y = lp1->p2->y - lp1->slope * (lp1->p2->x - x);

    if (gt(x, lp1->p1->x) && lt(x, lp1->p2->x) && gt(x, lp2->p1->x) &&
        lt(x, lp2->p2->x))
    {
        return true;
    }
    return false;
}

void PLFunction::append(shared_ptr<LinearPiece> lp)
{

    shared_ptr<LinearPiece> newPiece = lp->clone();

    if (nbPieces == 0)
    {
        pieces = vector<shared_ptr<LinearPiece>>();
        pieces.push_back(newPiece);
        nbPieces = 1;
    }
    else
    {
        shared_ptr<LinearPiece> lastPiece = this->pieces[this->nbPieces - 1];

        // check if we can merge new piece with the last piece
        // if (eq(lastPiece->slope, newPiece->slope) && eq(lastPiece->p2->x, newPiece->p1->x) && eq(lastPiece->p2->y,
        //                                                                                          newPiece->p1->y) &&
        //     lastPiece->fromC->eqlp(newPiece->fromC) && lastPiece->fromF->eqlp(newPiece->fromF)) {
        if (lastPiece->eqDeep(newPiece))
        {
            lastPiece->update(lastPiece->p1->x, lastPiece->p1->y, newPiece->p2->x, newPiece->p2->y);
        }
        else
        {
            //            newPiece->pre = lastPiece;
            lastPiece->next = newPiece;

            pieces.push_back(newPiece);
            nbPieces += 1;
        }
    }

    this->maxValue = std::max(this->maxValue, newPiece->p1->y);
    this->maxValue = std::max(this->maxValue, newPiece->p2->y);

    if (gt(this->minValue, newPiece->p1->y))
    {
        this->minValue = newPiece->p1->y;
        this->minimalPiece = newPiece;
    }

    if (gt(this->minValue, newPiece->p2->y))
    {
        this->minValue = newPiece->p2->y;
        this->minimalPiece = newPiece;
    }
}

void PLFunction::shiftLeft(double x_axis)
{
    for (int i = 0; i < this->nbPieces; i++)
    {
        pieces[i]->p1->x -= x_axis;
        pieces[i]->p2->x -= x_axis;
    }
}

void PLFunction::moveUp(double y_axis)
{
    for (int i = 0; i < this->nbPieces; i++)
    {
        pieces[i]->p1->y += y_axis;
        pieces[i]->p2->y += y_axis;
    }
}

void PLFunction::reflection(double x_axis)
{
}

std::shared_ptr<PLFunction> PLFunction::getInBound(double lb, double ub, bool updateValueAt0)
{

    std::shared_ptr<PLFunction> plFunction(std::make_shared<PLFunction>(params));

    plFunction->clear();
    plFunction->pieceAt0 = this->pieceAt0;
    plFunction->valueAt0 = this->valueAt0;

    if (updateValueAt0)
    {
        plFunction->pieceAt0 = nullptr;

        if (nbPieces == 1 && eq(this->pieces[0]->p2->x, lb))
        {
            plFunction->pieceAt0 = this->pieces[0];
            plFunction->valueAt0 = this->pieces[0]->p2->y;
        }
    }
    //    plFunction->valueAt0 = this->valueAt0;

    bool isFirstPiece = true;

    for (int i = 0; i < nbPieces; i++)
    {
        shared_ptr<LinearPiece> lp = pieces[i]->getInBound(lb, ub);

        if (lp != nullptr)
            plFunction->append(lp);

        if (updateValueAt0 && lt(pieces[i]->p1->x, lb) && le(lb, pieces[i]->p2->x))
        {
            plFunction->pieceAt0 = pieces[i];
            //            plFunction->copyPiece(pieces[i], plFunction->pieceAt0);
            plFunction->valueAt0 = pieces[i]->cost(lb);
        }
    }

    return plFunction;
}

double PLFunction::calculateCost(int day, int client, double detour, double demand, double freeload)
{
    // detour
    double cost = detour;

    // inventory cost
    double inventoryCost = (params->cli[client].inventoryCost - params->inventoryCostSupplier) * (double)(params->ancienNbDays - day);
    cost += inventoryCost * demand;

    // possible excess capacity
    cost += params->penalityCapa * std::max(0.0, demand - freeload);

    return cost;
}

double PLFunction::calculateDemandFromCost(int day, int client, double detour, double cost, double freeload)
{
    double inventoryCost = (params->cli[client].inventoryCost - params->inventoryCostSupplier) * (double)(params->ancienNbDays - day);

    double demand = (cost - detour + freeload * params->penalityCapa) / (inventoryCost + params->penalityCapa);

    // make sure demand > freeload
    assert(demand >= freeload);

    return demand;
}

bool PLFunction::testSuperposition()
{
    return false;
}

// void PLFunction::copyPiece(LinearPiece *source, LinearPiece *destination) {
//     if (destination == NULL)
//         destination = new LinearPiece();
//
//     destination->p1 = new Point(source->p1->x, source->p1->y);
//     destination->p2 = new Point(source->p2->x, source->p2->y);
//     destination->slope = source->slope;
//
//     destination->fromC = source->fromC;
//     destination->fromF = source->fromF;
//     destination->fromInst = source->fromInst;
// }

void PLFunction::print()
{
    for (int i = 0; i < nbPieces; i++)
    {
        cout << "(" << pieces[i]->p1->x << ", " << pieces[i]->p1->y << ", " << pieces[i]->p2->x
             << ", " << pieces[i]->p2->y << "), ";
    }
    cout << endl;
}

PLFunction::~PLFunction()
{

    //    cout << "Delete PLFunction, nbPieces: "<< pieces.size()<< endl;
    //    for(int i = 0; i < nbPieces; i++){
    //        pieces[i].reset();
    //    }
    //
    //    pieces.clear();
}
