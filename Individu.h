/*                       Algorithme - HGSADC                         */
/*                    Propri�t� de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */

#ifndef INDIVIDU_H
#define INDIVIDU_H

/*
Classe individu : chaque individu est represente par son chromT traduisant la sequence de parcours des sommets, sans inserer la depot nulle part. Le calcul du chemin VRP reel et de son fitness se fait avec la fonction split. 

Attention, certains champs ne sont calcules qu'apres une execution de split, ne pas tenter d'y acceder avant.

Des getters auraient pu etre faits afin de restreindre l'acces mais dans un souci d'efficacite algorithmique on se contentera de la conscience du codeur. 
*/

#include <vector>
#include <list>
#include <iostream> 
#include "Noeud.h"
#include "Params.h"
#include "LocalSearch.h"
using namespace std ;

class LocalSearch ;

struct coutSol {
  // valeur du fitness comportant les p�nalit�s, si il a ete calcule
  double evaluation ;

  // valeur du fitness non p�nalis�
  double fitness ;

  // violations de capacit�
  double capacityViol ;

  // violation de longueur 
  double lengthViol ;
};

class Individu ;

struct proxData {
  // individu en question
  Individu * indiv ;

  // sa distance
  double dist ;
};

class Individu
{

 private:

 // Acces aux parametres de l'instance et du genetique
 Params * params ;

 public:

  // fitness etendu
  double fitnessEtendu ;

  // rang diversite
  float divRank ;

  // rang fitness 
  float fitRank ;

  // evaluation de la solution
  struct coutSol coutSol ;

  // The giant tour of each individual 
  //“i”是天数，“j”是“i”天中访问的客户的顺序。例如，chromT[3][2]表示第3天访问的第2个客户。
  // chromT [i][j] -> jour i, client j dans la succession des clients du jour i
  vector < vector<int> > chromT ;

  // chromL [i][j] -> The load to be delivered to each customer [j] on day [i]
  //chromL是一个二维双精度浮点向量。它表示每天为每个客户提供的负载。“i”代表天数，“j”代表客户。
  //例如，chromL[4][5]表示第4天交付给第5个客户的负载。
  vector < vector<double> > chromL ;

  // Keeps the indices of the beginning of the routes, when they have been computed
  // chromT [i][j] -> jour i, route j , donne la position du premier client de cette route
  //chromR是一个二维整数向量，表示当已计算出路线时，路线开始的指标。
  //这里，“i”是天数，“j”是“i”天中的路线。例如，chromR[2][3]表示第2天的第3条路线中第一个客户的位置。
  vector < vector<int> > chromR ;

  // List of patterns used for the visits (relevant only for the PVRP)
  //chromP是一个模式向量，仅对周期性车辆路径问题(PVRP)有关。它可能包含了一个客户的访问模式或计划，如哪些日子会有货物交付。
  vector < pattern > chromP ;
  

  // For each node, the next nodes (several in case the PVRP where deliveries take place on several days)
  // Used for the distance measure
  //suivants 是一个二维整数向量，用于存储每个节点的后续节点。特别是在周期性车辆路径问题（PVRP）中，
  //可能会有多个后续节点，因为货物可能在多个日子进行交付。
  vector < vector<int> > suivants ;
  // The same as previously, but using the predecessors 
  //precedents 与 suivants 类似，但存储的是前驱节点而不是后续节点。
  vector < vector<int> > precedents ;
  // Function to compute suivants and precedents


  // 这是一个函数声明，用于计算 suivants 的值。
  void computeSuivants ();
  // Auxiliary data structure to run the Split
  // potentiels[i+1] -> distance pour atteindre le sommet i
  // de la sequence
  // potentiels[0] = 0 et non   
  // potentiels[1] = distance du sommet 0
  //potentiels 是一个二维浮点数向量。它用于在分割算法中运行，表示到达序列中的某个顶点的距离。
  //具体来说，potentiels[i+1] 表示到达序列中第 i 个顶点的距离。
  //potentiels[0] 被设为 0，而 potentiels[1] 则代表从起始点到序列中的第一个顶点的距离。
  vector < vector <double> > potentiels ;

  // pour chaque jour le tableau de [nbCamions] [predecesseur]
  // potentiels[i+1] -> predecesseur de i
  //pred 是一个三维整数向量。它可能表示对于每一天、每辆卡车，其前一个访问的客户或节点是什么。
  vector < vector < vector<int> > > pred ;

  // says is the fitness has been evaluated
  //这是一个布尔标记，用来说明是否已经计算了解决方案的适应度。适应度是遗传算法评估解决方案优劣的指标。
  //如果一个解决方案的适应度已经被计算过，则在后续的评估中可以避免重复计算，从而节省计算时间。
  bool isFitnessComputed ;

  // says if the individual is a feasible solution
  //这是一个布尔标记，表示该解决方案（或称为个体）是否是一个可行的解。
  //如果某个解决方案满足所有的约束条件和要求，那么它被认为是可行的。
  bool estValide ;

  // Auxiliary data structure used in crossover POX2 (for the PVRP)
  //这是一个整数向量，作为辅助数据结构，用于交叉过程中的POX2方法，这是适用于PVRP（周期性车辆路径问题）的一个交叉策略。
  //这个数据结构可能用来暂存或记录在某个交叉步骤中需要放置或考虑的客户或节点。
  vector < int > toPlace ;

  // distance measure
  double distance(Individu * indiv2);

  // individus class�s par proximit� dans la population, pour les politiques de remplacement
  //在人口中按亲缘关系分类的个体，用于替代政策
  list <proxData> plusProches;

  // ajoute un element proche dans les structures de proximit�
  //为近似结构添加近似元素。
  void addProche(Individu * indiv) ;

  // enleve un element dans les structures de proximit�
  //功能：从近似结构中移除一个元素。
  //这个函数可能是用于从某种近似结构中移除一个给定的个体。  
  void removeProche(Individu * indiv) ;

  // distance moyenne avec les n individus les plus proches
  //功能：计算与最近的n个个体的平均距离。
  //详解：此函数的目的可能是评估该个体与其最近邻的平均距离，这在确定种群的多样性或选择合作伙伴进行交叉时可能很有用。
  double distPlusProche(int n) ;

  // structure de donnee associee a l'individu au sein de la recherche locale
  // seul le rejeton de Genetic.cpp possede cette structure
  // sinon elle n'est pas initialis�e
  //说明：这是与个体相关的局部搜索数据结构。只有Genetic.cpp中的rejeton拥有这个结构。否则，它将不被初始化。
  LocalSearch * localSearch ;

  // fonction Split pour tous les jours
  // essaye d�ja le split simple
  // si la solution ne respecte pas le nombre de camions : essaye le split � flotte limit�e
  //功能：为所有的天数执行分裂功能。首先尝试简单的分裂，如果解不满足车辆数，则尝试有限车队的分裂。
  void generalSplit();

  // fonction split ne respectant pas forc�ment le nombre de v�hicules
  // retourne 1 si succ�s, 0 sinon
  //功能：一个不必满足特定车辆数的分裂功能。成功时返回1，失败时返回0。
  int splitSimple(int k) ;

  // fonction split pour probl�mes � flotte limit�e
  //：对有限车队的问题执行分裂功能。
  void splitLF(int k) ;

  // fonction qui se charge d'�valuer exactement les violations
  // et de remplir tous les champs d'�valuation de solution
  //功能：负责准确测量违规行为，并填充所有解评估字段。
  void measureSol() ;

  // initialisation du vecteur potentiels
  // 功能：初始化potentiels向量。
  void initPot(int day) ;

  // mise a jour de l'objet localSearch, 
  // Attention, Split doit avoir ete calcule avant
  //功能：更新localSearch对象。在此之前必须先计算分裂。
  void updateLS() ;
  int randomizedPartition(std::vector<Route*>& arr, int low, int high);
  void randomizedQuickSort(std::vector<Route*>& arr, int low, int high);
  // Inverse procedure, after local search to return to a giant tour solution representation and thus fill the chromT table. 
  //功能：在局部搜索后，执行逆过程以返回到giant tour解的表示，并因此填充chromT表。
  void updateIndiv() ;

  // Computes the maximum amount of load that can be delivered to client on a day k without exceeding the 
  // customer maximum inventory
  //功能：计算在不超过客户最大库存的情况下，可以在第k天交付给客户的最大负载量。
  double maxFeasibleDeliveryQuantity(int day, int client); 

  // constructor of an individual as a simple container
  //作为简单容器的个体的构造函数。
  Individu(Params * params);

  // constructor of a random individual with all research structures 
  //功能：具有所有研究结构的随机个体的构造函数。
  Individu(Params * params, double facteurSurete );

  //destructeur
  //析构函数。这通常用于释放对象在其生命周期中分配的任何资源。
  ~Individu();
};
#endif
