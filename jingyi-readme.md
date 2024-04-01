## 1.  for randomlize the routes for each day:
In the file individu.cpp, lines 601-605 were originally implemented using a bubble sort algorithm. 
However, to enhance the performance and efficiency of the sorting process, we replaced the bubble sort with a randomized quicksort algorithm. 
Upon testing, it was found that the quicksort function was invoked a total of 4,959 times. 
As a result of this optimization, the total processing time for the small dataset Istanze0105h3 improved from 127 seconds to 121 seconds.




## 2.how to run:
   `./irp path-to-instance -seed 1000 -type 38 -veh <number-of-vehicle> -stock <Stockout Penalty>` 
   \rou can be 100/200/300, ect.
  - Example: stockout test4
  ./irp Data/Small/Istanze0105h3/abs4n5_2.dat -seed 1000 -type 38 -veh 3 -stock 1000000


(1).commandline.cpp&.h -> add variable :\rou

    else if (string(argv[i]) == "-stock"){
          rou = atoi(argv[i + 1]);
          stockout=true;
      }

(2). params.cpp line 24
    ** if (fichier.is_open())
		preleveDonnees(nomInstance);

    ** void Params::preleveDonnees(string nomInstance,int rou, bool stockout)
            -> Line 169 isstockout = stockout;
            -> line 258 getClient(i,rou)
    
    ** Client Params::getClient(int i,int rou)
            -> Line 395 client.stockoutCost = client.inventoryCost*rou;
    
    ** Line526 void Params::computeConstant_stockout(){
        // Adding the total cost for supplier inventory over the planning horizon (CONSTANT IN OBJECTIVE)
    }


(3) Client.h Line 107 double stockoutCost;

(4) paprams.h
    line 153  bool isstockout;
    line 141  objectiveConstant_stockout



(5). double LocalSearch::evaluateSolutionCost()
{
    double myCost = 0.;
    //capacity+routing cost

    // And the necessary constants (inventory cost of depot only )
    myCost += params->objectiveConstant_stockout;
    
    // Adding inventory cost
    
    // minus depot holding cost from constant value 
     
    return myCost;
  }

 (6) int LocalSearch::mutation11(int client) // PI operator
        if(params -> isstockout) currentCost=evaluateCurrentCost_stockout(client);

        if(params-> isstockout) ok = lotsizingSolver->solve_stockout();
  	    else ok = lotsizingSolver->solve();

 (7) add function in Line 933:
    // for PI operator 
    double LocalSearch::evaluateCurrentCost_stockout(int client){ 
                // adding the inventory cost
                //stockout
                // the detour cost
                // and the possible excess capacity
                I = std::max<double> (0., I+demandPerDay[k][client]-params->cli[client].dailyDemand);
            
    }

 (8) LocalSearch.h
    Line 193 double evaluateCurrentCost_stockout (int client);

(9)void LocalSearch::printInventoryLevels()
  
## 3. lot sizing Part
(1). insertion (detour, load, place) nochange

(2) F function in DP we totally need :
      --> F1(q) = detour + CapacityPenaltty (q) -depotholding  (q) *t + stockoutCost(daily-q)
        PLFunction::calculateCost_stockout(int day, int client, double detour, double replenishment, double freeload)
      --> normally accompanied with StC (I) = stockout(-I)

      **void** PLFunction::addStockout(**double** stockoutCost) 
      **void** PLFunction::addStockout(**double** stockoutCost,**double** daily)*//x = 5,daily = 6  -----> +1\*stockoutcost --->dp(-1)*


      --> F2(q) = detour + CapacityPenaltty (q) -depotholding(q) *t + holdingCost (q-daily)
         double PLFunction:: calculateCost_holding(int day, int client,  double detour, double replenishment, double freeload)
      --> normally accompanied with   HoC (I) = holdingCost (I)
         **void** PLFunction::addHolding(**double** InventoryCost) 
        ​	**void** PLFunction::addHolding(**double** InventoryCost,**double** daily) *//x = 6,daily = 5  -----> +1\*holdingcost --->dp(1)*

      --> PLFunction.cpp add Constructor function for f1 & f2
       

(4)  new function **bool** LotSizingSolver::solve_stockout()
    1) Firstday
    2) next days:


  1）new function **bool** LotSizingSolver::solve_stockout() 
  ==**Initialization part **==
  void  LotSizingSolver::Firstday(vector<std::shared_ptr<PLFunction>> &C){//for the first day
  
  if initial inventory is not enough, i.e., update C[firstday][I_remain = 0]:
    //1 !!!!!!!!! modify C0[0] q!=0, replenishment not enough ->piece/;p-[0]   Istart+q-daily <=0 ->>>>>> q<=daily-Istart
    //2!!!..........   q ==0 replenishment not enough
  
    // 3!!!!!!!!!! q!=0, replenishment enough delivery enough
  // I_start(6)+q-daily(5) >=0     I_start+q<=maxinventory    =====>   q<=max-Istart && q>=daily-Istart
  //4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!q == 0, no delivery,but enough
    
  2)for (int t = 1; t < horizon; t++)
    f1. q(t) = 0,  I(t-1) > daily ==> I(t) > 0 
       //dp <--- C(t-1)(I) + inventoryCost (I-daily)
    f2. q(t) = 0,  I(t-1) < daily ==> I(t) ->>> 0 
       //dp <--- C(t-1)(I) + stockoutCost (daily-I)
    ///////************************** C[0] = f2->minValue;

    f3. q != 0, I(t-1)+q-daily > 0     ==> I(t)>0
      // C(t)(It) <----- C(t-1)(It-1) + holdingCost(It-1) + F2function (qt)

    f4. q != 0, I(t-1)+q-daily < 0     ==> I(t)=0
    // C(t)(It) <----- C(t-1)(It-1) + stockoutCost(It-1) + F1function (qt)
    

    min(f1,f3), update C[0] = min(f2->minValue,f4->minvalue);
（5） void PLFunction::append(shared_ptr<LinearPiece> lp){
    
      void PLFunction::update_minValue(double & rep (loss))

      std::shared_ptr<PLFunction> PLFunction::getInBound(double lb, double ub, bool updateValueAt0)


(6) min
    std::shared_ptr<LinearPiece> createPieceFromLowerY(
    ​    **const** std::shared_ptr<LinearPiece>**&** chosenPiece,
    ​    **double** x1, **double** y1, **double** x2, **double** y2) 

    std::shared_ptr<PLFunction> LotSizingSolver::min_stockout(std::shared_ptr<PLFunction> f1, std::shared_ptr<PLFunction> f2){

(7) **backtracking  determine quantity**
  -->1)
  while(day!=0)
  
    // if do not delivery any thing, then inventory at the end of previous day
    // equals this day demand
    if (tmp->replenishment_loss == -1 && tmp->fromF ==nullptr) //f1 q = 0
    
    else if (tmp->replenishment_loss != -1 && tmp->fromF ==nullptr) //f2  q= 0, not enough
    
    else if (tmp->replenishment_loss == -1 && tmp->fromF != nullptr) //f3
        
        solveEquationSystem_stockout(tmp, fromC, fromF, -tmp->replenishment_loss,
                            params->cli[client].dailyDemand[day], I[day - 1],
                            quantities[day]);
      
    
     else if (tmp->replenishment_loss != -1 && tmp->fromF != nullptr)//f4
    

  tmp = tmp->fromC_pre;

  -->2)
      void LotSizingSolver::solveEquationSystem_stockout(std::shared_ptr<LinearPiece> C,
                                          std::shared_ptr<LinearPiece> fromC,
                                          std::shared_ptr<LinearPiece> fromF,
                                          double I, double demand,
                                          double &fromI, double &quantity){
        we know I[day], aim to get quantities[day] and   I[day-1]                               