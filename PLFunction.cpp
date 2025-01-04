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
    //cout <<"hahah";
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

    // loop through all pieces
    while (index != insertions.end())
    {
        // make a first piece for the first insertion
        //首先，如果这是第一个insertion，它设置初始的y和x值。
    //对于其他insertion，它将基于给定的detour和load值创建两个线性片段。为此，它使用一个辅助函数calculateCost来计算与每个insertion相关的成本。
    //添加新的LinearPiece到PLFunction:
        if (index == insertions.begin())
        {
            pre_x = 0;
            pre_y = index->detour;
            x = index->load;
            y = calculateCost(day, client, index->detour, x, index->load); //x,load: demand loadfree  按照最大的算
            
        }
        else
        {
            double current_cost = calculateCost(day, client, index->detour, index->load, index->load);
            //按照最大的算
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
    //cout <<"hahah";
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

    // loop through all pieces
    while (index != insertions.end())
    {
        // make a first piece for the first insertion
        //首先，如果这是第一个insertion，它设置初始的y和x值。
    //对于其他insertion，它将基于给定的detour和load值创建两个线性片段。为此，它使用一个辅助函数calculateCost来计算与每个insertion相关的成本。
    //添加新的LinearPiece到PLFunction:
        if (index == insertions.begin())
        {
            pre_x = 0;
            pre_y = calculateCost_holding(day, client, index->detour, pre_x,  pre_x);
            x = index->load;
            y = calculateCost_holding(day, client, index->detour, x, index->load); //x,load: demand loadfree  按照最大的算
            
        }
        else
        {
            double current_cost = calculateCost_holding(day, client, index->detour, index->load, index->load);
            //按照最大的算
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
    bool trace = false;
    if(trace) cout <<"day "<<day <<" client "<<client<<endl;
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
            //按照最大的算
            std::vector<Insertion>::iterator pre_index = index - 1;
            double pre_insertion_cost_with_current_demand=calculateCost_stockout(day, client, pre_index->detour, index->load, pre_index->load);
            bool is_dominated_vehicle = (index->load <= pre_load) || (pre_insertion_cost_with_current_demand <= current_cost);
            
            if (!is_dominated_vehicle  )
            {    // make piecey
                //index->detour = pre_detour +  x *PenaltyCapacity - preload*PenalityCapacity 
                
                x = pre_load + (index->detour - pre_detour) / params->penalityCapa;
                 //cout <<"x = §§ "<<x<<" preload = "<<pre_load << "   detour = " << index->detour - pre_detour<< " capc "<< params->penalityCapa << endl;
                if(x > daily)   flag = 1, x= daily;
                y = calculateCost_stockout(day, client, pre_detour, x, pre_load);
                
                if (fabs(x- pre_x)>0.999) 
                {
                    shared_ptr<LinearPiece> tmp(make_shared<LinearPiece>(pre_x, pre_y, x, y));
                    tmp->fromInst = make_shared<Insertion>(pre_index->detour, pre_index->load, pre_index->place);
                    append(tmp);
                    if(trace) cout <<"else ( "<<pre_x<<" , "<< pre_y<<" ) " <<" --> "<<"( "<<x<<" , "<< y<<" )"<<endl, tmp->print();          
                    
                }
                pre_x = x;
                pre_y = y;
            }   

            x = index->load;
            y = current_cost;
        }
        
       // a = (y - pre_y) / (x - pre_x);

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
        if(trace) cout <<"( "<<pre_x<<" , "<< pre_y<<" ) " <<" --> "<<"( "<<x<<" , "<< y<<" )"<<endl,
                                tmp->print(); 
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
    bool trace = false;
    double min_tmp = std::min<double>(this->pieces[0]->p1->y,this->pieces[0]->p2->y);
    rep = -(gt(this->pieces[0]->p1->y,this->pieces[0]->p2->y)? this->pieces[0]->p2->x:this->pieces[0]->p1->x);
    if(lt(rep , 0)){
        cout<<"rep "<<rep+1e-4<<endl;
        cout <<"error.PLfunction update_minvalue"<<endl;
        int a;cin>>a;
    }
    if(trace)   cout <<"piece 0 rep "<<rep<<endl;
    int piece_tmp = 0;
    for(int i =1 ; i < this->nbPieces ; i++){
        double tmp = std::min<double>(this->pieces[i]->p1->y,this->pieces[i]->p2->y);
        if(gt(min_tmp,tmp)){
            min_tmp = tmp;
            rep=  -(gt(this->pieces[i]->p1->y,this->pieces[i]->p2->y)? this->pieces[i]->p2->x:this->pieces[i]->p1->x);
            if(trace)   cout <<"piece "<<i<<" rep "<<rep<<endl;
            piece_tmp = i;
        }
    }
    //cout <<"asdasdas  "<<piece_tmp<<endl;
    //this->print();
    //this->pieces[piece_tmp]->fromC_pre->print();
    this->minimalPiece = this->pieces[piece_tmp];
    //this->minimalPiece->fromC_pre->print();
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
    //cout <<"pieces "<<endl;
      //      pieces[0]->fromF->print();
    //cout <<"minPiece "<<endl;
      //      minPiece->fromF->print();
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
    //cout <<"l1s "<<lp1->slope<<"  l2 "<<lp2->slope;
    if (eq(lp1->slope, lp2->slope))
        return false;
    

    // get intersection point
    
    double del_b = ( (lp1->p2->y - lp1->slope * lp1->p2->x ) - (lp2->p2->y - lp2->slope * lp2->p2->x) );
    double del_k=lp2->slope - lp1->slope;
    x = del_b/del_k;
   // cout<<" (b1-b2)/slope "<<x<<endl;
    y = lp1->p2->y - lp1->slope * (lp1->p2->x - x);
    if(trsce) {
        cout<<"!!!!!!!!!!x = "<<x<<"  y= "<<y<<endl;
        cout<<"!    lp1 "<<lp1->slope<<" ";lp1->print();
        cout<<"!    lp2 "<<lp2->slope<<" ";lp2->print();}

    if(le(x, lp1->p1->x)||  le(x, lp2->p1->x) ||le( lp1->p2->x,x) || le( lp2->p2->x,x))
     return false;
    //if (gt(x-0.00001, lp1->p1->x) && lt(x+0.00001, lp1->p2->x) && gt(x-0.00001, lp2->p1->x) && lt(x+0.00001, lp2->p2->x))   
   //  cout<<"       sdsddd";
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
                //cout <<"Plfunction line 438:"<<newPiece->fromInst->detour<<endl;
                nbPieces += 1;
            }
        }
        
        //---------o -> --------o
        //         * ->          *
        else if (eq(newPiece->p1->x,newPiece->p2->x)&&eq(newPiece->p1->x,lastPiece->p2->x)  ){
             if(gt(lastPiece->p2->y,newPiece->p1->y)){
                pieces.push_back(newPiece);
                lastPiece->next = newPiece;
                //cout <<"Plfunction line 438:"<<newPiece->fromInst->detour<<endl;
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
    bool traces = false;
    if(traces){
        cout <<"before updated"<<endl;
            this->print();
    }
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
    if(traces){
        cout <<"after updated"<<endl;
        newPL->print();
        int a;cin>>a;
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
        //只有0点一个点
        if (nbPieces == 1 && eq(this->pieces[0]->p2->x, lb)  ) 
        {
            plFunction->pieceAt0 = this->pieces[0];
            plFunction->valueAt0 = this->pieces[0]->p2->y; //该片段结束点的y坐标
        }
    }
    //    plFunction->valueAt0 = this->valueAt0;

    bool isFirstPiece = true;

    for (int i = 0; i < nbPieces; i++){
        //cout<<"i "<<i<<" : ";
        //pieces[i]->print();
        //cout <<"sloup" <<pieces[i]->slope<<"  lb,ub "<<lb<<"  "<<ub<<endl;
        shared_ptr<LinearPiece> lp = pieces[i]->getInBound(lb, ub);
        
        //if(lp) lp->print();
        if (lp != nullptr)
            plFunction->append(lp);
            
        if (updateValueAt0 && lt(pieces[i]->p1->x, lb) && le(lb, pieces[i]->p2->x)   ) // lb is within this piece (inside)
        {
            plFunction->pieceAt0 = pieces[i];
            //            plFunction->copyPiece(pieces[i], plFunction->pieceAt0);
            plFunction->valueAt0 = pieces[i]->cost(lb);
        }
    }

    //cout <<"plFunctionhhhh "<<endl;
    //plFunction->print();
    //int a;cin>>a;
    return plFunction;
}

double PLFunction::calculateCost(int day, int client, double detour, double demand, double freeload)
{
    // detour
    double cost = detour;

    // inventory cost 首先，计算特定客户的库存成本减去供应商的库存成本，然后乘以剩余的天数。
    
    double inventoryCost = (params->cli[client].inventoryCost    -    params->inventoryCostSupplier) 
                            * (double)(params->ancienNbDays - day);
    cost += inventoryCost * demand; //得到的inventoryCost再乘以demand来获取总的库存成本，然后加到cost上。

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
        
        cout << i<<" :(" << pieces[i]->p1->x << ", " << pieces[i]->p1->y << ", " << pieces[i]->p2->x
             << ", " << pieces[i]->p2->y << "), ";
        //std::vector<double> xs = {pieces[i]->p1->x, pieces[i]->p2->x};
        //std::vector<double> ys = {pieces[i]->p1->y, pieces[i]->p2->y};
        
        //plt::plot(xs, ys, "o-"); // "o-"意味着每个点都用圈标记，并用线连接
    }
    cout << endl;
    /*
    plt::xlabel("X-axis");
    plt::ylabel("Y-axis");
    plt::title("Piecewise Linear Plot");
    plt::show();
    plt::save("output.png");
    //int a;
    //cin >>a;*/
}

//only calculate the &stockout cost &detour & more capacity cost
double PLFunction::calculateCost_stockout(int day, int client, double detour, double replenishment, double freeload)
{
    // detour
    //replenishment = 1;freeload = 27;
    bool trace = false;
    double cost = detour;
    //stock-out cost
    if(trace) cout<<"day"<<day<<" client "<<client<<"  replenishment "<<replenishment<<"  freeload "<<freeload<<endl;
    
    cost +=  params->cli[client].stockoutCost  * (params->cli[client].dailyDemand[day] - replenishment);
    //cost-depot(holdingcost)
    cost -= params->inventoryCostSupplier * replenishment * (double)(params->ancienNbDays - day); 
    
    // possible excess capacity
    cost += params->penalityCapa * std::max(0.0, replenishment - freeload);
    if(trace)cout <<"cost = "<<cost<<endl;
    return cost;
}

//only calculate the hodling cost &detour & more capacity cost
double PLFunction:: calculateCost_holding(int day, int client, double detour, double replenishment, double freeload)
{
    bool trace = false;
    // detour
    double cost = detour;
    // holding cost
    if(trace && day == 0 ){cout<<"cdetour = "<<cost<<endl;}
    cost +=  params->cli[client].inventoryCost  * (replenishment - params->cli[client].dailyDemand[day] );
    //cost-depot(holdingcost)
    if(trace && day == 0 ){cout<<"cosy += "<<cost<<endl;}
    cost -= params->inventoryCostSupplier * replenishment * (double)(params->ancienNbDays - day); 
    if(trace && day == 0 ){cout<<"cosy -= "<<cost<<endl;}
    
    // possible excess capacity
    cost += params->penalityCapa * std::max(0.0, replenishment - freeload);
    if(trace && day == 0 ){
        cout<<"day "<<day<<"  detour "<<detour <<" repli " <<replenishment <<" free "<<freeload<<endl;
        cout<<" ---holding "<<params->cli[client].inventoryCost<<" "<< replenishment - params->cli[client].dailyDemand[day] <<endl;
        cout<<" ---depot " << params->inventoryCostSupplier<<" " << replenishment * (double)(params->ancienNbDays - day) <<endl;
        cout <<" ---capacity " << params->penalityCapa * std::max(0.0, replenishment - freeload)
        << " ---cost "<<cost<<endl;
       }
    return cost;
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
