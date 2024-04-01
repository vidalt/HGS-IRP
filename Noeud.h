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
// est un depot ou un client 布尔值，表示这个节点是否是一个仓库。 

int cour ;
// indice du depot ou du client represente 节点的索引或编号

int place ;
// place dans la route 表示节点在路线中的位置。


int jour ;
// indice du jour en question 表示相关的天数或时间段。

bool estPresent ;
// presence de le ce client � ce jour ci 布尔值，表示在给定的天数是否有这个节点。


Noeud * suiv ;
// depot ou client suivant dans la route 它们是指向Noeud类型的指针，分别表示在路线中该节点的后续和前驱节点。


Noeud * pred ;


Route * route ;
// depot ou client precedent dans la route  指向Route类型的指针，表示与这个节点关联的路线。
// route associee / 相关路径 

double chargeAvant ;
// charge de la portion de route situ�e avant lui (lui compris)  双精度值，表示在这个节点之前的路线部分的负载。

double est ;
// start time of service  双精度值，表示开始服务的时间。

vector <Insertion> allInsertions ;
//  List of possible insertions in different routes 一个Insertion类型的向量，列出了在不同路线中可能的插入。

void removeDominatedInsertions (double penalityCapa);
// Removing dominated insertions

double coutInsertion ;
// cout insertion dans ce jour si il devait �tre ins�r� 如果要在这一天插入，成本就会增加。

Noeud * placeInsertion ;
// noeud o� serait ins�r�
bool firstLoop ;
// recherche global, dit si on est au premier parcours ou non 全局搜索，告诉你是否在第一条路线上


vector < int > moves ;
// mouvements possibles可能的动向

Noeud(void);
// constructeur 1	

Noeud(bool estUnDepot, int cour, int jour, bool estPresent, Noeud * suiv , Noeud * pred, Route * route, double est);
// constructeur 2

~Noeud(void);
// destructeur
};
/*
estUnDepot: 布尔值，表示这个节点是否是一个仓库。

cour: 节点的索引或编号。

place: 表示节点在路线中的位置。

jour: 表示相关的天数或时间段。

estPresent: 布尔值，表示在给定的天数是否有这个节点。

suiv 和 pred: 它们是指向Noeud类型的指针，分别表示在路线中该节点的后续和前驱节点。

route: 指向Route类型的指针，表示与这个节点关联的路线。

chargeAvant: 双精度值，表示在这个节点之前的路线部分的负载。

est: 双精度值，表示开始服务的时间。

allInsertions: 一个Insertion类型的向量，列出了在不同路线中可能的插入。

coutInsertion: 如果要在这一天插入，成本就会增加。

placeInsertion: 节点插入的位置。

firstLoop: 布尔值，告诉你是否在第一条路线上。

moves: 整数向量，代表可能的动作或变化。

此外，该类还提供了一些方法：

removeDominatedInsertions: 用于移除被支配的插入，可能与某种优化技术有关。

构造函数和析构函数，用于创建和销毁Noeud对象。

这个类似乎是为了解决某种物流或车辆路径问题而创建的，其中包括了对客户的服务、货物的交付和路径的选择等方面的内容。




*/
#endif
