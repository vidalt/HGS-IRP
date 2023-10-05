/*                       Algorithme - HGSADC                         */
/*                    Propriété de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */

#ifndef NOEUD_H
#define NOEUD_H

#include "Route.h"
#include<iostream>
using namespace std;

class Route ;

struct Move {
	
int destination ;

double cout ;

double type ;

};

class Noeud
{

public :

// est un depot ou un client
bool estUnDepot ;

// indice du depot ou du client represente
int cour ;

// place dans la route
int place ;

// indice du jour en question
int jour ;

// presence de le ce client à ce jour ci
bool estPresent ;

// depot ou client suivant dans la route
Noeud * suiv ;

// depot ou client precedent dans la route
Noeud * pred ;

// route associee
Route * route ;

// charge de la portion de route située avant lui (lui compris)
double chargeAvant ;

// start time of service
double est ;

//  List of possible insertions in different routes
vector <Insertion> allInsertions ;

// Removing dominated insertions
void removeDominatedInsertions (double penalityCapa);

// cout insertion dans ce jour si il devait être inséré
double coutInsertion ;

// noeud où serait inséré
Noeud * placeInsertion ;

// recherche global, dit si on est au premier parcours ou non
bool firstLoop ;

// mouvements possibles
vector < int > moves ;

// constructeur 1
Noeud(void);
	
// constructeur 2
Noeud(bool estUnDepot, int cour, int jour, bool estPresent, Noeud * suiv , Noeud * pred, Route * route, double est);

// destructeur
~Noeud(void);
};

#endif
