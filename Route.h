/*                       Algorithme - HGSADC                         */
/*                    Propri�t� de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */

#ifndef ROUTE_H
#define ROUTE_H

#include "Params.h"
#include "Noeud.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std ;

class Noeud ;
class LocalSearch ;

class Route
{

private:

// acces aux donnees de l'instance
Params * params ;

// access  to other features
LocalSearch * myLS ;

public:

// numero de la route路线的编号。
int cour ;

// day associated to the route 与路线相关联的天数
int day ;

// depot associe a la route
Noeud * depot ;

// distance total de parcours sur la route路线的总行驶时间。
double temps ;

// chargement total sur la route路线的总载量
double charge ;

// duree maximum de la route
double maxRouteTime ;

// chargement maximum de la route车辆的最大载量
double vehicleCapacity ;

// valide ou non
bool isFeasible ;

inline double excedentCharge(double charge)
{
	return std::max<double>(0,charge-vehicleCapacity);
}

inline double excedentLength(double length)
{
	return std::max<double>(0,length-maxRouteTime);
}

// coordonn�es du centroide de la route
// ainsi que l'angle pris par rapport au segment (0,0) (0,1)
// utilis� pour refaire le giant tour apres la LS
// pas d'utilit� lors de la recherche locale 路线的中心坐标及其与参考线的角度。
double centroidX ;
double centroidY ;
double centroidAngle ;

// calcule les coordonn�es du centroide计算路线的中心坐标
void updateCentroidCoord ();

// met � jour les charges partielles de la route associ�e au noeud U
//更新与节点 U 相关联的路线的数据。
void updateRouteData () ;
void printRouteData (std::ostream& file) ;
// pour chaque noeud, stocke le cout de l'insertion dans la route
//为每个节点存储在路线中的插入成本
vector < Insertion > bestInsertion ;

// pour chaque noeud, booleen indiquant si tous les mouvements impliquant ce noeud
// et cette route ont �t� test�s sans succ�s表示与该路线相关的节点是否已经被测试过。
vector <bool> nodeAndRouteTested ;

// pour un client donn�, trouve la meilleure position d'insertion
// �ventuellement fait le calcul pour tous les clients d'un coup, ou pour plusieurs
//对给定的节点 U，找到最佳的插入位置。这可能会为所有客户计算一次或多次。
void evalInsertClient (Noeud * U) ;
void evalInsertClientp (Noeud * U) ;
// no insertion is calculated
void initiateInsertions();

// moves having nodes in this route need to be examined again: 需要重新检查此路线中的移动s
void reinitSingleDayMoves();

Route(void);

Route(int cour, int day, Noeud * depot, double temps, double charge, double maxRouteTime, double vehicleCapacity, Params * params, LocalSearch * myLS);

~Route(void);
};

#endif
