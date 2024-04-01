#include "LocalSearch.h"
#include <algorithm>
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

// lance la recherche locale启动本地搜索
void LocalSearch::runSearchTotal(bool isRepPhase)
{
 
  this->isRepPhase = isRepPhase;
  int nbMoves = 0;
  int nbTotal = 0;
  int nbPhases = 0;
  bool traces = false;

  // LANCEMENT DE LA RECHERCHE
  if (traces)cout << "COST INITIAL " << evaluateSolutionCost() << endl;

  // reorganisation des plans de transport pour chaque jour
  updateMoves();
  for (int day = 1; day <= params->nbDays; day++)
    nbMoves += mutationSameDay(day);
  nbTotal += nbMoves;
  nbPhases++;
  if (traces)
    cout << "COST AFTER RI 1. " << (nbPhases + 1) / 2 << " : "
         << evaluateSolutionCost() << endl;
  
  // reorganisation des jours
  if (nbPhases < params->maxLSPhases)
  {
    
    nbMoves = mutationDifferentDay();
    nbTotal += nbMoves;
    nbPhases++;
    if (traces){
      cout << "COST AFTER PI 1. " << (nbPhases + 1) / 2 << " : "
           << evaluateSolutionCost() << endl;
    }
   
  }
  if(traces)  cout <<"nbMoves "<<nbMoves<<" phases "<<nbPhases<<" mazls " <<params->maxLSPhases<<endl;

  while (nbMoves > 0 && nbPhases < params->maxLSPhases)
  {
    //cout <<"nbMOves"<< nbMoves;
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
  //printInventoryLevels();
  // cout << endl << endl ;
}






void LocalSearch::runSearchTotalprint(bool isRepPhase)
{
 
  this->isRepPhase = isRepPhase;
  int nbMoves = 0;
  int nbTotal = 0;
  int nbPhases = 0;
  bool traces = false;

  // LANCEMENT DE LA RECHERCHE
  if (traces)cout << "COST INITIAL " << evaluateSolutionCost() << endl;

  // reorganisation des plans de transport pour chaque jour
  updateMoves();
  for (int day = 1; day <= params->nbDays; day++)
    nbMoves += mutationSameDay(day);
  nbTotal += nbMoves;
  nbPhases++;
  if (traces)
    cout << "COST AFTER RI 1. " << (nbPhases + 1) / 2 << " : "
         << evaluateSolutionCost() << endl;
  
  // reorganisation des jours
  if (nbPhases < params->maxLSPhases)
  {
    
    nbMoves = mutationDifferentDayprint();
    nbTotal += nbMoves;
    nbPhases++;
    if (traces){
      cout << "COST AFTER PI 1. " << (nbPhases + 1) / 2 << " : "
           << evaluateSolutionCost() << endl;
    }
   
  }
  if(traces)  cout <<"nbMoves "<<nbMoves<<" phases "<<nbPhases<<" mazls " <<params->maxLSPhases<<endl;

  while (nbMoves > 0 && nbPhases < params->maxLSPhases)
  {
    //cout <<"nbMOves"<< nbMoves;
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
      //cout <<"before"<<endl;
      nbMoves += mutationDifferentDayprint();
     // cout <<"after"<<endl;
      nbTotal += nbMoves;
      nbPhases++;
      if (traces)
        cout << "COST AFTER PI2 " << (nbPhases + 1) / 2 << " : "
             << evaluateSolutionCost() << endl;

     // int a;cin>>a;
    }
  }

  if (traces)
    cout << "COST FINAL : " << evaluateSolutionCost() << endl
         << endl;

  // cout << endl << endl ;
  //printInventoryLevels();
  // cout << endl << endl ;
}


// les tableaux ordreParcours de tous les jours sont r�organis�s al�atoirement是混合路线
//将每天的ordreParcours数组以及ordreJours数组进行随机混洗。我们可以将此视为随机化搜索过程的方法，以探索不同的客户顺序和天数，从而有机会找到更好的解决方案。
int LocalSearch::mutationDifferentDayprint()
{
  //çcout <<"mutation";
  bool traces = false;
  rechercheTerminee = false;
  int nbMoves = 0;
  
    if(traces){
       cout<<"nbClients "<<params->nbClients<<endl;
      for (size_t i = 0; i < ordreParcours.size(); ++i) { // Iterate through each day
        for (size_t j = 0; j < ordreParcours[i].size(); ++j) { // Iterate through each customer
            std::cout <<"!!!!!!!day "<<i<<" client "<<j<< ordreParcours[i][j] << " ";
        }
        std::cout << endl<<endl;
      }
    }
      cout <<"params->nbClients"<<endl;
    for (int posU = 0; posU < params->nbClients; posU++)
      nbMoves += mutation11(ordreParcours[0][posU]);//random每一个client
      
  
  return nbMoves;
}
// change the choices of visit periods and quantity for "client"


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
    /*
    cout <<"day:"<<endl;
    for(int i = 0 ; i < (int)ordreParcours[k].size(); i++)
      cout << ordreParcours [k][i]<<' ';
    int a ;
    cin >>a;
    */
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
//目的是更新每个客户的可能移动的列表，即与该客户相邻的其他客户。
//这是局部搜索策略的一部分，其中考虑了哪些客户可以与当前客户一起进行某种操作或移动。
//最后，函数打乱了邻居和客户的遍历顺序，以增加搜索的随机性。
void LocalSearch::updateMoves()
{
  int client, client2;
  int size;

  for (int k = 1; k <= params->nbDays; k++)
  {
    // pour chaque client present dans ce jour 这一天visit的每位顾客
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
      /*1. 处理与客户noeudU相关的所有可能的移动或变异
首先遍历与客户noeudU相关的所有其他客户noeudV。
对于每一个noeudV，检查是否已经对noeudU和noeudV进行了变异测试。
如果没有进行变异测试或者是第一次循环，则尝试进行多种变异。
变异尝试包括：mutation1()、mutation2()、mutation3()等。
特定的变异如mutation4()和mutation6()只有在noeudU的cour属性小于或等于noeudV的cour属性时才会被尝试。
如果某种变异成功（例如，如果moveEffectue被设置为1），则重置noeudU和noeudV的移动。*/
      for (int posV = 0; posV < size2 && moveEffectue == 0; posV++) //会遍历与客户noeudU相关的所有可能的移动或变异。
      {
        noeudV = clients[day][noeudU->moves[posV]]; //选择一个与noeudU相关的客户noeudV进行可能的变异。
        if (!noeudV->route->nodeAndRouteTested[noeudU->cour] ||
            !noeudU->route->nodeAndRouteTested[noeudU->cour] || firstLoop)
        {
          noeudVPred = noeudV->pred;//表示节点noeudV的前一个节点。这通常是在双向链表结构中用来找到给定节点的前一个节点。
          y = noeudV->suiv; //，表示节点noeudV的下一个节点。这通常是在链表结构中用来找到给定节点的下一个节点
          noeudYSuiv = y->suiv; //y的下一个节点，并将其赋给变量noeudYSuiv。
          ySuivCour = y->suiv->cour;//获取节点y的下一个节点的cour属性，并将其赋给ySuivCour。
          routeV = noeudV->route;//获取noeudV的路线（可能是它所在的路径或集合）并将其赋给routeV
          noeudVCour = noeudV->cour;//获取noeudV的cour属性，并将其赋给noeud
          noeudVPredCour = noeudVPred->cour;//获取noeudVPred（即noeudV的前一个节点）的cour属性，并将其赋给noeudVPredCour。
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

    
      /*
      2. 如果noeudU与当天的某个仓库存在关系，尝试进行其他变异
检查noeudU与当天的第一个仓库之间是否存在关联。
如果存在关联并且尚未进行移动，则遍历当天的所有仓库。
对于每一个仓库，检查是否已经对noeudU和该仓库进行了变异测试。
如果没有进行变异测试或者是第一次循环，则尝试进行mutation1()、mutation2()和mutation3()变异。
特别地，如果noeudV的下一个节点不是仓库，还将尝试进行mutation8()和mutation9()。
如果某种变异成功，则重置noeudU和仓库的移动。*/

  // c'est un d�pot on tente l'insertion derriere le depot de ce jour
      // si il ya corr�lation检
      if (params->isCorrelated1[noeudU->cour][depots[day][0]->cour] &&
          moveEffectue != 1)
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

  bool traces = false;
  rechercheTerminee = false;
  int nbMoves = 0;
  int times = 0;
  while(!rechercheTerminee){
    
    if(traces){
       cout<<"nbClients "<<params->nbClients<<endl;
      for (size_t i = 0; i < ordreParcours.size(); ++i) { // Iterate through each day
        for (size_t j = 0; j < ordreParcours[i].size(); ++j) { // Iterate through each customer
            std::cout <<"!!!!!!!day "<<i<<" client "<<j<< ordreParcours[i][j] << " ";
        }
        std::cout << endl<<endl;
      }
    }
     rechercheTerminee = true;
    
    for (int posU = 0; posU < params->nbClients; posU++){
      times++;
      //cout << "client: "<<ordreParcours[0][posU]<<endl;
      //int a;cin>>a;
      nbMoves += mutation11(ordreParcours[0][posU]);//random每一个client
    }
    if(times >= params->nbClients*10) break;
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
  bool trace = false;
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
  bool trace = false, traces = false;

  if(traces) cout <<"client: "<<client<<endl;

  Noeud *noeudTravail;
  double currentCost;
  // First, make sure all insertion costs are computed
  for (int k = 1; k <= params->ancienNbDays; k++){
    noeudTravail = clients[k][client]; //node* day k client
    computeCoutInsertion(noeudTravail); // detour,place (dominated) for each route
  }
  // Compute the current lot sizing solution cost (from the model point of view)
  //before optimizatio  currentCost = evaluateCurrentCost(client);
  if(params -> isstockout){
    currentCost=evaluateCurrentCost_stockout(client);
    if(traces)  cout <<"current: "<<currentCost<<endl;
  }

  else
    currentCost = evaluateCurrentCost(client);
  /* Generate the structures of the subproblem */
  
  vector<vector<Insertion>> insertions = vector<vector<Insertion>>(params->nbDays);
  vector<double> quantities = vector<double>(params->nbDays);
  vector<int> breakpoints = vector<int>(params->nbDays);
  double objective;
  for (int k = 1; k <= params->nbDays; k++)
  {
    insertions[k - 1] = clients[k][client]->allInsertions;
  }
  
  

  // using DP使用LotSizingSolver（批量调整求解器）对子问题进行求解。获取目标值和数量解决方案。
  unique_ptr<LotSizingSolver> lotsizingSolver(
      make_unique<LotSizingSolver>(params, insertions, client));

  //int a;cin>>a;
  bool ok = true;
  if(params-> isstockout) ok = lotsizingSolver->solve_stockout();

  else ok = lotsizingSolver->solve();

  objective = lotsizingSolver->objective;
  quantities = lotsizingSolver->quantities;
  if(lt(currentCost,objective-0.01)) return 0;


  /* APPLYING THE MOVEMENT */
  // Later on we will verify whether it's an improving move or not to trigger a
  // good termination.

  // First, removing all occurences of the node.
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    noeudTravail = clients[k][client];
    if (noeudTravail->estPresent){
      
      removeNoeud(noeudTravail);
      //if(trace) cout<<"which day,cli:( "<<k <<" "<<client<<endl;
    }
    demandPerDay[k][client] = 0.;

  }

  // Then looking at the solution of the model and inserting in the good place
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    if (quantities[k - 1] > 0.0001 || (lotsizingSolver->breakpoints[k - 1]&&gt(0,lotsizingSolver->breakpoints[k - 1]->detour) )) // don't forget that in the model the index      // goes from 0 to t-1
    {
      
      demandPerDay[k][client] = round(quantities[k - 1]);
      
      if(trace){
        //cout <<"day "<<k<<endl; lotsizingSolver->breakpoints[k - 1]->print();
      }
      clients[k][client]->placeInsertion = lotsizingSolver->breakpoints[k - 1]->place;
      // insertions[k - 1][breakpoints[k - 1]].place;
 
      addNoeud(clients[k][client]);
      
            // double re_obj = evaluateCurrentCost(client);
      // // print solution
      // cout << "client: " << client << " re-obj: " << re_obj << " obj: " << objective << endl;
      if(trace)  cout << "!day: " << k << " quantity: " << quantities[k - 1] << " route: " << clients[k][client]->placeInsertion->route->cour;
      if(trace)  cout << " !in route place: " << clients[k][client]->placeInsertion->cour << endl;
    }
  }

  double tmpCost = 0.0;
  if(params -> isstockout)
     tmpCost = evaluateCurrentCost_stockout(client);
  else
    tmpCost = evaluateCurrentCost(client);
  if(traces) cout <<tmpCost<<endl;
  if (fabs(tmpCost- objective)>0.01  )
    return 0;
  if ( currentCost-objective >=0.01 )// An improving move has been found,
                                        // the search is not finished.
  {
    //cout <<"client "<<client<<endl;
   // cout << "Objective: " << objective << "| current cost: " << currentCost << " | tmpCost"<<tmpCost<<endl;
    rechercheTerminee = false;
    return 1;
  }
  else
    return 0;
}
//的目的是为了计算特定客户的当前解决方案的成本
//评估了特定客户当前解决方案的总成本，基于库存、绕行和超出容量的成本。

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
      // adding the inventory cost库存成本:根据公式计算库存成本并累加到myCost。
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
  bool trace =false; //true;
  if (params ->isstockout == true){
    for (int k = 1; k <= params->ancienNbDays; k++){
      for (int r = 0; r < params->nombreVehicules[k]; r++){
        if(trace) cout <<" routes[k][r]->temps "<<routes[k][r]->vehicleCapacity<<" routes[k][r]->charge "<<routes[k][r]->charge<<endl;
        myCost += routes[k][r]->temps;
        myCost += params->penalityCapa * std::max<double>(
                      routes[k][r]->charge - routes[k][r]->vehicleCapacity, 0.);
      }
    }
     // And the necessary constants (inventory cost on depot only )
    myCost += params->objectiveConstant_stockout;
    if(trace) cout <<params->objectiveConstant_stockout<<endl;
        
    vector  <double> I(params->nbDepots + params->nbClients);
    for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++) {
      I[i] = params->cli[i].startingInventory; if(trace) cout <<"Istart "<<I[i]<<endl;
    }
      
    // Adding inventory cost
    for (int k = 1; k <= params->ancienNbDays; k++)
      for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++) // all the customers
      {
        //inventory cost at customer i 
        if(trace) cout <<"rest inventory "<<std::max<double>(0, I[i] + demandPerDay[k][i]- params->cli[i].dailyDemand[k]) <<endl;
        myCost += std::max<double>(0, I[i] + demandPerDay[k][i]- params->cli[i].dailyDemand[k]) 
                  * params->cli[i].inventoryCost;
        
        // minus depot holding cost from constant value 
        if(trace) cout <<"  quantity "<<demandPerDay[k][i]  <<endl;
        
        myCost -= demandPerDay[k][i] * (params->ancienNbDays + 1 - k) 
                  * params->inventoryCostSupplier;
        
        //stock-out penalty
        if(trace) cout <<"stockout "<<std::max<double> (0,  params->cli[i].dailyDemand[k]-demandPerDay[k][i]-I[i])  <<endl;
        
        myCost += std::max<double> (0,  params->cli[i].dailyDemand[k]-demandPerDay[k][i]-I[i]) 
                  * params->cli[i].stockoutCost;    
         
        if(trace) cout <<"I: "<<std::max<double>(0, I[i] + demandPerDay[k][i]- params->cli[i].dailyDemand[k])<<endl;
        I[i] = std::max<double>(0, I[i] + demandPerDay[k][i]- params->cli[i].dailyDemand[k]);
      }
   
    if(trace) cout <<"cost: "<<myCost<<endl;
    return myCost;
  }
  //******************************************************************************

  else{
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
        for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++) // all the customers
          myCost += demandPerDay[k][i] * (params->ancienNbDays + 1 - k) *
                    (params->cli[i].inventoryCost - params->inventoryCostSupplier);

      // And the necessary constants
      myCost += params->objectiveConstant;

      return myCost;
  }
  
}



// Evaluates the current objective function of the whole solution
void LocalSearch::printInventoryLevels(std::ostream& file,bool add)
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
      routeCosts += routes[k][r]->temps; // temps: total travel time
      
      if(!add)  file <<"day["<<k<<"] route["<<r<<"]: travel time = "<<routes[k][r]->temps<<endl;
      routes[k][r]->printRouteData(file);
      loadCosts +=
          params->penalityCapa *
          std::max<double>(routes[k][r]->charge - routes[k][r]->vehicleCapacity,
                           0.);
    }
  }

  // Printing customer inventory and computing customer inventory cost
  if(params->isstockout){

    double inventoryClient;
    for (int i = params->nbDepots; i < params->nbDepots + params->nbClients;
         i++)
    {
      inventoryClient = params->cli[i].startingInventory;
      if(!add) file  << "CUSTOMER " << i << " bounds (" << params->cli[i].minInventory
           << "," << params->cli[i].maxInventory << ") ";
      for (int k = 1; k <= params->nbDays; k++)
      {
        // print the level in the morning
        if(!add) file << "[morning: " << inventoryClient;
        // print the level after receiving inventory
        inventoryClient += demandPerDay[k][i];
        if(!add) file  << " ,replinishment: " << demandPerDay[k][i];
        // print the level after consumption
        double stock = std::max<double>(0,params->cli[i].dailyDemand[k]-inventoryClient);
        inventoryClient = std::max<double>(0,inventoryClient-params->cli[i].dailyDemand[k]);
        
        if(!add) file  << ", everning: " << inventoryClient << "] ";

        inventoryClientCosts += inventoryClient * params->cli[i].inventoryCost ;
        inventoryClientCosts += stock*params->cli[i].stockoutCost;
      }
      if(!add) file  << endl;
    }
  }
  else{
    double inventoryClient;
    for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++)
    {
      inventoryClient = params->cli[i].startingInventory;
      if(!add) file  << "CUSTOMER " << i << " bounds (" << params->cli[i].minInventory
           << "," << params->cli[i].maxInventory << ") ";
      for (int k = 1; k <= params->nbDays; k++)
      {
        // print the level in the morning
        if(!add) file  << "[" << inventoryClient;
        // print the level after receiving inventory
        inventoryClient += demandPerDay[k][i];
        if(!add) file  << "," << inventoryClient;
        // print the level after consumption
        inventoryClient -= params->cli[i].dailyDemand[k];
        if(!add) file << "," << inventoryClient << "] ";

        inventoryClientCosts += inventoryClient * params->cli[i].inventoryCost;
      }
      if(!add) file  << endl;
    }
  }
  

  double inventorySupply = 0;
  if(!add) file  << "SUPPLIER    ";
  for (int k = 1; k <= params->nbDays; k++)
  {
    inventorySupply += params->availableSupply[k];
    // print the level in the morning
    if(!add) file  << "[" << inventorySupply << ",";
    for (int i = params->nbDepots; i < params->nbDepots + params->nbClients;
         i++)
      inventorySupply -= demandPerDay[k][i];
    // print the level after delivery
    if(!add) file  << inventorySupply << "] ";
    inventorySupplyCosts += inventorySupply * params->inventoryCostSupplier;
  }
  if(!add) file  << endl;

  file  << "COST SUMMARY : ROUTE " << routeCosts << " | LOAD " << loadCosts
       << " | SUPPLY " << inventorySupplyCosts << " | CLIENT "
       << inventoryClientCosts << endl;
  file  << "COST SUMMARY : OVERALL "
       << routeCosts + loadCosts + inventorySupplyCosts + inventoryClientCosts
       << endl;
}

// supprime le noeud删除节点 remove node
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
{// 将节点 U 插入到其指定的插入位置。
  U->placeInsertion->suiv->pred = U;// 将U的后继节点的前驱设置为U。
  U->pred = U->placeInsertion;// 设置U的前驱为其插入位置的节点。
  U->suiv = U->placeInsertion->suiv;// 将U的后继设置为插入位置后的节点。
  U->placeInsertion->suiv = U;// 将插入位置的后继设置为U。

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
  /*这个函数为给定的客户和给定的天计算在不同路线中的插入成本。
  首先，清除所有的插入。对于当天的每条路线，计算最佳插入点及其负载。
  之后，从列表中消除被支配的插入。*/
  bool traces = false;//true;
  Route *myRoute;
  client->allInsertions.clear();
  //cout<<"client->jour"<<client->jour<<endl;
  // for each route of this day
  for (int r = 0; r < (int)routes[client->jour].size(); r++){
    // later on we can simply retrieve
    // calculate the best insertion point as well as its load

    myRoute = routes[client->jour][r];
    myRoute->evalInsertClient(client); //估将客户节点 client 插入到路线 myRoute 中的最佳位置。
    //bestInsertion 是一个映射（或数组），它为每个客户节点 U 存储了最佳插入信息。
    //这包括插入的成本（detour）、插入位置的节点（place）以及插入的负载（load）。
    client->allInsertions.push_back(myRoute->bestInsertion[client->cour]);
    if(traces)
    { cout<<"detour,load"<<endl;
      cout << myRoute->bestInsertion[client->cour].detour << " "<<myRoute->bestInsertion[client->cour].load <<endl<<endl<<endl;
    }
    
  }

  // eliminate dominated insertions
  client->removeDominatedInsertions(params->penalityCapa);
}

double LocalSearch::evaluateCurrentCost_stockout(int client)
{
  bool trace = false;
  Noeud *noeudClient;
  double myCost = 0.;
  double I = params->cli[client].startingInventory;
  // Sum up the detour cost, inventory cost, and eventual excess of capacity
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
    if(trace) cout <<"->day "<<k<<endl;
    noeudClient = clients[k][client];
    if (noeudClient->estPresent){
      // adding the inventory cost库存成本:根据公式计算库存成本并累加到myCost。
        if(trace) cout <<"holding cost: "<<params->cli[client].inventoryCost <<" I= "<< I<<" q="<<demandPerDay[k][client]<<" demand = "<<params->cli[client].dailyDemand[k]<<endl;
        myCost +=
          params->cli[client].inventoryCost * 
          std::max<double> (0., I+demandPerDay[k][client]-params->cli[client].dailyDemand[k]);
      //stockout
        if(trace) cout <<"stckout cost: "<<params->cli[client].stockoutCost * std::max<double> (0., -I-demandPerDay[k][client]+params->cli[client].dailyDemand[k])<<endl;
        myCost +=
          params->cli[client].stockoutCost * std::max<double> (0., -I-demandPerDay[k][client]+params->cli[client].dailyDemand[k]);
      
      //-supplier *q[]
        if(trace) cout <<"supplier minus"<<params->inventoryCostSupplier *
            (double)(params->ancienNbDays + 1 - k) * demandPerDay[k][client]<<endl;
        myCost -=  params->inventoryCostSupplier *
            (double)(params->ancienNbDays + 1 - k) * demandPerDay[k][client];
      // cout << "myCost 1: " << myCost;

      // the detour cost
        myCost +=
            params->timeCost[noeudClient->cour][noeudClient->suiv->cour] +
            params->timeCost[noeudClient->pred->cour][noeudClient->cour] -
            params->timeCost[noeudClient->pred->cour][noeudClient->suiv->cour];
        if(trace) cout<<"detour : "<< params->timeCost[noeudClient->cour][noeudClient->suiv->cour] +
            params->timeCost[noeudClient->pred->cour][noeudClient->cour] -
            params->timeCost[noeudClient->pred->cour][noeudClient->suiv->cour]<<endl;
      // cout << "myCost 2: " << myCost;
      // and the possible excess capacity, the privous penalty are calculated already.
        double x1 = noeudClient->route->charge -  noeudClient->route->vehicleCapacity;
        if(eq(x1,0)) x1 = 0;
        double x2=noeudClient->route->charge -
                  noeudClient->route->vehicleCapacity - demandPerDay[k][client];
        if(eq(x2,0)) x2 = 0;
        myCost += params->penalityCapa *(std::max<double>(0., x1) - std::max<double>(0., x2));
        
        if(trace) cout<<"possible penalty : "<< params->penalityCapa *
                (std::max<double>(0., noeudClient->route->charge -
                                          noeudClient->route->vehicleCapacity) -
                 std::max<double>(0., noeudClient->route->charge -
                                          noeudClient->route->vehicleCapacity -
                                          demandPerDay[k][client]))<<endl;
        I = std::max<double> (0., I+demandPerDay[k][client]-params->cli[client].dailyDemand[k]);
        // cout << "myCost 3: " << myCost;
      }
      else{ 
        myCost += params->cli[client].inventoryCost *  std::max<double>(0., I-params->cli[client].dailyDemand[k]);
        myCost += params->cli[client].stockoutCost * std::max<double>  (0., -I+params->cli[client].dailyDemand[k]);

        I = std::max<double> (0., I-params->cli[client].dailyDemand[k]);
        
      }
     if(trace) cout <<"mycost: "<<myCost<<endl;
  }
  return myCost;
}

double LocalSearch::evaluateCurrentCost_p(int client)
{
  bool trace = true;
  Noeud *noeudClient;
  double myCost = 0.;
  double I = params->cli[client].startingInventory;
  // Sum up the detour cost, inventory cost, and eventual excess of capacity
  for (int k = 1; k <= params->ancienNbDays; k++)
  {
        if(trace) cout <<"->day "<<k<<endl;
    noeudClient = clients[k][client];
    if (noeudClient->estPresent){
      // adding the inventory cost库存成本:根据公式计算库存成本并累加到myCost。
          if(trace) cout <<"holding cost: "<<params->cli[client].inventoryCost <<" I= "<< I<<" q="<<demandPerDay[k][client]<<" demand = "<<params->cli[client].dailyDemand[k]<<endl;
        double h1 = I+demandPerDay[k][client]-params->cli[client].dailyDemand[k];
        if(eq(h1,0))  h1 = 0;
        myCost +=  params->cli[client].inventoryCost *   std::max<double> (0.,h1 );
      //stockout
          if(trace) cout <<"stckout cost: "<<params->cli[client].stockoutCost * std::max<double> (0., -I-demandPerDay[k][client]+params->cli[client].dailyDemand[k])<<endl;
        double s1=-I-demandPerDay[k][client]+params->cli[client].dailyDemand[k];
        if( eq(s1,0))  s1=0;
        myCost +=   params->cli[client].stockoutCost * std::max<double> (0., s1);
      
      //-supplier *q[]
            if(trace) cout <<"supplier minus"<<params->inventoryCostSupplier *
                  (double)(params->ancienNbDays + 1 - k) * demandPerDay[k][client]<<endl;
      
      myCost -=  params->inventoryCostSupplier *
            (double)(params->ancienNbDays + 1 - k) * demandPerDay[k][client];
      // cout << "myCost 1: " << myCost;

      // the detour cost
      cout<<"oo mycost "<<myCost<<endl;
        myCost +=  (params->timeCost[noeudClient->cour][noeudClient->suiv->cour] +
            params->timeCost[noeudClient->pred->cour][noeudClient->cour] -
            params->timeCost[noeudClient->pred->cour][noeudClient->suiv->cour]);
        if(trace) cout<<"detour : "<< params->timeCost[noeudClient->cour][noeudClient->suiv->cour] +
            params->timeCost[noeudClient->pred->cour][noeudClient->cour] -
            params->timeCost[noeudClient->pred->cour][noeudClient->suiv->cour]<<"myCost = "<<myCost<<endl;
      // cout << "myCost 2: " << myCost;
      // and the possible excess capacity, the privous penalty are calculated already.
      double x1 = noeudClient->route->charge -  noeudClient->route->vehicleCapacity;
      if(eq(x1,0)) x1 = 0;
      double x2=noeudClient->route->charge -
                noeudClient->route->vehicleCapacity - demandPerDay[k][client];
      if(eq(x2,0)) x2 = 0;
        myCost += params->penalityCapa *(std::max<double>(0., x1) - std::max<double>(0., x2));
        if(trace) cout<<"possible penalty : "<< params->penalityCapa <<"  "<<
                std::max<double>(0., x1) <<"  "<< std::max<double>(0., x2)<<endl;
        I = std::max<double> (0., I+demandPerDay[k][client]-params->cli[client].dailyDemand[k]);
        // cout << "myCost 3: " << myCost;
      }
      else{ 
        double y1 = I-params->cli[client].dailyDemand[k];
        double y2 = -I+params->cli[client].dailyDemand[k];
        if(eq(y1,0))y1=0;
        if(eq(y2,0))y2=0;
        myCost += params->cli[client].inventoryCost *  std::max<double>(0., y1);
        myCost += params->cli[client].stockoutCost * std::max<double>  (0., y2);
        if(trace) cout<<"stockout "<<-I+params->cli[client].dailyDemand[k]<<"  coststockout "<<params->cli[client].stockoutCost * std::max<double>  (0., -I+params->cli[client].dailyDemand[k])<<endl;
        
        I = std::max<double> (0., I-params->cli[client].dailyDemand[k]);
        
      }
     if(trace) cout <<"mycost: "<<myCost<<endl;
  }
  return myCost;
}

void LocalSearch::computeCoutInsertionp(Noeud *client)
{
  /*这个函数为给定的客户和给定的天计算在不同路线中的插入成本。
  首先，清除所有的插入。对于当天的每条路线，计算最佳插入点及其负载。
  之后，从列表中消除被支配的插入。*/
  bool traces = false;//true;
  Route *myRoute;
  client->allInsertions.clear();
  //cout<<"client->jour"<<client->jour<<endl;
  // for each route of this day
  for (int r = 0; r < (int)routes[client->jour].size(); r++){
    // later on we can simply retrieve
    // calculate the best insertion point as well as its load

    myRoute = routes[client->jour][r];
    myRoute->evalInsertClientp(client); //估将客户节点 client 插入到路线 myRoute 中的最佳位置。
    //bestInsertion 是一个映射（或数组），它为每个客户节点 U 存储了最佳插入信息。
    //这包括插入的成本（detour）、插入位置的节点（place）以及插入的负载（load）。
    client->allInsertions.push_back(myRoute->bestInsertion[client->cour]);
    if(traces)
    { cout<<"detour,load"<<endl;
      cout << myRoute->bestInsertion[client->cour].detour << " "<<myRoute->bestInsertion[client->cour].load <<endl<<endl<<endl;
    }
    
  }

  // eliminate dominated insertions
  client->removeDominatedInsertions(params->penalityCapa);
  //evaluateCurrentCost_p(3);
}



void LocalSearch::shaking()
{
  updateMoves(); // shuffles the order of the customers in each day in the
  // table "ordreParcours"

  // For each day, perform one random swap
  int nbRandomSwaps = 1;
  for (int k = 1; k <= params->nbDays; k++)
  {
    if (ordreParcours[k].size() > 2) // if there are more than 2 customers in the day
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

// constructeur 2  需要提供两个参数：一个Params对象的指针和一个Individu对象的指针。
LocalSearch::LocalSearch(Params *params, Individu *individu)
    : params(params), individu(individu)
{
  // 创建并初始化多个临时向量，如tempNoeud, tempRoute等。这些向量稍后将用于填充类的成员向量。
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
    /*clients[kk]这个向量中。 false: 表示这不是一个仓库，而是一个客户。
          i: 客户的索引或编号。kk: 与这个客户相关的天数。false: 表示在给定的天数中，这个客户是不出现的。
            NULL, NULL, NULL: 这些是后续节点、前驱节点和与这个客户相关的路线的指针，它们都初始化为NULL。
            0: 表示开始服务的时间为0。
    // dimensionnement du champ depots et routes � la bonne taille

       循环遍历每一天（由params->nbDays指定），进行以下操作：
        为每一天添加对应的客户、仓库、结束仓库和路线。
        根据仓库数量，创建新的Noeud对象来表示客户。
        根据每天的可用车辆数量，创建新的仓库、结束仓库和路线。
    */
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
  //这段代码初始化了两个结构——一个记录每天的客户访问顺序，另一个记录日子的顺序。
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
