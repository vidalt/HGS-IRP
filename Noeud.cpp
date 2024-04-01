#include "Noeud.h"

Noeud::Noeud(void){}
/*
estUnDepot : 布尔值，表示这个节点是否是一个仓库。
cour : 整型，表示节点的索引或编号。
jour : 整型，表示相关的天数或时间段。
estPresent : 布尔值，表示在给定的天数是否有这个节点。
suiv 和 pred : 它们是指向Noeud类型的指针，分别表示在路线中该节点的后续和前驱节点。
route : 指向Route类型的指针，表示与这个节点关联的路线。
est : 双精度值，表示开始服务的时间。
*/
Noeud::Noeud(bool estUnDepot, int cour,int jour, bool estPresent, Noeud * suiv , Noeud * pred, Route * route, double est) 
: estUnDepot(estUnDepot),cour(cour),jour(jour), estPresent(estPresent),suiv(suiv), pred(pred), route(route), est(est)
{
coutInsertion = 1.e30 ; //表示这个节点的插入代价是很高的或者这个值是一个“无穷大”值，
placeInsertion = NULL ; //将placeInsertion指针初始化为NULL，表示没有指定插入位置。
place = -1 ; //节点在路线中的位置尚未定义
}

bool mySort (Insertion i, Insertion j) 
{ 
	if (i.detour < j.detour) return true ;
	else if (i.detour > j.detour) return false ;
	else return (i.load > j.load) ;
}
//经过函数后，allInsertions将只包含那些非支配的插入点，这将有助于提高后续处理的效率和效果。
void Noeud::removeDominatedInsertions (double penalityCapa)
{
	// First order the elements by increasing detour
	std::sort (allInsertions.begin(), allInsertions.end(), mySort);

	// Then make a new structure that keeps the non-dominated elements.
	vector <Insertion> newVector ;
	newVector.push_back(allInsertions[0]);
	Insertion courInsertion = allInsertions[0];

	for (int i=1 ; i < (int)allInsertions.size() ; i++)
	{
		if (allInsertions[i].load > courInsertion.load + 0.0001 && 
			courInsertion.detour + penalityCapa * (allInsertions[i].load - courInsertion.load) > allInsertions[i].detour + 0.0001 )
		{
			newVector.push_back(allInsertions[i]);
			courInsertion = allInsertions[i];
		}
	}

	// and replace the old structure by the new one
	allInsertions = newVector;
}

Noeud::~Noeud(void){}
