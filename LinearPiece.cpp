//
// Created by pta on 15/08/2018.
//

#include "LinearPiece.h"
#include <algorithm>
#include <iomanip>
LinearPiece::LinearPiece()
{
    p1 = make_shared<Point>();
    p2 = make_shared<Point>();

    next = NULL;

    fromC = NULL;
    fromF = NULL;
    fromC_pre = NULL;
    fromInst = NULL;
}

LinearPiece::LinearPiece(double left_x, double left_y, double right_x, double right_y)
{
    p1 = make_shared<Point>(left_x, left_y);
    p2 = make_shared<Point>(right_x, right_y);
    if(eq(right_y, left_y))slope =0;
    else slope = (right_y - left_y) / (right_x - left_x);

    next = NULL;
    //    pre = NULL;
    replenishment_loss=0;
    fromC = NULL;
    fromC_pre = NULL;
    fromF = NULL;
    fromInst = NULL;
}

void LinearPiece::updateLinearPiece(double left_x, double left_y, double right_x, double right_y)
{
    this->p1 = make_shared<Point>(left_x, left_y);
    this->p2 = make_shared<Point>(right_x, right_y);
 
    
    if(eq(right_y , left_y) )this->slope=0;
    else this->slope = (right_y - left_y) / (right_x - left_x);
    this->next = nullptr;
}

 LinearPiece::LinearPiece(LinearPiece *lp)
{
    p1 = make_shared<Point>(lp->p1->x, lp->p1->y);
    p2 = make_shared<Point>(lp->p2->x, lp->p2->y);
    slope = lp->slope;
    next = lp->next;
    //    pre = lp->pre;
    fromC = lp->fromC;
    fromC_pre = lp->fromC_pre;
    fromF = lp->fromF;
    fromInst = lp->fromInst;
    replenishment_loss=lp->replenishment_loss;
}

double LinearPiece::cost(double x)
{
    
    if(eq(p1->x,p2->x)){
        if(eq(x,p1->x)){
            return p1->y;
        }
        else{
            cout <<"x = "<<x<<" p1->x "<<p1->x<<" p2->x " <<p2->x<<"error linear piece cost function(line65) "<<endl;
            int a;cin>>a;
        }
    }
    slope = (p2->y-p1->y)/(p2->x-p1->x);
    return slope * (x - p2->x) + p2->y;
}

double LinearPiece::invertCost(double y)
{
    return p2->x + (y - p2->y) / slope;
}

std::shared_ptr<LinearPiece> LinearPiece::getInBound(double lb, double ub)
{
    std::shared_ptr<LinearPiece> lp;
    
    if(eq (lb,ub)){
        if(le(p1->x,lb) && ge(p2->x, lb) ){
            lp = std::make_shared<LinearPiece>
                (lb, cost(lb), lb, cost(lb));
            lp->fromInst = fromInst;
            lp->fromC = fromC;
            lp->fromC_pre = fromC_pre;
            lp->fromF = fromF;
            lp->fromInst = fromInst;
            lp->replenishment_loss=replenishment_loss;
            return lp;
        }
        else
            return nullptr; 
    }
    else if(eq(p2->x, lb)){
            lp = std::make_shared<LinearPiece>(p2->x,p2->y, p2->x, p2->y); 
            lp->fromInst = fromInst;
            lp->fromC = fromC;
            lp->fromC_pre =fromC_pre;
            lp->fromF = fromF;
            lp->fromInst = fromInst;
            lp->replenishment_loss=replenishment_loss;
            return lp;
    }
    else if(eq(p1->x, ub)){
            lp = std::make_shared<LinearPiece>(p1->x,p1->y, p1->x, p1->y); 
            lp->fromInst = fromInst;
            lp->fromC = fromC;
            lp->fromC_pre =fromC_pre;
            lp->fromF = fromF;
            lp->fromInst = fromInst;
            lp->replenishment_loss=replenishment_loss;
            return lp;
    }
    else if (lt(p1->x, ub) && ge(p2->x, lb)){
        
        if (lt(p1->x, lb)){
            if (le(p2->x, ub)){
                lp = std::make_shared<LinearPiece>(lb, cost(lb), p2->x, p2->y);
            }
            else{
                lp = std::make_shared<LinearPiece>(lb, cost(lb), ub, cost(ub));
            }
        }
        else{
            double x = lb;
            if (gt(p1->x, x))
                x = p1->x;
            if (le(p2->x, ub)){
                    // this piece is clone to new LP
                lp = std::make_shared<LinearPiece>(x, p1->y, p2->x, p2->y);
            }
            else {
                lp = std::make_shared<LinearPiece>(x, p1->y, ub, cost(ub));
             }
        }
        lp->fromInst = fromInst;
        lp->fromC = fromC;
        lp->fromC_pre =fromC_pre;
        lp->fromF = fromF;
        lp->fromInst = fromInst;
        lp->replenishment_loss=replenishment_loss;
        return lp;
    }
    else
        return nullptr; // useless piece
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

bool LinearPiece::eqFromCpre(const shared_ptr<LinearPiece> rhs)
{
    if (rhs == NULL)
        return false;

    if (fromC_pre == NULL && rhs->fromC_pre == NULL)
        return true;

    if (fromC_pre == NULL || rhs->fromC_pre == NULL)
        return false;

    return fromC_pre->eqlp(rhs->fromC_pre);
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

LinearPiece::~LinearPiece()
{

}

void LinearPiece::update(double left_x, double left_y, double right_x, double right_y)
{
    p1->x = left_x;
    p1->y = left_y;
    p2->x = right_x;
    p2->y = right_y;
    slope = (right_y - left_y) / (right_x - left_x);
}
std::shared_ptr<LinearPiece> LinearPiece::getInpiece(double start, double end) const {
    if (lt(p2->x, start) || gt(p1->x, end)) return nullptr;
    
    if(neq(p2->x,p1->x) && neq(start,end) && (le(p2->x, start) || ge(p1->x, end)) )   return nullptr;
    if (le(start, p1->x) && le(p2->x, end)) {
        return std::make_shared<LinearPiece>(*this);
    }
    
    double startX = std::max<double>(p1->x, start);
    double startY = (startX - p1->x) * slope + p1->y;
    
    double endX = std::min<double>(p2->x, end);
    double endY = (endX - p1->x) * slope + p1->y;

    std::shared_ptr<LinearPiece> lp= std::make_shared<LinearPiece>(startX, startY, endX, endY);
    lp->fromC = this->fromC;
    lp->fromF = this->fromF;
    lp->fromC_pre = this->fromC_pre;
    lp->replenishment_loss=this->replenishment_loss;
    lp->fromInst = this->fromInst;

    return lp;
}

std::shared_ptr<LinearPiece> LinearPiece::clone()
{
    shared_ptr<LinearPiece> lp(make_shared<LinearPiece>(this->p1->x, this->p1->y, this->p2->x, this->p2->y));
    lp->slope = this->slope;
    lp->next = this->next;
    lp->fromC = this->fromC;
    lp->fromF = this->fromF;
    lp->fromInst = this->fromInst;
    lp->fromC_pre = this->fromC_pre;  
    lp->replenishment_loss=this->replenishment_loss;
    
    return lp;
}

std::shared_ptr<LinearPiece> LinearPiece::cloneWithout()
{
    shared_ptr<LinearPiece> lp(make_shared<LinearPiece>(this->p1->x, this->p1->y, this->p2->x, this->p2->y));
    lp->slope = this->slope;
    lp->next = this->next;
    lp->fromC = nullptr;
    lp->fromF = nullptr;
    lp->fromInst = nullptr;
    lp->fromC_pre = nullptr;  
    lp->replenishment_loss=this->replenishment_loss;
    
    return lp;
}

void LinearPiece::print()
{   
    cout << "(" << p1->x << ", " << p1->y << ", " << p2->x
             << ", " << p2->y << ") ";
    cout << endl;
}
std::shared_ptr<LinearPiece> LinearPiece::clone_addHolding(double InventoryCost, double daily)
{
    shared_ptr<LinearPiece> lp(make_shared<LinearPiece>(this->p1->x, this->p1->y+InventoryCost*(this->p1->x-daily), 
                            this->p2->x, this->p2->y+InventoryCost*(this->p2->x-daily)));
    lp->next = this->next;
    lp->fromC = this->fromC;
    lp->fromF = this->fromF;
    lp->fromInst = this->fromInst;
    lp->replenishment_loss=this->replenishment_loss;
    lp->fromC_pre = this->fromC_pre;
    return lp;
}

std::shared_ptr<LinearPiece> LinearPiece::clone_addStockout(double stockout, double daily)
{
    shared_ptr<LinearPiece> lp(make_shared<LinearPiece>(this->p1->x, this->p1->y-stockout*(daily-this->p1->x), 
                            this->p2->x, this->p2->y-stockout*(daily-this->p1->x)) );
    lp->next = this->next;
    lp->fromC = this->fromC;
    lp->fromF = this->fromF;
    lp->fromC_pre = this->fromC_pre;
    lp->replenishment_loss=this->replenishment_loss;
    lp->fromInst = this->fromInst;

    return lp;
}

