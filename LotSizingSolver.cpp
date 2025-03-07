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
LotSizingSolver::LotSizingSolver(Params *params, vector<vector<Insertion>> inst, int client)
    : params(params), insertions(inst), client(client)
{
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
      F1[t] = std::make_shared<PLFunction>(params, tmp, t, client, true,params->cli[client].dailyDemand[t]); 

      F2[t] = std::make_shared<PLFunction>(params, tmp, t, client, false); 
      
    }
    else F[t] = std::make_shared<PLFunction>(params, tmp, t, client); 

    if(params-> isstockout){
      for (int i = 0; i < F1[t]->nbPieces; i++){
          F1[t]->pieces[i]->fromF = F1[t]->pieces[i]->clone();
          F1[t]->pieces[i]->replenishment_loss = 0;
          F1[t]->pieces[i]->fromC = nullptr;
          F1[t]->pieces[i]->fromC_pre = nullptr;
          F1[t]->pieces[i]->fromInst = make_shared<Insertion>(
              F1[t]->pieces[i]->fromInst->detour, 
              F1[t]->pieces[i]->fromInst->load,
              F1[t]->pieces[i]->fromInst->place);
      }
      for (int i = 0; i < F2[t]->nbPieces; i++){
          F2[t]->pieces[i]->fromF = F2[t]->pieces[i]->clone(); 
          F2[t]->pieces[i]->replenishment_loss = -1;
          F2[t]->pieces[i]->fromC = nullptr;
          F2[t]->pieces[i]->fromC_pre = nullptr;
          F2[t]->pieces[i]->fromInst = make_shared<Insertion>(
              F2[t]->pieces[i]->fromInst->detour, 
              F2[t]->pieces[i]->fromInst->load,
              F2[t]->pieces[i]->fromInst->place);
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
          breakpoints.push_back(f->pieces[i]->p1->x);
    }
}



vector<double> LotSizingSolver::getBreakpoints_final(std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2){
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

    breakpoints.insert(breakpoints.end(), repeat.begin(), repeat.end());
    
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
    if (f1->nbPieces == 0 && f2->nbPieces == 0) return f;
    if (f1->nbPieces == 0 && f2->nbPieces > 0) return copyPLFunction(f2);
    if (f1->nbPieces > 0 && f2->nbPieces == 0) return copyPLFunction(f1);

    vector<double> breakpoints = getBreakpoints_final(f1, f2);
    int nbBreakPoints = breakpoints.size();
    std::shared_ptr<LinearPiece> lp1 = f1->pieces[0];
    std::shared_ptr<LinearPiece> lp2 = f2->pieces[0];
    int n1=0,n2=0;
    for (int i = 1; i < nbBreakPoints; i++) {
        if(lp1){lp1->updateLinearPiece(lp1->p1->x,lp1->p1->y,lp1->p2->x,lp1->p2->y);}
        if(lp2)lp2->updateLinearPiece(lp2->p1->x,lp2->p1->y,lp2->p2->x,lp2->p2->y);
        std::shared_ptr<LinearPiece> piece1 = nullptr;
        
        if (lp1) piece1 = lp1->getInpiece(breakpoints[i - 1], breakpoints[i]); 
        
        std::shared_ptr<LinearPiece> piece2 = nullptr;
        if (lp2)  piece2 = lp2->getInpiece(breakpoints[i - 1], breakpoints[i]); 
        
        if (!piece1 && !piece2);
        else if (!piece1)  f->append(piece2);
        else if (!piece2) f->append(piece1);
        else {
            double x, y;
            bool intersects = f->intersect(piece1, piece2, x, y);
            if (intersects) {
                std::shared_ptr<LinearPiece> firstPiece, secondPiece, chosenPiece;
                chosenPiece = ( piece1->p1->y <= piece2->p1->y) ? piece1 : piece2;
                firstPiece = createPieceFromLowerY(chosenPiece, chosenPiece->p1->x, chosenPiece->p1->y, x, y);
                f->append(firstPiece);

                chosenPiece =( (piece1->p2->y < piece2->p2->y) )? piece1 : piece2;
                secondPiece = createPieceFromLowerY(chosenPiece, x, y, chosenPiece->p2->x, chosenPiece->p2->y);
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
        if (lp1&& fabs(breakpoints[i]- lp1->p2->x)<0.00001 ){
            if(n1 + 1 < f1->nbPieces) lp1 = f1->pieces[++n1];
            else lp1 = nullptr;
        }
        if (lp2&& fabs(breakpoints[i]- lp2->p2->x)<0.00001 ){
          if(n2 + 1 < f2->nbPieces) lp2 = f2->pieces[++n2];
          else lp2 = nullptr;
        }
        if (!lp1 && !lp2) break;
    }
    return f;
}





std::shared_ptr<PLFunction> LotSizingSolver::supperpositionl(
    std::shared_ptr<LinearPiece> fromPieceC,
    std::shared_ptr<LinearPiece> fromPieceF)
{
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

  int minYIndex1 = std::find(points.begin(), points.end(), minYPoint1) - points.begin();
  int minYIndex2 = std::find(points.begin(), points.end(), minYPoint2) - points.begin();
  int next1 = (minYIndex1 + 1) % 4,next2 = (minYIndex1+3)%4;

  double slope1 = (eq(points[next1].x, points[minYIndex1].x )||   eq(points[next1].y, points[minYIndex1].y ) )
                    ? 0 : (points[next1].y- points[minYIndex1].y )/(points[next1].x- points[minYIndex1].x );
  double slope2 = ( eq(points[next2].x, points[minYIndex1].x )||  eq(points[next2].y, points[minYIndex1].y ))
                    ? 0 : (points[next2].y- points[minYIndex1].y )/(points[next2].x- points[minYIndex1].x );
  if(eq(slope1, 0) || eq(slope2 ,0)){//one 0
    if(minYPoint1.x>minYPoint2.x )std::swap(minYIndex1,minYIndex2);
    next1 = (minYIndex1 + 1) % 4; next2 = (minYIndex1+3)%4;
    int rightup = 6-minYIndex1-next1-next2;
    int leftup = 6-minYIndex1-minYIndex2-rightup;

    if(lt(points[rightup].x,points[minYIndex2].x)){
        tmpPiece = make_shared<LinearPiece>(points[leftup].x, points[leftup].y,points[minYIndex1].x, points[minYIndex1].y); 
        newpiece = make_shared<LinearPiece>(points[minYIndex1].x, points[minYIndex1].y,  points[minYIndex2].x, points[minYIndex2].y);
    }
    else{
        tmpPiece = make_shared<LinearPiece>(points[minYIndex1].x, points[minYIndex1].y,  points[minYIndex2].x, points[minYIndex2].y); 
        newpiece = make_shared<LinearPiece>(points[minYIndex2].x, points[minYIndex2].y,points[rightup].x, points[rightup].y);
    }
  }  
  else if (lt(slope1,0) && lt(0,slope2) || lt(slope2,0) && lt(0,slope1)){//one negative one postive
    if(points[next1].x>points[next2].x)std::swap(next1,next2);
    tmpPiece = make_shared<LinearPiece>(points[next1].x, points[next1].y,points[minYIndex1].x, points[minYIndex1].y); 
    newpiece = make_shared<LinearPiece>(points[minYIndex1].x, points[minYIndex1].y,  points[next2].x, points[next2].y);
    
  }
  else if(lt(slope1,0)){//both negative
    if(eq(minYPoint2.x,minYPoint3.x) && eq(minYPoint2.y,minYPoint3.y) ){
      tmpPiece = make_shared<LinearPiece>(minYPoint4.x, minYPoint4.y,minYPoint2.x, minYPoint2.y); 
      newpiece = make_shared<LinearPiece>(minYPoint2.x, minYPoint2.y,minYPoint1.x, minYPoint1.y);  
    }
    else{
      int tmp1 = gt(slope1,slope2)?next1:next2;
      int tmp2 = 6-minYIndex1-next1-next2;
      tmpPiece = make_shared<LinearPiece>(points[tmp2].x, points[tmp2].y,points[tmp1].x, points[tmp1].y); 
      newpiece = make_shared<LinearPiece>(points[tmp1].x, points[tmp1].y,points[minYIndex1].x, points[minYIndex1].y);  
    }
  }
  else{//both positive
    if(eq(minYPoint2.x,minYPoint3.x) && eq(minYPoint2.y,minYPoint3.y) ){//line
      tmpPiece = make_shared<LinearPiece>(minYPoint1.x, minYPoint1.y,minYPoint2.x, minYPoint2.y); 
      newpiece = make_shared<LinearPiece>(minYPoint2.x, minYPoint2.y,minYPoint4.x, minYPoint4.y);  
    }
    else{
      int tmp1 = gt(slope1,slope2)?next2:next1;
      int tmp2 = 6-minYIndex1-next1-next2;
     
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
  return f;
}

std::shared_ptr<PLFunction> LotSizingSolver:: supperposition(
    std::shared_ptr<PLFunction> fromC, std::shared_ptr<PLFunction> fromF)
{
 std::shared_ptr<PLFunction> f(std::make_shared<PLFunction>(params));

  for (int i = 0; i < fromC->nbPieces; i++){
    for (int j = 0; j < fromF->nbPieces; j++){
      
      std::shared_ptr<PLFunction> tmpF = std::make_shared<PLFunction>(params);
      std::shared_ptr<LinearPiece> fromPieceC = fromC->pieces[i]->clone();
      
      std::shared_ptr<LinearPiece> fromPieceF = fromF->pieces[j]->clone();

      tmpF = supperpositionl(fromPieceC, fromPieceF);
      
      if (f->nbPieces > 0 || tmpF->nbPieces > 0){
        std::shared_ptr<PLFunction> fmin;
        fmin = min_final(f, tmpF);
        f = copyPLFunction(fmin);
      }

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
  if(eq(fromC->p1->x,fromC->p2->x )){
    fromI = round(fromC->p2->x + demand);
    quantity = round(I + demand - fromI);
    return;
  }

  double slopeC = (fromC->p2->y - fromC->p1->y) / (fromC->p2->x - fromC->p1->x);
  double slopeF = (fromF->p2->y - fromF->p1->y) / (fromF->p2->x - fromF->p1->x);
  
  if(eq(slopeC,slopeF)){
    double upperbound = std::min<double>(I-fromC->p1->x,fromF->p2->x);
    double lowerbound = std::max<double>(I-fromC->p2->x,fromF->p1->x);
    if (ge(upperbound ,lowerbound) ){
        quantity = std::floor(upperbound);
        fromI = round(I + demand - quantity);
    }
    else{
       quantity = std::round(upperbound);
        fromI = round(I + demand - quantity);
    }
    return;
  } 
  slopeC*=10000;slopeF*=10000;
  double x1 = fromC->p2->x, y1 = fromC->p2->y,x2 = fromF->p2->x,y2 = fromF->p2->y;
  double numerator = C->cost(std::max<double>(0,I)) *10000- y1*10000 - y2*10000;
    numerator -= slopeC * (I  - x1);
    numerator += slopeF * x2;

    quantity = round(numerator / (slopeF - slopeC));
    double left = round(std::max<double>(I -fromC->p2->x,fromF->p1->x));
    double right = round(std::min<double>(I -fromC->p1->x,fromF->p2->x));
   
   if(gt( quantity ,right )||lt(quantity,left)  ){
      if (gt(quantity ,right) )  quantity = right;
      if (lt(quantity,left) )  quantity = left;
   }
    
     fromI = round(I + demand - quantity);
}

void LotSizingSolver::solveEquationSystem_stockout(std::shared_ptr<LinearPiece> C,
                                          std::shared_ptr<LinearPiece> fromC,
                                          std::shared_ptr<LinearPiece> fromF,
                                          double I, double demand,
                                          double &fromI, double &quantity,std::shared_ptr<LinearPiece> Cpre,double stockout){
  if(eq(fromC->p1->x,fromC->p2->x )){
    fromI = round(fromC->p2->x + demand);
    quantity = round(I + demand - fromI);
    return;
  }

  
  double slopeC = (fromC->p2->y - fromC->p1->y) / (fromC->p2->x - fromC->p1->x);
  double slopeF = (fromF->p2->y - fromF->p1->y) / (fromF->p2->x - fromF->p1->x);
  if(eq(slopeC,slopeF)){
    double upperbound = std::min<double>(I-fromC->p1->x,fromF->p2->x);
    double lowerbound = std::max<double>(I-fromC->p2->x,fromF->p1->x);
    if (ge(upperbound ,lowerbound) ){
        quantity = std::floor(upperbound);
        fromI = round(I + demand - quantity);
    }else {
        std::cout << "No integers between upperbound and lowerbound:" << upperbound <<"  "<<lowerbound<<std::endl;
    }
    return;
  }

   slopeC*=10000;slopeF*=10000;
  double x1 = fromC->p2->x, y1 = fromC->p2->y,x2 = fromF->p2->x,y2 = fromF->p2->y;
  double numerator = C->cost(std::max<double>(0,I)) *10000- y1*10000 - y2*10000;

    numerator -= slopeC * (I  - x1);
    numerator += slopeF * x2;


    quantity = round(numerator / (slopeF - slopeC));
    double left = round(std::max<double>(I -fromC->p2->x,fromF->p1->x));
    double right = round(std::min<double>(I -fromC->p1->x,fromF->p2->x));
   if(gt( quantity ,right )||lt(quantity,left)  ){
      if (gt(quantity ,right) )  quantity = right;
      if (lt(quantity,left) )  quantity = left;
   }
    fromI = round(I + demand - quantity);
}


bool LotSizingSolver::solve()
{
  double minInventory, maxInventory;

  C = vector<std::shared_ptr<PLFunction>>(horizon);

  I = vector<double>(horizon);
  // final inventory at the end of each period

  quantities = vector<double>(horizon);
  // replinishment at each period

  breakpoints = vector<std::shared_ptr<Insertion>>(horizon);

  exceptionalPieces = vector<std::shared_ptr<LinearPiece>>(horizon);

  for (int i = 0; i < horizon; i++)
  {
    exceptionalPieces[i] = std::make_shared<LinearPiece>();
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
    //picewise linear functions
    std::shared_ptr<PLFunction> f1;
    std::shared_ptr<PLFunction> f2;
    std::shared_ptr<PLFunction> f3;
    std::shared_ptr<PLFunction>  fromF;
    std::shared_ptr<PLFunction>  fromC;
    minInventory = params->cli[client].minInventory;
    maxInventory = params->cli[client].maxInventory - params->cli[client].dailyDemand[t];

    // calculate f_1
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

    f1 = f1->getInBound(minInventory, maxInventory, true);
    
    // calculate f_2
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

    // C is not smooth at point of minInventory
    if (C[t - 1]->pieceAt0 != nullptr && C[t - 1]->valueAt0 != 0)
    {
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

      f3 = min(f3, tmpf);

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
    }

    C[t] = C[t]->getInBound(minInventory, maxInventory, false);

    I = I - params->cli[client].dailyDemand[t];
  }
  // get solution
  bool ok = backtracking();

  if (!ok)
  {
    return false;
  }

  objective = 0.;
  for (int i = 0; i < horizon; i++)
  {
    if (quantities[i] > 0)
      objective += F[i]->cost(quantities[i]);
  }

  return true;
}




bool LotSizingSolver::backtracking_stockout(){
  // initialization
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
      C[day]->getMinimalPiece_stockout(client, I[day], objective);


  
  while (tmp != nullptr){

    // if do not delivery any thing, then inventory at the end of previous day
    // equals this day demand
    if (eq(tmp->replenishment_loss,-1) && !tmp->fromF ) //f1 q = 0
    {
      I[day - 1] = I[day] + params->cli[client].dailyDemand[day];
    }
    
    else if (neq(tmp->replenishment_loss,-1) && !tmp->fromF) //f2  q= 0, not enough
    { 
      
      if(neq(I[day],0)){
        cout <<"lotsizing :: line 1076 f2 error!!!!Iday: "<<I[day] <<endl;
        int a=0;
        cin >>a;
      }
      double loss = -tmp->replenishment_loss;
      I[day-1] = loss+params->cli[client].dailyDemand[day];
      
    }
    
    else if (eq(tmp->replenishment_loss,-1) && tmp->fromF ) //f3
    {
      std::shared_ptr<LinearPiece> fromC = tmp->fromC->clone();
      std::shared_ptr<LinearPiece> fromF = tmp->fromF->clone();
      std::shared_ptr<LinearPiece> Cpre = tmp->fromC_pre->clone();
      solveEquationSystem_holding(tmp, fromC, fromF, I[day],
                            params->cli[client].dailyDemand[day], I[day - 1],
                            quantities[day],Cpre,params->cli[client].inventoryCost);
                            
      shared_ptr<LinearPiece> tmpF = tmp->fromF;
      breakpoints[day] = tmpF->fromInst;
    }
    else if (neq(tmp->replenishment_loss,-1) && tmp->fromF )//f4
    {
      std::shared_ptr<LinearPiece> fromC = tmp->fromC->clone();
      std::shared_ptr<LinearPiece> fromF = tmp->fromF->clone();
      std::shared_ptr<LinearPiece> Cpre = tmp->fromC_pre->clone();
      solveEquationSystem_stockout(tmp, fromC, fromF, -tmp->replenishment_loss,
                            params->cli[client].dailyDemand[day], I[day - 1],
                            quantities[day],Cpre,params->cli[client].stockoutCost);
      
      if(neq(I[day],0) ){
          cout <<"error:  1092 f4 , I[day-1]: "<<I[day]<<endl;
      }
      shared_ptr<LinearPiece> tmpF = tmp->fromF;
      breakpoints[day] = tmpF->fromInst;
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
  std::shared_ptr<PLFunction> Cnew,Cn1;
  double I = params->cli[client].startingInventory - params->cli[client].dailyDemand[0], tmpcost = 100000000000000;
  
  double current_min = 10000000000;
  int piece_idx=0;
  //1 !!!!!!!!! modify C0[0] q!=0, replenishment not enough ->piece/;p-[0]   Istart+q-daily <=0 ->>>>>> q<=daily-Istart
  if (I < 0)
  {
    Cn1 = copyPLFunction(F1[0]);
    Cn1->moveUp(-params->cli[client].stockoutCost * params->cli[client].startingInventory); // I_initial -daily =  I_it
    for (int i = 0; i < Cn1->nbPieces; i++)
    {
      Cn1->pieces[i]->fromF = F1[0]->pieces[i];
      Cn1->pieces[i]->fromInst = F1[0]->pieces[i]->fromInst;
    }
    Cn1 = Cn1->getInBound(0, std::min<double>(params->cli[client].maxInventory, params->cli[client].dailyDemand[0] - 1) - params->cli[client].startingInventory, false);
    Cn1->shiftLeft(params->cli[client].dailyDemand[0] - params->cli[client].startingInventory);
    for (int i = 0; i < Cn1->nbPieces; i++)
    {
      double tmp_min = std::min<double>(Cn1->pieces[i]->p1->y, Cn1->pieces[i]->p2->y);
      if (gt(current_min, tmp_min))
      {
        current_min = tmp_min;
        piece_idx = i;
        Cn1->pieces[i]->fromC = nullptr;
        Cn1->pieces[i]->fromC_pre = nullptr;
        Cn1->pieces[i]->replenishment_loss = -((gt(Cn1->pieces[i]->p1->y, Cn1->pieces[i]->p2->y)) ? Cn1->pieces[i]->p2->x : Cn1->pieces[i]->p1->x);
      }
    }
    // 2!!!..........   q ==0 replenishment not enough
    tmpcost = params->cli[client].stockoutCost * (-I);
    if (Cn1->nbPieces == 0){
       std::shared_ptr<LinearPiece> tmptmp = std::make_shared<LinearPiece>(0, tmpcost, 0.000000001, tmpcost);
       tmptmp->fromC = nullptr;
       tmptmp->fromC_pre = nullptr;
       tmptmp->replenishment_loss = -I;
       tmptmp->fromF = nullptr;
       tmptmp->fromInst = nullptr;
       C[0]->append(tmptmp); 
    }

      else{
        if (gt(current_min, tmpcost)){
          current_min = tmpcost;
          Cn1->pieces[piece_idx]->fromC = nullptr;
          Cn1->pieces[piece_idx]->fromC_pre = nullptr;
          Cn1->pieces[piece_idx]->replenishment_loss = -I;
          Cn1->pieces[piece_idx]->fromF = nullptr;
          Cn1->pieces[piece_idx]->fromInst = nullptr;
        }

        Cn1->pieces[piece_idx]->updateLinearPiece(0, current_min, 0.000000001, current_min);
        C[0]->append(Cn1->pieces[piece_idx]); // minvalue update inside
      }
  }

  // 3!!!!!!!!!! q!=0, replenishment enough delivery enough
  // I_start(6)+q-daily(5) >=0     I_start+q<=maxinventory    =====>   q<=max-Istart && q>=daily-Istart
  
  Cnew.reset();
  Cnew =copyPLFunction(F2[0]);
  Cnew->moveUp(params->cli[client].inventoryCost*params->cli[client].startingInventory);
  for (int i = 0; i < Cnew->nbPieces; i++){    Cnew->pieces[i]->fromF = F2[0]->pieces[i];
      Cnew->pieces[i]->fromInst = F2[0]->pieces[i]->fromInst;}
  Cnew= Cnew->getInBound(std::max<double>(0,params->cli[client].dailyDemand[0]-params->cli[client].startingInventory), 
                  params->cli[client].maxInventory - params->cli[client].startingInventory,  false);
  
  Cnew->shiftLeft(params->cli[client].dailyDemand[0] -  params->cli[client].startingInventory);
 
  for (int i = 0; i < Cnew->nbPieces; i++){
    //3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!q != 0, have inventory cost
      Cnew->pieces[i]->replenishment_loss = -1;
      Cnew->pieces[i]->fromC = nullptr;
      Cnew->pieces[i]->fromC_pre = nullptr;
    //4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!q == 0, no delivery,but enough
    if( i==0 && I>=0){ 
      double costtt= params->cli[client].inventoryCost*(I);
      
      std::shared_ptr<LinearPiece> Cn2 = std::make_shared<LinearPiece>(I, costtt, I+0.000000001, costtt);
      Cn2->fromC = nullptr;
      Cn2->fromC_pre = nullptr;
      Cn2->replenishment_loss = -1;
      Cn2->fromF = nullptr;
      Cn2->fromInst = nullptr;
      
      C[0]->append(Cn2); 
      
    }  
    C[0]->append(Cnew->pieces[i]); //minvalue update
  }
}

bool LotSizingSolver::solve_stockout()
{
  double minInventory, maxInventory;

  C = vector<std::shared_ptr<PLFunction>>(horizon);

  I = vector<double>(horizon);
  // final inventory at the end of each period

  quantities = vector<double>(horizon);
  // replinishment at each period

  breakpoints = vector<std::shared_ptr<Insertion>>(horizon);

  exceptionalPieces = vector<std::shared_ptr<LinearPiece>>(horizon);
  
  for (int i = 0; i < horizon; i++)
  {
    exceptionalPieces[i] = std::make_shared<LinearPiece>(); //std::make_shared 
    C[i] = std::make_shared<PLFunction>(params); // a piecewise linear function in each period
  }
  
  Firstday(C);

  // initialazition end***************************************************************************
  
  for (int t = 1; t < horizon; t++)
  {
    //picewise linear functions
    std::shared_ptr<PLFunction> f1;
    std::shared_ptr<PLFunction> f2;
    std::shared_ptr<PLFunction> f3;
    std::shared_ptr<PLFunction> f4;
    std::shared_ptr<PLFunction>  fromF;
    std::shared_ptr<PLFunction>  fromF1;
    std::shared_ptr<PLFunction>  fromF2;
    std::shared_ptr<PLFunction>  fromC;
    minInventory = params->cli[client].minInventory;
    maxInventory = params->cli[client].maxInventory - params->cli[client].dailyDemand[t];
   

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

    //(2) q(t) == 0,  I(t-1) < daily ==> I(t) ->>> 0 
    //dp <--- C(t-1)(I) + stockoutCost (daily-I)
    f2 = copyPLFunction(C[t - 1]);
    for (int i = 0; i < f2->nbPieces; i++){
      f2->pieces[i]->fromC_pre = C[t - 1]->pieces[i]->clone();
      f2->pieces[i]->replenishment_loss = 0; 
      f2->pieces[i]->fromC = nullptr;

      f2->pieces[i]->fromF = nullptr;
      f2->pieces[i]->fromInst = nullptr;
    }
  
    f2->addStockout(params->cli[client].stockoutCost,params->cli[client].dailyDemand[t]);  
    f2->shiftLeft(params->cli[client].dailyDemand[t]);
    
    f2 = f2->getInBound( -params->cli[client].dailyDemand[t] , 0, false);

    ///////************************** C[0] = f2->minValue;

    // q != 0, I(t-1)+q-daily > 0     ==> I(t)>0
    // C(t)(It) <----- C(t-1)(It-1) + holdingCost(It-1) + F2function (qt)
    f3 = std::make_shared<PLFunction>(params);
    fromF2 = copyPLFunction(F2[t]);
    //fromF2 = fromF2->getInBound(0,maxInventory,false);
    fromF2 = fromF2->getInBound(0,params->cli[client].maxInventory,false);
    
    fromC = copyPLFunction(C[t - 1]);
    fromC->addHoldingf(params->cli[client].inventoryCost);
    fromC->shiftLeft(params->cli[client].dailyDemand[t]);

    for (int i = 0; i < fromC->nbPieces; i++){
      fromC->pieces[i]->fromC_pre = C[t - 1]->pieces[i]->clone();
    } 

    f3 = supperposition(fromC, fromF2);
    for (int i = 0; i < f3->nbPieces; i++){
      f3->pieces[i]->replenishment_loss = -1; 
      f3->pieces[i]->fromC_pre = f3->pieces[i]->fromC->fromC_pre;
    } 
    f3 = f3->getInBound(0,maxInventory, false);

    // q != 0, I(t-1)+q-daily < 0     ==> I(t)=0
    // C(t)(It) <----- C(t-1)(It-1) + stockoutCost(It-1) + F1function (qt)
    f4 = std::make_shared<PLFunction>(params);
    fromF1 = copyPLFunction(F1[t]);
  
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
    }
    f4 = f4->getInBound(-params->cli[client].dailyDemand[t], 0, false);
    
    
    C[t] = min_final(f1,f3);

    double rep2=0.,rep4=0.,t2=10000000.,t4=10000000.;
    
    if(f2->nbPieces) {f2->update_minValue(rep2);  t2 = f2->minValue; }
    if(f4->nbPieces) {f4->update_minValue(rep4); t4 = f4->minValue;}
    if(f4->nbPieces&&gt(t2,t4)){  
      C[t]=C[t]->update0 (f4->minValue); 
      if(eq(C[t]->pieces[0]->p2->x,0) && eq(C[t]->pieces[0]->p2->y,f4->minValue) ){
        C[t]->pieces[0]->fromF = f4->minimalPiece->fromF; 
        C[t]->pieces[0]->fromC = f4->minimalPiece->fromC;
        C[t]->pieces[0]->fromC_pre = f4->minimalPiece->fromC_pre;
        C[t]->pieces[0]->fromInst = f4->minimalPiece->fromInst;
        C[t]->pieces[0]->replenishment_loss =rep4;
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
      }
    }
  }

  // get solution
  bool ok = backtracking_stockout();

  if (!ok)
  {
    return false;
  }
  return true;
}

LotSizingSolver::~LotSizingSolver()
{
  
}
