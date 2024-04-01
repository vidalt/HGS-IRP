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
    Params *params;          // 指向包含问题参数的对象的指针。
    bool rechercheTerminee;  // 一个标志，表示是否完成了本地搜索。
    bool isRepPhase;         // 标志，表示是否处于修复阶段。
    Individu *individu;      // 指向与本地搜索关联的个体的指针。

public:
  // vecteur donnant l'ordre de parcours des sommets pour chaque jour, ne
  // contenant pas les sommets
  // qui n'existent pas pour le jour donn�
  // afin de diversifier la recherche
  // 向量给出了每天遍历顶点的顺序，不包含
  // 不包含顶点
  // 在指定日期不存在的顶点
  // 为了使搜索多样化
  // 一个二维向量，表示每天要遍历的节点的顺序。
  vector<vector<int>> ordreParcours;

  // ajoute un client dans l'ordre de parcours // 方法，将客户添加到指定天的遍历顺序中
  void addOP(int day, int client);

  void removeOP(int day, int client);
  // 从指定天的遍历顺序中移除客户的方法。
  
  void melangeParcours();
  // 重新随机化遍历的顺序，可能是为了增加搜索的多样性。
  
  void updateMoves();
  // 更新将在mutationSameDay方法中尝试的每个节点的移动。
  
  vector<int> ordreJours;
  // 一个向量，表示用于跨日迁移的随机遍历的顺序。链接起来day
  
  bool deplacementIntraJour;
  // 标志，表示是否可以在同一天内移动客户以实现利润。
  
  bool firstLoop;
  // 标志，表示是否是首次遍历。

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

  /* vecteur de taille nbClients , l'element client(day)(i) contient des donnees
  // relatives
  // a l'emplacement de la visite du client i+1 dans les routes
  clients
│
├── Day 1
│   ├── Client 1 -> Noeud Pointer
│   ├── Client 2 -> Noeud Pointer
│   └── ...
│
├── Day 2
│   ├── Client 1 -> Noeud Pointer
│   ├── Client 2 -> Noeud Pointer
│   └── ...
│
└── ...
*/
  vector<vector<Noeud *>> clients;

  // noeuds associ�s aux depots utilis�s
  vector<vector<Noeud *>> depots;

  // noeuds associ�s aux terminaisons des routes (doublon des depots)
  vector<vector<Noeud *>> depotsFin;

  // vecteur repertoriant des donnees sur les routes routes是一个表示路线数据的向量。
  vector<vector<Route *>> routes;

  // demandPerDay[i][j] -> The load to be delivered to each customer [j] on day [i]
  vector<vector<double>> demandPerDay;

  // Straightforward ILS, using a simple shaking operator
  // 运行简单的迭代局部搜索方法，使用简单的振荡操作。
  void runILS(bool isRepPhase, int maxIterations);

  // lance la recherche locale avec changement de jours
  // 运行完整的局部搜索方法，可以更改天数。
  void runSearchTotal(bool isRepPhase);
  void runSearchTotalprint(bool isRepPhase);
  // effectue une parcours complet de toutes les mutations possibles
  // retourne le nombre de mouvements effectu�s
  // 在同一天内执行完整的可能变异。对所有可能的突变进行完整扫描  返回所做移动的次数/
  int mutationSameDay(int day);

  // pour un client, marque que tous les mouvements impliquant ce noeud ont �t�
  // test�s pour chaque route du jour day
  //对于一个客户端，标志着涉及该节点的所有运动都已在一天中的每条路线上进行了测试。
  // 测试了当天的每条路线
  void nodeTestedForEachRoute(int cli, int day);

  // effectue un parcours complet de tous les changement de pattern et swap
  // intra-jours possibles
  // retourne le nombre de mouvements effectu�s
  // 对所有模式的变化和交换进行完整的巡回检查
  // 日内可能
  // 返回移动次数
  int mutationDifferentDay();
  int mutationDifferentDayprint();
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
  int mutation11print(int client);
  // Evaluates the current objective function from the model
  double evaluateCurrentCost(int client);
  double evaluateCurrentCost_stockout (int client);
  double evaluateCurrentCost_p (int client);
  // Evaluates the current objective function of the whole solution
  double evaluateSolutionCost();

  // Prints some useful information on the current solution
  void printInventoryLevels(std::ostream& file,bool add);

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
  void computeCoutInsertionp(Noeud *client);

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
