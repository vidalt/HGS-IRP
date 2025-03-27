#include "Genetic.h"
#include <algorithm> 
#include <unistd.h>

inline double GetMemoryUsage(int pid, int flag=true) {
	std::ifstream status("/proc/" + std::to_string(pid) + "/status");
	std::string line;
	long long vm_size_kb = 0, vm_rss_kb = 0;

	while (std::getline(status, line)) {
		if (line.find("VmSize:") != std::string::npos) {
			vm_size_kb = std::stoll(line.substr(line.find(":") + 1));
		} else if (line.find("VmRSS:") != std::string::npos) {
			vm_rss_kb = std::stoll(line.substr(line.find(":") + 1));
		}
	}

	double vm_size_mb = vm_size_kb / 1024.0;
	double vm_rss_mb = vm_rss_kb / 1024.0;

	if (flag) {
		std::cout << "Virtual Memory Size: " << vm_size_mb << " MB\n";
		std::cout << "Resident Set Size (RSS): " << vm_rss_mb << " MB\n";
	}

	return vm_size_mb;
}

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

	
	while (nbIter < maxIter && nbIterNonProd < maxIterNonProd && clock() <=  nbRec * CLOCKS_PER_SEC)
	{
		// on demande deux individus � la population
		population->recopieIndividu(rejeton, population->getIndividuBinT(rangRelatif));
		population->recopieIndividu(rejeton2, population->getIndividuBinT(rangRelatif));
		// on choisit le crossover en fonction du probleme
		if (!params->isInventoryRouting)
			resultCross = crossOX();
		else
			resultCross = crossPOX2(); // TODO -- for now crossover is deactivated to debug the GA
		
		muter();
		
		// REPAIR IF NEEDED
		if (!rejeton->estValide)
		{
			place = population->addIndividu(rejeton);
			reparer();
		}
		// ADD IN THE POPULATION IF IS FEASIBLE
		if (rejeton->estValide)
			place = population->addIndividu(rejeton);
		if (place == 0 && rejeton->estValide)
			nbIterNonProd = 1;
		else
			nbIterNonProd++;
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
		if (traces && nbIter % 500 == 0)
			population->afficheEtat(nbIter);
		
		nbIter++;

	
	}
	
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
	rejeton->localSearch->runSearchTotal(false);
	
	rejeton->updateIndiv();
	population->updateNbValides(rejeton);
}

// eventuellement effectue une reparation de la solution
void Genetic::reparer()
{
	double temp, temp2;
	bool continuer = false;

	temp = params->penalityCapa;
	temp2 = params->penalityLength;

	/*First tentative*/
	params->penalityCapa *= 10;
	params->penalityLength *= 10;
	if (params->rng->genrand64_real1() < params->pRep)
	{
		rejeton->updateLS();
		rejeton->localSearch->runSearchTotal(true);
		rejeton->updateIndiv();

		/* Second tentative*/
		if (!rejeton->estValide)
		{
			params->penalityCapa *= 500;
			params->penalityLength *= 500;
			rejeton->generalSplit();
			rejeton->updateLS();
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

Genetic::Genetic(Params *params, Population *population, clock_t ticks, bool traces, bool writeProgress) : params(params), population(population), ticks(ticks), traces(traces), writeProgress(writeProgress)
{
	
	vector<int> tempVect;
	vector<vector<int>> chromTRand;
	vector<pattern> chromPRand;
	list<int> tempList;
	pattern p;
	p.dep = 0;
	p.pat = 0;

	for (int i = 0; i <= params->nbDays; i++)
		chromTRand.push_back(tempVect);  

	for (int i = 0; i < params->nbClients + params->nbDepots; i++)
	{
		freqClient.push_back(params->cli[i].freq);
		chromPRand.push_back(p);
	}

	rejeton = new Individu(params, 1.0);
	rejeton2 = new Individu(params, 1.0);
	delete rejeton->localSearch;
	rejeton->localSearch = new LocalSearch(params, rejeton);
}

int Genetic::crossOX()
{
	// on tire au hasard un debut et une fin pour la zone de crossover
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

	// choosing the type of inheritance for each day (nothing, all, or mixed)
	for (int k = 1; k <= params->nbDays; k++)
		joursPerturb.push_back(k);

	// std::random_shuffle(joursPerturb.begin(), joursPerturb.end());
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(joursPerturb.begin(), joursPerturb.end(), g);

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
		// on recopie un segment
		if (k < j1 && !rejeton->chromT[day].empty())
		{
			debut = (int)(params->rng->genrand64_int64() % rejeton->chromT[day].size());
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
					quantity = std::min<double>(rejeton->maxFeasibleDeliveryQuantity(day, ii), rejeton2->chromL[day][ii]);
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
	
	

	rejeton->generalSplit();

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
