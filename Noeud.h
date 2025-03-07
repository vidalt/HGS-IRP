/*                       Algorithme - HGSADC                         */
/*                    Propri�t� de Thibaut VIDAL                     */
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

  
bool estUnDepot ;
// est un depot ou un client

int cour ;
// indice du depot ou du client represente

int place ;
// place dans la route


int jour ;
// indice du jour en question

bool estPresent ;
// presence de le ce client � ce jour ci


Noeud * suiv ;
// depot ou client suivant dans la route


Noeud * pred ;


Route * route ;
// depot ou client precedent dans la route
// route associee 

double chargeAvant ;
// charge de la portion de route situ�e avant lui (lui compris)

double est ;
// start time of service

vector <Insertion> allInsertions ;
//  List of possible insertions in different routes

void removeDominatedInsertions (double penalityCapa);
// Removing dominated insertions

double coutInsertion ;
// cout insertion dans ce jour si il devait �tre ins�r�
Noeud * placeInsertion ;
// noeud o� serait ins�r�
bool firstLoop ;
// recherche global, dit si on est au premier parcours ou non


vector < int > moves ;
// mouvements possibles
Noeud(void);
// constructeur 1	

Noeud(bool estUnDepot, int cour, int jour, bool estPresent, Noeud * suiv , Noeud * pred, Route * route, double est);
// constructeur 2

~Noeud(void);
// destructeur
};

#endif
