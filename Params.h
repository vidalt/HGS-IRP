/*                       Algorithme - HGSADC                         */
/*                    Propri�t� de Thibaut VIDAL                     */
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
struct Insertion
{
       double detour;
       //detour: 表示插入操作所带来的路径偏离度或额外的路程。例如，如果我们在一条路线中添加一个新的客户，
       //那么可能需要多走一段路，detour就表示了这段额外的路程。默认值为一个很大的数字1.e30，
       double load;
       //the remain load of this route

       Noeud *place;

       Insertion()
       {
              detour = 1.e30;
              load = -1.e30;
              place = NULL;
       }
       Insertion(double detour, double load, Noeud *place)
           : detour(detour), load(load), place(place) {}
       void print()
       {
              cout << "(detour: " << detour << " possible_load:" << load << ") ";
              cout << endl;
       }
};

class Params {
 public:
  // g�n�rateur pseudo-aleatoire
  Rng* rng;

  // graine du g�n�rateur
  int seed;

  // adresse de l'instance
  string pathToInstance;

  // adresse de la solution
  string pathToSolution;

  // adresse de la BKS
  string pathToBKS;

  // flag indiquant si instance MDPVRP doit etre trait�e en tant que PVRP
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

  // crit�re de proximit� des individus (RI)
  int prox;

  // crit�re de proximit� des individus (RI -- constante)
  int proxCst;

  // crit�re de proximit� des individus (PI -- constante)
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

  // probabilit� de recherche locale totale pour la reparation (PVRP)
  double pRep;

  // coefficient de penalite associe a une violation de capacite
  double penalityCapa;

  // coefficient de penalite associe a une violation de longueur
  double penalityLength;

  // limite basse sur les indiv valides
  double minValides;

  // limite haute sur les indiv valides
  double maxValides;

  // fraction de la population conserv�e lors de la diversification
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
  double objectiveConstant_stockout;
  void computeConstant();
  void computeConstant_stockout();
  // pr�sence d'un probl�me MultiDepot ;
  //表示是否存在多仓库问题 (Multi-Depot)。
  bool multiDepot;

  // pr�sence d'un probl�me P�riodique ;
  //一个布尔值，表示问题是否是周期性的。
  bool periodique;

  // pr�sence d'un probl�me IRP ;
  bool isInventoryRouting;
  bool isstockout;

  // nombre de sommets clients
  int nbClients;

  // nombre de jours
  int nbDays;

  // ancien nombre de jours表示过去的天数，
  int ancienNbDays;

  // nombre de vehicules par d�pot
  int nbVehiculesPerDep;

  // nombre de depots (MDVRP)
  // correspond � l'indice du premier client dans le tableau C
  int nbDepots;

  // pour chaque jour, somme des capacites des vehicules
  vector<double> dayCapacity;

  // sequence des v�hicules utilisables chaque jour avec les contraintes et
  // d�pots associ�s
  //它存储了每天可用的车辆序列以及与它们关联的约束和仓库。从名称来看，这可能用于规划每天应该使用哪些车辆。
  vector<vector<Vehicle> > ordreVehicules;

  // nombre de v�hicules utilisables par jour
  //这是一个整数向量，存储了每天可用的车辆数量。每天的数量可能会有所不同
  vector<int> nombreVehicules;

  // vecteur des depots et clients 客户，depot向量
  //vector<Client> cli;

       //这是一个向量，存储了客户和仓库的信息。从上下文来看，Client 类可能同时包含客户和仓库的信息。
  vector<Client> cli;

  // temps de trajet , calcul�s lors du parsing
  //这是一个二维向量，存储了各点之间的行驶时间。这通常用于路径规划，以确定最短或最快的路径。
  vector<vector<double> > timeCost;

  // crit�re de corr�lation用于确定两点之间是否有某种特定的关系或相似性。
  vector<vector<bool> > isCorrelated1;

  // crit�re de corr�lation，代表了另一种判定标准。
  vector<vector<bool> > isCorrelated2;

  // SPECIFIC DATA FOR THE INVENTORY ROUTING PROBLEM //

  // availableSupply[t] gives the new additional supply at day t.
  // availableSupply[1] gives the initial supply + production for day 1
  vector<double> availableSupply;

  // inventory cost per day at the supplier
  double inventoryCostSupplier;

  // TRANSFORMATIONS D'INSTANCES //

  // table de correspondance : le client i dans le nouveau pb correspond �
  // correspondanceTable[i] dans l'ancien
  vector<int> correspondanceTable;

  // table de correspondance : le client i dans le nouveau pb correspond aux
  // elements de correspondanceTable[i] (dans l'ordre) dans l'ancien
  // utile lorsque des d�compositions de probl�me avec shrinking sont
  // envisag�es.
  vector<vector<int> > correspondanceTableExtended;

  // table de correspondance : le client i dans l'ancien pb correspond �
  // correspondanceTable2[i] dans le nouveau
  vector<int> correspondanceTable2;

  // ROUTINES DE PARSING //

  // flux d'entree du parser 解析器输入流
  ifstream fichier;

  // initializes the parameters of the method
  void setMethodParams();

  // effectue le prelevement des donnees du fichier
  void preleveDonnees(string nomInstance,int rou, bool stockout);

  // sous routine du prelevement de donnees
  Client getClient(int i,int rou);

  // computes the distance matrix
  void computeDistancierFromCoords();

  // calcule les autres structures du programme
  void calculeStructures();

  // modifie al�atoirement les tableaux de proximit� des clients
  void shuffleProches();

  // constructeur de Params qui remplit les structures en les pr�levant dans le
  // fichier
  Params(string nomInstance, string nomSolution, int type, int nbVeh,
         string nomBKS, int seedRNG);
  Params(string nomInstance, string nomSolution, int type, int nbVeh,
         string nomBKS, int seedRNG, int rou, bool stockout);

  // Transformation de probl�me, le nouveau fichier params cr�� correspond � un
  // sous-probl�me:
  // et est pr�t � �tre r�solu ind�pendamment
  // si decom = 2 -> depots fix�s, extraction du PVRP associ� au d�pot (MDPVRP
  // -> PVRP et MDVRP -> VRP).
  // si decom = 1 -> patterns fix�s, extraction du VRP associ� au jour "jour",
  // (PVRP->VRP), (SDVRP->VRP)
  // si decom = 0 -> on extrait un probl�me de VRP, qui contient l'ensemble de
  // clients debutSeq ... finSeq (debut et finseq sont des valeurs et non des
  // indices). (VRP->VRP)
  Params(Params* params, int decom, int* serieVisites, Vehicle** serieVehicles,
         int* affectDepots, int* affectPatterns, int depot, int jour,
         int nbVisites, int nbVeh);
  

  void decomposeRoutes(Params* params, int* serieVisites,
                       Vehicle** serieVehicles, int nbVisites, int nbVeh);

  // createur des parametres � partir du fichier d'instance et d'un type et
  // autres param�tres donn�s d'office
  Params(string nomInstance, string nomSolution, string nomBKS, int seedRNG,
         int type, string regul, int nbVeh, int nbCli, int relax);
 

  // destructeur de Params
  ~Params(void);
};
#endif
