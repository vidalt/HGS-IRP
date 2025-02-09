/*                       Algorithme - HGSADC                         */
/*                    Propri�t� de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */

#ifndef GENETIC_H
#define GENETIC_H

// definition de la structure d'un algorithme genetique
// un algorithme genetique contient ses parametres (Params)
// ainsi que l'adresse de la population sur laquelle il travaille

#include "Population.h"
#include "Params.h"
#include "Individu.h"
#include "time.h"
#include <stdlib.h>
#include <stdio.h> 
#include <vector>
#include <list>
#include <math.h>
#include <random>
using namespace std ;

class Genetic
{

private:

	//on stocke le temps restant
    clock_t ticks ;

	// Number of non-improving iterations before stopping
	int nbIterNonProd ;

	// Number of iterations before stopping
	int nbIter ;

	// Pointer towards the parameters of the problem
	Params * params ;

	// traces d'execution ou non
	bool traces ;

	// exportation r�guli�re des solutions ou non
	bool writeProgress ;

	// Population
	Population * population ;

	// First individual to be used as input for the crossover
	Individu * rejeton ; 

	// Second individual to be used as input for the crossover
	Individu * rejeton2 ;

	// effectue la mutation
	void muter ();
	void muter1 ();
	// eventuellement effectue une reparation de la solution
	void reparer ();

	// procedure de crossover OX
	// retourne -1 si l'individu ainsi cr�� n'est pas valide
	int crossOX ();

	// procedure de crossover POX 2
	// retourne -1 si l'individu ainsi cr�� n'est pas valide
	int crossPOX2 ();

	// tableau utilis� lors des crossovers
	vector < int > freqClient ;

	// positions pour le crossover
	int debut,fin,debutDay1,finDay1,debutPos1,finPos1,debutDay2,finDay2,debutPos2,finPos2 ;

	// procede de redemarrage avec remplacement d'une partie de la population
	void diversify ();

	// gestion des penalites
	void gererPenalites ();

	// Part of UHGS 2014.
	// S�paration du VRP en groupes de routes, r�solution s�par�e de sous-probl�mes
	// pour l'instant, approche basique en prenant les routes dans l'ordre des p�tales.
	// on regroupe des routes jusque avoir plus de 100 clients par sous-probl�me.
	// on se sert de la population pour avoir la population initiale
	void improve_decompRoutes (int maxIter , int maxIterNonProd, Individu * indiv, int grainSize, int decal, Population * pop, int nbRec);


public:

    // lancement de l'algorithme sur les parametres "params" et la population "population"
	// la boucle s'arrete lorsque l'on a effectu� maxIterProd it�rations productives
	// ou maxIterNonProd it�rations non productives 
    void evolve (int maxIter , int maxIterNonProd, int nbRec) ;

	// constructeur du solveur genetique
	Genetic(Params * params,Population * population, clock_t ticks, bool traces, bool writeProgress);

	// destructeur
	~Genetic(void);
};

#endif
