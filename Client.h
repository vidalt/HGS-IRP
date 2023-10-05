/*                       Algorithme - HGSADC                         */
/*                    Propriété de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */

#ifndef CLIENT_H
#define CLIENT_H

#include <iostream> 
#include <vector>
#include <math.h>
using namespace std ;

struct couple {
	double x;
	double y;
};

struct pattern {
	int pat;
	int dep;
};

class Client
{
public:

	// nombre de jours
    int nbJours ;

	// customer number
    int custNum ;

    // coordonnées des points
    couple coord ;

	// delai à un sommet
	double serviceDuration ;

	// demande associée à un sommet
	double demand ;

	// DATA STRUCTURES USED FOR THE PVRP //

	// frequence de visite
	int freq ;

	// variable globale à la classe utilisée dans la fonction de remplissage
	int codeTravail ;

	// list of all possible visit combinations (PVRP)
	// Each visit combination is coded with the decimal equivalent of
    // the corresponding binary bit string. For example, in a 5-day
    // period, the code 10 which is equivalent to the bit string 01010
    // means that a customer is visited on days 2 and 4. (Days are
    // numbered from left to right.)
    vector <pattern> visits ;

	// list of all possible visit combinations copiées pour qui n'est pas
	// modifiée lors des set_pattern etc...
	vector <pattern> visitsOrigin ;

	// vecteur de programmation dynamique, dimensionné à 2^(OldnbJours+1) (PVRP)
	// visitsDyn[code] donne l'indice du pattern en question dans la liste de pattern du problème
	vector <int> visitsDyn ;

	// calcul de visitsDyn (PVRP)
	void computeVisitsDyn () ;

	// programmation dynamique, calculé lors de l'initialisation des structures
	// pour les crossPOX (PVRP)
	// jourSuiv[code][jour] dit à quel jour suivant doit etre placé le client qui est
	// déja placé conformément au code actuel
	// **** n'est utilisé que dans les crossover
	vector < vector < int > > jourSuiv ;

	// liste des jours ou l'on peut rajouter l'individu pour un code donné (PVRP)
	// **** n'est utilisé que dans les crossover
	vector < vector < int > > jourAccept ;

	// calcul de JourSuiv et de jourAccept (PVRP)
	void computeJourSuiv () ;

	// fonction recursive impliquee dans le calcul de computeJourSuiv (PVRP)
	void frec (int y, int z,int n) ;

	// fonction impliquée dans le calcul de computeJourSuiv
	// ajoute la représentation binaire de z à la ligne y
	// du tableau JourSuiv (PVRP)
	void ajoute (int y,int z) ;

	// DATA STRUCTURES USED FOR THE IRP //
	
	// starting inventory level
	double startingInventory ;

	// bounds for the inventory
	double minInventory ;

	// bounds for the inventory
	double maxInventory ;

	// daily demand of the customer.
	vector <double> dailyDemand ;

	// daily inventory cost of the customer
	double inventoryCost ;

	// ordre des sommets et depots, par proximité
	vector <int> ordreProximite ;

	// les sommets les plus proches selon le critere de proximite
	vector <int> sommetsVoisins ;

	Client(void);

	~Client(void);
};

#endif
