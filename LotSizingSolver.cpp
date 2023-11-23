//
// Created by pta on 16/08/2018.
//

#include "LotSizingSolver.h"
#include <memory>

LotSizingSolver::LotSizingSolver(Params *params, vector<vector<Insertion>> inst,
                                 int client)
    : params(params), insertions(inst), client(client)
{
  traces = false;

  horizon = (int)insertions.size();
  F = vector<std::shared_ptr<PLFunction>>(horizon);

  for (int t = 0; t < horizon; t++)
  {
    vector<Insertion> tmp = insertions[t];

    F[t] = std::make_shared<PLFunction>(params, tmp, t, client);

    for (int i = 0; i < F[t]->nbPieces; i++)
    {
      F[t]->pieces[i]->fromF = F[t]->pieces[i]->clone();

      F[t]->pieces[i]->fromC = nullptr;
      F[t]->pieces[i]->fromInst = make_shared<Insertion>(
          F[t]->pieces[i]->fromInst->detour, F[t]->pieces[i]->fromInst->load,
          F[t]->pieces[i]->fromInst->place);
    }

    if (traces)
    {
      cout << "capacity penalty: " << params->penalityCapa << endl;
      cout << "fs[" << t << "]: ";
      F[t]->print();
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

std::shared_ptr<PLFunction> LotSizingSolver::supperposition(
    std::shared_ptr<LinearPiece> fromPieceC,
    std::shared_ptr<LinearPiece> fromPieceF)
{
  std::shared_ptr<PLFunction> f(std::make_shared<PLFunction>(params));
  vector<Point> points;
  shared_ptr<LinearPiece> tmpPiece;
  std::shared_ptr<PLFunction> fmin = std::make_shared<PLFunction>(params);

  points.push_back(fromPieceC->p1->convolve(fromPieceF->p2));
  points.push_back(fromPieceC->p1->convolve(fromPieceF->p1));
  points.push_back(fromPieceC->p2->convolve(fromPieceF->p1));
  points.push_back(fromPieceC->p2->convolve(fromPieceF->p2));

  for (int i = 0; i < 4; i++)
  {
    int next = (i + 1) % 4;
    if (points[i].x < points[next].x)
      tmpPiece = make_shared<LinearPiece>(points[i].x, points[i].y,
                                          points[next].x, points[next].y);
    else
      tmpPiece = make_shared<LinearPiece>(points[next].x, points[next].y,
                                          points[i].x, points[i].y);

    tmpPiece->fromC = fromPieceC->clone();
    tmpPiece->fromF = fromPieceF->clone();

    if (i == 0)
      f->append(tmpPiece);
    else
    {
      std::shared_ptr<PLFunction> tmpF = std::make_shared<PLFunction>(params);
      tmpF->append(tmpPiece);

      //            fmin->clear();
      fmin = min(f, tmpF);

      //            if(traces){
      //                cout << "f: "; f->print();
      //                cout << "tmpF: "; tmpF->print();
      //                cout << "fmin: "; fmin->print();
      //            }

      f = copyPLFunction(fmin);

      tmpF.reset();
      fmin.reset();
    }

    tmpPiece.reset();
  }
  return f;
}

std::shared_ptr<PLFunction> LotSizingSolver::supperposition(
    std::shared_ptr<PLFunction> fromC, std::shared_ptr<PLFunction> fromF)
{
  std::shared_ptr<PLFunction> f(std::make_shared<PLFunction>(params));
  //    std::shared_ptr<PLFunction> fmin(std::make_shared<PLFunction>(params));

  for (int i = 0; i < fromC->nbPieces; i++)
  {
    for (int j = 0; j < fromF->nbPieces; j++)
    {
      std::shared_ptr<PLFunction> tmpF = std::make_shared<PLFunction>(params);

      std::shared_ptr<LinearPiece> fromPieceC = fromC->pieces[i]->clone();
      std::shared_ptr<LinearPiece> fromPieceF = fromF->pieces[j]->clone();

      tmpF = supperposition(fromPieceC, fromPieceF);

      if (f->nbPieces > 0 || tmpF->nbPieces > 0)
      {
        std::shared_ptr<PLFunction> fmin = min(f, tmpF);

        f = copyPLFunction(fmin);
      }

      tmpF.reset();
      fromPieceC.reset();
      fromPieceF.reset();
    }
  }

  return f;
}

bool LotSizingSolver::backtracking()
{
  // initialization
  for (int i = 0; i < horizon; i++)
  {
    quantities[i] = 0.0;
    breakpoints[i] = nullptr;
    I[i] = 0; // inventory level at the end of each day
  }
  // backtracking from the last day
  int day = horizon - 1;

  if (C[day]->nbPieces == 0 && C[day]->pieceAt0 == nullptr)
    return false;

  std::shared_ptr<LinearPiece> tmp = C[day]->getMinimalPiece(client, I[day], objective);

  //  std::shared_ptr<LinearPiece> tmp = C[day]->pieces[0];
  //
  //  objective = tmp->p1->y;
  //
  //  if (C[day]->pieceAt0 && gt(objective, C[day]->valueAt0)) {
  //    objective = C[day]->valueAt0;
  //    tmp = C[day]->pieceAt0->clone();
  //    I[day] = 0;
  //  }
  //
  //  if (gt(objective, C[day]->pieces[0]->p2->y)) {
  //    objective = tmp->p2->y;
  //    I[day] = tmp->p2->x;
  //  }

  while (tmp != nullptr)
  {
    // no delivery at day 0
    if (day == 0 && (tmp->fromC != nullptr || tmp->fromF == nullptr))
      break;

    // no delivery before
    if (tmp->fromF == nullptr && tmp->fromC == nullptr)
      break;

    // no delivery before and no delivery cost
    // if (tmp->fromC == nullptr && tmp->fromF != nullptr && tmp->fromF->cost(I[day]) == 0)
    //   break;

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
        quantities[day] = I[day] + (day + 1) * params->cli[client].dailyDemand[day] - params->cli[client].startingInventory;
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
        solveEquationSystem(tmp, fromC, fromF, I[day], params->cli[client].dailyDemand[day], I[day - 1], quantities[day]);
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
    quantity = fromF->p2->x;
    fromI = std::max<double>(0., I + demand - quantity);
  }
  else
  {
    double x1 = fromC->p2->x;
    double y1 = fromC->p2->y;

    double x2 = fromF->p2->x;
    double y2 = fromF->p2->y;

    // C(I) = fromC(fromI) + fromF(quantity)
    //    fromC(fromI) = fromC->p2->y + slopeFromC * (fromC->p2->x - fromI): calculate value at a point fromI of Piece fromC
    //    fromF(quantity) = fromF->p2->y + slopeFromF * (fromF->p2->x - quantity)
    //    fromI = I + demand - quantity
    // ==> get quantity
    double numerator = C->cost(I) - y1 - y2;
    numerator -= slopeC * (I + demand - x1);
    numerator += slopeF * x2;

    quantity = round(numerator / (slopeF - slopeC));
    fromI = I + demand - quantity;
  }
}

bool LotSizingSolver::solve()
{
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
  I = vector<double>(horizon);
  quantities = vector<double>(horizon);
  breakpoints = vector<std::shared_ptr<Insertion>>(horizon);

  exceptionalPieces = vector<std::shared_ptr<LinearPiece>>(horizon);
  for (int i = 0; i < horizon; i++)
  {
    exceptionalPieces[i] = std::make_shared<LinearPiece>();
    C[i] = std::make_shared<PLFunction>(params);
  }

  //    std::shared_ptr<PLFunction> f1;
  //    std::shared_ptr<PLFunction> f2(std::make_shared<PLFunction>(params));
  //
  //    std::shared_ptr<PLFunction> fromF(std::make_shared<PLFunction>(params));
  //    std::shared_ptr<PLFunction> fromC(std::make_shared<PLFunction>(params));
  //    std::shared_ptr<PLFunction> f3;
  //
  //    std::shared_ptr<PLFunction> tmpF1(std::make_shared<PLFunction>(params));
  //    PLFunction *tmpF2 = new PLFunction(params);

  // for the first day
  C[0] = F[0]->getInBound(0, params->cli[client].maxInventory - params->cli[client].startingInventory,
                          true);

  C[0]->shiftLeft(params->cli[client].dailyDemand[0] - params->cli[client].startingInventory);
  for (int i = 0; i < C[0]->nbPieces; i++)
  {
    C[0]->pieces[i]->fromC = nullptr;
    C[0]->pieces[i]->fromF = F[0]->pieces[i];
  }
  C[0] = C[0]->getInBound(params->cli[client].minInventory,
                          params->cli[client].maxInventory, true);

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

    std::shared_ptr<PLFunction> f1;
    std::shared_ptr<PLFunction> f2; //(std::make_shared<PLFunction>(params));

    std::shared_ptr<PLFunction>
        fromF; //(std::make_shared<PLFunction>(params));
    std::shared_ptr<PLFunction>
        fromC; //(std::make_shared<PLFunction>(params));
    std::shared_ptr<PLFunction> f3;

    //        std::shared_ptr<PLFunction>
    //        tmpF1;//(std::make_shared<PLFunction>(params));

    minInventory = params->cli[client].minInventory;
    maxInventory =
        params->cli[client].maxInventory - params->cli[client].dailyDemand[t];

    // calculate f_1
    //        tmpF1->clear();
    f1 = F[t]->getInBound(0, params->cli[client].maxInventory - (params->cli[client].startingInventory - t * params->cli[client].dailyDemand[t]),
                          true);
    for (int i = 0; i < f1->nbPieces; i++)
    {
      //            f1->pieces[i]->fromF = new LinearPiece();
      //            f1->copyPiece(f1->pieces[i], f1->pieces[i]->fromF);
      f1->pieces[i]->fromF =
          make_shared<LinearPiece>(f1->pieces[i]->p1->x, f1->pieces[i]->p1->y,
                                   f1->pieces[i]->p2->x, f1->pieces[i]->p2->y);
      f1->pieces[i]->fromF->fromInst = f1->pieces[i]->fromInst;
    }

    f1->shiftLeft((t + 1) * params->cli[client].dailyDemand[t] -
                  params->cli[client].startingInventory);

    f1 = f1->getInBound(minInventory, maxInventory, true);

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

    // calculate f_3
    f3 = std::make_shared<PLFunction>(new PLFunction(params));

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
      //            shared_ptr<LinearPiece> tmpPiece =
      //            C[t-1]->pieceAt0->clone();
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
    cout << "Objective before update: " << objective << endl;
  }
  objective = 0.;
  for (int i = 0; i < horizon; i++)
  {
    if (quantities[i] > 0)
      objective += F[i]->cost(quantities[i]);
  }

  if (traces)
  {
    cout << "objective: " << objective << endl;
    for (int t = 0; t < horizon; t++)
    {
      if (quantities[t] > 0)
      {
        cout << "day " << t + 1 << ": ";
        cout << "quantity: " << quantities[t];
        cout << " cost: " << F[t]->cost(quantities[t]);
        //             cout << " routeidx: " <<
        //                  breakPoints[t]->place->route->cour;
        //             cout << " nodeidx: " << breakPoints[t]->place->cour
        //                  <<
        //                  endl;
        cout << endl;
      }
    }
  }
  // free memory

  return true;
}

vector<double> LotSizingSolver::getBreakpoints(std::shared_ptr<PLFunction> f1,
                                               std::shared_ptr<PLFunction> f2)
{
  // get breakpoints of pieces
  vector<double> breakpoints = vector<double>();
  // from f1
  for (int i = 0; i < f1->nbPieces; i++)
  {
    if (i == 0)
      breakpoints.push_back(f1->pieces[i]->p1->x);
    breakpoints.push_back(f1->pieces[i]->p2->x);
  }
  // from f2
  for (int i = 0; i < f2->nbPieces; i++)
  {
    if (i == 0)
      breakpoints.push_back(f2->pieces[i]->p1->x);
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

LotSizingSolver::~LotSizingSolver()
{
  //    for (int i = 0; i < horizon; i++) {
  //        delete C[i];
  //        delete F[i];
  //        delete exceptionalPieces[i];
  //        delete breakpoints[i];
  //    }
}
