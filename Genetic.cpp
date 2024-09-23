#include "Genetic.h"
#include <algorithm> 
void Genetic::evolve(int maxIter, int maxIterNonProd, int nbRec)
{
	Individu *parent1;
	Individu *parent2;
	int place;
	double rangRelatif;
	nbIterNonProd = 1;
	nbIter = 0;
	int resultCross;
	int measure = 0;
	string temp;

	//while (nbIter < maxIter && nbIterNonProd < maxIterNonProd && (clock() - params->debut <= ticks))
	while (nbIter < maxIter && nbIterNonProd < maxIterNonProd && clock() - params->debut <= 30 * 60 * CLOCKS_PER_SEC)
	{
		//cout <<"nbIter: "<<nbIter<<endl;
				
		// on demande deux individus � la population 我们要求人口中的两个个体
		population->recopieIndividu(rejeton, population->getIndividuBinT(rangRelatif));
		population->recopieIndividu(rejeton2, population->getIndividuBinT(rangRelatif));
		//cout <<"before: "<<endl;
		// on choisit le crossover en fonction du probleme 我们根据问题选择分频器
		if (!params->isInventoryRouting)
			resultCross = crossOX();
		else
			resultCross = crossPOX2(); // TODO -- for now crossover is deactivated to debug the GA
		
		muter();
		
		// REPAIR IF NEEDED
		if (!rejeton->estValide)
		{
			//cout <<"repairs: "<<nbIter<<endl;
			place = population->addIndividu(rejeton);
			reparer();
		}
		//cout <<"out"<<endl;
		// ADD IN THE POPULATION IF IS FEASIBLE
		if (rejeton->estValide)
			place = population->addIndividu(rejeton);
		//cout <<"d: "<<nbIter<<endl;
		if (place == 0 && rejeton->estValide)
			nbIterNonProd = 1;
		else
			nbIterNonProd++;
		//out <<"p: "<<nbIter<<endl;
		//cout <<"rejrton "<<rejeton->estValide<<" place "<<place<<endl;
		if (place == 0 && rejeton->estValide)
		{
			cout << "NEW BEST FEASIBLE ";
			cout << rejeton->coutSol.evaluation;	
			cout << " Cost : " << rejeton->coutSol.fitness
				 << " capacity Violation : " << rejeton->coutSol.capacityViol
				 << " length Violation : " << rejeton->coutSol.lengthViol;
			cout << endl;
			cout << endl;
		}
		//cout <<"k: "<<nbIter<<endl;
		if (nbRec > 0 && nbIterNonProd % (maxIterNonProd / 3 + 1) == maxIterNonProd / 3)
		{
			if (traces)
				cout << "Diversification" << endl;
			population->diversify();
		}
	
		// MANAGEMENT OF PARAMETERS
		if (nbIter % 100 == 0)
			gererPenalites();
	
		// TRACES
		if (traces && nbIter % 100 == 0)
			population->afficheEtat(nbIter);
		
		nbIter++;
		//cout <<"see"<<endl;
		//cout <<nbIter << maxIter << nbIterNonProd << maxIterNonProd << (clock() - params->debut) << ticks<<endl;
	}
	
		
	//int a;cin>>a;
	// fin de l'algorithme , diverses informations affich�es
	if (traces)
	{
		cout << "time passes : " << clock() << endl;
		cout << "number of iterations : " << nbIter << endl;
	}
}

// effectue la mutation
void Genetic::muter()
{
	rejeton->updateLS();
	//cout <<"run"<<endl;
	rejeton->localSearch->runSearchTotal(false);
	
	rejeton->updateIndiv();
	population->updateNbValides(rejeton);
}

// eventuellement effectue une reparation de la solution如有必要，修复解决方案
void Genetic::reparer()
{
	double temp, temp2;
	bool continuer = false;

	temp = params->penalityCapa;
	temp2 = params->penalityLength;

	/*First tentative尝试修复（第一次尝试）：

首先，提高容量和长度的罚分，使其变为原来的10倍。
如果一个随机数小于某个阈值 params->pRep（可能是进行修复尝试的概率），则尝试更新和修复解决方案。
运行局部搜索，尝试找到一个更好的、可能是合规范的解决方案。
更新rejeton以反映局部搜索的结果。*/
	params->penalityCapa *= 10;
	params->penalityLength *= 10;
	if (params->rng->genrand64_real1() < params->pRep)
	{
		rejeton->updateLS();
		rejeton->localSearch->runSearchTotal(true);
		rejeton->updateIndiv();

		/* Second tentative尝试修复（第二次尝试）：
如果rejeton仍然不合规范，则进行第二次修复尝试。
这次，容量和长度的罚分被提高了500倍，这意味着算法将更加重视解决这些不合规范的方面。
再次运行局部搜索并更新解决方案。
恢复原始罚分值：*/
		if (!rejeton->estValide)
		{
			//cout <<"second"<<endl;
			params->penalityCapa *= 500;
			params->penalityLength *= 500;
			rejeton->generalSplit();
			//cout <<"general";
			rejeton->updateLS();
			//cout <<"localse";
			rejeton->localSearch->runSearchTotal(true);
			rejeton->updateIndiv();
		}
	}
	params->penalityCapa = temp;
	params->penalityLength = temp2;
}

// gestion des penalites
void Genetic::gererPenalites()
{
	double fractionCharge = population->fractionValidesCharge();
	double fractionTemps = population->fractionValidesTemps();

	if (fractionCharge < params->minValides && params->penalityCapa < 1000)
		params->penalityCapa = params->penalityCapa * 1.2;
	else if (fractionCharge > params->maxValides && params->penalityCapa > 0.01)
		params->penalityCapa = params->penalityCapa * 0.85;

	if (fractionTemps < params->minValides && params->penalityLength < 1000)
		params->penalityLength = params->penalityLength * 1.2;
	else if (fractionTemps > params->maxValides && params->penalityLength > 0.01)
		params->penalityLength = params->penalityLength * 0.85;

	population->validatePen();
}
/*
这是`Genetic`类的构造函数的定义，它主要做了以下几件事情：
1. **参数初始化**：通过构造函数的初始化列表，它将传入的参数值分别赋给类的成员变量。
例如，传入的`params`参数的值被赋给了类的成员变量`params`。这样做主要是为了初始化这些成员变量。

2. **局部变量初始化**：初始化了几个临时的变量`tempVect`（一个整数类型的向量）
和`chromTRand`（一个整数向量的向量）以及`chromPRand`（一个`pattern`类型的向量）和`tempList`（一个整数类型的列表）。

3. **`pattern`对象初始化**：创建了一个新的`pattern`对象`p`，并将其成员`dep`和`pat`初始化为0。

4. **初始化`chromTRand`**：使用`tempVect`为`chromTRand`添加`params->nbDays + 1`个空的整数向量。

*/
Genetic::Genetic(Params *params, Population *population, clock_t ticks, bool traces, bool writeProgress) : params(params), population(population), ticks(ticks), traces(traces), writeProgress(writeProgress)
{
	
	vector<int> tempVect;
	vector<vector<int>> chromTRand;
	vector<pattern> chromPRand;
	list<int> tempList;
	pattern p;
	p.dep = 0;
	p.pat = 0;

	//4. **初始化`chromTRand`**：使用`tempVect`为`chromTRand`添加`params->nbDays + 1`个空的整数向量。
	for (int i = 0; i <= params->nbDays; i++)
		chromTRand.push_back(tempVect);  

	//5. **填充`freqClient`和`chromPRand`向量**：对于每一个客户和仓库，
	// 将其频率添加到`freqClient`向量中，并在`chromPRand`中为每个客户和仓库添加一个`pattern`对象。

	for (int i = 0; i < params->nbClients + params->nbDepots; i++)
	{
		freqClient.push_back(params->cli[i].freq);
		chromPRand.push_back(p);
	}
	

// **初始化`rejeton`和`rejeton2`**：为`rejeton`和`rejeton2`分配新的`Individu`对象。

//**为`rejeton`配置`localSearch`**：删除`rejeton`的`localSearch`成员，然后为其分配一个新的`LocalSearch`对象。

	rejeton = new Individu(params, 1.0);
	rejeton2 = new Individu(params, 1.0);
	delete rejeton->localSearch;
	rejeton->localSearch = new LocalSearch(params, rejeton);
}

int Genetic::crossOX()
{
	// on tire au hasard un debut et une fin pour la zone de crossover
	//为交叉区域随机绘制起点和终点
	int debut = params->rng->genrand64_int64() % params->nbClients;
	int fin = params->rng->genrand64_int64() % params->nbClients;
	while (fin == debut)
		fin = params->rng->genrand64_int64() % params->nbClients;

	// on initialise le tableau de frequence signifiant si l'individu a �t� plac� ou non
	for (int i = params->nbDepots; i < params->nbClients + params->nbDepots; i++)
		freqClient[i] = 1;

	int j = debut;
	// on garde les elements de debut � fin
	while ((j % params->nbClients) != ((fin + 1) % params->nbClients))
	{
		freqClient[rejeton->chromT[1][j % params->nbClients]] = 0;
		j++;
	}

	int temp;
	// on remplit les autres elements non d�ja plac�s dans l'ordre du
	// deuxieme chromosome
	for (int i = 1; i <= params->nbClients; i++)
	{
		temp = rejeton2->chromT[1][(fin + i) % params->nbClients];
		if (freqClient[temp] == 1)
		{
			rejeton->chromT[1][j % params->nbClients] = temp;
			j++;
		}
	}

	// on calcule le fitness
	rejeton->generalSplit();
	return 0;
}

int Genetic::crossPOX2()
{
	vector<int> vide, garder, joursPerturb, tableauFin, tableauEtat;
	vector<double> charge;
	int debut, fin, day;
	int j1, j2;
	double quantity;

	// Keeping track of the chromL of the parent
	vector<vector<double>> chromLParent1 = rejeton->chromL;

	// Reinitializing the chromL of the rejeton (will become the child)
	// Keeping for each day and each customer the total sum of delivered load and initial inventory
	// (when inserting a customer, need to make sure that we are not exceeding this)
	for (int k = 1; k <= params->nbDays; k++)
		for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++)
			rejeton->chromL[k][i] = 0.;

	// Keeping a vector to remember if a delivery has alrady been inserted for on day k for customer i
	vector<vector<bool>> hasBeenInserted = vector<vector<bool>>(params->nbDays + 1, vector<bool>(params->nbClients + params->nbDepots, false));

	// choosing the type of inheritance for each day (nothing, all, or mixed) 根据随机顺序确定哪些天会被完全继承、哪些天会继承部分，以及哪些天不会继承任何内容。
	for (int k = 1; k <= params->nbDays; k++)
		joursPerturb.push_back(k);

	std::random_shuffle(joursPerturb.begin(), joursPerturb.end());

	// Picking j1 et j2
	j1 = params->rng->genrand64_int64() % params->nbDays;
	j2 = params->rng->genrand64_int64() % params->nbDays;
	if (j1 > j2)
	{
		int temp = j2;
		j2 = j1;
		j1 = temp;
	}

	// Inheriting the data from rejeton1.
	// For each day, we will keep a sequence going from debut to fin
	for (int k = 0; k < params->nbDays; k++)
	{
		day = joursPerturb[k];
		// on recopie un segment复制一个片段
		if (k < j1 && !rejeton->chromT[day].empty())
		{
			debut = (int)(params->rng->genrand64_int64() % rejeton->chromT[day].size()); //定从第一个父代rejeton继承哪些数据。这可能是一个完整的序列、部分序列或没有数据。
			fin = (int)(params->rng->genrand64_int64() % rejeton->chromT[day].size());
			tableauFin.push_back(fin);
			int j = debut;
			garder.clear();
			while (j != ((fin + 1) % rejeton->chromT[day].size()))
			{
				int ii = rejeton->chromT[day][j]; // getting the index to be inherited
				garder.push_back(ii);
				rejeton->chromL[day][ii] = chromLParent1[day][ii];
				//rejeton->chromL[day][ii] = std::min<double>(rejeton->maxFeasibleDeliveryQuantity(day, ii),  chromLParent1[day][ii]);
				hasBeenInserted[day][ii] = true;
				j = (j + 1) % rejeton->chromT[day].size();
			}
			rejeton->chromT[day].clear();
			for (int g = 0; g < (int)garder.size(); g++)
				rejeton->chromT[day].push_back(garder[g]);
		}
		else if (k < j2) // on recopie rien
		{
			rejeton->chromT[day].clear();
			tableauFin.push_back(-1);
		}
		else // on recopie tout
		{
			tableauFin.push_back(0);
			for (int j = 0; j < (int)rejeton->chromT[day].size(); j++)
			{
				int ii = rejeton->chromT[day][j]; // getting the index to be inherited
				garder.push_back(ii);
				rejeton->chromL[day][ii] = chromLParent1[day][ii];
				//rejeton->chromL[day][ii] = std::min<double>(rejeton->maxFeasibleDeliveryQuantity(day, ii),  
														//				chromLParent1[day][ii]);
				hasBeenInserted[day][ii] = true;
				j = (j + 1) % rejeton->chromT[day].size();
			}
		}
	}
	
	// completing with rejeton 2
	for (int k = 0; k < params->nbDays; k++)
	{
		day = joursPerturb[k];
		fin = tableauFin[k];
		if (k < j2)
		{
			for (int i = 0; i < (int)rejeton2->chromT[day].size(); i++)
			{
				int ii = rejeton2->chromT[day][(i + fin + 1) % (int)rejeton2->chromT[day].size()];
				if (!hasBeenInserted[day][ii]) // it has not been inserted yet
				{
					// computing maximum possible delivery quantity
					quantity = rejeton2->chromL[day][ii];
					if (quantity > 0.0001)
					{
						rejeton->chromT[day].push_back(ii);
						rejeton->chromL[day][ii] = quantity;
						hasBeenInserted[day][ii] = true;
					}
				}
			}
		}
	}
	
	vector<vector<double>> I_end(params->nbDays+2, vector<double>(params->nbDepots + params->nbClients));
	//vector<double> sum(params->nbDepots + params->nbClients);
	//for(int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++) sum[i] = params->cli[i].startingInventory;

	for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++){
		I_end[0][i] = params->cli[i].startingInventory;
		//if(i == 157)cout<<"day0 cus = "<<i<<" "<<I_end[0][i]<<endl;	
	}

	for (int k = 1; k <= params->nbDays; k++){
		for (int cus = params->nbDepots; cus < params->nbDepots + params->nbClients; cus++){
			//if(cus == 157)cout<<"cus = "<<cus<<endl;
			//if(sum[cus]+rejeton->chromL[k][cus] >params->cli[cus].maxInventory) rejeton->chromL[k][cus]=params->cli[cus].maxInventory-sum[cus];

			rejeton->chromL[k][cus] = std::min<double>(rejeton->chromL[k][cus],params->cli[cus].maxInventory-I_end[k-1][cus]);
			
			//if(cus == 157)cout<<"yesterday "<<I_end[k-1][cus]<<" today "<<rejeton->chromL[k][cus]<<" maxIn "<<params->cli[cus].maxInventory<<endl;

			I_end[k][cus] = std::max<double>(0,I_end[k-1][cus] + rejeton->chromL[k][cus] - params->cli[cus].dailyDemand[k]);
			
			//if(cus == 157) cout<<"Iend "<<I_end[k][cus]<<endl;
		}	
	}

	
	/*
	for (int k = 1; k <= params->nbDays; k++){
		cout <<"rejeton->chromT["<<k<<"]: (";
		for (int i = 0; i < (int)rejeton->chromT[k].size(); i++){
				cout<<rejeton->chromT[k][i]<<" ";
		}
		cout<<")"<<endl;
	}
		*/
	return 0;
}

// A depot fix�s, am�liorer une solution, d�composition du probl�me par d�pot
void Genetic::improve_decompRoutes(int maxIter, int maxIterNonProd, Individu *indiv, int grainSize, int decal, Population *pop, int nbRec)
{
	double place;
	bool addBestIndivFeatures = false;
	bool randomOrder = false;
	if (params->rng->genrand64_real1() < 1.0)
	{
		addBestIndivFeatures = true;
		cout << "taking features from the best individual" << endl;
	}
	if (params->rng->genrand64_real1() < 0.3)
	{
		randomOrder = true;
		cout << "using random order" << endl;
	}

	vector<int> orderVehicles;
	for (int i = 0; i < params->nbVehiculesPerDep; i++)
		orderVehicles.push_back(i);
	int temp, temp2;
	// on introduit du desordre
	for (int a1 = 0; a1 < (int)orderVehicles.size() - 1; a1++)
	{
		temp2 = a1 + params->rng->genrand64_int64() % ((int)orderVehicles.size() - a1);
		temp = orderVehicles[a1];
		orderVehicles[a1] = orderVehicles[temp2];
		orderVehicles[temp2] = temp;
	}

	double init = indiv->coutSol.evaluation;
	if (clock() - params->debut > ticks)
		return;
	Params *sousProbleme;
	Population *sousPopulation;
	Genetic *sousSolver;
	Individu *initialIndiv;
	Individu *bestIndiv;
	Individu *bestIndiv2;
	Individu *bestIndiv3;
	int k1, myk;
	bool found;
	vector<int> route;
	vector<vector<int>> routes;
	vector<vector<int>> routes2;
	vector<vector<int>> routes3;
	for (int k = 0; k < params->nbVehiculesPerDep; k++)
	{
		routes.push_back(route);
		routes2.push_back(route);
		routes3.push_back(route);
	}
	SousPop *subpop;
	SousPop *subsubPop;
	if (pop->valides->nbIndiv != 0)
		subpop = pop->valides;
	else
		subpop = pop->invalides;

	vector<Vehicle *> serieVehiclesTemp;
	vector<int> serieNumeroRoutes;
	vector<int> serieVisitesTemp;
	Vehicle **serieVehicles;
	int *serieVisites;
	int fin;

	// normalement les routes sont d�ja organis�es dans l'ordre des centroides, puisque le probl�me de base �tait un VRP
	// on va donc les accumuler jusque d�passer "grainSize"
	// puis r�soudre le probl�me
	// et enfin remettre les choses � la bonne place.
	for (int k = 0; k <= params->nbVehiculesPerDep; k++)
	{
		if ((int)serieVisitesTemp.size() > grainSize || k == params->nbVehiculesPerDep)
		{
			// on va traiter le sous-probl�me associ� aux routes premierVeh...kk-1
			serieVehicles = new Vehicle *[(int)serieVehiclesTemp.size()];
			for (int ii = 0; ii < (int)serieVehiclesTemp.size(); ii++)
				serieVehicles[ii] = serieVehiclesTemp[ii];
			serieVisites = new int[(int)serieVisitesTemp.size()];
			for (int ii = 0; ii < (int)serieVisitesTemp.size(); ii++)
				serieVisites[ii] = serieVisitesTemp[ii];

			if ((int)serieVisitesTemp.size() > 0)
			{
				sousProbleme = new Params(params, 0, serieVisites, serieVehicles, NULL, NULL, 0, 0, (int)serieVisitesTemp.size(), (int)serieVehiclesTemp.size());
				sousPopulation = new Population(sousProbleme);

				// se servir des caract�ristiques des individus originaux pour cr�er la population
				initialIndiv = new Individu(sousProbleme, 1.0);
				if (addBestIndivFeatures)
					for (int kl = 0; kl < subpop->nbIndiv; kl++)
					{
						initialIndiv->chromT[1].clear();
						for (int ii = 0; ii < (int)subpop->individus[kl]->chromT[1].size(); ii++)
							if (sousProbleme->correspondanceTable2[subpop->individus[kl]->chromT[1][ii]] != -1)
								initialIndiv->chromT[1].push_back(sousProbleme->correspondanceTable2[subpop->individus[kl]->chromT[1][ii]]);

						initialIndiv->generalSplit();
						sousPopulation->addIndividu(initialIndiv);
					}

				// on cree le solver (we don't solve for a problem with 1 customer only).
				sousSolver = new Genetic(sousProbleme, sousPopulation, (clock_t)(10000000000 * CLOCKS_PER_SEC), true, true);
				if ((int)serieVisitesTemp.size() > 1)
					sousSolver->evolve(maxIter, maxIterNonProd, nbRec - 1);

				// on r�cup�re les trois meilleurs diff�rents
				if (sousPopulation->getIndividuBestValide() != NULL)
					subsubPop = sousPopulation->valides;
				else
					subsubPop = sousPopulation->invalides;

				bestIndiv = subsubPop->individus[0];
				k1 = 1;
				found = false;
				while (k1 < subsubPop->nbIndiv && !found)
				{
					if (subsubPop->individus[k1]->coutSol.evaluation >= bestIndiv->coutSol.evaluation + 0.1)
					{
						bestIndiv2 = subsubPop->individus[k1];
						found = true;
					}
					k1++;
				}
				if (!found)
					bestIndiv2 = subsubPop->individus[0];
				found = false;
				while (k1 < subsubPop->nbIndiv && !found)
				{
					if (subsubPop->individus[k1]->coutSol.evaluation >= bestIndiv2->coutSol.evaluation + 0.1)
					{
						bestIndiv3 = subsubPop->individus[k1];
						found = true;
					}
					k1++;
				}
				if (!found)
					bestIndiv3 = subsubPop->individus[0];

				// on transfere leurs caract�ristiques
				for (int ii = 0; ii < (int)bestIndiv->chromR[1].size(); ii++)
				{
					if (ii == (int)bestIndiv->chromR[1].size() - 1)
						fin = (int)bestIndiv->chromT[1].size();
					else
						fin = bestIndiv->chromR[1][ii + 1];
					for (int j = bestIndiv->chromR[1][ii]; j < fin; j++)
						routes[serieNumeroRoutes[ii]].push_back(sousProbleme->correspondanceTable[bestIndiv->chromT[1][j]]);
				}
				for (int ii = 0; ii < (int)bestIndiv2->chromR[1].size(); ii++)
				{
					if (ii == (int)bestIndiv2->chromR[1].size() - 1)
						fin = (int)bestIndiv2->chromT[1].size();
					else
						fin = bestIndiv2->chromR[1][ii + 1];
					for (int j = bestIndiv2->chromR[1][ii]; j < fin; j++)
						routes2[serieNumeroRoutes[ii]].push_back(sousProbleme->correspondanceTable[bestIndiv2->chromT[1][j]]);
				}
				for (int ii = 0; ii < (int)bestIndiv3->chromR[1].size(); ii++)
				{
					if (ii == (int)bestIndiv3->chromR[1].size() - 1)
						fin = (int)bestIndiv3->chromT[1].size();
					else
						fin = bestIndiv3->chromR[1][ii + 1];
					for (int j = bestIndiv3->chromR[1][ii]; j < fin; j++)
						routes3[serieNumeroRoutes[ii]].push_back(sousProbleme->correspondanceTable[bestIndiv3->chromT[1][j]]);
				}

				delete initialIndiv;
				delete sousProbleme;
				delete sousPopulation;
				delete sousSolver;
			}

			delete[] serieVehicles;
			delete[] serieVisites;
			serieVehiclesTemp.clear();
			serieVisitesTemp.clear();
			serieNumeroRoutes.clear();
		}

		// si ce n'est pas la derni�re it�ration, on accumule les v�hicules et clients de la route consid�r�e
		// pour d�finir le prochain sous-probl�me
		if (k != params->nbVehiculesPerDep)
		{
			if (!randomOrder)
				myk = (k + decal) % params->nbVehiculesPerDep;
			else
				(myk = orderVehicles[k]);

			serieVehiclesTemp.push_back(&params->ordreVehicules[1][myk]);
			serieNumeroRoutes.push_back(myk);
			if (myk == params->nbVehiculesPerDep - 1)
				fin = indiv->chromT[1].size();
			else
				fin = indiv->chromR[1][myk + 1];
			for (int ii = indiv->chromR[1][myk]; ii < fin; ii++)
				serieVisitesTemp.push_back(indiv->chromT[1][ii]);
		}
	}

	indiv->chromT[1].clear();
	for (int k = 0; k < params->nbVehiculesPerDep; k++)
		for (int kk = 0; kk < (int)routes[k].size(); kk++)
			indiv->chromT[1].push_back(routes[k][kk]);
	indiv->generalSplit();
	place = population->addIndividu(indiv);
	cout << "INTENSIFICATION PAR DECOMPOSITION GEOMETRIQUE: " << init << " --> " << indiv->coutSol.evaluation << endl;
	if (indiv->estValide && place == 0)
		nbIterNonProd = 1;
}

Genetic::~Genetic(void)
{
	delete rejeton;
	delete rejeton2;
}
