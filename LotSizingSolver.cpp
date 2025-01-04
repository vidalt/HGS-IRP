//
// //
// Created by zjy on 13/05/2024 (SOA).
//
//

#include "LotSizingSolver.h"
#include <memory>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iomanip>
//配置一个PLFunction的向量F，并为其每个PLFunction中的每个片段设置相关属性。
LotSizingSolver::LotSizingSolver(Params *params, vector<vector<Insertion>> inst, int client)
    : params(params), insertions(inst), client(client)
{
  traces = false;

  horizon = (int)insertions.size();
  
  if(params-> isstockout){
    F1 = vector<std::shared_ptr<PLFunction>>(horizon);
    F2 = vector<std::shared_ptr<PLFunction>>(horizon);
    
    
  }
  else F = vector<std::shared_ptr<PLFunction>>(horizon);

  for (int t = 0; t < horizon; t++)
  {
    vector<Insertion> tmp = insertions[t]; // all possible place in day t

    
    if(params-> isstockout){
      if(traces) cout <<"F1 stockout"<<endl;
      F1[t] = std::make_shared<PLFunction>(params, tmp, t, client, true,params->cli[client].dailyDemand[t]); 
      if(traces)F1[t]->print();

      if(traces)cout <<"F2 holdingcost"<<endl;
      F2[t] = std::make_shared<PLFunction>(params, tmp, t, client, false); 
      if(traces)F2[t]->print();
      
    }
    else F[t] = std::make_shared<PLFunction>(params, tmp, t, client); 
    //该对象使用params, tmp, t 和 client作为构造函数的参数。这个新对象的智能指针存储在F的第t个位置。我们在t索引处存储了一个新创建的PLFunction对象的shared_ptr。

    //cout<<"stockout"<<params-> isstockout<<endl;

    if(params-> isstockout){
      for (int i = 0; i < F1[t]->nbPieces; i++){
          F1[t]->pieces[i]->fromF = F1[t]->pieces[i]->cloneWithout();
          F1[t]->pieces[i]->replenishment_loss = 0;
          F1[t]->pieces[i]->fromC = nullptr;
          F1[t]->pieces[i]->fromC_pre = nullptr;
          F1[t]->pieces[i]->fromInst = make_shared<Insertion>(
              F1[t]->pieces[i]->fromInst->detour, 
              F1[t]->pieces[i]->fromInst->load,
              F1[t]->pieces[i]->fromInst->place);
      }
      for (int i = 0; i < F2[t]->nbPieces; i++){
          F2[t]->pieces[i]->fromF = F2[t]->pieces[i]->cloneWithout(); 
          F2[t]->pieces[i]->replenishment_loss = -1;
          F2[t]->pieces[i]->fromC = nullptr;
          F2[t]->pieces[i]->fromC_pre = nullptr;
          F2[t]->pieces[i]->fromInst = make_shared<Insertion>(
              F2[t]->pieces[i]->fromInst->detour, 
              F2[t]->pieces[i]->fromInst->load,
              F2[t]->pieces[i]->fromInst->place);
         //if(t ==0){cout <<"F2[0]frominst   piece  "<<i <<"   : ";F2[0]->pieces[i]->fromInst->print();F2[0]->pieces[i]->print()}
      }
      
    }
    else{
      for (int i = 0; i < F[t]->nbPieces; i++)
      {
        F[t]->pieces[i]->fromF = F[t]->pieces[i]->clone();
        F[t]->pieces[i]->fromC_pre = nullptr;
        F[t]->pieces[i]->fromC = nullptr;
        F[t]->pieces[i]->fromInst = make_shared<Insertion>(
            F[t]->pieces[i]->fromInst->detour,
            F[t]->pieces[i]->fromInst->load,
            F[t]->pieces[i]->fromInst->place);
      }
    }

    if (traces)
    {
      //cout << "capacity penalty: " << params->penalityCapa << endl;
      
      cout << "f[" << t << "]: "<<endl;
      //F[t]->print();
      F1[t]->print();
      F2[t] -> print();
    }
  }
  
}

std::shared_ptr<PLFunction> LotSizingSolver::copyPLFunction(
    std::shared_ptr<PLFunction> source)
{
  std::shared_ptr<PLFunction> destination(std::make_shared<PLFunction>(params));
  destination->nbPieces = 0;
  destination->pieces = vector<shared_ptr<LinearPiece>>();
  for (int i = 0; i < source->nbPieces; i++)
  {
    destination->append(source->pieces[i]);
    //cout<<"!!"; 
    //if(source->pieces[i]->fromInst)source->pieces[i]->fromInst->print();
    //if(destination->pieces[0]->fromC_pre)destination->pieces[0]->fromC_pre->print();
  }

  if (source->pieceAt0 != nullptr)
  {
    destination->pieceAt0 = source->pieceAt0->clone();
  }
  destination->valueAt0 = source->valueAt0;

  return destination;
}

std::shared_ptr<PLFunction> LotSizingSolver::min(
    std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2)
{
  std::shared_ptr<PLFunction> f(std::make_shared<PLFunction>(params));
  f->pieceAt0 = f2->pieceAt0;
  f->valueAt0 = f2->valueAt0;

  if (f1->pieceAt0 != nullptr)
  {
    if (f2->pieceAt0 != nullptr)
    {
      if (le(f1->valueAt0, f2->valueAt0))
      {
        f->valueAt0 = f1->valueAt0;
        f->pieceAt0 = f1->pieceAt0;
      }
      else
      {
        f->valueAt0 = f2->valueAt0;
        f->pieceAt0 = f2->pieceAt0;
      }
    }
    else
    {
      f->valueAt0 = f1->valueAt0;
      f->pieceAt0 = f1->pieceAt0;
    }
  }
  if (f1->nbPieces == 0 && f2->nbPieces == 0)
    return f;
  if (f1->nbPieces == 0 && f2->nbPieces > 0)
  {
    f = copyPLFunction(f2);
  }
  else if (f1->nbPieces > 0 && f2->nbPieces == 0)
  {
    f = copyPLFunction(f1);
  }
  else
  {
    shared_ptr<LinearPiece> lp1 = f1->pieces[0];
    shared_ptr<LinearPiece> lp2 = f2->pieces[0];

    shared_ptr<LinearPiece> piece1;   // = new LinearPiece();
    shared_ptr<LinearPiece> piece2;   // = new LinearPiece();
    shared_ptr<LinearPiece> tmpPiece; // = new LinearPiece();

    double x, y;

    vector<double> breakpoints = getBreakpoints(f1, f2);

    int nbBreakPoints = breakpoints.size();
    for (int i = 1; i < nbBreakPoints; i++)
    {
      piece1 = nullptr;
      piece2 = nullptr;

      if (lp1 != nullptr)
        piece1 = lp1->getInBound(breakpoints[i - 1], breakpoints[i]);

      if (lp2 != nullptr)
      {
        piece2 = lp2->getInBound(breakpoints[i - 1], breakpoints[i]);
      }

      if (piece1 == nullptr && piece2 == nullptr)
        break;

      // if one of these pieces equals nullptr
      if (piece1 == nullptr)
      {
        f->append(piece2);
      }
      else if (piece2 == nullptr)
      {
        f->append(piece1);
      }
      else
      {
        // if two pieces intersect inside
        if (f->intersect(piece1, piece2, x, y))
        {
          // first piece
          if (le(piece1->p1->y, piece2->p1->y))
          {
            tmpPiece =
                make_shared<LinearPiece>(piece1->p1->x, piece1->p1->y, x, y);

            tmpPiece->fromC = piece1->fromC;
            tmpPiece->fromF = piece1->fromF;
            tmpPiece->fromInst = piece1->fromInst;
          }
          else
          {
            tmpPiece =
                make_shared<LinearPiece>(piece2->p1->x, piece2->p1->y, x, y);

            tmpPiece->fromC = piece2->fromC;
            tmpPiece->fromF = piece2->fromF;
            tmpPiece->fromInst = piece2->fromInst;
          }
          f->append(tmpPiece);

          // second piece
          if (le(piece1->p2->y, piece2->p2->y))
          {
            tmpPiece =
                make_shared<LinearPiece>(x, y, piece1->p2->x, piece1->p2->y);

            tmpPiece->fromC = piece1->fromC;
            tmpPiece->fromF = piece1->fromF;
            tmpPiece->fromInst = piece1->fromInst;
          }
          else
          {
            tmpPiece =
                make_shared<LinearPiece>(x, y, piece2->p2->x, piece2->p2->y);

            tmpPiece->fromC = piece2->fromC;
            tmpPiece->fromF = piece2->fromF;
            tmpPiece->fromInst = piece2->fromInst;
          }
          f->append(tmpPiece);
        }
        else
        {
          // get lower piece
          if (lt(piece1->p1->y, piece2->p1->y))
            f->append(piece1);
          else if (gt(piece1->p1->y, piece2->p1->y))
            f->append(piece2);
          else
          {
            if (le(piece1->p2->y, piece2->p2->y))
              f->append(piece1);
            else
              f->append(piece2);
          }
        }
      }

      // consider if get next piece
      if (lp1 != nullptr && eq(breakpoints[i], lp1->p2->x))
        lp1 = lp1->next;
      if (lp2 != nullptr && eq(breakpoints[i], lp2->p2->x))
        lp2 = lp2->next;

      if (lp1 == nullptr && lp2 == nullptr)
        break;
    }
  }

  return f;
}

void LotSizingSolver::extractBreakpoints(const std::shared_ptr<PLFunction>& f, vector<double>& breakpoints) {
    for (int i = 0; i < f->nbPieces; i++) {
        if( neq(f->pieces[i]->p1->x,f->pieces[i]->p2->x) &&
              ( i== 0 ||  neq(f->pieces[i]->p1->x,f->pieces[i-1]->p2->x) ) )
            breakpoints.push_back(f->pieces[i]->p1->x);
        breakpoints.push_back(f->pieces[i]->p2->x);
    }
}
void LotSizingSolver::extractRepeat(const std::shared_ptr<PLFunction>& f, vector<double>& breakpoints) {
    for (int i = 0; i < f->nbPieces; i++) {
        if(fabs(f->pieces[i]->p1->x-f->pieces[i]->p2->x)<0.00001)
          breakpoints.push_back(f->pieces[i]->p1->x);//cout <<"jia"<<f->pieces[i]->p1->x<<endl;}
    }
}



vector<double> LotSizingSolver::getBreakpoints_final(std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2){
    bool trace = false;
    vector<double> breakpoints;
    vector<double> repeat;
    extractBreakpoints(f1, breakpoints);
    extractBreakpoints(f2, breakpoints);
    extractRepeat(f1, repeat);
    extractRepeat(f2, repeat);
    std::sort(breakpoints.begin(), breakpoints.end());
    std::sort(repeat.begin(), repeat.end());
  auto it = std::unique(breakpoints.begin(), breakpoints.end(), 
                      [](double a, double b) { return fabs(a - b) < 0.00001; });
  breakpoints.erase(it, breakpoints.end());

  auto it1 = std::unique(repeat.begin(), repeat.end(), 
                        [](double a, double b) { return fabs(a - b) < 0.00001; });
  repeat.erase(it1, repeat.end());

     //breakpoints.erase(unique(breakpoints.begin(), breakpoints.end()), breakpoints.end());
    //repeat.erase(unique(repeat.begin(), repeat.end()), repeat.end());
    // 合并两个向量
    breakpoints.insert(breakpoints.end(), repeat.begin(), repeat.end());
    if(trace){
      cout<<"  breakpoint:   ";
      for(int i =0 ; i < breakpoints.size();i++)
        cout <<breakpoints[i]<<" ";
    }
    
    std::sort(breakpoints.begin(), breakpoints.end());
    return breakpoints;
}

std::shared_ptr<LinearPiece> LotSizingSolver::createPieceFromLowerY(
    std::shared_ptr<LinearPiece> &chosenPiece,
    double x1, double y1, double x2, double y2) 
{
    std::shared_ptr<LinearPiece> tmpPiece = std::make_shared<LinearPiece>(x1, y1, x2, y2);
    tmpPiece->fromC_pre = chosenPiece->fromC_pre;
    tmpPiece->fromC = chosenPiece->fromC;
    tmpPiece->fromF = chosenPiece->fromF;
    tmpPiece->fromInst = chosenPiece->fromInst;
    tmpPiece->replenishment_loss = chosenPiece->replenishment_loss;

    return tmpPiece;
}


std::shared_ptr<PLFunction> LotSizingSolver::min_final(std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2) {
  std::shared_ptr<PLFunction> f = std::make_shared<PLFunction>(params);
   bool trace=false;
  //if ( f1->nbPieces>0 && f2->nbPieces>1 && eq(f1->pieces[0]->p1->x,-85)&& eq(f1->pieces[0]->p2->x,-1)&& eq(f2->pieces[1]->p1->x,-1) && eq(f2->pieces[1]->p2->x,62)) trace = true;
    // if (!f1) exit(0);
    // if (!f2) exit(0);
    if (f1->nbPieces == 0 && f2->nbPieces == 0) return f;
    if (f1->nbPieces == 0 && f2->nbPieces > 0) return copyPLFunction(f2);
    if (f1->nbPieces > 0 && f2->nbPieces == 0) return copyPLFunction(f1);
    if(trace){cout <<" f1 ";f1->print(); cout <<" f2 ";f2->print();}

    vector<double> breakpoints = getBreakpoints_final(f1, f2);
    int nbBreakPoints = breakpoints.size();
    std::shared_ptr<LinearPiece> lp1 = f1->pieces[0];
    std::shared_ptr<LinearPiece> lp2 = f2->pieces[0];
    int n1=0,n2=0;
    for (int i = 1; i < nbBreakPoints; i++) {
        if(lp1){lp1->updateLinearPiece(lp1->p1->x,lp1->p1->y,lp1->p2->x,lp1->p2->y);}
        if(lp2)lp2->updateLinearPiece(lp2->p1->x,lp2->p1->y,lp2->p2->x,lp2->p2->y);
        //cout<<"BP: "<<breakpoints[i-1]<<"  "<<breakpoints[i]<<endl;
        std::shared_ptr<LinearPiece> piece1 = nullptr;
        
        if (lp1) piece1 = lp1->getInpiece(breakpoints[i - 1], breakpoints[i]); 
        
        std::shared_ptr<LinearPiece> piece2 = nullptr;
        if (lp2)  piece2 = lp2->getInpiece(breakpoints[i - 1], breakpoints[i]); 

        if(trace){cout<<"breakpoint: "<<breakpoints[i-1]<<" "<<breakpoints[i]<<endl;
        cout<<"after get in pircr: 1  ";
        if(piece1)piece1->print();if(lp1){cout<<lp1->slope<<"  ";lp1->print();}
        cout<<"after get in pircr: 2  ";
        if(piece2)piece2->print();if(lp2){cout<<lp2->slope<<"  ";lp2->print();}}
        

        if (!piece1 && !piece2);
        else if (!piece1)  f->append(piece2);
        else if (!piece2) f->append(piece1);
        else {
            double x, y;
            bool intersects = f->intersect(piece1, piece2, x, y);
            if (intersects) {
              if(trace)cout <<"x = "<<x<<" y = "<<y<<" ";
                std::shared_ptr<LinearPiece> firstPiece, secondPiece, chosenPiece;
                chosenPiece = ( piece1->p1->y <= piece2->p1->y) ? piece1 : piece2;
                firstPiece = createPieceFromLowerY(chosenPiece, chosenPiece->p1->x, chosenPiece->p1->y, x, y);
                f->append(firstPiece);

                chosenPiece =( (piece1->p2->y < piece2->p2->y) )? piece1 : piece2;
                secondPiece = createPieceFromLowerY(chosenPiece, x, y, chosenPiece->p2->x, chosenPiece->p2->y);
                //cout <<setprecision(15) << "~~~~~~~~~~~~~~~~~~~~~~~~~~x "<<x<<" slope "<<secondPiece->slope<<endl;
                //cout <<setprecision(15) <<" secondx1 "<<secondPiece->p1->x <<" secondy1 "<<secondPiece->p1->y<<x<<" secondx2 "<<secondPiece->p2->x<<" secondy2 "<<secondPiece->p2->y<<endl;
                
                f->append(secondPiece);
            } 
            else if ( eq(piece1->p1->x ,x) ){
                if(piece1->p2->y < piece2->p2->y) 
                    f->append(piece1);
                 else 
                    f->append(piece2);
            }
            else if ( eq(piece1->p2->x ,x) ){
                if(piece1->p1->y <piece2->p1->y) 
                    f->append(piece1);
                 else 
                    f->append(piece2);
            }
            else {
              if ( (piece1->p1->y -piece2->p1->y) + (piece1->p2->y -piece2->p2->y) < 0) 
                    f->append(piece1);
                else 
                    f->append(piece2);
                
            }
           
        }
        if(trace){cout <<"f  ";f->print();}
        if (lp1&& fabs(breakpoints[i]- lp1->p2->x)<0.00001 ){
            if(n1 + 1 < f1->nbPieces) lp1 = f1->pieces[++n1];
            else lp1 = nullptr;
          //cout<<"lp1"<<endl;
          //if(lp1)lp1->print();
        }
        if (lp2&& fabs(breakpoints[i]- lp2->p2->x)<0.00001 ){
          if(n2 + 1 < f2->nbPieces) lp2 = f2->pieces[++n2];
          else lp2 = nullptr;
          //cout<<"lp2"<<endl;
          //if(lp2)lp2->print();
        }

        if (!lp1 && !lp2) break;
    }
    
    return f;
}





std::shared_ptr<PLFunction> LotSizingSolver::supperpositionl(
    std::shared_ptr<LinearPiece> fromPieceC,
    std::shared_ptr<LinearPiece> fromPieceF)
{
  bool traces = false;
  bool trace = false;
  std::shared_ptr<PLFunction> f(std::make_shared<PLFunction>(params));
  vector<Point> points;
  shared_ptr<LinearPiece> tmpPiece,newpiece;
  points.push_back(fromPieceC->p1->convolve(fromPieceF->p2));  
  points.push_back(fromPieceC->p1->convolve(fromPieceF->p1));  
  points.push_back(fromPieceC->p2->convolve(fromPieceF->p1));
  points.push_back(fromPieceC->p2->convolve(fromPieceF->p2));
   
  std::vector<Point> sortedPoints = points; 
  std::sort(sortedPoints.begin(), sortedPoints.end(), 
    [](const Point& a, const Point& b) { return lt(a.y, b.y); });
  Point minYPoint1 = sortedPoints[0];
  Point minYPoint2 = sortedPoints[1]; Point minYPoint3 = sortedPoints[2];Point minYPoint4 = sortedPoints[3];

 // if (eq(fromPieceC->p1->x,-77) &&  eq(fromPieceC->p1->y, 27.72)&& eq(fromPieceC->p2->x, -77) && eq(fromPieceC->p2->y, 27.72) && eq(fromPieceF->p1->x,21.798404) &&  eq(fromPieceF->p1->y,4173.1145)&& eq(fromPieceF->p2->x, 77) && eq(fromPieceF->p2->y, 132.35765))trace = true;
      
  //if (eq(fromPieceC->p1->x,-91) &&  eq(fromPieceC->p1->y, -128.51 )&& eq(fromPieceC->p2->x, 0) && eq(fromPieceC->p2->y,  -121.23) && eq(fromPieceF->p1->x,159) &&  eq(fromPieceF->p1->y, -50.08)&& eq(fromPieceF->p2->x, 182) && eq(fromPieceF->p2->y,  -50.082692))trace = true;
  if(trace) cout <<" from [or 1] "<<minYPoint1.x<<" "<<minYPoint1.y<<endl;
  if(trace) cout <<" from [or 2] "<<minYPoint2.x<<" "<<minYPoint2.y<<"  same? "<<eq(minYPoint1.x,minYPoint2.x) <<" "<< eq(minYPoint1.y,minYPoint2.y)<<endl;
  
  if(eq(minYPoint1.x,minYPoint2.x) && eq(minYPoint1.y,minYPoint2.y)){
    minYPoint2 = sortedPoints[2];
    if(gt(minYPoint1.x,minYPoint2.x) )std::swap(minYPoint1,minYPoint2);

    tmpPiece = make_shared<LinearPiece>(minYPoint1.x, minYPoint1.y,minYPoint2.x, minYPoint2.y);
    f->append(tmpPiece); 
    f->pieces[0]->fromC = fromPieceC->clone();
    f->pieces[0]->fromF = fromPieceF->clone();
    f->pieces[0]->fromInst = fromPieceF->fromInst; 
    return f;
  }

 // double parallel_x = (minYPoint1.y-minYPoint2.y)/(minYPoint1.x-minYPoint2.x);
 // if(eq(parallel_x ,0) &&gt(minYPoint1.x,minYPoint2.x) )std::swap(minYPoint1,minYPoint2);
  int minYIndex1 = std::find(points.begin(), points.end(), minYPoint1) - points.begin();
  int minYIndex2 = std::find(points.begin(), points.end(), minYPoint2) - points.begin();
  int next1 = (minYIndex1 + 1) % 4,next2 = (minYIndex1+3)%4;
  if(trace){
      cout <<"minYPoint1 "<<minYPoint1.x<<" "<<minYPoint1.y<<endl;
      cout <<"next1 "<<points[next1].x<<" "<<points[next1].y<<endl;
      cout <<"next2 "<<points[next2].x<<" "<<points[next2].y<<endl;
  }


  double slope1 = (eq(points[next1].x, points[minYIndex1].x )||   eq(points[next1].y, points[minYIndex1].y ) )
                    ? 0 : (points[next1].y- points[minYIndex1].y )/(points[next1].x- points[minYIndex1].x );
  double slope2 = ( eq(points[next2].x, points[minYIndex1].x )||  eq(points[next2].y, points[minYIndex1].y ))
                    ? 0 : (points[next2].y- points[minYIndex1].y )/(points[next2].x- points[minYIndex1].x );
  if(eq(slope1, 0) || eq(slope2 ,0)){//one 0
    if(minYPoint1.x>minYPoint2.x )std::swap(minYIndex1,minYIndex2);
    next1 = (minYIndex1 + 1) % 4; next2 = (minYIndex1+3)%4;
    if(trace) cout <<"slope 0 ";
    int rightup = 6-minYIndex1-next1-next2;
    int leftup = 6-minYIndex1-minYIndex2-rightup;

    if(lt(points[rightup].x,points[minYIndex2].x)){  
        if(trace) cout <<"leftup "<<points[leftup].x<<" "<<points[leftup].y<<" "<<points[minYIndex1].x<< " "<<points[minYIndex1].y<<endl;
        tmpPiece = make_shared<LinearPiece>(points[leftup].x, points[leftup].y,points[minYIndex1].x, points[minYIndex1].y); 
        newpiece = make_shared<LinearPiece>(points[minYIndex1].x, points[minYIndex1].y,  points[minYIndex2].x, points[minYIndex2].y);
    }
    else{
      //if(trace) cout <<"!!!!!!!!!!!!!!!!!!!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~";
        tmpPiece = make_shared<LinearPiece>(points[minYIndex1].x, points[minYIndex1].y,  points[minYIndex2].x, points[minYIndex2].y); 
        newpiece = make_shared<LinearPiece>(points[minYIndex2].x, points[minYIndex2].y,points[rightup].x, points[rightup].y);
        if(trace) {tmpPiece->print();newpiece->print();}
    }
  }  
  else if (lt(slope1,0) && lt(0,slope2) || lt(slope2,0) && lt(0,slope1)){//one negative one postive
    if(trace) cout <<"one negative one postive ";
    if(points[next1].x>points[next2].x)std::swap(next1,next2);
    tmpPiece = make_shared<LinearPiece>(points[next1].x, points[next1].y,points[minYIndex1].x, points[minYIndex1].y); 
    newpiece = make_shared<LinearPiece>(points[minYIndex1].x, points[minYIndex1].y,  points[next2].x, points[next2].y);
    
  }
  else if(lt(slope1,0)){//both negative
    if(trace) cout <<"negative: ";
    if(eq(minYPoint2.x,minYPoint3.x) && eq(minYPoint2.y,minYPoint3.y) ){
      if(trace) cout <<"!!!~~line~~~";
      tmpPiece = make_shared<LinearPiece>(minYPoint4.x, minYPoint4.y,minYPoint2.x, minYPoint2.y); 
      newpiece = make_shared<LinearPiece>(minYPoint2.x, minYPoint2.y,minYPoint1.x, minYPoint1.y);  
    }
    else{
      int tmp1 = gt(slope1,slope2)?next1:next2;
      int tmp2 = 6-minYIndex1-next1-next2;
      if(trace) cout<<"both negative ("<<points[tmp1].x<<" "<<points[tmp1].y<<" )"<<"  ( "<<points[tmp2].x<<" "<<points[tmp2].y<<" )"<<endl;
      tmpPiece = make_shared<LinearPiece>(points[tmp2].x, points[tmp2].y,points[tmp1].x, points[tmp1].y); 
      newpiece = make_shared<LinearPiece>(points[tmp1].x, points[tmp1].y,points[minYIndex1].x, points[minYIndex1].y);  
    }
  }
  else{//both positive
    if(trace) cout <<"!!! positive: ";
    if(eq(minYPoint2.x,minYPoint3.x) && eq(minYPoint2.y,minYPoint3.y) ){//line
    if(trace) cout <<"!!!~~line~~~";
      tmpPiece = make_shared<LinearPiece>(minYPoint1.x, minYPoint1.y,minYPoint2.x, minYPoint2.y); 
      newpiece = make_shared<LinearPiece>(minYPoint2.x, minYPoint2.y,minYPoint4.x, minYPoint4.y);  
    }
    else{
      int tmp1 = gt(slope1,slope2)?next2:next1;
      int tmp2 = 6-minYIndex1-next1-next2;
      if(trace) cout<<"both positive("<<points[tmp1].x<<" "<<points[tmp1].y<<" )"<<"  ( "<<points[tmp2].x<<" "<<points[tmp2].y<<" )"<<endl;
     
      tmpPiece = make_shared<LinearPiece>(points[minYIndex1].x, points[minYIndex1].y,points[tmp1].x, points[tmp1].y); 
      newpiece = make_shared<LinearPiece>(points[tmp1].x, points[tmp1].y,points[tmp2].x, points[tmp2].y);  
    }
  }
  f->append(tmpPiece);
  f->append(newpiece);
  for(int i = 0 ; i < f->nbPieces ; i++){
    f->pieces[i]->fromC = fromPieceC->clone();
    f->pieces[i]->fromF = fromPieceF->clone();
    f->pieces[i]->fromInst = fromPieceF->fromInst; 
  }
  if(trace) {cout <<"            after supplerl : "; f->print();}
  return f;
}

std::shared_ptr<PLFunction> LotSizingSolver:: supperposition(
    std::shared_ptr<PLFunction> fromC, std::shared_ptr<PLFunction> fromF)
{
  bool trace=false;
 std::shared_ptr<PLFunction> f(std::make_shared<PLFunction>(params));
  //    std::shared_ptr<PLFunction> fmin(std::make_shared<PLFunction>(params));

  for (int i = 0; i < fromC->nbPieces; i++){
    for (int j = 0; j < fromF->nbPieces; j++){
      
      std::shared_ptr<PLFunction> tmpF = std::make_shared<PLFunction>(params);
      std::shared_ptr<LinearPiece> fromPieceC = fromC->pieces[i]->clone();
      
      std::shared_ptr<LinearPiece> fromPieceF = fromF->pieces[j]->clone();
     //if (eq(fromPieceC->p1->x,-85)&& eq(fromPieceC->p2->x, -22)&& eq(fromPieceF->p1->x, 0) && eq(fromPieceF->p2->x, 84))
     // {trace = true;cout<<"??";}

      if(trace) {cout <<"cccccc";fromPieceC ->print();cout<<"fffff";fromPieceF->print();}
      tmpF = supperpositionl(fromPieceC, fromPieceF);
      
     
      if(trace) {
        //cout <<" ~     tmpc: ";fromPieceC->print();
        //cout <<"  ~    tmpf: ";fromPieceF->print();
        cout <<"   ~   tmpf from supperpisition (fromPieceC, fromPieceF): ";tmpF->print();}
      if (f->nbPieces > 0 || tmpF->nbPieces > 0){
        if(trace){cout<<"before min_final  ";f->print();}
        std::shared_ptr<PLFunction> fmin;
        fmin = min_final(f, tmpF);
        if(trace){cout <<"after final ";fmin->print();}
        f = copyPLFunction(fmin);
        if(trace) {cout <<"after copy ";f->print();}
        //if(eq(fromPieceC->p1->x,-69) && eq(fromPieceC->p2->x, -69))
        //  {cout <<"      f: ";f->print();}
      }

      if(trace){cout<<"after f  ";f->print(); }
      trace = false;
      tmpF.reset();
      fromPieceC.reset();
      fromPieceF.reset();
    }
   
  }
  
  return f;
}

vector<double> LotSizingSolver::getBreakpoints(std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2){
  // get breakpoints of pieces
  vector<double> breakpoints = vector<double>();
  // from f1
  for (int i = 0; i < f1->nbPieces; i++)
  { 
    if(i==0)  breakpoints.push_back(f1->pieces[i]->p1->x);
    breakpoints.push_back(f1->pieces[i]->p2->x);
  }
  // from f2
  for (int i = 0; i < f2->nbPieces; i++)
  {
    if(i==0) breakpoints.push_back(f2->pieces[i]->p1->x);
    breakpoints.push_back(f2->pieces[i]->p2->x);
  }

  // sort
  std::sort(breakpoints.begin(), breakpoints.end());
  //    breakpoints.erase(unique(breakpoints.begin(), breakpoints.end()),
  //                      breakpoints.end());
  
  vector<double> newbreakpoints = vector<double>();
  newbreakpoints.push_back(breakpoints[0]);
  int k = 0;
  int n = breakpoints.size();

  for (int i = 1; i < n; i++)
  {
    if (lt(newbreakpoints[k], breakpoints[i]))
    {
      newbreakpoints.push_back(breakpoints[i]);
      k += 1;
    }
  }

  return newbreakpoints;
}



bool LotSizingSolver::backtracking()
{
  // initialization
  for (int i = 0; i < horizon; i++)
  {
    quantities[i] = 0.0;
    breakpoints[i] = nullptr;
    I[i] = 0;
  }
  // backtracking
  int day = horizon - 1;

  if (C[day]->nbPieces == 0 && C[day]->pieceAt0 == nullptr)
    return false;

  std::shared_ptr<LinearPiece> tmp = C[day]->getMinimalPiece(client, I[day], objective);

  while (tmp != nullptr)
  {
    if (day == 0 && (tmp->fromC != nullptr || tmp->fromF == nullptr))
      break;

    if (tmp->fromF == nullptr && tmp->fromC == nullptr)
      break;

    if (tmp->cost(I[day]) == 0 && tmp->fromC == nullptr)
      break;

    // if do not delivery any thing, then inventory at the end of previous day
    // equals this day demand
    if (tmp->fromF == nullptr)
    {
      I[day - 1] = I[day] + params->cli[client].dailyDemand[day];

      if (tmp->fromC != nullptr)
        tmp = tmp->fromC;
      else
        tmp = nullptr;
    }
    else if (tmp->fromC == nullptr)
    {
     
      shared_ptr<LinearPiece> tmpF = tmp->fromF;
      while (tmpF->fromF != nullptr)
        tmpF = tmpF->fromF;
      breakpoints[day] = tmpF->fromInst;

      if (params->cli[client].startingInventory >= (day + 1) * params->cli[client].dailyDemand[day])
      {
        quantities[day] = I[day] +
                          (day + 1) * params->cli[client].dailyDemand[day] -
                          params->cli[client].startingInventory;
      }
      else
      {
        quantities[day] = I[day] + params->cli[client].dailyDemand[day];
      }

      if (tmp->fromC != nullptr)
        tmp = tmp->fromC;
      else
        tmp = nullptr;
    }
    else
    {
      std::shared_ptr<LinearPiece> fromC = tmp->fromC->clone();
      std::shared_ptr<LinearPiece> fromF = tmp->fromF->clone();
      //            while (fromF->fromF != nullptr) fromF = fromF->fromF;

      fromC->p1->x += params->cli[client].dailyDemand[day];
      fromC->p2->x += params->cli[client].dailyDemand[day];

      bool ok = false;
      if ((exceptionalPieces[day] != nullptr) && fromC->eqlp(exceptionalPieces[day]))
      {
        double q = I[day] + params->cli[client].dailyDemand[day];
        double fromI = 0;

        if (eq(tmp->cost(I[day]), fromC->cost(fromI) + fromF->cost(q)))
        {
          I[day - 1] = 0;
          quantities[day] = q;
          ok = true;
        }
        else if (eq(tmp->cost(I[day]), fromC->cost(q) + fromF->cost(0)))
        {
          quantities[day] = 0;
          I[day - 1] = q;
          ok = true;
        }
      }

      if (ok)
      {
        tmp = exceptionalPieces[day];
      }
      else
      {
        solveEquationSystem(tmp, fromC, fromF, I[day],
                            params->cli[client].dailyDemand[day], I[day - 1],
                            quantities[day]);

        tmp = fromC;
      }

      //            Fs[day] = tmp->fromF;

      breakpoints[day] = fromF->fromInst;
    }

    day = day - 1;
  }

  return true;
}
void LotSizingSolver::solveEquationSystem(std::shared_ptr<LinearPiece> C,
                                          std::shared_ptr<LinearPiece> fromC,
                                          std::shared_ptr<LinearPiece> fromF,
                                          double I, double demand,
                                          double &fromI, double &quantity)
{
  double slopeC = (fromC->p2->y - fromC->p1->y) / (fromC->p2->x - fromC->p1->x);
  double slopeF = (fromF->p2->y - fromF->p1->y) / (fromF->p2->x - fromF->p1->x);

  if (eq(slopeC, slopeF))
  {
    if (lt(fromF->p2->x, demand))
    {
      quantity = demand;
    }
    else
    {
      quantity = fromF->p2->x;
    }
    fromI = I + demand - quantity;
  }
  else
  {
    double x1 = fromC->p2->x;
    double y1 = fromC->p2->y;

    double x2 = fromF->p2->x;
    double y2 = fromF->p2->y;

    double numerator = C->cost(I) - y1 - y2;
    numerator -= slopeC * (I + demand - x1);
    numerator += slopeF * x2;

    quantity = round(numerator / (slopeF - slopeC));
    fromI = I + demand - quantity;
  }
}

void LotSizingSolver::    solveEquationSystem_holding(std::shared_ptr<LinearPiece> C,
                                          std::shared_ptr<LinearPiece> fromC,
                                          std::shared_ptr<LinearPiece> fromF,
                                          double I, double demand,
                                          double &fromI, double &quantity,std::shared_ptr<LinearPiece> Cpre,double hodling){
  bool trace = false;
  //if (eq(fromC->p1->x,-94) &&  eq(fromC->p1->y, 18.544202)&& eq(fromC->p2->x, -76.520924) && eq(fromC->p2->y, 143.25741) && eq(fromF->p1->x,182) &&  eq(fromF->p1->y,  -17.822211)&& eq(fromF->p2->x, 188) && eq(fromF->p2->y,  24.987719))trace = true;
  
  //Ct[0] = [0,y,1e-8,y]
  if(eq(fromC->p1->x,fromC->p2->x )){
    fromI = round(fromC->p2->x + demand);
    quantity = round(I + demand - fromI);
   if(trace)cout<<"0 quantity : "<<quantity <<"  fromI : "<<fromI<<endl;
    return;
  }

  double slopeC = (fromC->p2->y - fromC->p1->y) / (fromC->p2->x - fromC->p1->x);
  double slopeF = (fromF->p2->y - fromF->p1->y) / (fromF->p2->x - fromF->p1->x);
  
  if(eq(slopeC,slopeF)){
    double upperbound = std::min<double>(I-fromC->p1->x,fromF->p2->x);
    double lowerbound = std::max<double>(I-fromC->p2->x,fromF->p1->x);
    //quantity = upperbound 到lowerbound之间的 iterger;
    if (ge(upperbound ,lowerbound) ){
        quantity = std::floor(upperbound);
        fromI = round(I + demand - quantity);
        if(trace)cout<<"01 quantity : "<<quantity <<"  fromI : "<<fromI<<endl;
    }else if(fabs(upperbound -lowerbound)<0.01){
       quantity = std::round(upperbound);
        fromI = round(I + demand - quantity);
        if(trace)cout<<"02 quantity : "<<quantity <<"  fromI : "<<fromI<<endl;
    }
    else {
        cout <<"I: "<<I<<" fromC: ";fromC->print();
        cout <<"fromF: ";fromF->print();
        cout <<"customer "<<client<<" ";
        std::cout << "No integers between upperbound and lowerbound." << upperbound<<" "<<lowerbound<<std::endl;int a;cin>>a;
    }
    return;
  } 
  slopeC*=10000;slopeF*=10000;
  if(trace) cout <<"slopeC "<<slopeC<<"  slopeF "<<slopeF<<"  cost: "<<C->cost(std::max<double>(0,I))<<endl;
  double x1 = fromC->p2->x, y1 = fromC->p2->y,x2 = fromF->p2->x,y2 = fromF->p2->y;
  double numerator = C->cost(std::max<double>(0,I)) *10000- y1*10000 - y2*10000;
   if(trace) cout <<"-y1-y2 : "<<y1<<"  "<<y2<<" "<<numerator<<endl;
    numerator -= slopeC * (I  - x1);
    numerator += slopeF * x2;

    if(trace) cout <<"lhs : "<<numerator<<endl;
    quantity = round(numerator / (slopeF - slopeC));
    double left = round(std::max<double>(I -fromC->p2->x,fromF->p1->x));
    double right = round(std::min<double>(I -fromC->p1->x,fromF->p2->x));
   
   if(gt( quantity ,right )||lt(quantity,left)  ){
      //cout <<"quantity when backtracing got problem holding"<<endl;
      //fromC->print();
      //Cpre->print();
      //fromF->print();
      //cout <<"client "<<client<<endl;
      //cout <<" quantity" << quantity<<"  left "<<left<<" right "<<right<<endl;
      if (gt(quantity ,right) )  quantity = right;
      if (lt(quantity,left) )  quantity = left;
     // int a;cin>>a;
   }
    
     fromI = round(I + demand - quantity);
    if(trace) cout <<"quan "<<quantity<<endl;
    if(trace)C->print();
    if(trace)fromC->print();
    if(trace)fromF->print();
    if(trace)cout<< "i " <<I <<" "<<C->cost(I)<<"  "<<y1<<"  "<<y2<<endl;
    if(trace)cout<<"quantity : "<<quantity <<"  fromI : "<<fromI<<endl;
   
}

void LotSizingSolver::solveEquationSystem_stockout(std::shared_ptr<LinearPiece> C,
                                          std::shared_ptr<LinearPiece> fromC,
                                          std::shared_ptr<LinearPiece> fromF,
                                          double I, double demand,
                                          double &fromI, double &quantity,std::shared_ptr<LinearPiece> Cpre,double stockout){
  bool trace = false;
  //Ct[0] = [0,y,1e-8,y]
  if(eq(fromC->p1->x,fromC->p2->x )){
    fromI = round(fromC->p2->x + demand);
    quantity = round(I + demand - fromI);
   if(trace)cout<<"0 quantity : "<<quantity <<"  fromI : "<<fromI<<endl;
    return;
  }

  
  double slopeC = (fromC->p2->y - fromC->p1->y) / (fromC->p2->x - fromC->p1->x);
  double slopeF = (fromF->p2->y - fromF->p1->y) / (fromF->p2->x - fromF->p1->x);
  if(trace) cout <<"slopeC "<<slopeC<<"  slopeF "<<slopeF<<"  cost: "<<C->cost(std::max<double>(0,I))<<endl;
  if(eq(slopeC,slopeF)){
    double upperbound = std::min<double>(I-fromC->p1->x,fromF->p2->x);
    double lowerbound = std::max<double>(I-fromC->p2->x,fromF->p1->x);
    //quantity = upperbound 到lowerbound之间的 iterger;
    if (ge(upperbound ,lowerbound) ){
        quantity = std::floor(upperbound);
        fromI = round(I + demand - quantity);
    }else {
        std::cout << "No integers between upperbound and lowerbound:" << upperbound <<"  "<<lowerbound<<std::endl;int a;cin>>a;
    }
    return;
  }

   slopeC*=10000;slopeF*=10000;
  double x1 = fromC->p2->x, y1 = fromC->p2->y,x2 = fromF->p2->x,y2 = fromF->p2->y;
  double numerator = C->cost(std::max<double>(0,I)) *10000- y1*10000 - y2*10000;
   if(trace) cout <<"-y1-y2 : "<<y1<<"  "<<y2<<" "<<numerator<<endl;

    numerator -= slopeC * (I  - x1);
    numerator += slopeF * x2;
  if(trace) cout <<"lhs : "<<numerator<<endl;


    quantity = round(numerator / (slopeF - slopeC));
    double left = round(std::max<double>(I -fromC->p2->x,fromF->p1->x));
    double right = round(std::min<double>(I -fromC->p1->x,fromF->p2->x));
   if(gt( quantity ,right )||lt(quantity,left)  ){
      //cout <<"quantity when backtracing got problem stockout"<<endl;
      //fromC->print();
      //Cpre->print();
      //fromF->print();
      //cout <<" quantity" << quantity<<"  left "<<left<<" right "<<right<<endl;
      if (gt(quantity ,right) )  quantity = right;
      if (lt(quantity,left) )  quantity = left;
     // int a;cin>>a;
   }
    fromI = round(I + demand - quantity);
    if(trace) cout <<"quan "<<quantity<<endl;
    if(trace)C->print();
    if(trace)fromC->print();
    if(trace)fromF->print();
    if(trace)cout<< "i " <<I <<" "<<C->cost(0)<<"  "<<y1<<"  "<<y2<<endl;
    if(trace)cout<<"quantity : "<<quantity <<"  fromI : "<<fromI<<endl;
   
}


bool LotSizingSolver::solve()
{
  traces = false;
  if (traces)
  {
    cout << "initial inventory: " << params->cli[client].startingInventory
         << endl;
    cout << "daily demand: " << params->cli[client].dailyDemand[0] << endl;
    cout << "inventory cost: " << params->cli[client].inventoryCost << endl;
    cout << "supplier inventory cost: " << params->inventoryCostSupplier
         << endl;
    cout << "max inventory: " << params->cli[client].maxInventory << endl;
    cout << "min inventory: " << params->cli[client].minInventory << endl;
  }

  double minInventory, maxInventory;

  C = vector<std::shared_ptr<PLFunction>>(horizon);
  //我们创建了一个大小为 horizon 的 vector，其中每个元素都是一个指向PLFunction (picewiselienar, variable: I^t) 对象

  I = vector<double>(horizon);
  // final inventory at the end of each period

  quantities = vector<double>(horizon);
  // replinishment at each period

  breakpoints = vector<std::shared_ptr<Insertion>>(horizon);
//我们创建了一个大小为 horizon 的 vector，其中每个元素都是一个指向 Insertion (detour load place) 对象的 shared_ptr。

  exceptionalPieces = vector<std::shared_ptr<LinearPiece>>(horizon);
  //我们创建了一个大小为 horizon 的 vector，其中每个元素都是一个指向 LinearPiece (slope, p1,p2) 对象

  for (int i = 0; i < horizon; i++)
  {
    exceptionalPieces[i] = std::make_shared<LinearPiece>(); //std::make_shared 可以提高性能和内存利用率，因为它可以将对象和共享指针的控制块（包含引用计数等信息的数据结构）一起分配在一块连续的内存中，从而减少了内存分配的次数和额外的开销
    C[i] = std::make_shared<PLFunction>(params); // a piecewise linear function in each period
  }

  // for the first day
  C[0] = F[0]->getInBound(0, params->cli[client].maxInventory - params->cli[client].startingInventory, true);
  C[0]->shiftLeft(params->cli[client].dailyDemand[0] -  params->cli[client].startingInventory);

  for (int i = 0; i < C[0]->nbPieces; i++)
  {
    C[0]->pieces[i]->fromC = nullptr;
    C[0]->pieces[i]->fromF = F[0]->pieces[i];
  }
  C[0] = C[0]->getInBound(params->cli[client].minInventory, params->cli[client].maxInventory, true);

  if (C[0]->pieceAt0 != nullptr)
    exceptionalPieces[0] = C[0]->pieceAt0->clone();

  double I = params->cli[client].startingInventory -
             params->cli[client].dailyDemand[0];
  if (I >= 0)
  {
    // do not need to delivery: q==0
    C[0]->minValue = 0;
  }


  for (int t = 1; t < horizon; t++)
  {
    if (traces)
      cout << endl
           << "day: " << t << endl;
    
    //picewise linear functions
    std::shared_ptr<PLFunction> f1;
    std::shared_ptr<PLFunction> f2; //(std::make_shared<PLFunction>(params));
    std::shared_ptr<PLFunction> f3;
    std::shared_ptr<PLFunction>  fromF; //(std::make_shared<PLFunction>(params));
    std::shared_ptr<PLFunction>  fromC; //(std::make_shared<PLFunction>(params));
    minInventory = params->cli[client].minInventory;
    maxInventory = params->cli[client].maxInventory - params->cli[client].dailyDemand[t];

    // calculate f_1
    //        tmpF1->clear();
    f1 = F[t]->getInBound(0, params->cli[client].maxInventory - (params->cli[client].startingInventory - t * params->cli[client].dailyDemand[t]),
                          true);
    for (int i = 0; i < f1->nbPieces; i++)
    {
      
      f1->pieces[i]->fromF = make_shared<LinearPiece>(
                              f1->pieces[i]->p1->x, f1->pieces[i]->p1->y,
                              f1->pieces[i]->p2->x, f1->pieces[i]->p2->y);
      f1->pieces[i]->fromF->fromInst = f1->pieces[i]->fromInst;
    }

    f1->shiftLeft(  (t + 1) * params-> cli[client].dailyDemand[t] -  params->cli[client].startingInventory  );

    f1 = f1->getInBound(minInventory, maxInventory, true); // min = 0;
    
    // calculate f_2
    //            PLFunction *f2 = C->clone();
    f2 = copyPLFunction(C[t - 1]);
    for (int i = 0; i < f2->nbPieces; i++)
    {
      f2->pieces[i]->fromC = C[t - 1]->pieces[i]->clone();
      f2->pieces[i]->fromF = nullptr;
    }

    f2->shiftLeft(params->cli[client].dailyDemand[t]);

    f2 = f2->getInBound(minInventory, maxInventory, true);

    // calculate f_3syste
    f3 = std::make_shared<PLFunction>(params);

    fromF = copyPLFunction(F[t]);
    fromC = copyPLFunction(C[t - 1]);

    fromC->shiftLeft(params->cli[client].dailyDemand[t]);

    f3 = supperposition(fromC, fromF);

    f3 = f3->getInBound(minInventory, maxInventory, true);

    if (traces)
    {
      cout << "fromC: ";
      fromC->print();
      cout << "fromF: ";
      fromF->print();
      cout << "fromC + fromF: ";
      f3->print();
    }

    // C is not smooth at point of minInventory
    if (C[t - 1]->pieceAt0 != nullptr && C[t - 1]->valueAt0 != 0)
    {
      //            shared_ptr<LinearPiece> tmpPiece = C[t-1]->pieceAt0->clone();
      exceptionalPieces[t] = C[t - 1]->pieceAt0->clone();

      std::shared_ptr<PLFunction> tmpf = copyPLFunction(fromF);
      tmpf->shiftLeft(params->cli[client].dailyDemand[t]);

      for (int i = 0; i < tmpf->nbPieces; i++)
      {
        shared_ptr<LinearPiece> fromPieceC = C[t - 1]->pieceAt0->clone();
        fromPieceC->p1->x -= params->cli[client].dailyDemand[t];
        fromPieceC->p2->x -= params->cli[client].dailyDemand[t];

        tmpf->pieces[i]->fromC = fromPieceC;
        tmpf->pieces[i]->fromF = fromF->pieces[i];
      }

      tmpf->moveUp(C[t - 1]->valueAt0);
      tmpf = tmpf->getInBound(minInventory, maxInventory, true);

      if (traces)
      {
        cout << "tmpf: ";
        tmpf->print();
        cout << "f3: ";
        f3->print();
      }
      f3 = min(f3, tmpf);

      if (traces)
      {
        cout << "f3: ";
        f3->print();
      }
    }

    if (traces)
    {
      cout << "f1: ";
      f1->print();
      cout << "f2: ";
      f2->print();
      cout << "f3: ";
      f3->print();
    }

    if (f2->nbPieces > 0 || f3->nbPieces > 0)
    {
      C[t]->clear();
      C[t] = min(f2, f3); // *ft = ft->min(f2, f3);
    }

    if ((C[t]->pieceAt0) ||
        (f2->pieceAt0 && lt(f2->valueAt0, C[t]->valueAt0)))
    {
      C[t]->valueAt0 = f2->valueAt0;
      C[t]->pieceAt0 = f2->pieceAt0;
    }

    if (traces)
    {
      cout << "ft = min(f2, f3): ";
      C[t]->print();
    }

    if (I >= 0)
    {
      std::shared_ptr<PLFunction> tmpF = std::make_shared<PLFunction>(params);
      tmpF = min(C[t], f1);
      C[t] = copyPLFunction(tmpF);

      if (lt(f1->valueAt0, C[t]->valueAt0))
      {
        C[t]->valueAt0 = f1->valueAt0;
        C[t]->pieceAt0 = f1->pieceAt0;
      }
      if (traces)
      {
        cout << "ft=min(f1,f2,f3): ";
        C[t]->print();
      }
    }

    C[t] = C[t]->getInBound(minInventory, maxInventory, false);

    if (traces)
    {
      cout << "C[I," << t << "]: ";
      C[t]->print();
    }

    // 4. if q(t-1) = q(t) = 0 then C(t)=0
    I = I - params->cli[client].dailyDemand[t];
  }
  // get solution
  bool ok = backtracking();

  if (!ok)
  {
    //        cout << "ERROR: Backtracking result" << endl;
    return false;
  }

  if (traces)
  {
    cout << "back done, Objective:  " << objective << endl;
  }
  objective = 0.;
  for (int i = 0; i < horizon; i++)
  {
    if (quantities[i] > 0)
      objective += F[i]->cost(quantities[i]);
  }

  if (traces){
    cout << "objective: " << objective << endl;
    for (int t = 0; t < horizon; t++){
      if (quantities[t] > 0){
        cout << "day " << t + 1 << ": ";
        cout << "quantity: " << quantities[t];
        cout << " cost: " << F[t]->cost(quantities[t]);
        cout << endl;
      }
    }
  }
  // free memory
  //int a;
  //cin >>a;
  return true;
}




bool LotSizingSolver::backtracking_stockout(){
  // initialization
  bool traces = false;
  for (int i = 0; i < horizon; i++){
    quantities[i] = 0.0;
    breakpoints[i] = nullptr;
    I[i] = 0;
  }
  // backtracking
  int day = horizon - 1;
  
  if (C[day]->nbPieces == 0)
    return false;
  std::shared_ptr<LinearPiece> tmp = 
      C[day]->getMinimalPiece_stockout(client, I[day], objective); //Iday:两个端点之一 x; minValue:两个端点之一 y
    
  if(traces){
    cout <<"I[ "<<day<<" ] = "<<I[day]<<endl;
    cout <<"objective: "<<objective<<endl;
     cout<<"now picked piece"<<endl;tmp->print();cout <<"while begin"<<endl;
  }

  
  while (tmp != nullptr){
    //int a;cin>>a;
    if(traces){
      cout<<"day "<<day<<"  ";
      cout <<"loss?"<< tmp->replenishment_loss<<endl;
      cout<<"now picked piece"<<endl;tmp->print();
      if(tmp->fromF) cout<<"fromF"<<endl,tmp->fromF->print();
      //if(!tmp->fromF) cout <<"nuu"<<endl;
      if(tmp->fromC) cout<<"fromC"<<endl,tmp->fromC->print();
      if(tmp->fromC_pre) cout<<"fromCpre"<<endl,tmp->fromC_pre->print();
      if(tmp->fromInst) cout<<"fromInst"<<endl,tmp->fromF->fromInst->print();
    }

    // if do not delivery any thing, then inventory at the end of previous day
    // equals this day demand
    if (eq(tmp->replenishment_loss,-1) && !tmp->fromF ) //f1 q = 0
    {
      //if(traces) cout <<"daily "<< params->cli[client].dailyDemand[day]<<endl;
      I[day - 1] = I[day] + params->cli[client].dailyDemand[day];
      if(traces)  cout <<"f1 : I[day-1]= "<<I[day-1]<<" quant["<<day<<
      "]= "<<quantities[day]<<endl;
    }
    
    else if (neq(tmp->replenishment_loss,-1) && !tmp->fromF) //f2  q= 0, not enough
    { 
      
      if(neq(I[day],0)){
        cout <<"lotsizing :: line 1076 f2 error!!!!Iday: "<<I[day] <<endl;
        int a=0;
        cin >>a;
      }
      //cout <<"day"<<day<<endl;
      double loss = -tmp->replenishment_loss;
      I[day-1] = loss+params->cli[client].dailyDemand[day];
      
      if(traces)  cout <<"f2 : I[day-1]= "<<I[day-1]<<" quant["<<day<<
      "]= "<<quantities[day]<<endl;
    }
    
    else if (eq(tmp->replenishment_loss,-1) && tmp->fromF ) //f3
    {
      //cout <<"in 3L"<<endl;
      if(traces)cout<< endl<<endl<<"day "<<day+1<<"  "<<"f3: "<<endl;
      if(traces)cout <<"I["<<day<<"] = "<<I[day]<<endl;
      std::shared_ptr<LinearPiece> fromC = tmp->fromC->clone();
      std::shared_ptr<LinearPiece> fromF = tmp->fromF->clone();
      std::shared_ptr<LinearPiece> Cpre = tmp->fromC_pre->clone();
      solveEquationSystem_holding(tmp, fromC, fromF, I[day],
                            params->cli[client].dailyDemand[day], I[day - 1],
                            quantities[day],Cpre,params->cli[client].inventoryCost);
                            
      shared_ptr<LinearPiece> tmpF = tmp->fromF;
      breakpoints[day] = tmpF->fromInst;
      if(traces) cout <<"day "<<day<<" ",breakpoints[day]->print();
      if(traces)  cout <<"f3 : I[day-1]= "<<I[day-1]<<" quant["<<day<<
      "]= "<<quantities[day]<<endl;

      //cout <<"3333"<<endl;
    }
    else if (neq(tmp->replenishment_loss,-1) && tmp->fromF )//f4
    {
      if(traces)cout<< endl<<endl<<"day "<<day+1<<"  "<<"f4: "<<endl;
      std::shared_ptr<LinearPiece> fromC = tmp->fromC->clone();
      std::shared_ptr<LinearPiece> fromF = tmp->fromF->clone();
      std::shared_ptr<LinearPiece> Cpre = tmp->fromC_pre->clone();
      if(traces) {tmp->print(),fromC->print(),fromF->print(),cout<<tmp->replenishment_loss;}
      //cout <<"before sokve"<<endl;
      solveEquationSystem_stockout(tmp, fromC, fromF, -tmp->replenishment_loss,
                            params->cli[client].dailyDemand[day], I[day - 1],
                            quantities[day],Cpre,params->cli[client].stockoutCost);
      
      if(neq(I[day],0) ){
          cout <<"error:  1092 f4 , I[day-1]: "<<I[day]<<endl;
      }
      shared_ptr<LinearPiece> tmpF = tmp->fromF;
      breakpoints[day] = tmpF->fromInst;
      
      if(traces) cout <<"day "<<day<<" ",breakpoints[day]->print();
      if(traces)  cout <<"f4 : I[day-1]= "<<I[day-1]<<" quant["<<day<<
      "]= "<<quantities[day]<<endl;
      
    }
    tmp = tmp->fromC_pre;

    day = day - 1;
    if(day == 0) break;
  }
  if(eq(tmp->replenishment_loss,-1)){
    quantities[0] = I[0]+params->cli[client].dailyDemand[0]-params->cli[client].startingInventory;
    breakpoints[day] = tmp->fromInst;
  }

  else{
      quantities[0] = -tmp->replenishment_loss+params->cli[client].dailyDemand[0]-params->cli[client].startingInventory;
      
      breakpoints[day] = tmp->fromInst;
  }

  return true;
}
void  LotSizingSolver::Firstday(vector<std::shared_ptr<PLFunction>> &C){//for the first day
  bool traces = false;
  std::shared_ptr<PLFunction> Cnew,Cn1;
  double I = params->cli[client].startingInventory - params->cli[client].dailyDemand[0], tmpcost = 100000000000000;
  
  double current_min = 10000000000;
  int piece_idx=0;
  //1 !!!!!!!!! modify C0[0] q!=0, replenishment not enough ->piece/;p-[0]   Istart+q-daily <=0 ->>>>>> q<=daily-Istart
  if(I<0){
      Cn1 = copyPLFunction(F1[0]);
      Cn1->moveUp( - params->cli[client].stockoutCost*params->cli[client].startingInventory); //I_initial -daily =  I_it
      for (int i = 0; i < Cn1->nbPieces; i++){
            Cn1->pieces[i]->fromF = F1[0]->pieces[i];
            Cn1->pieces[i]->fromInst = F1[0]->pieces[i]->fromInst;}
      Cn1 = Cn1->getInBound(0, std::min<double>(params->cli[client].maxInventory,
            params->cli[client].dailyDemand[0]-1)-params->cli[client].startingInventory, false);
      Cn1->shiftLeft(params->cli[client].dailyDemand[0]-params->cli[client].startingInventory);
      if(traces){
        cout <<"f1 shift:"<<endl;
        Cn1->print();
      }   
      for (int i = 0; i < Cn1->nbPieces; i++){
        double tmp_min = std::min<double>(Cn1->pieces[i]->p1->y,Cn1->pieces[i]->p2->y);
        if( gt(current_min, tmp_min ) ){
            current_min = tmp_min;
            piece_idx = i;
            Cn1->pieces[i]->fromC = nullptr;
            Cn1->pieces[i]->fromC_pre = nullptr;
            Cn1->pieces[i]->replenishment_loss = - ((gt(Cn1->pieces[i]->p1->y,Cn1->pieces[i]->p2->y))?Cn1->pieces[i]->p2->x:Cn1->pieces[i]->p1->x);
        } 
    } 
    //2!!!..........   q ==0 replenishment not enough
    tmpcost= params->cli[client].stockoutCost*(-I);
    if(traces) cout <<" no delivery cost on stockout   "<<tmpcost<<endl;
    if(gt(current_min,tmpcost)){
      current_min = tmpcost;
      Cn1->pieces[piece_idx]->fromC = nullptr;
      Cn1->pieces[piece_idx]->fromC_pre = nullptr;
      Cn1->pieces[piece_idx]->replenishment_loss = -I;
      Cn1->pieces[piece_idx]->fromF = nullptr;
      Cn1->pieces[piece_idx]->fromInst = nullptr;
    }

    if(Cn1->nbPieces!=0){
      Cn1->pieces[piece_idx]->updateLinearPiece(0,current_min,0.000000001,current_min);
      C[0]->append(Cn1->pieces[piece_idx]);//minvalue update inside 
      if(traces){
        cout <<"C_0[0]"<<endl;
        C[0]->print();
        cout <<"fromF "<<endl;
        C[0]->pieces[0]->fromF->print();
        cout <<"fromInst" <<endl;
        C[0]->pieces[0]->fromInst->print();
        cout <<"replenishment"<<C[0]->pieces[0]->replenishment_loss<<endl;
      }  
    }
  }
  
  
 
  
  // 3!!!!!!!!!! q!=0, replenishment enough delivery enough
  // I_start(6)+q-daily(5) >=0     I_start+q<=maxinventory    =====>   q<=max-Istart && q>=daily-Istart
  
  Cnew.reset();
  Cnew =copyPLFunction(F2[0]);
  //cout <<"F2[0] "; F2[0]->print();
  Cnew->moveUp(params->cli[client].inventoryCost*params->cli[client].startingInventory);
  for (int i = 0; i < Cnew->nbPieces; i++){    Cnew->pieces[i]->fromF = F2[0]->pieces[i];
      Cnew->pieces[i]->fromInst = F2[0]->pieces[i]->fromInst;}
  Cnew= Cnew->getInBound(std::max<double>(0,params->cli[client].dailyDemand[0]-params->cli[client].startingInventory), 
                  params->cli[client].maxInventory - params->cli[client].startingInventory,  false);
  
  Cnew->shiftLeft(params->cli[client].dailyDemand[0] -  params->cli[client].startingInventory);
  
  if(traces){
    cout <<"f2 shift: "<<endl;
    Cnew->print();
  }  
  for (int i = 0; i < Cnew->nbPieces; i++){
    //3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!q != 0, have inventory cost
      Cnew->pieces[i]->replenishment_loss = -1;
      Cnew->pieces[i]->fromC = nullptr;
      Cnew->pieces[i]->fromC_pre = nullptr;
      //cout <<" pieces [" <<i<<" ] ";Cnew->pieces[i]->print();Cnew->pieces[i]->fromInst->print();F2[0]->pieces[i]->print();
    //4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!q == 0, no delivery,but enough
    if( i==0 && I>=0){ 
      double costtt= params->cli[client].inventoryCost*(I);
      if(traces)  cout <<"Costttt "<<costtt<<" i =  "<<I<<" ("<<Cnew->pieces[i]->p1->x<<"  "<<Cnew->pieces[i]->p1->y<<" )" <<endl;
      
      std::shared_ptr<LinearPiece> Cn2 = std::make_shared<LinearPiece>(I, costtt, I+0.000000001, costtt);
      Cn2->fromC = nullptr;
      Cn2->fromC_pre = nullptr;
      Cn2->replenishment_loss = -1;
      Cn2->fromF = nullptr;
      Cn2->fromInst = nullptr;
      
      
        //Cnew->pieces[i]->updateLinearPiece(I+0.00000001,Cnew->pieces[i]->cost(I+0.00000001),Cnew->pieces[i]->p2->x,Cnew->pieces[i]->p2->y);
        C[0]->append(Cn2);
        if(traces){
          cout <<"C_q=0[*]"<<endl;
          C[0]->print();
        }  
      
    }  
    C[0]->append(Cnew->pieces[i]); //minvalue update
    if(traces){
          cout <<"C_q!=0[**]"<<endl;
          C[0]->print();
    } 
  }
  //C[0]->print();
  //int a;
  //cin>>a;
}

bool LotSizingSolver::solve_stockout()
{
  bool traces = false;//true;
  bool answer = false;//true;//true;
  if (traces)
  {
    cout << "initial inventory: " << params->cli[client].startingInventory
         << endl;
    cout << "daily demand: " << params->cli[client].dailyDemand[0] << endl;
    cout << "inventory cost: " << params->cli[client].inventoryCost << endl;
    cout << "supplier inventory cost: " << params->inventoryCostSupplier
         << endl;
    cout << "max inventory: " << params->cli[client].maxInventory << endl;
    cout << "min inventory: " << params->cli[client].minInventory << endl;
  }

  double minInventory, maxInventory;

  C = vector<std::shared_ptr<PLFunction>>(horizon);
  //我们创建了一个大小为 horizon 的 vector，其中每个元素都是一个指向PLFunction (picewiselienar, variable: I^t) 对象

  I = vector<double>(horizon);
  // final inventory at the end of each period

  quantities = vector<double>(horizon);
  // replinishment at each period

  breakpoints = vector<std::shared_ptr<Insertion>>(horizon);
//我们创建了一个大小为 horizon 的 vector，其中每个元素都是一个指向 Insertion (detour load place) 对象的 shared_ptr。

  exceptionalPieces = vector<std::shared_ptr<LinearPiece>>(horizon);
  //我们创建了一个大小为 horizon 的 vector，其中每个元素都是一个指向 LinearPiece (slope, p1,p2) 对象
  
  for (int i = 0; i < horizon; i++)
  {
    exceptionalPieces[i] = std::make_shared<LinearPiece>(); //std::make_shared 
    C[i] = std::make_shared<PLFunction>(params); // a piecewise linear function in each period
  }
  
  Firstday(C);
  
  if(traces){
    cout << endl<<endl<<"day: 1 "<<endl;
    C[0]->print();
    cout <<"next "<<horizon-1<<" days:"<<endl;
  }
  // initialazition end***************************************************************************
  
  for (int t = 1; t < horizon; t++)
  {
    
    if (traces){
      cout << endl<< "day: " << t+1 << endl; 
    }
    
    //picewise linear functions
    std::shared_ptr<PLFunction> f1;
    std::shared_ptr<PLFunction> f2; //(std::make_shared<PLFunction>(params));
    std::shared_ptr<PLFunction> f3;
    std::shared_ptr<PLFunction> f4;
    std::shared_ptr<PLFunction>  fromF; //(std::make_shared<PLFunction>(params));
    std::shared_ptr<PLFunction>  fromF1;
    std::shared_ptr<PLFunction>  fromF2;
    std::shared_ptr<PLFunction>  fromC; //(std::make_shared<PLFunction>(params));
    minInventory = params->cli[client].minInventory;
    maxInventory = params->cli[client].maxInventory - params->cli[client].dailyDemand[t];
  //cout <<"max§in"<<maxInventory<<endl;
   

    //(1). q(t) = 0,  I(t-1) > daily ==> I(t) > 0 
    //dp <--- C(t-1)(I) + inventoryCost (I-daily)
    f1 = copyPLFunction(C[t-1]);
    f1->addHolding(params->cli[client].inventoryCost,params->cli[client].dailyDemand[t]);
    for (int i = 0; i < f1->nbPieces; i++)
    {
      f1->pieces[i]->fromC_pre = C[t - 1]->pieces[i]->clone();
      f1->pieces[i]->replenishment_loss = -1; 
      f1->pieces[i]->fromF = nullptr;
      f1->pieces[i]->fromC = f1->pieces[i]->clone();
      f1->pieces[i]->fromInst = nullptr;
    }
    f1->shiftLeft(params->cli[client].dailyDemand[t]);
    f1 = f1->getInBound(0, maxInventory, false);
    if(traces){
      cout <<"f1: q= 0&& I(t-1) is enough"<<endl;
      if(f1) f1->print();
    }

    //(2) q(t) == 0,  I(t-1) < daily ==> I(t) ->>> 0 
    //dp <--- C(t-1)(I) + stockoutCost (daily-I)
    f2 = copyPLFunction(C[t - 1]);
    for (int i = 0; i < f2->nbPieces; i++){
      f2->pieces[i]->fromC_pre = C[t - 1]->pieces[i]->clone();
      f2->pieces[i]->replenishment_loss = 0; 
      f2->pieces[i]->fromC = nullptr;//f2->pieces[i]->getInpiece(-params->cli[client].dailyDemand[t] , 0);

      f2->pieces[i]->fromF = nullptr;
      f2->pieces[i]->fromInst = nullptr;
    }
  
    f2->addStockout(params->cli[client].stockoutCost,params->cli[client].dailyDemand[t]);  
    f2->shiftLeft(params->cli[client].dailyDemand[t]);
    
    
    //if(traces) cout <<"f2f2f2f2f2 ", f2->print();
    f2 = f2->getInBound( -params->cli[client].dailyDemand[t] , 0, false);
    if(traces){
      cout <<"f2: q= 0&& I(t-1) is not enough ";
      if(f2)f2->print();
    }
    ///////************************** C[0] = f2->minValue;

    // q != 0, I(t-1)+q-daily > 0     ==> I(t)>0
    // C(t)(It) <----- C(t-1)(It-1) + holdingCost(It-1) + F2function (qt)
    f3 = std::make_shared<PLFunction>(params);
    fromF2 = copyPLFunction(F2[t]);
    //fromF2 = fromF2->getInBound(0,maxInventory,false);
    fromF2 = fromF2->getInBound(0,params->cli[client].maxInventory,false);
    if(traces){cout <<"fromf2 ";fromF2->print();}
    
    fromC = copyPLFunction(C[t - 1]);
    fromC->addHoldingf(params->cli[client].inventoryCost);
    fromC->shiftLeft(params->cli[client].dailyDemand[t]);
    if(traces){cout <<"fromc "; fromC->print();}

    for (int i = 0; i < fromC->nbPieces; i++){
      fromC->pieces[i]->fromC_pre = C[t - 1]->pieces[i]->clone();
    } 
    /*
    if (t == 2){
    cout <<"before super:"<<endl;
    cout <<"C ";fromC->print();
    cout <<"F ";fromF2->print();
    }  
    */
    f3 = supperposition(fromC, fromF2);
    //cout <<"before bound:"<<endl;
    //f3->print();
    for (int i = 0; i < f3->nbPieces; i++){
      f3->pieces[i]->replenishment_loss = -1; 
      f3->pieces[i]->fromC_pre = f3->pieces[i]->fromC->fromC_pre;
      
      //cout <<"aaaa"<<endl;
      //cout <<"f3: ";
      //if(f3->pieces[i]) f3->pieces[i]->print();
     if(traces){
      cout <<"f3 fromC"; 
      if(f3->pieces[i])f3->pieces[i]->fromC->print();
      cout <<"f3 fromF"; 
      if(f3->pieces[i])f3->pieces[i]->fromF->print();
      cout <<"pre:";  
      if(f3->pieces[i]->fromC->fromC_pre)f3->pieces[i]->fromC->fromC_pre->print();}
     
    } 
    //cout <<"~~~~f3: q != 0&& I(t) today is  enough";f3->print();
    f3 = f3->getInBound(0,maxInventory, false);
    
    if(traces){
      cout <<"f3: q != 0&& I(t) today is  enough   ";
      f3->print();
    }

    // q != 0, I(t-1)+q-daily < 0     ==> I(t)=0
    // C(t)(It) <----- C(t-1)(It-1) + stockoutCost(It-1) + F1function (qt)
    f4 = std::make_shared<PLFunction>(params);
    fromF1 = copyPLFunction(F1[t]);
    //fromF1 = fromF1->getInBound(1,std::min<double>(params->cli[client].maxInventory,params->cli[client].dailyDemand[t]),false);
    //cout<<"fromf1"<<endl;
    //fromF1->print();
    //int a;cin>>a;
    fromC = copyPLFunction(C[t - 1]);
    fromC->addStockoutf(params->cli[client].stockoutCost);
    fromC->shiftLeft(params->cli[client].dailyDemand[t]);
    for (int i = 0; i < fromC->nbPieces; i++){
      fromC->pieces[i]->fromC_pre = C[t - 1]->pieces[i]->clone();
    }
    fromC = fromC->getInBound(-params->cli[client].dailyDemand[t],-1,false);
    
    
    f4 = supperposition(fromC, fromF1);
    for (int i = 0; i < f4->nbPieces; i++){
      f4->pieces[i]->replenishment_loss = 0; 
      f4->pieces[i]->fromC_pre = f4->pieces[i]->fromC->fromC_pre; 
      if(traces){
        cout <<"         f4 " ;f4->pieces[i]->print();
        cout <<"       fromF ";f4->pieces[i]->fromF->print();
        cout <<"       fromC ";f4->pieces[i]->fromC->print();
        cout <<"       fromCpre ";f4->pieces[i]->fromC_pre->print();}
      //if(f4->pieces[i]->fromC_pre != nullptr){cout <<"!!!!!!!!"<< endl;f4->pieces[i]->fromC_pre ->print();}
    }
    f4 = f4->getInBound(-params->cli[client].dailyDemand[t], 0, false);
    //cout <<"f40"<<f4->nbPieces<<endl;f4->pieces[0]->fromC_pre->print();
    if (traces){
      cout <<"f4: q != 0&& I(t) today is not enough  ";
      f4->print();
    }
    
    
    C[t] = min_final(f1,f3);
   
    if(traces){
      //cout<<"f1:    ";f1->print();
      //cout<<"f3:    "; f3->print();
      cout <<endl<<"f1&f3: "<<endl<< "C[t]   ";
      C[t]->print();
    }
    double rep2=0.,rep4=0.,t2=10000000.,t4=10000000.;
    
    if(f2->nbPieces) {f2->update_minValue(rep2);  t2 = f2->minValue; }
    if(f4->nbPieces) {f4->update_minValue(rep4); t4 = f4->minValue;}
    //cout <<"rep2 "<<rep2 <<"  rep4 "<<rep4<<endl;
    if(f4->nbPieces&&gt(t2,t4)){  
      C[t]=C[t]->update0 (f4->minValue); 
      if(eq(C[t]->pieces[0]->p2->x,0) && eq(C[t]->pieces[0]->p2->y,f4->minValue) ){
        C[t]->pieces[0]->fromF = f4->minimalPiece->fromF; 
        C[t]->pieces[0]->fromC = f4->minimalPiece->fromC;
        C[t]->pieces[0]->fromC_pre = f4->minimalPiece->fromC_pre;
        C[t]->pieces[0]->fromInst = f4->minimalPiece->fromInst;
        C[t]->pieces[0]->replenishment_loss =rep4;
        if(traces){cout <<"!!rep4: "<<rep4<<" ! "<< C[t]->pieces[0]->replenishment_loss<<endl;}
      } 
      
      if(traces){
        cout <<"f4 : --> C[0]"<<endl;
        C[t]->pieces[0]->print();
        C[t]->pieces[0]->fromC_pre ->print();
      }
      
    }
    
    else if(f2->nbPieces){
      C[t]=C[t]->update0(f2->minValue);
      if(eq(C[t]->pieces[0]->p2->x,0) && eq(C[t]->pieces[0]->p2->y,f2->minValue) ){
        C[t]->pieces[0]->fromF = f2->minimalPiece->fromF;
        C[t]->pieces[0]->fromC = f2->minimalPiece->fromC;
        C[t]->pieces[0]->fromC_pre = f2->minimalPiece->fromC_pre;
        C[t]->pieces[0]->fromInst = f2->minimalPiece->fromInst;
        C[t]->pieces[0]->replenishment_loss = rep2;
        if(traces){cout <<"!!rep2 "<<rep2<<"  "<< C[t]->pieces[0]->replenishment_loss<<endl;}
      }
      
      if(traces){
        cout <<"f2 : --> C[0]"<<endl;
        f2->minimalPiece->print();
        cout <<f2->minValue<<endl;
        C[t]->pieces[0]->print();
      }
    }
     
    if(traces){ cout <<"after f2 f4:"<<endl;
      C[t]->pieces[0]->print();cout <<"rep"<<C[t]->pieces[0]->replenishment_loss<<endl;}
    
    C[t] = C[t]->getInBound(minInventory, maxInventory, false);
    //cout <<minInventory<<"  "<<maxInventory<<endl;

    if(traces){
      cout<<endl<<"day "<< t+1<<" end updated:"<<endl<< "C[t]:  ";
      C[t]->print();
      cout <<"rep"<<C[t]->pieces[0]->replenishment_loss<<endl;
 
      /*
      for(int i = 0 ; i < C[t]->nbPieces;i++){
            C[t]->pieces[i]->print();
           //cout <<"loss?: "<<C[t]->pieces[i]->replenishment_loss<<"  F"<<endl;
           //if(C[t]->pieces[i]->fromF)C[t]->pieces[i]->fromF->print();
           //cout <<"C,pre,Inst   "<<endl;
          //C[t]->pieces[i]->fromC->print();
          //C[t]->pieces[i]->fromC_pre->print();
          //if(C[t]->pieces[i]->fromInst) C[t]->pieces[i]->fromInst->print();
      } 
      */     
    }
  }
  // get solution
  if(traces){
    cout<<endl<<endl<<endl <<"backtracking"<<endl;
    //cout <<"rep"<<C[horizon-1]->pieces[0]->replenishment_loss;
  }
  bool ok = backtracking_stockout();

  if (!ok)
  {
    //        cout << "ERROR: Backtracking result" << endl;
    return false;
  }

  if (traces)
  {
    cout << "backtracking done, Objective: " << objective <<"  "<<params->objectiveConstant_stockout<< endl;
  }

  if (answer)
  {
    cout << "objective: " << objective << endl;
    for (int t = 0; t < horizon; t++)
    {
      cout << "day " << t + 1 << ": ";
      cout << "quantity: " << quantities[t];
      cout <<"    I_remain: "<<I[t]<< endl;
      
        double tt = quantities[t]+params->cli[client].startingInventory;
        if(t==0 &&gt(params->cli[client].dailyDemand[t],tt) )
          cout <<"stockout day0: "<< params->cli[client].dailyDemand[t]-tt<<endl;
        else if(t>=1&&lt(I[t-1]+quantities[t],params->cli[client].dailyDemand[t]) )
         cout <<"stockout: "<<params->cli[client].dailyDemand[t]-quantities[t]-I[t-1]<<endl;
      
    }
    cout <<"print done"<<endl;
  }
  // free memory
  //int a;
  //cin >>a;
  return true;
}

LotSizingSolver::~LotSizingSolver()
{
  //    for (int i = 0; i < horizon; i++) {
  //        delete C[i];
  //        delete F[i];
  //        delete exception——————alPieces[i];
  //        delete breakpoints[i];
  //    }
}
