//
// Created by pta on 15/08/2018.
//

#include "PLFunction.h"
#include <cmath>
#include <cassert>
//#include "matplotlibcpp.h"
//namespace plt = matplotlibcpp;
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

    std::vector<Insertion>::iterator index = insertions.begin();

    double pre_x = 0;
    double pre_y, x, y, a, pre_load, pre_detour;
    Noeud *pre_place = nullptr;

    // loop through all pieces
    while (index != insertions.end())
    {
        // make a first piece for the first insertion
        if (index == insertions.begin())
        {
            pre_x = 0;
            pre_y = index->detour;
            x = index->load;
            y = calculateCost(day, client, index->detour, x, index->load); //x,load: demand loadfree
            
        }
        else
        {
            double current_cost = calculateCost(day, client, index->detour, index->load, index->load);
            std::vector<Insertion>::iterator pre_index = index - 1;
            double pre_insertion_cost_with_current_demand = calculateCost(day, client, pre_index->detour, index->load, pre_index->load);

            bool is_dominated_vehicle = (index->load <= pre_load) || (pre_insertion_cost_with_current_demand <= current_cost);

            if (!is_dominated_vehicle)
            {    // make piecey
                //index->detour = pre_detour +  x *PenalityCapacity - preload*PenalityCapacity 
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

            x = index->load;
            y = current_cost;
        }

        a = (y - pre_y) / (x - pre_x);

        if (neq(x, pre_x))
        {
            // make piecey
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


PLFunction::PLFunction(Params *params, vector<Insertion> insertions, int day, int client,bool st) : params(params)
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

    std::vector<Insertion>::iterator index = insertions.begin();

    double pre_x = 0;
    double pre_y, x, y, a, pre_load, pre_detour;
    Noeud *pre_place = nullptr;

    // loop through all pieces
    while (index != insertions.end())
    {
        // make a first piece for the first insertion
        if (index == insertions.begin())
        {
            pre_x = 0;
            pre_y = calculateCost_holding(day, client, index->detour, pre_x,  pre_x);
            x = index->load;
            y = calculateCost_holding(day, client, index->detour, x, index->load); //x,load: demand loadfree
            
        }
        else
        {
            double current_cost = calculateCost_holding(day, client, index->detour, index->load, index->load);
            std::vector<Insertion>::iterator pre_index = index - 1;
            double pre_insertion_cost_with_current_demand = calculateCost_holding(day, client, pre_index->detour, index->load, pre_index->load);

            bool is_dominated_vehicle = (index->load <= pre_load) || (pre_insertion_cost_with_current_demand <= current_cost);

            if (!is_dominated_vehicle)
            {    // make piecey
                //index->detour = pre_detour +  x *PenalityCapacity - preload*PenalityCapacity 
                x = pre_load + (index->detour - pre_detour) / params->penalityCapa;

                y = calculateCost_holding(day, client, pre_detour, x, pre_load);

                if (fabs(x- pre_x)>0.999)
                {
                    shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
                    tmp->fromInst = make_shared<Insertion>(pre_index->detour, pre_index->load, pre_index->place);
                    append(tmp);   
                    
                }
                pre_x = x;
                pre_y = y;
            }   

            x = index->load;
            y = current_cost;
        }

        a = (y - pre_y) / (x - pre_x);

        if (fabs(x- pre_x)>0.999)
        {
            // make piecey
            pre_y = calculateCost_holding(day, client, index->detour, pre_x, index->load);
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
    if (x-pre_x <= 0.01 )
    {
        return;
    }
   if (fabs(x- pre_x)>0.999)
    {
        y = calculateCost_holding(day, client, pre_detour, x, pre_load);
        // make piecey
        shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
        tmp->fromInst = make_shared<Insertion>(pre_detour, pre_load, pre_place);
        append(tmp);
        
    }
    
}


PLFunction::PLFunction(Params *params, vector<Insertion> insertion, int day, int client, bool st, int daily) : params(params)
{
    std::vector<Insertion> insertions = insertion;

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
    bool flag= false;

    // loop through all pieces
    while (index != insertions.end())
    {
        if(index->load>daily) flag = true, index->load=daily;
        if (index == insertions.begin()){
            pre_x = 0;
            pre_y = calculateCost_stockout(day, client, index->detour, pre_x, pre_x);
            x = index->load;
            y = calculateCost_stockout(day, client, index->detour, x, index->load);
        }
        else{
            double current_cost = calculateCost_stockout(day, client, index->detour, index->load, index->load);
            std::vector<Insertion>::iterator pre_index = index - 1;
            double pre_insertion_cost_with_current_demand=calculateCost_stockout(day, client, pre_index->detour, index->load, pre_index->load);
            bool is_dominated_vehicle = (index->load <= pre_load) || (pre_insertion_cost_with_current_demand <= current_cost);
            
            if (!is_dominated_vehicle  )
            {    // make piecey
                //index->detour = pre_detour +  x *PenaltyCapacity - preload*PenalityCapacity 
                
                x = pre_load + (index->detour - pre_detour) / params->penalityCapa;
                if(x > daily)   flag = 1, x= daily;
                y = calculateCost_stockout(day, client, pre_detour, x, pre_load);
                
                if (fabs(x- pre_x)>0.999) 
                {
                    shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
                    tmp->fromInst = make_shared<Insertion>(pre_index->detour, pre_index->load, pre_index->place);
                    append(tmp);       
                }
                pre_x = x;
                pre_y = y;
            }   

            x = index->load;
            y = current_cost;
        }

        if (fabs(x- pre_x)>0.999)
        {
            // make piecey
            pre_y= calculateCost_stockout(day, client, index->detour, pre_x, index->load);
            shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
            tmp->fromInst = make_shared<Insertion>(index->detour, index->load, index->place);

            append(tmp);
        }
        if(flag)    return;
        pre_x = x;
        pre_y = y;
        
        pre_load = index->load;
        pre_detour = index->detour;
        pre_place = index->place;
        index++;
    }

    // the last piece
    x = params->cli[client].maxInventory;
    if (x -pre_x <= 0.01)
    {
        return;
    }
   if (fabs(x- pre_x)>0.999)
    {
        y = calculateCost_stockout(day, client, pre_detour, x, pre_load);
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
void PLFunction::update_minValue(double & rep){
    double min_tmp = std::min<double>(this->pieces[0]->p1->y,this->pieces[0]->p2->y);
    rep = -(gt(this->pieces[0]->p1->y,this->pieces[0]->p2->y)? this->pieces[0]->p2->x:this->pieces[0]->p1->x);
    if(lt(rep , 0)){
        cout<<"rep "<<rep+1e-4<<endl;
        cout <<"error.PLfunction update_minvalue"<<endl;
        int a;cin>>a;
    }
    int piece_tmp = 0;
    for(int i =1 ; i < this->nbPieces ; i++){
        double tmp = std::min<double>(this->pieces[i]->p1->y,this->pieces[i]->p2->y);
        if(gt(min_tmp,tmp)){
            min_tmp = tmp;
            rep=  -(gt(this->pieces[i]->p1->y,this->pieces[i]->p2->y)? this->pieces[i]->p2->x:this->pieces[i]->p1->x);
            piece_tmp = i;
        }
    }
    this->minimalPiece = this->pieces[piece_tmp];
    this->minValue = min_tmp;
}
 

double PLFunction::cost(double x)
{
    if (eq(x, 0))
    {
        return 0; //*********************************************
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

std::shared_ptr<LinearPiece> PLFunction::getMinimalPiece_stockout
(int client, double &minAt, double &minValue){
//client, I[day], objective
    if (nbPieces == 0)
        return nullptr;

    shared_ptr<LinearPiece> minPiece = pieces[0];
    minValue = pieces[0]->p1->y;
    minAt = pieces[0]->p1->x;

    if (lt(minPiece->p2->y, minValue)) {
        minValue = minPiece->p2->y;
        minAt = minPiece->p2->x;
    }

    for (int i = 1; i < nbPieces; i++) {
        if (gt(minValue, pieces[i]->p2->y)){
            
            minPiece = pieces[i];
            
            minValue = pieces[i]->p2->y;
            minAt = pieces[i]->p2->x;
        }
        else if (gt(minValue, pieces[i]->p1->y)){
            
            minPiece = pieces[i];
            
            minValue = pieces[i]->p1->y;
            minAt = pieces[i]->p1->x;
        }
    }

    return minPiece;
}

bool PLFunction::intersect(shared_ptr<LinearPiece> lp1, shared_ptr<LinearPiece> lp2, double &x, double &y)
{
    bool trsce = false;
    if(lp1)lp1->updateLinearPiece(lp1->p1->x,lp1->p1->y,lp1->p2->x,lp1->p2->y);
    if(lp2)lp2->updateLinearPiece(lp2->p1->x,lp2->p1->y,lp2->p2->x,lp2->p2->y);
   // if(eq(lp1->p1->x,-1)&&eq(lp1->p2->x,62))  trsce = true;
    
    
    if(trsce)    cout <<"slope1 "<<lp1->slope<<"  slope2  "<<lp2->slope<<endl;
    // parallel
    if (eq(lp1->slope, lp2->slope))
        return false;
    

    // get intersection point
    
    double del_b = ( (lp1->p2->y - lp1->slope * lp1->p2->x ) - (lp2->p2->y - lp2->slope * lp2->p2->x) );
    double del_k=lp2->slope - lp1->slope;
    x = del_b/del_k;
    y = lp1->p2->y - lp1->slope * (lp1->p2->x - x);

    if(le(x, lp1->p1->x)||  le(x, lp2->p1->x) ||le( lp1->p2->x,x) || le( lp2->p2->x,x))
     return false;
    return true;
}

void PLFunction::append(shared_ptr<LinearPiece> lp){
    if(!lp) return;
    double minNum = std::min(lp->p1->x, lp->p2->x);
    double maxNum = std::max(lp->p1->x, lp->p2->x);
    if(ceil(minNum) >floor(maxNum)) return;
    double exp=0;
    if(lp->slope<100&&lp->slope>-100)exp= 0.01;
    else if(lp->slope<1000&& lp->slope>-1000)exp = 0.001;
    else    exp = 0.0001;
    if(fabs(lp->p1->y*1000-floor(lp->p1->y*1000)) <=exp) lp->p1->y=floor(lp->p1->y*1000)/1000;  
    if(fabs(lp->p1->y*1000-ceil(lp->p1->y*1000))<=exp) lp->p1->y=ceil(lp->p1->y*1000)/1000;  
    if(fabs(lp->p2->y*1000-floor(lp->p2->y*1000))<=exp) lp->p2->y=floor(lp->p2->y*1000)/1000;  
    if(fabs(lp->p2->y*1000-ceil(lp->p2->y*1000))<=exp) lp->p2->y=ceil(lp->p2->y*1000)/1000; 
    
    if(fabs(lp->p1->x*1000-floor(lp->p1->x*1000)) <=exp) lp->p1->x=floor(lp->p1->x*1000)/1000;  
    if(fabs(lp->p1->x*1000-ceil(lp->p1->x*1000))<=exp) lp->p1->x=ceil(lp->p1->x*1000)/1000;  
    if(fabs(lp->p2->x*1000-floor(lp->p2->x*1000))<=exp) lp->p2->x=floor(lp->p2->x*1000)/1000;  
    if(fabs(lp->p2->x*1000-ceil(lp->p2->x*1000))<=exp) lp->p2->x=ceil(lp->p2->x*1000)/1000;
    shared_ptr<LinearPiece> newPiece = lp->clone();
    if(newPiece)    newPiece->updateLinearPiece(newPiece->p1->x, newPiece->p1->y,newPiece->p2->x, newPiece->p2->y);
        
    if (nbPieces == 0){
        pieces = vector<shared_ptr<LinearPiece>>();
        pieces.push_back(newPiece);
        nbPieces = 1;
    }
    else{
        shared_ptr<LinearPiece> lastPiece = this->pieces[this->nbPieces - 1];
        lastPiece->updateLinearPiece(lastPiece->p1->x, lastPiece->p1->y,lastPiece->p2->x, lastPiece->p2->y);
                //。= *
        if (lastPiece->eqDeep(newPiece))
            lastPiece->update(lastPiece->p1->x, lastPiece->p1->y, newPiece->p2->x, newPiece->p2->y);
        
        else if (eq(lastPiece->p1->x,lastPiece->p2->x) && eq(newPiece->p1->x,lastPiece->p2->x)){
            if(eq(newPiece->p1->x,newPiece->p2->x)&&ge(newPiece->p1->y,lastPiece->p2->y)){
                //* (ignore new)
                //. 
            }
            else if(ge(lastPiece->p2->y,newPiece->p1->y)){
                //.
                //*(-----------)
                pieces.pop_back();
                pieces.push_back(newPiece);  
                
            }
            else{
                //*-----------
                //.
               
                pieces.push_back(newPiece);
                lastPiece->next = newPiece;
                nbPieces += 1;
            }
        }
        
        //---------o -> --------o
        //         * ->          *
        else if (eq(newPiece->p1->x,newPiece->p2->x)&&eq(newPiece->p1->x,lastPiece->p2->x)  ){
             if(gt(lastPiece->p2->y,newPiece->p1->y)){
                pieces.push_back(newPiece);
                lastPiece->next = newPiece;
                nbPieces += 1;
             }
            else{
                //         * (ignore new)
                //---------o
            }
        }
        else{
            //link together
            if(eq(lastPiece->slope,newPiece->slope)&&  eq(newPiece->p1->y, lastPiece->p2->y) &&lastPiece->eqFromC(newPiece) && lastPiece->eqFromF(newPiece) &&lastPiece->eqFromCpre(newPiece) && lastPiece->replenishment_loss == newPiece->replenishment_loss){
                newPiece->p1->x = lastPiece->p1->x;
                newPiece->p1->y = lastPiece->p1->y;
                pieces.pop_back();
                pieces.push_back(newPiece);  
            }
            //add in newPiece
            else{
                lastPiece->next = newPiece;
                pieces.push_back(newPiece);
                nbPieces += 1;
            }
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

void PLFunction::addHolding(double InventoryCost,double daily) //x = 6,daily = 5  -----> +1*holdingcost --->dp(1)
{
    for (int i = 0; i < this->nbPieces; i++)
    {
        pieces[i]->p1->y += InventoryCost*(pieces[i]->p1->x-daily);
        pieces[i]->p2->y += InventoryCost*(pieces[i]->p2->x-daily);
        pieces[i]->update(pieces[i]->p1->x,pieces[i]->p1->y,pieces[i]->p2->x,pieces[i]->p2->y);
    }
}
void PLFunction::addHoldingf(double InventoryCost) //x = 6,daily = 5  -----> +1*holdingcost --->dp(1)
{
    for (int i = 0; i < this->nbPieces; i++)
    {
        pieces[i]->p1->y += InventoryCost*(pieces[i]->p1->x);
        pieces[i]->p2->y += InventoryCost*(pieces[i]->p2->x); 
        pieces[i]->update(pieces[i]->p1->x,pieces[i]->p1->y,pieces[i]->p2->x,pieces[i]->p2->y);
    }
}
void PLFunction::addStockout(double stockoutCost,double daily)//x = 2,daily = 6  -----> +1*stockoutcost --->dp(-1)
{
    for (int i = 0; i < this->nbPieces; i++)
    {
        pieces[i]->p1->y += stockoutCost*(daily-pieces[i]->p1->x);
        pieces[i]->p2->y += stockoutCost*(daily-pieces[i]->p2->x);
        pieces[i]->update(pieces[i]->p1->x,pieces[i]->p1->y,pieces[i]->p2->x,pieces[i]->p2->y);
    }
}
void PLFunction::addStockoutf(double stockoutCost)//x = 2,daily = 6  -----> +1*stockoutcost --->dp(-1)
{
    for (int i = 0; i < this->nbPieces; i++)
    {
        pieces[i]->p1->y -= stockoutCost*(pieces[i]->p1->x);
        pieces[i]->p2->y -= stockoutCost*(pieces[i]->p2->x);

        pieces[i]->update(pieces[i]->p1->x,pieces[i]->p1->y,pieces[i]->p2->x,pieces[i]->p2->y);
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

std::shared_ptr<PLFunction> PLFunction::update0(double min0)
{  
    std::shared_ptr<PLFunction> newPL(std::make_shared<PLFunction>(params));
    std::shared_ptr<LinearPiece> tmp = 
            std::make_shared<LinearPiece>(0,min0,0.000000001,min0);
    if(this->nbPieces == 0) { 
        newPL->append(tmp);
        return newPL;
    }
   

    if(gt(this->pieces[0]->p1->y , min0)){
        if(eq(this->pieces[0]->p1->x,this->pieces[0]->p2->x) &&eq(this->pieces[0]->p1->x,0)){
            newPL->append(tmp);
        }
        else{
            newPL->append(tmp);
            newPL->append(this->pieces[0]);
        }
       
       for (int i = 1; i < this->nbPieces;i++){
            newPL->append(this->pieces[i]);
        }
        newPL->minValue = min0;
        newPL->minimalPiece = newPL->pieces[0];
    } 
    else{
        for (int i = 0; i < this->nbPieces;i++){
            newPL->append(this->pieces[i]);
        }
        newPL->minValue = this ->minValue;
        newPL->minimalPiece = this->minimalPiece;
    }
    return newPL;
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
        if (nbPieces == 1 && eq(this->pieces[0]->p2->x, lb)  ) 
        {
            plFunction->pieceAt0 = this->pieces[0];
            plFunction->valueAt0 = this->pieces[0]->p2->y;
        }
    }
    //    plFunction->valueAt0 = this->valueAt0;

    bool isFirstPiece = true;

    for (int i = 0; i < nbPieces; i++){
        shared_ptr<LinearPiece> lp = pieces[i]->getInBound(lb, ub);
        
        if (lp != nullptr)
            plFunction->append(lp);
            
        if (updateValueAt0 && lt(pieces[i]->p1->x, lb) && le(lb, pieces[i]->p2->x)   ) // lb is within this piece (inside)
        {
            plFunction->pieceAt0 = pieces[i];
            plFunction->valueAt0 = pieces[i]->cost(lb);
        }
    }
    return plFunction;
}

double PLFunction::calculateCost(int day, int client, double detour, double demand, double freeload)
{
    // detour
    double cost = detour;
    
    double inventoryCost = (params->cli[client].inventoryCost    -    params->inventoryCostSupplier) 
                            * (double)(params->ancienNbDays - day);
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

void PLFunction::print()
{
    for (int i = 0; i < nbPieces; i++)
    {
        
        cout << i<<" :(" << pieces[i]->p1->x << ", " << pieces[i]->p1->y << ", " << pieces[i]->p2->x
             << ", " << pieces[i]->p2->y << "), ";
    }
    cout << endl;
}

//only calculate the &stockout cost &detour & more capacity cost
double PLFunction::calculateCost_stockout(int day, int client, double detour, double replenishment, double freeload)
{
    // detour
    double cost = detour;
    //stock-out cost
    cost +=  params->cli[client].stockoutCost  * (params->cli[client].dailyDemand[day] - replenishment);
    //cost-depot(holdingcost)
    cost -= params->inventoryCostSupplier * replenishment * (double)(params->ancienNbDays - day); 
    
    // possible excess capacity
    cost += params->penalityCapa * std::max(0.0, replenishment - freeload);
    return cost;
}

//only calculate the hodling cost &detour & more capacity cost
double PLFunction:: calculateCost_holding(int day, int client, double detour, double replenishment, double freeload)
{
    // detour
    double cost = detour;
    // holding cost
    cost +=  params->cli[client].inventoryCost  * (replenishment - params->cli[client].dailyDemand[day] );
    //cost-depot(holdingcost)
    cost -= params->inventoryCostSupplier * replenishment * (double)(params->ancienNbDays - day); 
    
    // possible excess capacity
    cost += params->penalityCapa * std::max(0.0, replenishment - freeload);
    return cost;
}

PLFunction::~PLFunction()
{

}
