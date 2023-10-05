/*                       Algorithme - HGSADC                         */
/*                    Propriété de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */

#ifndef PARAMS_H
#define PARAMS_H

#include <math.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "Client.h"
#include "Rng.h"
#include "Vehicle.h"
using namespace std;

const double MAXCOST = 1.e30;

class Vehicle;
class Noeud;

// needed structure for a few places in the code (easily accessible from here)
struct Insertion {
  double detour;
  double load;
  Noeud* place;

  Insertion() {
    detour = 1.e30;
    load = -1.e30;
    place = NULL;
  }

  Insertion(double detour, double load, Noeud* place)
      : detour(detour), load(load), place(place) {}
};

class Params {
 public:
  // générateur pseudo-aleatoire
  Rng* rng;

  // graine du générateur
  int seed;

  // adresse de l'instance
  string pathToInstance;

  // adresse de la solution
  string pathToSolution;

  // adresse de la BKS
  string pathToBKS;

  // flag indiquant si instance MDPVRP doit etre traitée en tant que PVRP
  bool conversionToPVRP;

  // flag indiquant si l'on doit trier les routes dans l'ordre des centroides
  bool triCentroides;

  // entier indiquant le nombre max d'alternances RI/PI
  int maxLSPhases;

  // debut de l'algo
  clock_t debut;

  // PARAMETRES DE L'ALGORITHME GENETIQUE //

  // constante d'espacement entre les fitness des individus
  double delta;

  // limite du split
  double borneSplit;

  // critère de proximité des individus (RI)
  int prox;

  // critère de proximité des individus (RI -- constante)
  int proxCst;

  // critère de proximité des individus (PI -- constante)
  int prox2Cst;

  // nombre d'individus pris en compte dans la mesure de distance
  int nbCountDistMeasure;

  // distance min
  double distMin;

  // nombre d'individus elite
  int el;

  // nombre d'individus dans la population
  int mu;

  // nombre d'offspring dans une generation
  int lambda;

  // probabilité de recherche locale totale pour la reparation (PVRP)
  double pRep;

  // coefficient de penalite associe a une violation de capacite
  double penalityCapa;

  // coefficient de penalite associe a une violation de longueur
  double penalityLength;

  // limite basse sur les indiv valides
  double minValides;

  // limite haute sur les indiv valides
  double maxValides;

  // fraction de la population conservée lors de la diversification
  double rho;

  // PARAMETRES DE L'INSTANCE //

  // type du probleme
  /*		   0 (VRP) - Christofides format
         1 (PVRP)
         2 (MDVRP)
         3 (SDVRP)
                     38 (IRP)
                                          */
  int type;

  // rounding convention
  bool isRoundingInteger;
  bool isRoundingTwoDigits;

  // Constant value in the objective
  double objectiveConstant;
  void computeConstant();

  // présence d'un problème MultiDepot ;
  bool multiDepot;

  // présence d'un problème Périodique ;
  bool periodique;

  // présence d'un problème IRP ;
  bool isInventoryRouting;

  // nombre de sommets clients
  int nbClients;

  // nombre de jours
  int nbDays;

  // ancien nombre de jours
  int ancienNbDays;

  // nombre de vehicules par dépot
  int nbVehiculesPerDep;

  // nombre de depots (MDVRP)
  // correspond à l'indice du premier client dans le tableau C
  int nbDepots;

  // pour chaque jour, somme des capacites des vehicules
  vector<double> dayCapacity;

  // sequence des véhicules utilisables chaque jour avec les contraintes et
  // dépots associés
  vector<vector<Vehicle> > ordreVehicules;

  // nombre de véhicules utilisables par jour
  vector<int> nombreVehicules;

  // vecteur des depots et clients
  vector<Client> cli;

  // temps de trajet , calculés lors du parsing
  vector<vector<double> > timeCost;

  // critère de corrélation
  vector<vector<bool> > isCorrelated1;

  // critère de corrélation
  vector<vector<bool> > isCorrelated2;

  // SPECIFIC DATA FOR THE INVENTORY ROUTING PROBLEM //

  // availableSupply[t] gives the new additional supply at day t.
  // availableSupply[1] gives the initial supply + production for day 1
  vector<double> availableSupply;

  // inventory cost per day at the supplier
  double inventoryCostSupplier;

  // TRANSFORMATIONS D'INSTANCES //

  // table de correspondance : le client i dans le nouveau pb correspond à
  // correspondanceTable[i] dans l'ancien
  vector<int> correspondanceTable;

  // table de correspondance : le client i dans le nouveau pb correspond aux
  // elements de correspondanceTable[i] (dans l'ordre) dans l'ancien
  // utile lorsque des décompositions de problème avec shrinking sont
  // envisagées.
  vector<vector<int> > correspondanceTableExtended;

  // table de correspondance : le client i dans l'ancien pb correspond à
  // correspondanceTable2[i] dans le nouveau
  vector<int> correspondanceTable2;

  // ROUTINES DE PARSING //

  // flux d'entree du parser
  ifstream fichier;

  // initializes the parameters of the method
  void setMethodParams();

  // effectue le prelevement des donnees du fichier
  void preleveDonnees(string nomInstance);

  // sous routine du prelevement de donnees
  Client getClient(int i);

  // computes the distance matrix
  void computeDistancierFromCoords();

  // calcule les autres structures du programme
  void calculeStructures();

  // modifie aléatoirement les tableaux de proximité des clients
  void shuffleProches();

  // constructeur de Params qui remplit les structures en les prélevant dans le
  // fichier
  Params(string nomInstance, string nomSolution, int type, int nbVeh,
         string nomBKS, int seedRNG);

  // Transformation de problème, le nouveau fichier params créé correspond à un
  // sous-problème:
  // et est prêt à être résolu indépendamment
  // si decom = 2 -> depots fixés, extraction du PVRP associé au dépot (MDPVRP
  // -> PVRP et MDVRP -> VRP).
  // si decom = 1 -> patterns fixés, extraction du VRP associé au jour "jour",
  // (PVRP->VRP), (SDVRP->VRP)
  // si decom = 0 -> on extrait un problème de VRP, qui contient l'ensemble de
  // clients debutSeq ... finSeq (debut et finseq sont des valeurs et non des
  // indices). (VRP->VRP)
  Params(Params* params, int decom, int* serieVisites, Vehicle** serieVehicles,
         int* affectDepots, int* affectPatterns, int depot, int jour,
         int nbVisites, int nbVeh);

  void decomposeRoutes(Params* params, int* serieVisites,
                       Vehicle** serieVehicles, int nbVisites, int nbVeh);

  // createur des parametres à partir du fichier d'instance et d'un type et
  // autres paramètres donnés d'office
  Params(string nomInstance, string nomSolution, string nomBKS, int seedRNG,
         int type, string regul, int nbVeh, int nbCli, int relax);

  // destructeur de Params
  ~Params(void);
};
#endif
