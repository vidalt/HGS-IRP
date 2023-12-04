#include "Population.h"

// constructeur
Population::Population(Params *params) : params(params)
{
	Individu *randomIndiv;
	trainer = new Individu(params, 1.0);
	delete trainer->localSearch;
	trainer->localSearch = new LocalSearch(params, trainer);

	double temp = params->penalityCapa;
	double temp2 = params->penalityLength;
	valides = new SousPop();
	invalides = new SousPop();
	valides->nbIndiv = 0;
	invalides->nbIndiv = 0;
	valides->nbGenerations = 0;
	invalides->nbGenerations = 0;
	bool compter = true;

	// TODO -- for testing for now
	/*randomIndiv = new Individu (params,1.0);
	recopieIndividu(trainer,randomIndiv);
	trainer->generalSplit();
	trainer->updateLS();
	trainer->localSearch->runILS(false,100);
	return ;*/

	for (int i = 0; i < params->mu * 2; i++)
	{
		if (i == params->mu)
		{
			params->penalityCapa *= 50;
			params->penalityLength *= 50;
			compter = false;
		}
		randomIndiv = new Individu(params, 1.0);
		education(randomIndiv);
		if (compter)
			updateNbValides(randomIndiv);
		addIndividu(randomIndiv);
		delete randomIndiv;
	}

	// on initialise par d�faut � 100, comme si tout �tait valide au d�but
	// mais c'est arbitraire
	for (int i = 0; i < 100; i++)
	{
		listeValiditeCharge.push_back(true);
		listeValiditeTemps.push_back(true);
	}

	params->penalityCapa = temp;
	params->penalityLength = temp2;
}

// destructeur
Population::~Population()
{
	for (int i = 0; i < (int)valides->individus.size(); i++)
		delete valides->individus[i];

	for (int i = 0; i < (int)invalides->individus.size(); i++)
		delete invalides->individus[i];

	delete trainer;
}

void Population::evalExtFit(SousPop *pop)
{
	int temp;
	vector<int> classement;
	vector<double> distances;

	for (int i = 0; i < pop->nbIndiv; i++)
	{
		classement.push_back(i);
		distances.push_back(pop->individus[i]->distPlusProche(params->nbCountDistMeasure));
	}

	// classement des individus en fonction de leur note de distance
	for (int n = 0; n < pop->nbIndiv; n++)
		for (int i = 0; i < pop->nbIndiv - n - 1; i++)
			if (distances[classement[i]] < distances[classement[i + 1]] - 0.000001)
			{
				temp = classement[i + 1];
				classement[i + 1] = classement[i];
				classement[i] = temp;
			}

	for (int i = 0; i < pop->nbIndiv; i++)
	{
		pop->individus[classement[i]]->divRank = (float)i / (float)(pop->nbIndiv - 1);
		pop->individus[classement[i]]->fitRank = (float)classement[i] / (float)(pop->nbIndiv - 1);
		pop->individus[classement[i]]->fitnessEtendu = pop->individus[classement[i]]->fitRank + ((float)1.0 - (float)params->el / (float)pop->nbIndiv) * pop->individus[classement[i]]->divRank;
	}
}

int Population::addIndividu(Individu *indiv)
{
	SousPop *souspop;
	int k, result;

	if (indiv->estValide)
		souspop = valides;
	else
		souspop = invalides;

	result = placeIndividu(souspop, indiv);
	// il faut �ventuellement enlever la moiti� de la pop
	if (result != -1 && souspop->nbIndiv > params->mu + params->lambda)
	{

		while (souspop->nbIndiv > params->mu)
		{
			k = selectCompromis(souspop);
			removeIndividu(souspop, k);
		}

		souspop->nbGenerations++;
	}

	return result;
}

// met � jour les individus les plus proches d'une population
// en fonction de l'arrivant
void Population::updateProximity(SousPop *pop, Individu *indiv)
{
	for (int k = 0; k < pop->nbIndiv; k++)
		if (pop->individus[k] != indiv)
		{
			pop->individus[k]->addProche(indiv);
			indiv->addProche(pop->individus[k]);
		}
}

// fonction booleenne verifiant si le fitness n'existe pas d�ja
bool Population::fitExist(SousPop *pop, Individu *indiv)
{
	double fitness = indiv->coutSol.evaluation;
	for (int i = 0; i < (int)pop->nbIndiv; i++)
	{
		if (indiv != pop->individus[i] && pop->individus[i]->coutSol.evaluation >= (fitness - params->delta) && pop->individus[i]->coutSol.evaluation <= (fitness + params->delta))
			return true;
	}
	return false;
}
// procede de redemarrage avec remplacement d'une partie de la population
// modele tres simplifie
// on remplace la moitie individus de fitness situes sous la moyenne par des individus aleatoires
void Population::diversify()
{
	Individu *randomIndiv;
	double temp = params->penalityCapa;
	double temp2 = params->penalityLength;

	while (valides->nbIndiv > (int)(params->rho * (double)params->mu))
	{
		delete valides->individus[valides->nbIndiv - 1];
		valides->individus.pop_back();
		valides->nbIndiv--;
	}

	while (invalides->nbIndiv > (int)(params->rho * (double)params->mu))
	{
		delete invalides->individus[invalides->nbIndiv - 1];
		invalides->individus.pop_back();
		invalides->nbIndiv--;
	}

	for (int i = 0; i < params->mu * 2; i++)
	{
		if (i == params->mu)
		{
			params->penalityCapa *= 50;
			params->penalityLength *= 50;
		}
		randomIndiv = new Individu(params, 1.0);
		education(randomIndiv);
		addIndividu(randomIndiv);
		delete randomIndiv;
	}

	params->penalityCapa = temp;
	params->penalityLength = temp2;
}

int Population::placeIndividu(SousPop *pop, Individu *indiv)
{

	Individu *monIndiv = new Individu(params);
	recopieIndividu(monIndiv, indiv);

	// regarde si son fitness est suffisamment espace
	bool placed = false;
	int i = (int)pop->individus.size() - 1;

	pop->individus.push_back(monIndiv);
	while (i >= 0 && !placed)
	{
		if (pop->individus[i]->coutSol.evaluation >= indiv->coutSol.evaluation + 0.001)
		{
			pop->individus[i + 1] = pop->individus[i];
			i--;
		}
		else
		{
			pop->individus[i + 1] = monIndiv;
			placed = true;
			pop->nbIndiv++;
			updateProximity(pop, pop->individus[i + 1]);
			return i + 1; // reussite
		}
	}
	if (!placed)
	{
		pop->individus[0] = monIndiv;
		placed = true;
		pop->nbIndiv++;
		updateProximity(pop, pop->individus[0]);
		if (pop == valides)
			timeBest = clock();
		return 0; // reussite
	}
	cout << "erreur placeIndividu" << endl;
	return -3;
}

void Population::removeIndividu(SousPop *pop, int p)
{
	Individu *partant = pop->individus[p];

	// on place notre individu � la fin
	for (int i = p + 1; i < (int)pop->individus.size(); i++)
		pop->individus[i - 1] = pop->individus[i];

	// on l'enleve de la population
	pop->individus.pop_back();
	pop->nbIndiv--;

	// on l'enleve des structures de proximit�
	for (int i = 0; i < pop->nbIndiv; i++)
		pop->individus[i]->removeProche(partant);

	// et on supprime le partant
	delete partant;
}
// recalcule l'evaluation des individus a partir des violation
// puis effectue un tri � bulles de la population
// operateur de tri peu efficace mais fonction appell�e tr�s rarement
void Population::validatePen()
{
	Individu *indiv;
	// on met � jour les evaluations
	for (int i = 0; i < invalides->nbIndiv; i++)
		invalides->individus[i]->coutSol.evaluation = invalides->individus[i]->coutSol.fitness + params->penalityCapa * invalides->individus[i]->coutSol.capacityViol + params->penalityLength * invalides->individus[i]->coutSol.lengthViol;

	for (int i = 0; i < invalides->nbIndiv; i++)
		for (int j = 0; j < invalides->nbIndiv - i - 1; j++)
		{
			if (invalides->individus[j]->coutSol.evaluation > invalides->individus[j + 1]->coutSol.evaluation)
			{
				indiv = invalides->individus[j];
				invalides->individus[j] = invalides->individus[j + 1];
				invalides->individus[j + 1] = indiv;
			}
		}
}

Individu *Population::getIndividuBinT(double &rangRelatif)
{
	Individu *individu1;
	Individu *individu2;
	int place1, place2;
	double rangRelatif1, rangRelatif2;

	place1 = params->rng->genrand64_int64() % (valides->nbIndiv + invalides->nbIndiv);
	if (place1 >= valides->nbIndiv)
	{
		individu1 = invalides->individus[place1 - valides->nbIndiv];
		rangRelatif1 = (double)(place1 - valides->nbIndiv) / (double)invalides->nbIndiv;
	}
	else
	{
		individu1 = valides->individus[place1];
		rangRelatif1 = (double)place1 / (double)valides->nbIndiv;
	}

	place2 = params->rng->genrand64_int64() % (valides->nbIndiv + invalides->nbIndiv);
	if (place2 >= valides->nbIndiv)
	{
		individu2 = invalides->individus[place2 - valides->nbIndiv];
		rangRelatif2 = (double)(place2 - valides->nbIndiv) / (double)invalides->nbIndiv;
	}
	else
	{
		individu2 = valides->individus[place2];
		rangRelatif2 = (double)place2 / (double)valides->nbIndiv;
	}

	evalExtFit(valides);
	evalExtFit(invalides);

	if (individu1->fitnessEtendu < individu2->fitnessEtendu)
	{
		rangRelatif = rangRelatif1;
		return individu1;
	}
	else
	{
		rangRelatif = rangRelatif2;
		return individu2;
	}
}

// rank donne le rang de l'individu choisi
Individu *Population::getIndividuRand(double &rangRelatif)
{
	Individu *individu1;

	int place1 = params->rng->genrand64_int64() % (valides->nbIndiv + invalides->nbIndiv);
	if (place1 >= valides->nbIndiv)
	{
		individu1 = invalides->individus[place1 - valides->nbIndiv];
		rangRelatif = (double)(place1 - valides->nbIndiv) / (double)invalides->nbIndiv;
	}
	else
	{
		individu1 = valides->individus[place1];
		rangRelatif = (double)place1 / (double)valides->nbIndiv;
	}
	return valides->individus[place1];
}
// getter de 1 individu par selection au hasard dans un certain % des meilleurs meilleurs
// rank donne le rang de l'individu choisi
Individu *Population::getIndividuPourc(int pourcentage, double &rangRelatif)
{
	int place;
	if ((valides->nbIndiv * pourcentage) / 100 != 0)
	{
		place = params->rng->genrand64_int64() % ((valides->nbIndiv * pourcentage) / 100);
		rangRelatif = (double)place / (double)params->mu;
		return valides->individus[place];
	}
	else
		return getIndividuBinT(rangRelatif);
}
Individu *Population::getIndividuBestValide()
{
	if (valides->nbIndiv != 0)
		return valides->individus[0];
	else
		return NULL;
}

Individu *Population::getIndividuBestInvalide()
{
	if (invalides->nbIndiv != 0)
		return invalides->individus[0];
	else
		return NULL;
}

// getter simple d'un individu
Individu *Population::getIndividu(int p)
{
	return valides->individus[p];
}
// recopie un Individu dans un autre
// ATTENTION !!! ne recopie que le chromP, chromT et les attributs du fitness
void Population::recopieIndividu(Individu *destination, Individu *source)
{
	destination->chromT = source->chromT;
	destination->chromL = source->chromL;
	destination->chromR = source->chromR;
	destination->coutSol = source->coutSol;
	destination->isFitnessComputed = source->isFitnessComputed;
	destination->estValide = source->estValide;
	destination->suivants = source->suivants;
	destination->precedents = source->precedents;
}

void Population::ExportPop(string nomFichier)
{
	// exporte les solutions actuelles des individus dans un dossier
	vector<int> rout;
	vector<double> routTime;
	int compteur;
	Noeud *noeudActuel;
	LocalSearch *loc;
	ofstream myfile;
	double cost;
	double temp, temp2;
	char *myBuff;

	Individu *bestValide = getIndividuBestValide();

	if (bestValide != NULL)
	{

		// on met � jour la structure de recherche locale pour bien avoir acces aux routes
		// oblig� de mettre des p�nalit�s tr�s forte , pour que le split n'aie pas l'id�e de cr�er une
		// solution invalide � partir de la meilleure solution valide
		temp = params->penalityCapa;
		temp2 = params->penalityLength;
		params->penalityCapa = 10000;
		params->penalityLength = 10000;
		education(bestValide);
		// le trainer a gard� les infos des routes de bestValide
		loc = trainer->localSearch;
		params->penalityCapa = temp;
		params->penalityLength = temp2;

		loc->printInventoryLevels();

		myfile.precision(10);
		cout.precision(10);

		cout << "ecriture de la solution du meilleur : fitness " << trainer->coutSol.evaluation << " dans : " << nomFichier.c_str() << endl;
		myfile.open(nomFichier.data());

		// 1: export cost
		myfile << trainer->coutSol.evaluation << endl;

		// 2: exporting the number of routes
		compteur = 0;
		for (int k = 1; k <= params->nbDays; k++)
			for (int i = 0; i < (int)loc->routes[k].size(); i++)
				if (!loc->depots[k][i]->suiv->estUnDepot)
					compteur++;
		myfile << compteur << endl;

		// exporting the total CPU time (ms)
		myBuff = new char[100];

		int cpuTime = (int)(clock() / 1000);
		cout << "cpu time: " << cpuTime << " size: " << sizeof(cpuTime) << endl;

		sprintf(myBuff, "%d", (int)(clock() / 1000));
		// strncpy(myBuff, "%d", (int)(clock() / 1000));
		myfile << myBuff << endl;

		// exporting the time to best solution
		myBuff = new char[100];
		int bestTime = (int)(timeBest / 1000);
		cout << "best time: " << bestTime << " size: " << sizeof(bestTime) << endl;

		sprintf(myBuff, "%d", (int)(timeBest / 1000));
		// strncpy(myBuff, "%d", (int)(timeBest / 1000));
		myfile << myBuff << endl;

		for (int k = 1; k <= params->nbDays; k++)
		{
			for (int i = 0; i < (int)loc->routes[k].size(); i++)
			{
				compteur = 1;
				if (!loc->depots[k][i]->suiv->estUnDepot)
				{

					myfile << " " << loc->routes[k][i]->depot->cour << " " << (k - 1) % params->ancienNbDays + 1 << " " << compteur << " " << loc->routes[k][i]->temps << " "
						   << loc->routes[k][i]->charge << " ";

					// on place la route dans un vecteur de noeuds clients
					noeudActuel = loc->routes[k][i]->depot->suiv;
					cost = params->timeCost[loc->routes[k][i]->depot->cour][noeudActuel->cour];

					rout.clear();
					routTime.clear();
					rout.push_back(loc->routes[k][i]->depot->cour);
					routTime.push_back(0);
					rout.push_back(noeudActuel->cour);
					routTime.push_back(cost);

					while (!noeudActuel->estUnDepot)
					{
						cost += params->cli[noeudActuel->cour].serviceDuration + params->timeCost[noeudActuel->cour][noeudActuel->suiv->cour];
						noeudActuel = noeudActuel->suiv;
						rout.push_back(noeudActuel->cour);
						routTime.push_back(cost);
					}

					myfile << " " << (int)rout.size();

					for (int j = 0; j < (int)rout.size(); j++)
					{
						myfile << " " << rout[j];
					}
					myfile << endl;
					compteur++;
				}
			}
		}

		myfile.close();
	}
	else
	{
		cout << " impossible de trouver un individu valide " << endl;
	}
}

void Population::ExportBKS(string nomFichier)
{
	double fit;
	ifstream fichier;
	fichier.open(nomFichier.c_str());
	if (fichier.is_open())
	{
		fichier >> fit;
		fichier.close();
		if (getIndividuBestValide() != NULL && getIndividuBestValide()->coutSol.evaluation < fit - 0.01)
		{
			cout << "!!! new BKS !!! : " << getIndividuBestValide()->coutSol.evaluation << endl;
			ExportPop(nomFichier);
		}
	}
	else
	{
		cout << " Fichier de BKS introuvable " << endl;
		ExportPop(nomFichier);
	}
}
// retourne la fraction d'individus valides en terme de charge
double Population::fractionValidesCharge()
{
	int count = 0;
	for (list<bool>::iterator it = listeValiditeCharge.begin(); it != listeValiditeCharge.end(); ++it)
		if (*it == true)
			count++;

	return double(count) / (double)(100);
}

// retourne la fraction d'individus valides en terme de temps
double Population::fractionValidesTemps()
{
	int count = 0;
	for (list<bool>::iterator it = listeValiditeTemps.begin(); it != listeValiditeTemps.end(); ++it)
		if (*it == true)
			count++;

	return double(count) / (double)(100);
}

double Population::getDiversity(SousPop *pop)
{
	double total = 0;
	int count = 0;
	for (int i = 0; i < pop->nbIndiv / 2; i++)
		for (int j = i + 1; j < pop->nbIndiv / 2; j++)
		{
			total += pop->individus[i]->distance(pop->individus[j]);
			count++;
		}
	return total / (double)count;
}

double Population::getMoyenneValides()
{
	double moyenne = 0;
	for (int i = 0; i < valides->nbIndiv / 2; i++)
		moyenne += valides->individus[i]->coutSol.evaluation;
	return moyenne / (valides->nbIndiv / 2);
}
double Population::getMoyenneInvalides()
{
	double moyenne = 0;
	for (int i = 0; i < invalides->nbIndiv / 2; i++)
		moyenne += invalides->individus[i]->coutSol.evaluation;
	return moyenne / (invalides->nbIndiv / 2);
}

int Population::selectCompromis(SousPop *souspop)
{
	double pireFitnessEtendu = 0;
	int mauvais = -1;
	vector<int> classement;
	int temp, sortant;

	evalExtFit(souspop);

	// pour chaque individu on modifie le fitness etendu
	for (int i = 0; i < souspop->nbIndiv; i++)
	{
		classement.push_back(i);
		if (souspop->individus[i]->distPlusProche(1) < params->distMin)
			souspop->individus[i]->fitnessEtendu += 5;
		// for the CVRP instances, we need to allow duplicates with the same fitness since in the Golden instances
		// there is a lot of symmetry.
		if (fitExist(souspop, souspop->individus[i]))
			souspop->individus[i]->fitnessEtendu += 5;
	}

	// on classe les elements par fitness etendu et on prend le plus mauvais
	for (int n = 0; n < souspop->nbIndiv; n++)
		for (int i = 1; i < souspop->nbIndiv - n - 1; i++)
			if (souspop->individus[classement[i]]->fitnessEtendu > souspop->individus[classement[i + 1]]->fitnessEtendu)
			{
				temp = classement[i + 1];
				classement[i + 1] = classement[i];
				classement[i] = temp;
			}

	sortant = classement[souspop->nbIndiv - 1];

	if (params->rng->genrand64_real1() < -1)
		cout << souspop->individus[sortant]->fitRank << " "
			 << souspop->individus[sortant]->divRank << " "
			 << souspop->individus[sortant]->fitnessEtendu << endl;

	return sortant;
}

void Population::education(Individu *indiv)
{
	recopieIndividu(trainer, indiv);
	trainer->generalSplit();
	trainer->updateLS();
	trainer->localSearch->runSearchTotal(false);
	trainer->updateIndiv();
	recopieIndividu(indiv, trainer);
}

// met a jour le compte des valides
void Population::updateNbValides(Individu *indiv)
{
	listeValiditeCharge.push_back(indiv->coutSol.capacityViol < 0.0001);
	listeValiditeCharge.pop_front();
	listeValiditeTemps.push_back(indiv->coutSol.lengthViol < 0.0001);
	listeValiditeTemps.pop_front();
}

void Population::afficheEtat(int nbIter)
{
	cout.precision(8);

	cout << "It " << nbIter << " | Sol ";

	if (getIndividuBestValide() != NULL)
		cout << getIndividuBestValide()->coutSol.evaluation << " ";
	else
		cout << "NO-VALID ";

	if (getIndividuBestInvalide() != NULL)
		cout << getIndividuBestInvalide()->coutSol.evaluation;
	else
		cout << "NO-INVALID";

	cout.precision(8);

	cout << " | Moy " << getMoyenneValides() << " " << getMoyenneInvalides()
		 << " | Div " << getDiversity(valides) << " " << getDiversity(invalides)
		 << " | Val " << fractionValidesCharge() << " " << fractionValidesTemps()
		 << " | Pen " << params->penalityCapa << " " << params->penalityLength << " | Pop " << valides->nbIndiv << " " << invalides->nbIndiv << endl;
}
