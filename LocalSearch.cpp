#include "LocalSearch.h"

// lance la recherche locale
void LocalSearch::runILS(bool isRepPhase, int maxIterations)
{
  double bestCost = 1.e30;
  for (int it = 0; it < maxIterations; it++)
  {
    if (it > 0)
      shaking();
    runSearchTotal(isRepPhase);
  }
}

// lance la recherche locale
void LocalSearch::runSearchTotal(bool isRepPhase)
{
  this->isRepPhase = isRepPhase;
  int nbMoves = 0;
  int nbTotal = 0;
  int nbPhases = 0;
  bool traces = false;

  // LANCEMENT DE LA RECHERCHE
  if (traces)
    cout << "COST INITIAL " << evaluateSolutionCost() << endl;

  // reorganisation des plans de transport pour chaque jour
  updateMoves();
  for (int day = 1; day <= params->nbDays; day++)
    nbMoves += mutationSameDay(day);
  nbTotal += nbMoves;
  nbPhases++;
  if (traces)
    cout << "COST AFTER RI " << (nbPhases + 1) / 2 << " : "
         << evaluateSolutionCost() << endl;

  // reorganisation des jours
  if (nbPhases < params->maxLSPhases)
  {
    nbMoves = mutationDifferentDay();
    nbTotal += nbMoves;
    nbPhases++;
    if (traces)
      cout << "COST AFTER PI " << (nbPhases + 1) / 2 << " : "
           << evaluateSolutionCost() << endl;
  }

  while (nbMoves > 0 && nbPhases < params->maxLSPhases)
  {
    nbMoves = 0;
    updateMoves();
    for (int day = 1; day <= params->nbDays; day++)
      nbMoves += mutationSameDay(day);
    nbPhases++;
    if (traces)
      cout << "COST AFTER RI " << (nbPhases + 1) / 2 << " : "
           << evaluateSolutionCost() << endl;

    if (nbMoves > 0 && nbPhases < params->maxLSPhases)
    {
      nbMoves += mutationDifferentDay();
      nbTotal += nbMoves;
      nbPhases++;
      if (traces)
        cout << "COST AFTER PI " << (nbPhases + 1) / 2 << " : "
             << evaluateSolutionCost() << endl;
    }
  }

  if (traces)
    cout << "COST FINAL : " << evaluateSolutionCost() << endl
         << endl;

  // cout << endl << endl ;
  // printInventoryLevels();
  // cout << endl << endl ;
}

// les tableaux ordreParcours de tous les jours sont r�organis�s al�atoirement
void LocalSearch::melangeParcours()
{
  int j, temp;
  for (int k = 0; k <= params->nbDays; k++)
  {
    for (int i = 0; i < (int)ordreParcours[k].size() - 1; i++)
    {
      j = i +
          params->rng->genrand64_int64() % ((int)ordreParcours[k].size() - i);
      temp = ordreParcours[k][i];
      ordreParcours[k][i] = ordreParcours[k][j];
      ordreParcours[k][j] = temp;
    }
  }

  for (int i = 0; i < (int)ordreJours.size() - 1; i++)
  {
    j = i + params->rng->genrand64_int64() % ((int)ordreJours.size() - i);
    temp = ordreJours[i];
    ordreJours[i] = ordreJours[j];
    ordreJours[j] = temp;
  }
}

// updates the moves for each node which will be tried in mutationSameDay
void LocalSearch::updateMoves()
{
  int client, client2;
  int size;

  for (int k = 1; k <= params->nbDays; k++)
  {
    // pour chaque client present dans ce jour
    for (int i = 0; i < (int)ordreParcours[k].size(); i++)
    {
      client = ordreParcours[k][i];
      clients[k][client]->moves.clear();
      size = params->cli[client].sommetsVoisins.size();

      for (int a1 = 0; a1 < size; a1++)
      {
        client2 = params->cli[client].sommetsVoisins[a1];
        if (client2 >= params->nbDepots && clients[k][client2]->estPresent)
          clients[k][client]->moves.push_back(client2);
      }
    }
  }

  params->shuffleProches();
  melangeParcours();
}

int LocalSearch::mutationSameDay(int day)
{
  dayCour = day;
  int size = (int)ordreParcours[day].size();
  int size2;
  rechercheTerminee = false;
  int moveEffectue = 0;
  int nbMoves = 0;
  firstLoop = true;

  while (!rechercheTerminee)
  {
    rechercheTerminee = true;
    moveEffectue = 0;
    for (int posU = 0; posU < size; posU++)
    {
      posU -= moveEffectue; // on retourne sur le dernier noeud si on a modifi�
      nbMoves += moveEffectue;
      moveEffectue = 0;
      noeudU = clients[day][ordreParcours[day][posU]];

      noeudUPred = noeudU->pred;
      x = noeudU->suiv;
      noeudXSuiv = x->suiv;
      xSuivCour = x->suiv->cour;
      routeU = noeudU->route;
      noeudUCour = noeudU->cour;
      noeudUPredCour = noeudUPred->cour;
      xCour = x->cour;

      size2 = (int)noeudU->moves.size();
      for (int posV = 0; posV < size2 && moveEffectue == 0; posV++)
      {
        noeudV = clients[day][noeudU->moves[posV]];
        if (!noeudV->route->nodeAndRouteTested[noeudU->cour] ||
            !noeudU->route->nodeAndRouteTested[noeudU->cour] || firstLoop)
        {
          noeudVPred = noeudV->pred;
          y = noeudV->suiv;
          noeudYSuiv = y->suiv;
          ySuivCour = y->suiv->cour;
          routeV = noeudV->route;
          noeudVCour = noeudV->cour;
          noeudVPredCour = noeudVPred->cour;
          yCour = y->cour;

          if (moveEffectue != 1)
            moveEffectue = mutation1();
          if (moveEffectue != 1)
            moveEffectue = mutation2();
          if (moveEffectue != 1)
            moveEffectue = mutation3();

          // les mutations 4 et 6 (switch) , sont sym�triques
          if (noeudU->cour <= noeudV->cour)
          {
            if (moveEffectue != 1)
              moveEffectue = mutation4();
            if (moveEffectue != 1)
              moveEffectue = mutation6();
          }
          if (moveEffectue != 1)
            moveEffectue = mutation5();

          // mutations 2-opt
          if (moveEffectue != 1)
            moveEffectue = mutation7();
          if (moveEffectue != 1)
            moveEffectue = mutation8();
          if (moveEffectue != 1)
            moveEffectue = mutation9();

          if (moveEffectue == 1)
          {
            routeU->reinitSingleDayMoves();
            routeV->reinitSingleDayMoves();
          }
        }
      }

      // c'est un d�pot on tente l'insertion derriere le depot de ce jour
      // si il ya corr�lation
      if (params->isCorrelated1[noeudU->cour][depots[day][0]->cour] && moveEffectue != 1)
        for (int route = 0; route < (int)depots[day].size(); route++)
        {
          noeudV = depots[day][route];
          if (!noeudV->route->nodeAndRouteTested[noeudU->cour] ||
              !noeudU->route->nodeAndRouteTested[noeudU->cour] || firstLoop)
          {
            noeudVPred = noeudV->pred;
            y = noeudV->suiv;
            noeudYSuiv = y->suiv;
            ySuivCour = y->suiv->cour;
            routeV = noeudV->route;
            noeudVCour = noeudV->cour;
            noeudVPredCour = noeudVPred->cour;
            yCour = y->cour;

            if (moveEffectue != 1)
              moveEffectue = mutation1();
            if (moveEffectue != 1)
              moveEffectue = mutation2();
            if (moveEffectue != 1)
              moveEffectue = mutation3();

            if (!noeudV->suiv->estUnDepot)
            {
              if (moveEffectue != 1)
                moveEffectue = mutation8();
              if (moveEffectue != 1)
                moveEffectue = mutation9();
            }

            if (moveEffectue == 1)
            {
              routeU->reinitSingleDayMoves();
              routeV->reinitSingleDayMoves();
            }
          }
        }
      // if (moveEffectue != 1) nodeTestedForEachRoute(noeudU->cour,day); //
      // TODO -- check that memories are working
    }
    firstLoop = false;
  }
  return nbMoves;
}

// pour un noeud, marque que tous les mouvements impliquant ce noeud ont �t�
// test�s pour chaque route du jour day
void LocalSearch::nodeTestedForEachRoute(int cli, int day)
{
  for (int route = 0; route < (int)depots[day].size(); route++)
    routes[day][route]->nodeAndRouteTested[cli] = true;
}

// trying to change the delivery plan (lot sizing for a given customer)
int LocalSearch::mutationDifferentDay()
{
  rechercheTerminee = false;
  int nbMoves = 0;
  while (!rechercheTerminee)
  {
    rechercheTerminee = true;
    for (int posU = 0; posU < params->nbClients; posU++)
      nbMoves += mutation11(ordreParcours[0][posU]);
  }
  return nbMoves;
}

// enleve un client de l'ordre de parcours
void LocalSearch::removeOP(int day, int client)
{
  int it = 0;
  while (ordreParcours[day][it] != client)
  {
    it++;
  }
  ordreParcours[day][it] =
      ordreParcours[day][(int)ordreParcours[day].size() - 1];
  ordreParcours[day].pop_back();
}

// ajoute un client dans l'ordre de parcours
void LocalSearch::addOP(int day, int client)
{
  int it, temp2;
  if (ordreParcours[day].size() != 0)
  {
    it = (int)params->rng->genrand64_int64() % ordreParcours[day].size();
    temp2 = ordreParcours[day][it];
    ordreParcours[day][it] = client;
    ordreParcours[day].push_back(temp2);
  }
  else
    ordreParcours[day].push_back(client);
}

// change the choices of visit periods and quantity for "client"
int LocalSearch::mutation11(int client)
{
  Noeud *noeudTravail;
  double currentCost;

  // First, make sure all insertion costs are computed
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    noeudTravail = clients[k][client];
    computeCoutInsertion(noeudTravail);
  }

  // Compute the current lot sizing solution cost (from the model point of view)
  // before optimizatio  currentCost = evaluateCurrentCost(client);
  currentCost = evaluateCurrentCost(client);

  /* Generate the structures of the subproblem */
  vector<double> myA = vector<double>(params->nbDays);
  vector<double> myB = vector<double>(params->nbDays);
  vector<vector<Insertion>> insertions = vector<vector<Insertion>>(params->nbDays);
  double netInventoryCost = params->cli[client].inventoryCost - params->inventoryCostSupplier;
  vector<double> quantities = vector<double>(params->nbDays);
  vector<int> breakpoints = vector<int>(params->nbDays);
  double objective;

  /* Calculate the parameters of the subproblem */
  // Take care, the conventions on the indices are slightly different
  // On the CVRP code we cound the days from 1 to t
  // In the CPLEX model, we count the days from 0 to t-1
  // kept minInventory here even if its always equal to 0.
  myA[0] = params->cli[client].dailyDemand[1] -
           params->cli[client].startingInventory +
           params->cli[client].minInventory;
  for (int k = 2; k <= params->nbDays; k++)
    myA[k - 1] = myA[k - 2] + params->cli[client].dailyDemand[k];

  // for now, the supplier inventory constraints are not counted in myB, so myB
  // = myA + inventory capacity of the customer
  for (int k = 1; k <= params->nbDays; k++)
    myB[k - 1] = myA[k - 1] - params->cli[client].dailyDemand[k] +
                 params->cli[client].maxInventory -
                 params->cli[client].minInventory;

  // cout << endl << "------------------------------" << endl;
  // copying the non-dominated insertions obtained from the pre-processing
  for (int k = 1; k <= params->nbDays; k++)
  {
    insertions[k - 1] = clients[k][client]->allInsertions;

    // cout << "insertion " << k - 1 << ": ";
    // for (int i = 0; i < insertions[k - 1].size(); i++) {
    //   cout << " detour:" << insertions[k - 1][i].detour
    //        << " load: " << insertions[k - 1][i].load;
    // }
    // cout << endl;
  }
  /* Solve it and get the solution */
  // ModelLotSizingPI::solveCPLEX(myA, myB, insertions, netInventoryCost,
  //                              params->penalityCapa, quantities, breakpoints,
  //                              objective);

  // cout << "Client: " << client << " Best cost: " << objective << endl;

  // using DP
  unique_ptr<LotSizingSolver> lotsizingSolver(make_unique<LotSizingSolver>(params, insertions, client));
  bool ok = lotsizingSolver->solve();

  objective = lotsizingSolver->objective;
  quantities = lotsizingSolver->quantities;

  // if (abs(objective - lotsizingSolver->objective) > 0.01) {
  //   cout << "cplex objective: " << objective
  //        << "DP objective: " << lotsizingSolver->objective << endl
  //        << endl;

  //   exit(EXIT_FAILURE);
  // }

  // cout << "------------------------------" << endl;

  /* APPLYING THE MOVEMENT */
  // Later on we will verify whether it's an improving move or not to trigger a
  // good termination.

  // First, removing all occurences of the node.
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    noeudTravail = clients[k][client];
    if (noeudTravail->estPresent)
      removeNoeud(noeudTravail);
    demandPerDay[k][client] = 0.;
  }

  // Then looking at the solution of the model and inserting in the good place
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    if (breakpoints[k - 1] != -1 && quantities[k - 1] > 0.0001) // don't forget that in the model the index
                                                                // goes from 0 to t-1
    {
      // if (quantities[k-1] < 0.0001)
      //{
      //	cout << "ISSUE : ILP suggests inserting a visit with 0 quantity"
      //<< endl ;
      //	throw ("ISSUE : ILP suggests inserting a visit with 0
      // quantity");
      //}
      demandPerDay[k][client] = quantities[k - 1];
      clients[k][client]->placeInsertion = lotsizingSolver->breakpoints[k - 1]->place;
      // insertions[k - 1][breakpoints[k - 1]].place;
      addNoeud(clients[k][client]);

      // double re_obj = evaluateCurrentCost(client);
      // // print solution
      // cout << "client: " << client << " re-obj: " << re_obj << " obj: " << objective << endl;
      // cout << "day: " << k << " quantity: " << quantities[k - 1] << " route: " << clients[k][client]->placeInsertion->route->cour;
      // cout << " in route: " << clients[k][client]->placeInsertion->cour << endl;
    }
  }

  double tmpCost = evaluateCurrentCost(client);

  if (neq(tmpCost, objective))
  {
    double cost = lotsizingSolver->F[2]->cost(47.5);
    cout << "!!!!!!!!!!!!!!INCONSISTENT!!!!!!!!!!!! " << tmpCost << "<>" << objective << " | " << tmpCost - objective << endl;
    exit(EXIT_FAILURE);
  }

  // if (currentCost > 0.001 && objective > currentCost + 0.001) {
  //   cout << "CLIENT : " << client << " | currentCost: " << currentCost
  //        << " objective: " << objective << endl;
  //   exit(EXIT_FAILURE);
  // }

  if (objective < currentCost - 0.0001) // An improving move has been found,
                                        // the search is not finished.
  {
    // cout << "Objective: " << objective << "| current cost: " << currentCost << endl;
    rechercheTerminee = false;
    return 1;
  }
  else
    return 0;
}

double LocalSearch::evaluateCurrentCost(int client)
{
  Noeud *noeudClient;
  double myCost = 0.;

  // Sum up the detour cost, inventory cost, and eventual excess of capacity
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    noeudClient = clients[k][client];
    if (noeudClient->estPresent)
    {
      // adding the inventory cost
      myCost +=
          (params->cli[client].inventoryCost - params->inventoryCostSupplier) *
          (double)(params->ancienNbDays + 1 - k) * demandPerDay[k][client];
      // cout << "myCost 1: " << myCost;
      // the detour cost
      myCost +=
          params->timeCost[noeudClient->cour][noeudClient->suiv->cour] +
          params->timeCost[noeudClient->pred->cour][noeudClient->cour] -
          params->timeCost[noeudClient->pred->cour][noeudClient->suiv->cour];

      // cout << "myCost 2: " << myCost;
      // and the possible excess capacity
      myCost += params->penalityCapa *
                (std::max<double>(0., noeudClient->route->charge -
                                          noeudClient->route->vehicleCapacity) -
                 std::max<double>(0., noeudClient->route->charge -
                                          noeudClient->route->vehicleCapacity -
                                          demandPerDay[k][client]));
      // cout << "myCost 3: " << myCost;
    }
  }
  return myCost;
}

// Evaluates the current objective function of the whole solution
double LocalSearch::evaluateSolutionCost()
{
  double myCost = 0.;

  // Summing distance and load penalty
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    for (int r = 0; r < params->nombreVehicules[k]; r++)
    {
      myCost += routes[k][r]->temps;
      myCost += params->penalityCapa *
                std::max<double>(
                    routes[k][r]->charge - routes[k][r]->vehicleCapacity, 0.);
    }
  }

  // Adding inventory cost
  for (int k = 1; k <= params->ancienNbDays; k++)
    for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++)
      myCost += demandPerDay[k][i] * (params->ancienNbDays + 1 - k) * (params->cli[i].inventoryCost - params->inventoryCostSupplier);

  // And the necessary constants
  myCost += params->objectiveConstant;

  return myCost;
}

// Evaluates the current objective function of the whole solution
void LocalSearch::printInventoryLevels()
{
  double inventoryClientCosts = 0.;
  double inventorySupplyCosts = 0.;
  double routeCosts = 0.;
  double loadCosts = 0.;

  // Summing distance and load penalty
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    for (int r = 0; r < params->nombreVehicules[k]; r++)
    {
      routeCosts += routes[k][r]->temps;
      loadCosts +=
          params->penalityCapa *
          std::max<double>(routes[k][r]->charge - routes[k][r]->vehicleCapacity,
                           0.);
    }
  }

  // Printing customer inventory and computing customer inventory cost
  double inventoryClient;
  for (int i = params->nbDepots; i < params->nbDepots + params->nbClients;
       i++)
  {
    inventoryClient = params->cli[i].startingInventory;
    cout << "CUSTOMER " << i << " bounds (" << params->cli[i].minInventory
         << "," << params->cli[i].maxInventory << ") ";
    for (int k = 1; k <= params->nbDays; k++)
    {
      // print the level in the morning
      cout << "[" << inventoryClient;
      // print the level after receiving inventory
      inventoryClient += demandPerDay[k][i];
      cout << "," << inventoryClient;
      // print the level after consumption
      inventoryClient -= params->cli[i].dailyDemand[k];
      cout << "," << inventoryClient << "] ";

      inventoryClientCosts += inventoryClient * params->cli[i].inventoryCost;
    }
    cout << endl;
  }

  double inventorySupply = 0;
  cout << "SUPPLIER    ";
  for (int k = 1; k <= params->nbDays; k++)
  {
    inventorySupply += params->availableSupply[k];
    // print the level in the morning
    cout << "[" << inventorySupply << ",";
    for (int i = params->nbDepots; i < params->nbDepots + params->nbClients;
         i++)
      inventorySupply -= demandPerDay[k][i];
    // print the level after delivery
    cout << inventorySupply << "] ";
    inventorySupplyCosts += inventorySupply * params->inventoryCostSupplier;
  }
  cout << endl;

  cout << "COST SUMMARY : ROUTE " << routeCosts << " | LOAD " << loadCosts
       << " | SUPPLY " << inventorySupplyCosts << " | CLIENT "
       << inventoryClientCosts << endl;
  cout << "COST SUMMARY : OVERALL "
       << routeCosts + loadCosts + inventorySupplyCosts + inventoryClientCosts
       << endl;
}

// supprime le noeud
void LocalSearch::removeNoeud(Noeud *U)
{
  // mettre a jour les noeuds
  U->pred->suiv = U->suiv;
  U->suiv->pred = U->pred;
  U->route->updateRouteData();

  // on g�re les autres structures de donn�es
  removeOP(U->jour, U->cour);
  U->estPresent = false;

  // signifier que les insertions sur cette route ne sont plus bonnes
  U->route->initiateInsertions();

  // signifier que pour ce jour les insertions de noeuds ne sont plus bonnes
  for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++)
    clients[U->jour][i]->coutInsertion = 1.e30;
}

void LocalSearch::addNoeud(Noeud *U)
{
  U->placeInsertion->suiv->pred = U;
  U->pred = U->placeInsertion;
  U->suiv = U->placeInsertion->suiv;
  U->placeInsertion->suiv = U;

  // et mettre a jour les routes
  U->route = U->placeInsertion->route;
  U->route->updateRouteData();

  // on g�re les autres structures de donn�es
  addOP(U->jour, U->cour);
  U->estPresent = true;

  // signifier que les insertions sur cette route ne sont plus bonnes
  U->route->initiateInsertions();

  // signifier que pour ce jour les insertions de noeuds ne sont plus bonnes
  for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++)
    clients[U->jour][i]->coutInsertion = 1.e30;
}

// calcule pour un jour donn� et un client donn� (repr�sent� par un noeud)
// les couts d'insertion dans les differentes routes constituant ce jour
void LocalSearch::computeCoutInsertion(Noeud *client)
{
  Route *myRoute;
  client->allInsertions.clear();

  // for each route of this day
  for (int r = 0; r < (int)routes[client->jour].size(); r++)
  {
    // later on we can simply retrieve
    // calculate the best insertion point as well as its load
    myRoute = routes[client->jour][r];
    myRoute->evalInsertClient(client);
    client->allInsertions.push_back(myRoute->bestInsertion[client->cour]);
  }

  // eliminate dominated insertions
  client->removeDominatedInsertions(params->penalityCapa);
}

void LocalSearch::shaking()
{
  updateMoves(); // shuffles the order of the customers in each day in the
  // table "ordreParcours"

  // For each day, perform one random swap
  int nbRandomSwaps = 1;
  for (int k = 1; k <= params->nbDays; k++)
  {
    if (ordreParcours[k].size() >
        2) // if there are more than 2 customers in the day
    {
      for (int nSwap = 0; nSwap < nbRandomSwaps; nSwap++)
      {
        // Select the two customers to be swapped
        int client1 = ordreParcours[k][params->rng->genrand64_int63() %
                                       ordreParcours[k].size()];
        int client2 = client1;
        while (client2 == client1)
          client2 = ordreParcours[k][params->rng->genrand64_int63() %
                                     ordreParcours[k].size()];

        // Perform the swap
        Noeud *noeud1 = clients[k][client1];
        Noeud *noeud2 = clients[k][client2];

        // If the nodes are not identical or consecutive (TODO : check why
        // consecutive is a problem in the function swap)
        if (client1 != client2 &&
            !(noeud1->suiv == noeud2 || noeud1->pred == noeud2))
        {
          // cout << "SWAP " << client1 << " " << client2 << " " << k << endl ;
          swapNoeud(noeud1, noeud2);
        }
      }
    }
  }

  // Take one customer, and put it back to the days corresponding to the best
  // lot sizing (without detour cost consideration)
  int nbRandomLotOpt = 2;
  Noeud *noeudTravail;
  for (int nLotOpt = 0; nLotOpt < nbRandomLotOpt; nLotOpt++)
  {
    // Choose a random customer
    int client =
        params->nbDepots + params->rng->genrand64_int63() % params->nbClients;

    // Remove all occurences of this customer
    for (int k = 1; k <= params->ancienNbDays; k++)
    {
      noeudTravail = clients[k][client];
      if (noeudTravail->estPresent)
        removeNoeud(noeudTravail);
      demandPerDay[k][client] = 0.;
    }

    // Find the best days of insertion (Lot Sizing point of view)
    vector<double> insertionQuantity;
    // ModelLotSizingPI::bestInsertionLotSizing(client, insertionQuantity, params);

    // And insert in the good days after a random customer
    // Then looking at the solution of the model and inserting in the good place
    for (int k = 1; k <= params->ancienNbDays; k++)
    {
      if (insertionQuantity[k - 1] > 0.0001) // don't forget that in the model
                                             // the index goes from 0 to t-1
      {
        demandPerDay[k][client] = insertionQuantity[k - 1];

        // If the day is not currently empty
        if (ordreParcours[k].size() >
            0) // place after a random existing customer
          clients[k][client]->placeInsertion =
              clients[k][ordreParcours[k][params->rng->genrand64_int63() %
                                          ordreParcours[k].size()]];
        else // place after a depot
          clients[k][client]->placeInsertion = depots[k][0];

        addNoeud(clients[k][client]);
      }
    }
  }
}

// constructeur
LocalSearch::LocalSearch(void) {}

// constructeur 2
LocalSearch::LocalSearch(Params *params, Individu *individu)
    : params(params), individu(individu)
{
  vector<Noeud *> tempNoeud;
  vector<Route *> tempRoute;
  vector<bool> tempB2;
  vector<vector<bool>> tempB;
  vector<vector<int>> temp;
  vector<int> temp2;
  vector<vector<paireJours>> tempPair;
  vector<paireJours> tempPair2;
  Noeud *myDepot;
  Noeud *myDepotFin;
  Route *myRoute;

  clients.push_back(tempNoeud);
  depots.push_back(tempNoeud);
  depotsFin.push_back(tempNoeud);
  routes.push_back(tempRoute);

  for (int kk = 1; kk <= params->nbDays; kk++)
  {
    clients.push_back(tempNoeud);
    depots.push_back(tempNoeud);
    depotsFin.push_back(tempNoeud);
    routes.push_back(tempRoute);
    // dimensionnement du champ noeuds a la bonne taille
    for (int i = 0; i < params->nbDepots; i++)
      clients[kk].push_back(NULL);
    for (int i = params->nbDepots; i < params->nbClients + params->nbDepots;
         i++)
      clients[kk].push_back(
          new Noeud(false, i, kk, false, NULL, NULL, NULL, 0));
    // dimensionnement du champ depots et routes � la bonne taille
    for (int i = 0; i < params->nombreVehicules[kk]; i++)
    {
      myDepot = new Noeud(true, params->ordreVehicules[kk][i].depotNumber, kk,
                          false, NULL, NULL, NULL, 0);
      myDepotFin = new Noeud(true, params->ordreVehicules[kk][i].depotNumber,
                             kk, false, NULL, NULL, NULL, 0);
      myRoute = new Route(
          i, kk, myDepot, 0, 0, params->ordreVehicules[kk][i].maxRouteTime,
          params->ordreVehicules[kk][i].vehicleCapacity, params, this);
      myDepot->route = myRoute;
      myDepotFin->route = myRoute;
      routes[kk].push_back(myRoute);
      depots[kk].push_back(myDepot);
      depotsFin[kk].push_back(myDepotFin);
    }
  }

  // initialisation de la structure ordreParcours
  for (int day = 0; day <= params->nbDays; day++)
    ordreParcours.push_back(temp2);

  for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++)
    ordreParcours[0].push_back(i);

  // initialisation de la structure ordreJours
  for (int day = 1; day <= params->nbDays; day++)
    ordreJours.push_back(day);
}

// destructeur
LocalSearch::~LocalSearch(void)
{
  if (!clients.empty())
    for (int i = 0; i < (int)clients.size(); i++)
      if (!clients[i].empty())
        for (int j = 0; j < (int)clients[i].size(); j++)
          delete clients[i][j];

  if (!routes.empty())
    for (int i = 0; i < (int)routes.size(); i++)
      if (!routes[i].empty())
        for (int j = 0; j < (int)routes[i].size(); j++)
          delete routes[i][j];

  if (!depots.empty())
    for (int i = 0; i < (int)depots.size(); i++)
      if (!depots[i].empty())
        for (int j = 0; j < (int)depots[i].size(); j++)
          delete depots[i][j];
}
