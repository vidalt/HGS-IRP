//
// Created by pta on 15/08/2018.
//

#include "LinearPiece.h"

LinearPiece::LinearPiece()
{
    p1 = make_shared<Point>();
    p2 = make_shared<Point>();

    next = NULL;
    //    pre = NULL;

    fromC = NULL;
    fromF = NULL;
    fromInst = NULL;
}

LinearPiece::LinearPiece(double left_x, double left_y, double right_x, double right_y)
{
    p1 = make_shared<Point>(left_x, left_y);
    p2 = make_shared<Point>(right_x, right_y);
    slope = (right_y - left_y) / (right_x - left_x);

    next = NULL;
    //    pre = NULL;

    fromC = NULL;
    fromF = NULL;
    fromInst = NULL;
}

LinearPiece::LinearPiece(LinearPiece *lp)
{
    p1 = make_shared<Point>(lp->p1->x, lp->p1->y);
    p2 = make_shared<Point>(lp->p2->x, lp->p2->y);
    slope = lp->slope;
    next = lp->next;
    //    pre = lp->pre;
    fromC = lp->fromC;
    fromF = lp->fromF;
    fromInst = lp->fromInst;
}

double LinearPiece::cost(double x)
{
    return slope * (x - p2->x) + p2->y;
}

double LinearPiece::invertCost(double y)
{
    return p2->x + (y - p2->y) / slope;
}

std::shared_ptr<LinearPiece> LinearPiece::getInBound(double lb, double ub)
{

    std::shared_ptr<LinearPiece> lp;

    if (lt(p1->x, ub) && gt(p2->x, lb))
    {
        if (lt(p1->x, lb))
        {
            if (gt(p2->x, lb))
            {
                if (le(p2->x, ub))
                {
                    lp = std::make_shared<LinearPiece>(lb, cost(lb), p2->x, p2->y);
                }
                else
                {
                    lp = std::make_shared<LinearPiece>(lb, cost(lb), ub, cost(ub));
                }
            }
        }
        else
        { // p1.x > lb
            if (lt(p1->x, ub))
            {
                double x = lb;
                if (gt(p1->x, x))
                    x = p1->x;

                if (le(p2->x, ub))
                {
                    // this piece is clone to new LP
                    lp = std::make_shared<LinearPiece>(x, p1->y, p2->x, p2->y);
                }
                else
                {
                    lp = std::make_shared<LinearPiece>(x, p1->y, ub, cost(ub));
                }
            }
        }
        lp->fromC = fromC;
        lp->fromF = fromF;
        lp->fromInst = fromInst;

        return lp;
    }
    else
        return nullptr;
}

bool LinearPiece::eqlp(const shared_ptr<LinearPiece> rhs)
{
    if (rhs == NULL)
        return false;

    return eq(p1->x, rhs->p1->x) && eq(p1->y, rhs->p1->y) && eq(p2->x, rhs->p2->x) && eq(p2->y, rhs->p2->y);
}

bool LinearPiece::eqFromC(const shared_ptr<LinearPiece> rhs)
{
    if (rhs == NULL)
        return false;

    if (fromC == NULL && rhs->fromC == NULL)
        return true;

    if (fromC == NULL || rhs->fromC == NULL)
        return false;

    return fromC->eqlp(rhs->fromC);
}

bool LinearPiece::eqFromF(const shared_ptr<LinearPiece> rhs)
{
    if (rhs == NULL)
        return false;

    if (fromF == NULL && rhs->fromF == NULL)
        return true;

    if (fromF == NULL || rhs->fromF == NULL)
        return false;

    return fromF->eqlp(rhs->fromF);
}

bool LinearPiece::eqDeep(const shared_ptr<LinearPiece> rhs)
{
    return eqlp(rhs) && eqFromC(rhs) && eqFromF(rhs);
}

// LinearPiece *LinearPiece::clone() {
//         LinearPiece *lp = new LinearPiece(this->p1->x, this->p1->y, this->p2->x, this->p2->y);
//
//         if (this->fromC != NULL)
//             lp->fromC = this->fromC;
//
//         if (this->fromF != NULL)
//             lp->fromF = this->fromF;
//
//         lp->fromInst = this->fromInst;
//
//         return lp;
// }

LinearPiece::~LinearPiece()
{
    //    delete p1;
    //    delete p2;

    //    cout << "Delete LinearPiece" << endl;
}

void LinearPiece::update(double left_x, double left_y, double right_x, double right_y)
{
    p1->x = left_x;
    p1->y = left_y;
    p2->x = right_x;
    p2->y = right_y;
    slope = (right_y - left_y) / (right_x - left_x);
}

std::shared_ptr<LinearPiece> LinearPiece::clone()
{
    shared_ptr<LinearPiece> lp(make_shared<LinearPiece>(this->p1->x, this->p1->y, this->p2->x, this->p2->y));
    lp->slope = this->slope;
    lp->next = this->next;
    //    lp->pre = this->pre;
    lp->fromC = this->fromC;
    lp->fromF = this->fromF;
    lp->fromInst = this->fromInst;

    return lp;
}
