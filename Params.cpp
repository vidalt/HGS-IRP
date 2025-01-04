#include "Params.h"

// creating the parameters from the instance file
Params::Params(string nomInstance, string nomSolution, int type, int nbVeh, string nomBKS, int seedRNG, int rou,bool stockout) : 
	type(type), nbVehiculesPerDep(nbVeh)
{
	seed = seedRNG;

	if (seed == 0)
		rng = new Rng((unsigned long long)time(NULL));
	else
		rng = new Rng((unsigned long long)(seed));

	pathToInstance = nomInstance;
	pathToSolution = nomSolution;
	pathToBKS = nomBKS;

	debut = clock();
	//PItime = 0;
	nbVehiculesPerDep = nbVeh;

	// ouverture du fichier en lecture
	fichier.open(nomInstance.c_str());

	// parsing des donn�es打开文件供读取
	if (fichier.is_open())
		preleveDonnees(nomInstance, rou, stockout);
	else
	{
		cout << "Unable to open file : " << nomInstance << endl;
		throw string(" Unable to open file ");
	}
	cout << "Read file done" << endl;

	// Setting the parameters
	setMethodParams();
	
	// Simply compute the distances from the coordinates
	computeDistancierFromCoords();

	// calcul des structures 结构计算
	calculeStructures();

	// Compute the constant value in the objective function
	if(stockout) computeConstant_stockout();
	else computeConstant();
	
}

Params::Params(string nomInstance, string nomSolution, int type, int nbVeh, string nomBKS, int seedRNG) : 
	type(type), nbVehiculesPerDep(nbVeh)
{
	seed = seedRNG;

	if (seed == 0)
		rng = new Rng((unsigned long long)time(NULL));
	else
		rng = new Rng((unsigned long long)(seed));

	pathToInstance = nomInstance;
	pathToSolution = nomSolution;
	pathToBKS = nomBKS;

	debut = clock();
	nbVehiculesPerDep = nbVeh;

	// ouverture du fichier en lecture
	fichier.open(nomInstance.c_str());

	// parsing des donn�es打开文件供读取
	if (fichier.is_open())
		preleveDonnees(nomInstance, 0,0);
	else
	{
		cout << "Unable to open file : " << nomInstance << endl;
		throw string(" Unable to open file ");
	}
	cout << "read file done" << endl;

	// Setting the parameters
	setMethodParams();

	// Simply compute the distances from the coordinates
	computeDistancierFromCoords();

	// calcul des structures 结构计算
	calculeStructures();

	// Compute the constant value in the objective function
	computeConstant();

	// for instances 27-32 of the PVRP, there is a small patch to avoid errors because the objective function
	// is of a very different magnitude, need to give a more adapted starting value for the penalties
	
}

void Params::computeConstant()
{
	objectiveConstant = 0;


	if (isInventoryRouting)
	{

		// Removing the customer inventory cost once the product has been consumed (CONSTANT IN OBJECTIVE)
		for (int k = 1; k <= ancienNbDays; k++)
			for (int i = nbDepots; i < nbDepots + nbClients; i++)
				objectiveConstant -= cli[i].dailyDemand[k] * (ancienNbDays + 1 - k) * cli[i].inventoryCost;

		// Adding the cost of the initial inventory at the customer location (CONSTANT IN OBJECTIVE)
		for (int i = nbDepots; i < nbDepots + nbClients; i++)
			objectiveConstant += cli[i].startingInventory * (ancienNbDays + 1 - 1) * cli[i].inventoryCost;

		// Adding the total cost for supplier inventory over the planning horizon (CONSTANT IN OBJECTIVE)
		for (int k = 1; k <= ancienNbDays; k++)
			objectiveConstant += availableSupply[k] * (ancienNbDays + 1 - k) * inventoryCostSupplier;

	}
	
}

void Params::computeDistancierFromCoords()
{
	double d;
	vector<double> dist;

	// on remplit les distances dans timeCost
	for (int i = 0; i < nbClients + nbDepots; i++)
	{
		dist.clear();
		for (int j = 0; j < nbClients + nbDepots; j++)
		{
			d = sqrt((cli[i].coord.x - cli[j].coord.x) * (cli[i].coord.x - cli[j].coord.x) +
					 (cli[i].coord.y - cli[j].coord.y) * (cli[i].coord.y - cli[j].coord.y));

			// integer rounding
			if (isRoundingInteger) // to be able to deal with other rounding conventions
			{
				d += 0.5;
				d = (double)(int)d;
			}
			else if (isRoundingTwoDigits)
			{
				d = d * 100.0;
				d += 0.5;
				d = (double)(int)d;
				d = d * 0.01;
			}

			dist.push_back((double)d);
		}
		timeCost.push_back(dist);
	}
}

void Params::setMethodParams()
{
	// parameters related to how the problem is treated
	conversionToPVRP = true;
	triCentroides = false;	  // is the chromosome ordered by barycenter of the routes (CVRP case, c.f. OR2012 paper)
	isRoundingInteger = true; // using the rounding (for now set to true, because currently testing on the instances of Uchoa)
	isRoundingTwoDigits = false;

	// parameters of the population  //人口参数
	el = 3;					// ***
	mu = 12;				// *
	lambda = 25;			// *
	//PItime=0;
	nbCountDistMeasure = 5; // o
	rho = 0.30;				// o
	delta = 0.001;			// o

	// parameters of the mutation	//  突变参数
	maxLSPhases = 10000; // number of LS phases, here RI-PI-RI  // ***
	prox = 40;			 // granularity parameter (expressed as a percentage of the problem size -- 35%) // ***
	proxCst = 1000000;	 // granularity parameter (expressed as a fixed maximum)
	prox2Cst = 1000000;	 // granularity parameter on PI
	pRep = 0.5;			 // probability of repair // o

	// param�tres li�s aux p�nalit�s adaptatives  适应性参数
	penalityCapa = max(10,seed*50);	// Initial penalty values // o
	penalityLength = 1; // Initial penalty values // o
	minValides = 0.15;	// Target range for the number of feasible solutions // **
	maxValides = 0.25;	// Target range for the number of feasible solutions // **
	distMin = 0.01;		// Distance in terms of objective function under which the solutions are considered to be the same // o
	borneSplit = 2.0;	// Split parameter (how far to search beyond the capacity limit) // o

	// necessary adjustments for the CVRP (cf. OR2012)
	if (!isInventoryRouting)
	{
		maxLSPhases = 1;
		triCentroides = true;
		proxCst = 20;
		prox2Cst = 200;
	}
}

Params::~Params(void) {}

// effectue le prelevement des donnees du fichier 从文件中获取数据
void Params::preleveDonnees(string nomInstance, int rou, bool stockout)
{
	// variables temporaires utilisees dans la fonction //临时变量
	vector<Client> cTemp;
	vector<Vehicle> tempI;
	Client client;
	double rt, vc;
	string contenu;
	string useless2;
	multiDepot = false;
	periodique = false;
	isInventoryRouting = false;
	isstockout = false;
	//C. Archetti, L. Bertazzi, G. Laporte, and M.G. Speranza. A branch-and-cut algorithm for a vendor-managed inventory-routing problem. Transportation Science, 41:382-391, 2007 Instances

	if (type == 38) // IRP format of Archetti http://or-brescia.unibs.it/instances 
	{
		cout << "path: " << nomInstance << endl;
		isInventoryRouting = true;
		isstockout = stockout;
		cout<<"isstockout "<<isstockout<<endl;
		// nbVehiculesPerDep = 2;
		if (nbVehiculesPerDep == -1)
		{
			cout << "ERROR : Need to specify fleet size" << endl;
			throw string("ERROR : Need to specify fleet size");
		}
		fichier >> nbClients;
		nbClients--; // the given number of nodes also counts the supplier/depot
		fichier >> nbDays;
		fichier >> vc; //vehicle capacityx
		rt = 1000000;
		nbDepots = 1;
		ancienNbDays = nbDays;

		ordreVehicules.push_back(tempI);
		nombreVehicules.push_back(0);
		dayCapacity.push_back(0);
		for (int kk = 1; kk <= nbDays; kk++)
		{
			ordreVehicules.push_back(tempI);
			dayCapacity.push_back(0); //每天的车辆总容量。xxxz
			nombreVehicules.push_back(nbDepots * nbVehiculesPerDep);  //仓库*每个仓库的车辆数
			for (int i = 0; i < nbDepots; i++)
				for (int j = 0; j < nbVehiculesPerDep; j++) //规划每天应该使用哪些车辆。每个车都初始化
					ordreVehicules[kk].push_back(Vehicle(-1, -1, -1)); //特定的仓库号、最大行驶时间和车辆容量。
		}

		for (int kk = 1; kk <= nbDays; kk++)
		{
			for (int j = 0; j < nbVehiculesPerDep; j++)
			{
				ordreVehicules[kk][j].depotNumber = 0;
				ordreVehicules[kk][j].maxRouteTime = rt;
				ordreVehicules[kk][j].vehicleCapacity = vc;
				dayCapacity[kk] += vc;
			}
		}
	}

	// format classique de VRP Cordeau
	else if (type == 0)
	{
		// premiere ligne: description du probleme，问题描述
		fichier >> type >> nbVehiculesPerDep >> nbClients;
		fichier >> nbDays;
		nbDepots = 1;
		ancienNbDays = nbDays;

		ordreVehicules.push_back(tempI);
		nombreVehicules.push_back(0);
		dayCapacity.push_back(0);
		for (int kk = 1; kk <= nbDays; kk++)
		{
			ordreVehicules.push_back(tempI);
			dayCapacity.push_back(0);
			nombreVehicules.push_back(nbDepots * nbVehiculesPerDep);
			for (int i = 0; i < nbDepots; i++)
				for (int j = 0; j < nbVehiculesPerDep; j++)
					ordreVehicules[kk].push_back(Vehicle(-1, -1, -1));
		}

		// caracteristiques des vehicules , dans l'ordre depot1*day1 ... depot1*day2 ...
		for (int i = 0; i < nbDepots; i++)
		{
			for (int kk = 1; kk <= nbDays; kk++)
			{
				fichier >> rt >> vc;
				if (rt == 0)
					rt = 100000;
				for (int j = 0; j < nbVehiculesPerDep; j++)
				{
					ordreVehicules[kk][nbVehiculesPerDep * i + j].depotNumber = i;
					ordreVehicules[kk][nbVehiculesPerDep * i + j].maxRouteTime = rt;
					ordreVehicules[kk][nbVehiculesPerDep * i + j].vehicleCapacity = vc;
					dayCapacity[kk] += vc;
				}
			}
		}

		// Filling the data of the IRP to be able to solve the CVRP as a special case
		availableSupply.push_back(0.);
		availableSupply.push_back(1.e30); // all supply needed on day 1
		inventoryCostSupplier = 0.;
	}
	
	// Liste des clients
	for (int i = 0; i < nbClients + nbDepots; i++)
	{
		Client client = getClient(i,rou);
		
		cli.push_back(client);
	}
	//cout <<"client["<<2<<"]"<<cli[2].maxInventory<<"   "<<cli[2].minInventory<<"   "<<cli[2].startingInventory<<endl;
}

// calcule les autres structures du programme  计算程序中的其他结构
void Params::calculeStructures()
{
	int temp;
	vector<bool> tempB2;
	vector<vector<bool>> tempB;
	vector<double> dist;
	float distanceMax = 0;

	// initializing the correlation matrix
	for (int i = 0; i < nbClients + nbDepots; i++)
	{
		isCorrelated1.push_back(tempB2);
		isCorrelated2.push_back(tempB2);
		for (int j = 0; j < nbClients + nbDepots; j++)
		{
			isCorrelated1[i].push_back(false);
			isCorrelated2[i].push_back(false);
		}
	}

	for (int i = 0; i < nbClients + nbDepots; i++)
	{
		cli[i].ordreProximite.clear();
		cli[i].sommetsVoisins.clear();
	}

	// on remplit la liste des plus proches pour chaque client
	//根据客户与每个客户/仓库之间的时间距离对ordreProximite列表进行排序
	for (int i = nbDepots; i < nbClients + nbDepots; i++)
	{
		for (int j = 0; j < nbClients + nbDepots; j++)
			if (i != j)
				cli[i].ordreProximite.push_back(j); 

		// et on la classe
		for (int a1 = 0; a1 < nbClients + nbDepots - 1; a1++)
			for (int a2 = 0; a2 < nbClients + nbDepots - a1 - 2; a2++)
				if (timeCost[i][cli[i].ordreProximite[a2]] > timeCost[i][cli[i].ordreProximite[a2 + 1]])
				{
					temp = cli[i].ordreProximite[a2 + 1];
					cli[i].ordreProximite[a2 + 1] = cli[i].ordreProximite[a2];
					cli[i].ordreProximite[a2] = temp;
				}

		// on remplit les x% plus proches
		//基于prox（可能表示要考虑的最近客户的百分比）和proxCst（可能表示要考虑的最近客户的固定数量）
		//来确定哪些客户/仓库与当前客户最接近，并将它们添加到sommetsVoisins列表中。这也会在isCorrelated1中设置相应的值。
		for (int j = 0; j < (prox * (int)cli[i].ordreProximite.size()) / 100 && j < proxCst; j++)
		{
			cli[i].sommetsVoisins.push_back(cli[i].ordreProximite[j]);
			isCorrelated1[i][cli[i].ordreProximite[j]] = true;
		}
		for (int j = 0; j < (int)cli[i].ordreProximite.size() && j < prox2Cst; j++)
			isCorrelated2[i][cli[i].ordreProximite[j]] = true;
	}

	// on melange les proches来随机化sommetsVoisins列表的顺序。
	shuffleProches();

	// on calcule les tableaux de pattern dynamiques
	for (int i = 0; i < (int)cli.size(); i++)
	{
		cli[i].computeVisitsDyn();
		cli[i].computeJourSuiv();
	}
}

// sous routine du prelevement de donnees
Client Params::getClient(int i,int rou)
{
	struct couple coordonnees;
	Client client;
	int nbPattern;
	pattern p;

	// file format of Cordeau et al.
	if (type == 0)
	{
		fichier >> client.custNum >> coordonnees.x >> coordonnees.y >> client.serviceDuration >> client.demand >> client.freq >> nbPattern;

		// Filling the IRP fields to be able to preserve the CVRP as a special case of the IRP solver
		client.dailyDemand.push_back(0.);
		client.dailyDemand.push_back(client.demand);
		client.inventoryCost = 0.;
		client.startingInventory = 0.;
		client.minInventory = 0.;
		client.maxInventory = client.demand;

		client.coord = coordonnees;
		client.nbJours = nbDays;

		p.dep = 0;
		for (int j = 0; j < nbPattern; j++)
		{
			fichier >> p.pat;
			client.visits.push_back(p);
			client.visitsOrigin.push_back(p);
		}
	}
	else if (type == 38) // IRP format of Archetti et al.
	{
		fichier >> client.custNum;
		client.custNum--;
		fichier >> coordonnees.x >> coordonnees.y;
		client.serviceDuration = 0;
		client.coord = coordonnees;
		client.nbJours = nbDays;

		if (client.custNum == 0) // information of the supplier
		{
			client.freq = 0;
			double initInventory;
			double dailyProduction;
			fichier >> initInventory;
			fichier >> dailyProduction;
			availableSupply = vector<double>(nbDays + 1, 0.); // days are indexed from 1 ... t
			for (int t = 1; t <= nbDays; t++)
				availableSupply[t] = dailyProduction;
			availableSupply[1] += initInventory;
			fichier >> inventoryCostSupplier;
		}
		else //information of each customer
		{
			client.freq = 1;
			fichier >> client.startingInventory;
			fichier >> client.maxInventory;
			fichier >> client.minInventory;
			double myDailyDemand;
			fichier >> myDailyDemand;
			client.dailyDemand = vector<double>(nbDays + 1, myDailyDemand);
			fichier >> client.inventoryCost;
			client.stockoutCost = client.inventoryCost*rou;
		}

		client.demand = 1.e20; // Just to make sure I never rely on this field for the IRP for now (later on it will totally disappear)
		p.dep = 0;
		p.pat = 1;
		client.visits.push_back(p);
		client.visitsOrigin.push_back(p);
	}
	return client;
}

void Params::shuffleProches()
{
	int temp, temp2;

	// on introduit du d�sordre dans la liste des plus proches pour chaque client
	for (int i = nbDepots; i < nbClients + nbDepots; i++)
	{
		// on introduit du desordre
		for (int a1 = 0; a1 < (int)cli[i].sommetsVoisins.size() - 1; a1++)
		{
			temp2 = a1 + rng->genrand64_int64() % ((int)cli[i].sommetsVoisins.size() - a1);
			temp = cli[i].sommetsVoisins[a1];
			cli[i].sommetsVoisins[a1] = cli[i].sommetsVoisins[temp2];
			cli[i].sommetsVoisins[temp2] = temp;
		}
	}
}

Params::Params(Params *params, int decom, int *serieVisites, Vehicle **serieVehicles, int *affectDepots, int *affectPatterns, int depot, int jour, int nbVisites, int nbVeh)
{
	debut = clock();
	rng = params->rng;
	type = params->type;
	seed = params->seed;

	/* For now I just kept the CVRP decomposition */
	// if (decom == 2) decomposeDepots(params,affectDepots,depot);
	// else if (decom == 1) decomposeDays(params,affectPatterns,jour);
	if (decom == 0)
		decomposeRoutes(params, serieVisites, serieVehicles, nbVisites, nbVeh);
	else
		cout << "Error : Transformation not available actually" << endl;

	periodique = params->periodique;
	multiDepot = params->multiDepot;
	isInventoryRouting = params->isInventoryRouting;
	if (decom == 2)
		multiDepot = false;
	if (decom == 1)
		periodique = false;

	// affectation des autres parametres  其他参数的分配
	setMethodParams();
	penalityCapa = params->penalityCapa;
	penalityLength = params->penalityLength;

	mu = params->mu;
	lambda = params->lambda;
	el = params->el;
	nbCountDistMeasure = params->nbCountDistMeasure;

	// calcul des distances 计算距离
	// on remplit les distances dans timeCost 在 timeCost 中填写距离
	for (int i = 0; i < nbClients + nbDepots; i++)
	{
		timeCost.push_back(vector<double>());
		for (int j = 0; j < nbClients + nbDepots; j++)
			timeCost[i].push_back(params->timeCost[correspondanceTable[i]][correspondanceTable[j]]);
	}

	// calcul des structures
	calculeStructures();
}

void Params::decomposeRoutes(Params *params, int *serieVisites, Vehicle **serieVehicles, int nbVisites, int nbVeh)
{
	vector<Vehicle> temp;

	if (params->multiDepot || params->periodique)
		cout << "Attention decomposition VRP incorrecte" << endl;

	correspondanceTable2.clear();
	for (int i = 0; i < params->nbClients + params->nbDepots; i++)
		correspondanceTable2.push_back(-1);

	correspondanceTable.push_back(0);

	for (int i = 0; i < nbVisites; i++)
	{
		correspondanceTable.push_back(serieVisites[i]);
		correspondanceTable2[serieVisites[i]] = (int)correspondanceTable.size() - 1;
	}

	nbVehiculesPerDep = nbVeh;
	nbClients = correspondanceTable.size() - 1;
	nbDepots = 1;
	nbDays = 1;
	ancienNbDays = 1;
	borneSplit = 1.5;
	type = params->type;

	// on place les donn�es sur les v�hicules
	ordreVehicules.clear();
	nombreVehicules.clear();
	dayCapacity.clear();
	ordreVehicules.push_back(temp);
	nombreVehicules.push_back(0);
	dayCapacity.push_back(0);
	ordreVehicules.push_back(temp);
	nombreVehicules.push_back(nbVeh);
	dayCapacity.push_back(0);
	for (int v = 0; v < nbVeh; v++)
	{
		ordreVehicules[1].push_back(*serieVehicles[v]);
		dayCapacity[1] += serieVehicles[v]->vehicleCapacity;
	}

	// on met les bons clients
	// ils ont toujours les anciens num�ros
	for (int i = 0; i < nbDepots + nbClients; i++)
		cli.push_back(params->cli[correspondanceTable[i]]);
}

void Params::computeConstant_stockout()
{
	objectiveConstant_stockout = 0.;

	if (isInventoryRouting)
	{
		// Adding the total cost for supplier inventory over the planning horizon (CONSTANT IN OBJECTIVE)
		for (int k = 1; k <= ancienNbDays; k++)
			objectiveConstant_stockout += availableSupply[k] * (ancienNbDays + 1 - k) * inventoryCostSupplier;

	}
	
}