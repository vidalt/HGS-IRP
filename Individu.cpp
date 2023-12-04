#include "Individu.h"

// constructeur d'un Individu comme simple conteneur
Individu::Individu(Params *params) : params(params)
{
	vector<int> tempVect;
	pattern p;
	p.dep = 0;
	p.pat = 0;
	localSearch = new LocalSearch();

	for (int i = 0; i <= params->nbDays; i++)
	{
		chromT.push_back(tempVect);
		chromR.push_back(tempVect);
		for (int j = 0; j < params->nombreVehicules[i]; j++)
			chromR[i].push_back(-1);
	}

	for (int i = 0; i < params->nbClients + params->nbDepots; i++)
		chromP.push_back(p);

	for (int i = 0; i < params->nbDepots + params->nbClients; i++)
	{
		suivants.push_back(tempVect);
		precedents.push_back(tempVect);
	}
}

// constructeur d'un individu al�atoire avec toutes les structures de recherche (split)
Individu::Individu(Params *params, double facteurSurete) : params(params)
{
	vector<int> tempVect;
	vector<vector<int>> tempVect2;
	localSearch = new LocalSearch();
	isFitnessComputed = false;
	coutSol.evaluation = 0;
	coutSol.fitness = 0;
	coutSol.capacityViol = 0;
	coutSol.lengthViol = 0;
	int j, temp;

	// IRP, this construction has to be fully changed.
	// There is no more concept of pattern
	// And the chromP is useless
	// So, for now let's simply assume that we serve all customers in one delivery on day 1 (we ignore the product availability at the supplier for now)
	// Later on we will arrange this construction
	chromT = vector<vector<int>>(params->nbDays + 1);
	chromL = vector<vector<double>>(params->nbDays + 1, vector<double>(params->nbClients + params->nbDepots, 0.));
	chromR = vector<vector<int>>(params->nbDays + 1, vector<int>(params->nombreVehicules[1], -1));

	// Initialization of the chromL and chrom T

	/*
	// OPTION 1
	// Generate a random order for the customers
	vector <int> randomizedOrder ;
	for (int i=params->nbDepots ; i < params->nbDepots + params->nbClients ; i++)
		randomizedOrder.push_back(i);
	std::random_shuffle(randomizedOrder.begin(),randomizedOrder.end());

	// Insert each customer w.r.t. to the optimal lot sizing policy, disregarding routing costs
	int client;
	vector <double> insertionQuantity ;
	for (int i = 0 ; i < params->nbClients ; i++)
	{
		client = randomizedOrder[i];
		ModelLotSizingPI::bestInsertionLotSizing(client,insertionQuantity,params);
		for (int k=1 ; k <= params->nbDays ; k++)
		{
			// An insertion is needed.
			if (insertionQuantity[k-1] > 0.0001)
			{
				chromL[k][client] = insertionQuantity[k-1] ;
				chromT[k].push_back(client);
			}
		}
	}
	*/

	// OPTION 2 -- JUST IN TIME POLICY
	double startInventory;
	double dailyDemand;
	for (int i = params->nbDepots; i < params->nbClients + params->nbDepots; i++)
	{
		startInventory = params->cli[i].startingInventory;
		for (int k = 1; k <= params->nbDays; k++)
		{
			if (startInventory >= params->cli[i].dailyDemand[k])
			{
				// enough initial inventory, no need to service
				startInventory -= params->cli[i].dailyDemand[k];
				chromL[k][i] = 0;
			}
			else
			{
				// not enough initial inventory, just in time policy for the initial solution
				dailyDemand = params->cli[i].dailyDemand[k] - startInventory;
				startInventory = 0;
				chromL[k][i] = dailyDemand;
				chromT[k].push_back(i);
			}
		}
	}

	// And shuffle the whole solution
	for (int k = 1; k <= params->nbDays; k++)
	{
		for (int i = 0; i <= (int)chromT[k].size() - 1; i++)
		{
			j = i + params->rng->genrand64_int64() % ((int)chromT[k].size() - i);
			temp = chromT[k][i];
			chromT[k][i] = chromT[k][j];
			chromT[k][j] = temp;
		}
	}

	// initialisation of the other structures
	for (int k = 0; k <= params->nbDays; k++)
	{
		pred.push_back(tempVect2);
		for (int i = 0; i < params->nombreVehicules[k] + 1; i++)
		{
			pred[k].push_back(tempVect);
			pred[k][i].push_back(0);
			for (int j = 0; j < (int)params->nbClients + params->nbDepots + 1; j++)
				pred[k][i].push_back(0);
		}
	}

	vector<double> potTemp;
	for (int i = 0; i <= params->nombreVehicules[1]; i++)
	{
		potentiels.push_back(potTemp);
		for (int j = 0; j < (int)params->nbClients + params->nbDepots + 1; j++)
			potentiels[i].push_back(1.e30);
	}
	potentiels[0][0] = 0;

	for (int i = 0; i < params->nbDepots + params->nbClients; i++)
	{
		suivants.push_back(tempVect);
		precedents.push_back(tempVect);
	}
}

// destructeur
Individu::~Individu()
{
	delete localSearch;
}

// The Split is done as follows, we test if it's possible to split without setting a fixed limit on the number of vehicles
// If the resulting solution satisfies the number of vehicle, it's perfectly fine, we return it
// Otherwise we call the Split version with limited fleet (which is a bit slower).
void Individu::generalSplit()
{
	coutSol.evaluation = 0;

	// lancement de la procedure split pour chaque jour
	// on essaye d�ja le split simple, si c'est sans succes , le split LF
	for (int k = 1; k <= params->nbDays; k++)
		if (chromT[k].size() != 0)
		{
			int enoughVehicle = splitSimple(k);
			// cout << "Enough Vehicle: " << enoughVehicle << endl;
			if (enoughVehicle == 0)
			{
				splitLF(k);
			}
		}

	// After Split
	// we call a function that fills all other data structures and computes the cost
	measureSol();
	isFitnessComputed = true;

	if (params->borneSplit > 1000)
		throw string("Erreur Split");

	if (coutSol.evaluation > 1.e20)
	{
		// if no feasible solution has been found,
		// then we relax the limit on the max capacity violation adn try again (which is initially set to 4*Q)
		// it's a very exceptional case (mostly for the PVRP, should not happen for the CVRP).
		cout << " Impossible de Split, augmentation de l'acceptation du split " << endl;
		params->borneSplit *= 1.1;
		generalSplit();
	}

	// on �value les suivants -- its not used for the inventory routing
	if (!params->isInventoryRouting)
		computeSuivants();
}

// fonction split which does not consider a limit on the number of vehicles
// just uses the line "1" of the "potentiels" table.
int Individu::splitSimple(int k)
{
	// on va utiliser la ligne 1 des potentiels et structures pred
	double load, distance, time, time2, cost;
	int s0, s1, sb, j;
	potentiels[1][0] = 0;
	s0 = params->ordreVehicules[k][0].depotNumber;

	for (int i = 0; i < (int)chromT[k].size(); i++)
	{
		load = 0;
		distance = 0;
		time = 0; // needed to check the duration constraint for the PVRP, not needed for CVRP
		j = i;
		while (j < (int)chromT[k].size() && load <= params->ordreVehicules[k][0].vehicleCapacity * params->borneSplit)
		{
			s1 = chromT[k][j];
			load += chromL[k][s1];
			if (i == j)
			{
				distance = params->timeCost[s0][s1];
				time = params->timeCost[s0][s1];
			}
			else
			{
				sb = chromT[k][j - 1];
				distance += params->timeCost[sb][s1];
				time += params->cli[sb].serviceDuration + params->timeCost[sb][s1];
			}

			// computing the penalized cost
			cost = distance + params->timeCost[s1][s0];
			if (load > params->ordreVehicules[k][0].vehicleCapacity)
				cost += (load - params->ordreVehicules[k][0].vehicleCapacity) * params->penalityCapa;

			// needed to check the duration constraint for the PVRP, not needed for CVRP
			time2 = time + params->cli[s1].serviceDuration + params->timeCost[s1][s0];
			if (time2 > params->ordreVehicules[k][0].maxRouteTime)
				cost += (time2 - params->ordreVehicules[k][0].maxRouteTime) * params->penalityLength;

			if (potentiels[1][i] + cost < potentiels[1][j + 1]) // basic Bellman algorithm
			{
				potentiels[1][j + 1] = potentiels[1][i] + cost;
				pred[k][1][j + 1] = i;
			}
			j++;
		}
	}

	// testing if le number of vehicles is correct
	// in addition, the table pred is updated to keep track of everything
	j = (int)chromT[k].size();
	for (int jj = 0; jj < params->nombreVehicules[k]; jj++)
	{
		pred[k][params->nombreVehicules[k] - jj][j] = pred[k][1][j];
		j = pred[k][params->nombreVehicules[k] - jj][j];
	}

	coutSol.evaluation = -1.e30; // just for security, making sure this value is not used (as it does not contain the constants)
	initPot(k);
	if (j == 0)
		return 1;
	else
		return 0;
}

// fonction split pour probl�mes � flotte limit�e
void Individu::splitLF(int k)
{
	double load, distance, time, time2, cost;
	int sb, s0, s1, i, j;

	// pour chaque camion
	for (int cam = 0; cam < params->nombreVehicules[k]; cam++)
	{
		i = 0;
		s0 = params->ordreVehicules[k][cam].depotNumber;
		while (i < (int)chromT[k].size() && potentiels[cam][i] < 1.e29)
		{
			if (potentiels[cam][i] < potentiels[cam + 1][i])
			{
				potentiels[cam + 1][i] = potentiels[cam][i];
				pred[k][cam + 1][i] = i;
			}
			load = 0;
			distance = 0;
			time = 0;
			j = i;

			while (j < (int)chromT[k].size() && load <= params->ordreVehicules[k][cam].vehicleCapacity * params->borneSplit)
			{
				s1 = chromT[k][j];
				load += chromL[k][s1];
				if (i == j)
				{
					distance = params->timeCost[s0][s1];
					time = params->timeCost[s0][s1];
				}
				else
				{
					sb = chromT[k][j - 1];
					distance += params->timeCost[sb][s1];
					time += params->cli[sb].serviceDuration + params->timeCost[sb][s1];
				}

				// computing the penalized cost
				cost = distance + params->timeCost[s1][s0];
				if (load > params->ordreVehicules[k][cam].vehicleCapacity)
					cost += (load - params->ordreVehicules[k][cam].vehicleCapacity) * params->penalityCapa;

				// needed to check the duration constraint for the PVRP, not needed for CVRP
				time2 = time + params->cli[s1].serviceDuration + params->timeCost[s1][s0];
				if (time2 > params->ordreVehicules[k][cam].maxRouteTime)
					cost += (time2 - params->ordreVehicules[k][cam].maxRouteTime) * params->penalityLength;

				if (potentiels[cam][i] + cost < potentiels[cam + 1][j + 1]) // Basic Bellman iteration
				{
					potentiels[cam + 1][j + 1] = potentiels[cam][i] + cost;
					pred[k][cam + 1][j + 1] = i;
				}
				j++;
			}
			i++;
		}
	}

	// on ajoute le fitness du jour donn�
	coutSol.evaluation = -1.e30; // just for security, making sure this value is not used (as it does not contain the constants)

	// on nettoye ce que l'on a d�plac�
	initPot(k);
}

void Individu::measureSol()
{
	int depot;
	int i, j;
	double distance, load, time;
	coutSol.fitness = 0;
	coutSol.capacityViol = 0;
	coutSol.lengthViol = 0;
	int nbServices = 0;

	for (int kk = 1; kk <= params->nbDays; kk++)
	{
		// on parcourt les sommets grace aux resultats de split pour
		// remplir les structures
		j = (int)chromT[kk].size();

		for (int jj = 0; jj < params->nombreVehicules[kk]; jj++)
		{
			depot = params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].depotNumber;
			distance = 0;
			load = 0;
			i = (int)pred[kk][params->nombreVehicules[kk] - jj][j];
			chromR[kk][params->nombreVehicules[kk] - jj - 1] = i;

			if (j == i)
			{
				distance = 0;
				load = 0;
			}
			// case where there is only one delivery in the route
			else if (j == i + 1)
			{
				distance = params->timeCost[depot][chromT[kk][i]] + params->timeCost[chromT[kk][i]][depot];
				load = chromL[kk][chromT[kk][i]];
				time = params->timeCost[depot][chromT[kk][i]];
				time += params->cli[chromT[kk][i]].serviceDuration + params->timeCost[chromT[kk][i]][depot];
				if (time > params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].maxRouteTime)
					coutSol.lengthViol += time - params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].maxRouteTime;
				nbServices++;
			}
			else
			{
				distance = params->timeCost[depot][chromT[kk][i]];
				load = 0;
				time = params->timeCost[depot][chromT[kk][i]];

				// infos sommets milieu
				for (int k = i; k <= j - 2; k++)
				{
					time += params->cli[chromT[kk][k]].serviceDuration + params->timeCost[chromT[kk][k]][chromT[kk][k + 1]];
					distance += params->timeCost[chromT[kk][k]][chromT[kk][k + 1]];
					load += chromL[kk][chromT[kk][k]];
					nbServices++;
				}

				// infos sommet fin
				time += params->cli[chromT[kk][j - 1]].serviceDuration + params->timeCost[chromT[kk][j - 1]][depot];
				distance += params->timeCost[chromT[kk][j - 1]][depot];
				load += chromL[kk][chromT[kk][j - 1]];
				if (time > params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].maxRouteTime)
					coutSol.lengthViol += time - params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].maxRouteTime;
				nbServices++;
			}

			coutSol.fitness += distance;
			if (load > params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].vehicleCapacity)
				coutSol.capacityViol += load - params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].vehicleCapacity;

			j = i;
		}
	}

	// Add to the fitness the constants and the inventory cost
	for (int k = 1; k <= params->ancienNbDays; k++)
		for (int i = params->nbDepots; i < params->nbDepots + params->nbClients; i++)
			coutSol.fitness += chromL[k][i] * (params->ancienNbDays + 1 - k) * (params->cli[i].inventoryCost - params->inventoryCostSupplier);

	// And the necessary constants
	coutSol.fitness += params->objectiveConstant;

	if (coutSol.capacityViol < 0.0001 && coutSol.lengthViol < 0.0001)
		estValide = true;
	else
		estValide = false;

	coutSol.evaluation = params->penalityCapa * coutSol.capacityViol + params->penalityLength * coutSol.lengthViol + coutSol.fitness;
}

// initialisation du vecteur potentiels
void Individu::initPot(int day)
{
	for (int i = 0; i < params->nombreVehicules[day] + 1; i++)
	{
		for (size_t j = 0; j <= chromT[day].size() + 1; j++)
		{
			potentiels[i][j] = 1.e30;
		}
	}
	potentiels[0][0] = 0;
}
// mise a jour de l'objet localSearch, attention, split doit avoir ete calcule avant
void Individu::updateLS()
{
	int depot;
	int i, j;
	Noeud *myDepot;
	Noeud *myDepotFin;
	Noeud *myClient;
	Route *myRoute;

	// We copy the amount of delivery per day
	// (more clean to make sure that LocalSearch can work totally independently of the Individu structure)
	localSearch->demandPerDay = chromL;

	for (int kk = 1; kk <= params->nbDays; kk++)
	{
		// on r�initialise l'ordreParcours
		localSearch->ordreParcours[kk].clear();

		// on replace les champs estPresent � false
		for (i = params->nbDepots; i < (int)localSearch->clients[kk].size(); i++)
		{
			localSearch->clients[kk][i]->estPresent = false;
		}

		// on parcourt les sommets grace aux resultats de split pour
		// remplir les structures
		j = (int)chromT[kk].size();

		for (int jj = 0; jj < params->nombreVehicules[kk]; jj++)
		{
			depot = params->ordreVehicules[kk][params->nombreVehicules[kk] - jj - 1].depotNumber;
			i = (int)pred[kk][params->nombreVehicules[kk] - jj][j];

			myDepot = localSearch->depots[kk][params->nombreVehicules[kk] - jj - 1];
			myDepotFin = localSearch->depotsFin[kk][params->nombreVehicules[kk] - jj - 1];
			myRoute = localSearch->routes[kk][params->nombreVehicules[kk] - jj - 1];

			myDepot->suiv = myDepotFin;
			myDepot->pred = myDepotFin;
			myDepotFin->suiv = myDepot;
			myDepotFin->pred = myDepot;

			// cas ou on a un seul sommet dans le cycle
			if (j == i + 1)
			{
				myClient = localSearch->clients[kk][chromT[kk][i]];
				myClient->pred = myDepot;
				myClient->suiv = myDepotFin;
				myClient->route = myRoute;
				myClient->estPresent = true;
				myDepot->suiv = myClient;
				myDepotFin->pred = myClient;
				localSearch->ordreParcours[kk].push_back(myClient->cour);
			}
			else if (j > i + 1)
			{
				// on a au moins 2 sommets
				// infos sommet debut
				myClient = localSearch->clients[kk][chromT[kk][i]];
				myClient->pred = myDepot;
				myClient->suiv = localSearch->clients[kk][chromT[kk][i + 1]];
				myClient->route = myRoute;
				myClient->estPresent = true;
				myDepot->suiv = myClient;
				localSearch->ordreParcours[kk].push_back(myClient->cour);

				// infos sommet fin
				myClient = localSearch->clients[kk][chromT[kk][j - 1]];
				myClient->pred = localSearch->clients[kk][chromT[kk][j - 2]];
				myClient->suiv = myDepotFin;
				myClient->route = myRoute;
				myClient->estPresent = true;
				myDepotFin->pred = myClient;
				localSearch->ordreParcours[kk].push_back(myClient->cour);

				// infos sommets milieu
				for (int k = (int)i + 1; k <= j - 2; k++)
				{
					myClient = localSearch->clients[kk][chromT[kk][k]];
					myClient->pred = localSearch->clients[kk][chromT[kk][k - 1]];
					myClient->suiv = localSearch->clients[kk][chromT[kk][k + 1]];
					myClient->route = myRoute;
					myClient->estPresent = true;
					localSearch->ordreParcours[kk].push_back(myClient->cour);
				}
			}
			j = i;
		}
		// pour chaque route on met les charges partielles � jour
		for (i = 0; i < (int)localSearch->routes[kk].size(); i++)
			localSearch->routes[kk][i]->updateRouteData();
	}
}

// mise � jour du chromT suite aux modification de localSearch
void Individu::updateIndiv()
{
	// Don't forget to copy back the load delivered to each customer on each day
	chromL = localSearch->demandPerDay;
	vector<Route *> ordreRoutesAngle;
	Route *temp;
	Noeud *node;

	for (int kk = 1; kk <= params->nbDays; kk++)
	{
		ordreRoutesAngle = localSearch->routes[kk];

		// on calcule les angles des centroides
		for (int r = 0; r < (int)ordreRoutesAngle.size(); r++)
			ordreRoutesAngle[r]->updateCentroidCoord();

		// on trie les routes dans l'ordre des centroides
		if (params->triCentroides)
			for (int r = 0; r < (int)ordreRoutesAngle.size(); r++)
				for (int rr = 1; rr < (int)ordreRoutesAngle.size() - r - 1; rr++)
					if (ordreRoutesAngle[rr]->centroidAngle > ordreRoutesAngle[rr + 1]->centroidAngle)
					{
						temp = ordreRoutesAngle[rr + 1];
						ordreRoutesAngle[rr + 1] = ordreRoutesAngle[rr];
						ordreRoutesAngle[rr] = temp;
					}

		// on recopie les noeuds dans le chromosome
		chromT[kk].clear();
		for (int r = 0; r < (int)ordreRoutesAngle.size(); r++)
		{
			node = ordreRoutesAngle[r]->depot->suiv;
			while (!node->estUnDepot)
			{
				chromT[kk].push_back(node->cour);
				node = node->suiv;
			}
		}
	}

	generalSplit();
}

// Computes the maximum amount of load that can be delivered to client on a day k without exceeding the
// customer maximum inventory
double Individu::maxFeasibleDeliveryQuantity(int day, int client)
{
	// Printing customer inventory and computing customer inventory cost
	double inventoryClient;
	double minSlack = 1.e30;

	inventoryClient = params->cli[client].startingInventory;
	for (int k = 1; k <= params->nbDays; k++)
	{
		// here level in the morning
		inventoryClient += chromL[k][client];
		// level after receiving inventory

		// updating the residual capacity if k is greater than the day
		if (k >= day && params->cli[client].maxInventory - inventoryClient < minSlack)
			minSlack = params->cli[client].maxInventory - inventoryClient;

		if (minSlack < -0.0001)
		{
			cout << "ISSUE : negative slack capacit during crossover, this should not happen" << endl;
			throw("ISSUE : negative slack capacit during crossover, this should not happen");
		}

		inventoryClient -= params->cli[client].dailyDemand[k];
		// level after consumption
	}

	if (minSlack < 0.0001) // avoiding rounding issues
		return 0.;
	else
		return minSlack;
}

// distance g�n�rale
// il faut que les suivants de chaque individu aient �t� calcul�s
double Individu::distance(Individu *indiv2)
{
	int note = 0;
	bool isIdentical;

	// Inventory Routing
	// distance based on number of customers which have different service days
	if (params->isInventoryRouting)
	{
		for (int j = params->nbDepots; j < params->nbClients + params->nbDepots; j++)
		{
			isIdentical = true;
			for (int k = 1; k <= params->nbDays; k++)
				if ((chromL[k][j] < 0.0001 && indiv2->chromL[k][j] > 0.0001) || (indiv2->chromL[k][j] < 0.0001 && chromL[k][j] > 0.0001))
					isIdentical = false;
			if (isIdentical == false)
				note++;
		}
	}
	// CVRP case, broken-pairs distance
	else
	{
		for (int j = params->nbDepots; j < params->nbClients + params->nbDepots; j++)
		{
			isIdentical = true;
			for (int s = 0; s < (int)suivants[j].size(); s++)
				if ((suivants[j][s] != indiv2->suivants[j][s] || precedents[j][s] != indiv2->precedents[j][s]) && (precedents[j][s] != indiv2->suivants[j][s] || suivants[j][s] != indiv2->precedents[j][s]))
					isIdentical = false;
			if (!isIdentical)
				note++;
		}
	}

	return ((double)note / (double)(params->nbClients));
}

// calcul des suivants
void Individu::computeSuivants()
{
	vector<int> vide;
	int s, jj;
	for (int i = 0; i < params->nbDepots + params->nbClients; i++)
	{
		suivants[i] = vide;
		precedents[i] = vide;
	}

	// on va noter pour chaque individu et chaque client
	// la liste des clients plac�s juste apr�s lui dans le giant tour pour chaque jour
	for (int k = 1; k <= params->nbDays; k++)
		if (chromT[k].size() != 0)
		{
			for (int i = 0; i < (int)chromT[k].size() - 1; i++)
				suivants[chromT[k][i]].push_back(chromT[k][i + 1]);

			for (int i = 1; i < (int)chromT[k].size(); i++)
				precedents[chromT[k][i]].push_back(chromT[k][i - 1]);

			suivants[chromT[k][chromT[k].size() - 1]].push_back(chromT[k][0]);
			precedents[chromT[k][0]].push_back(chromT[k][chromT[k].size() - 1]);

			// on enl�ve ceux qui sont des fins de routes
			s = (int)chromT[k].size();
			jj = 0;
			while (s != 0 && jj < params->nombreVehicules[k])
			{
				suivants[chromT[k][s - 1]].pop_back();
				suivants[chromT[k][s - 1]].push_back(params->ordreVehicules[k][params->nombreVehicules[k] - jj - 1].depotNumber);
				s = pred[k][params->nombreVehicules[k] - jj][s];
				precedents[chromT[k][s]].pop_back();
				precedents[chromT[k][s]].push_back(params->ordreVehicules[k][params->nombreVehicules[k] - jj - 1].depotNumber);
				jj++;
			}
		}
}

// ajoute un element proche dans les structures de proximit�
void Individu::addProche(Individu *indiv)
{
	list<proxData>::iterator it;
	proxData data;
	data.indiv = indiv;
	data.dist = distance(indiv);

	if (plusProches.empty())
		plusProches.push_back(data);
	else
	{
		it = plusProches.begin();
		while (it != plusProches.end() && it->dist < data.dist)
			++it;
		plusProches.insert(it, data);
	}
}

// enleve un element dans les structures de proximit�
void Individu::removeProche(Individu *indiv)
{
	list<proxData>::iterator last = plusProches.end();
	for (list<proxData>::iterator first = plusProches.begin(); first != last;)
		if (first->indiv == indiv)
			first = plusProches.erase(first);
		else
			++first;
}

// distance moyenne avec les n individus les plus proches
double Individu::distPlusProche(int n)
{
	double result = 0;
	double compte = 0;
	list<proxData>::iterator it = plusProches.begin();

	for (int i = 0; i < n && it != plusProches.end(); i++)
	{
		result += it->dist;
		compte += 1.0;
		++it;
	}
	return result / compte;
}
