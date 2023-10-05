/*                       Algorithme - HGSADC                         */
/*                    Propri�t� de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */

#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

// structure de donnees adaptee a la recherche locale
// une structure de ce type associee a chaque individu de la population

// nouvelle representation des structures

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include "Individu.h"
#include "LotSizingSolver.h"
// #include "Model_LotSizingPI.h"
#include "Noeud.h"
#include "Route.h"

using namespace std;

class Individu;

struct paireJours
{
  int j1;
  int j2;
};

class LocalSearch
{
private:
  // acces aux donnees de l'instance
  Params *params;

  // test pour sortir de la LS
  bool rechercheTerminee;

  // inique si l'on est ans une phase de reparation
  bool isRepPhase;

  // acces � son individu associ�
  Individu *individu;

public:
  // vecteur donnant l'ordre de parcours des sommets pour chaque jour, ne
  // contenant pas les sommets
  // qui n'existent pas pour le jour donn�
  // afin de diversifier la recherche
  vector<vector<int>> ordreParcours;

  // ajoute un client dans l'ordre de parcours
  void addOP(int day, int client);

  // enleve un client de l'ordre de parcours
  void removeOP(int day, int client);

  // les tableaux ordreParcours et ordreJours sont r�organis�s al�atoirement
  void melangeParcours();

  // updates the moves for each node which will be tried in mutationSameDay
  void updateMoves();

  // vecteur donnant l'ordre al�atoire de parcours des jours pour les migrations
  // entre-jours
  vector<int> ordreJours;

  // flag lev� si il est possible de d�placer un client dans son jour avec un
  // b�n�fice
  bool deplacementIntraJour;

  // dit si on est au premier parcours ou non
  bool firstLoop;

  Noeud *noeudU;
  Noeud *noeudUPred;
  Noeud *x;
  Noeud *noeudXSuiv;
  Noeud *noeudV;
  Noeud *noeudVPred;
  Noeud *y;
  Noeud *noeudYSuiv;
  Route *routeU;
  Route *routeV;
  int noeudUCour, noeudUPredCour, xCour, xSuivCour, ySuivCour, noeudVCour,
      noeudVPredCour, yCour;
  int dayCour;

  // vecteur de taille nbClients , l'element client(day)(i) contient des donnees
  // relatives
  // a l'emplacement de la visite du client i+1 dans les routes
  vector<vector<Noeud *>> clients;

  // noeuds associ�s aux depots utilis�s
  vector<vector<Noeud *>> depots;

  // noeuds associ�s aux terminaisons des routes (doublon des depots)
  vector<vector<Noeud *>> depotsFin;

  // vecteur repertoriant des donnees sur les routes
  vector<vector<Route *>> routes;

  // demandPerDay[i][j] -> The load to be delivered to each customer [j] on day
  // [i]
  vector<vector<double>> demandPerDay;

  // Straightforward ILS, using a simple shaking operator
  void runILS(bool isRepPhase, int maxIterations);

  // lance la recherche locale avec changement de jours
  void runSearchTotal(bool isRepPhase);

  // effectue une parcours complet de toutes les mutations possibles
  // retourne le nombre de mouvements effectu�s
  int mutationSameDay(int day);

  // pour un client, marque que tous les mouvements impliquant ce noeud ont �t�
  // test�s pour chaque route du jour day
  void nodeTestedForEachRoute(int cli, int day);

  // effectue un parcours complet de tous les changement de pattern et swap
  // intra-jours possibles
  // retourne le nombre de mouvements effectu�s
  int mutationDifferentDay();

  // Neighborhoods

  /* RELOCATE */

  // If posUreal is a client node, remove posUreal then insert it after posVreal
  int mutation1();

  // If posUreal and x are clients, remove them then insert (posUreal,x) after
  // posVreal
  int mutation2();

  // If posUreal and x are clients, remove them then insert (x,posUreal) after
  // posVreal
  int mutation3();

  /* SWAP */

  // If posUreal and posVreal are clients, swap posUreal and posVreal
  int mutation4();

  // If posUreal, x and posVreal are clients, swap (posUreal,x) and posVreal
  int mutation5();

  // If (posUreal,x) and (posVreal,y) are cliens, swap (posUreal,x) and
  // (posVreal,y)
  int mutation6();

  /* 2-OPT and 2-OPT* */

  // If T(posUreal) = T(posVreal) , replace (posUreal,x) and (posVreal,y) by
  // (posUreal,posVreal) and (x,y)
  int mutation7();

  // If T(posUreal) != T(posVreal) , replace (posUreal,x) and (posVreal,y) by
  // (posUreal,posVreal) and (x,y)
  int mutation8();

  // If T(posUreal) != T(posVreal) , replace (posUreal,x) and (posVreal,y) by
  // (posUreal,y) and (posVreal,x)
  int mutation9();

  /* PI procedure for IRP */
  int mutation11(int client);

  // Evaluates the current objective function from the model
  double evaluateCurrentCost(int client);

  // Evaluates the current objective function of the whole solution
  double evaluateSolutionCost();

  // Prints some useful information on the current solution
  void printInventoryLevels();

  /* Routines to update the solution */

  // effectue l'insertion du client U apres V
  void insertNoeud(Noeud *U, Noeud *V);

  // effectue le swap du client U avec V
  void swapNoeud(Noeud *U, Noeud *V);

  // supprime le noeud
  void removeNoeud(Noeud *U);

  // ajoute un noeud � l'endroit indique dans Noeud->placeRoute
  void addNoeud(Noeud *U);

  // calcule pour un jour donn� et un client donn� (repr�sent� par un noeud)
  // les couts d'insertion dans les differentes routes constituant ce jour
  void computeCoutInsertion(Noeud *client);

  // performs a basic shaking for the problem
  // only for testing
  void shaking();

  LocalSearch();

  // constructeur, cree les structures de noeuds
  // n'initialise pas pas la pile ni les routes
  LocalSearch(Params *params, Individu *individu);

  ~LocalSearch(void);
};

#endif
