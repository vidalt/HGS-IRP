#include "Route.h"
#include "LocalSearch.h"

Route::Route(void){}

Route::Route(int cour, int day, Noeud * depot, double temps, double charge, double maxRouteTime, double vehicleCapacity, Params * params, LocalSearch * myLS) : 
cour(cour), day(day), depot(depot), temps(temps) , charge(charge), maxRouteTime(maxRouteTime), vehicleCapacity(vehicleCapacity), params(params), myLS(myLS)
{
	bestInsertion = vector <Insertion> (params->nbClients + params->nbDepots);

	for (int i=0 ; i < params->nbClients + params->nbDepots ; i++ )
		nodeAndRouteTested.push_back(false);
}

Route::~Route(void){}

void Route::printRouteData(std::ostream& file)
{
	bool firstIt = true ;
	int place = 0 ;
	double charge = 0 ;
	double earlT = 0 ;

	// on parcourt du debut � la fin
	Noeud * noeud = depot ;
	noeud->place = place ;
	depot->chargeAvant = 0 ;
	depot->est = 0 ;

	while ( !noeud->estUnDepot || firstIt )
	{
		firstIt = false ;
		file <<" node[ "<<noeud->cour <<" ] ->";
		noeud = noeud->suiv ;
		place ++ ;
		noeud->place = place ;
		charge += myLS->demandPerDay[day][noeud->cour];
		earlT += params->cli[noeud->pred->cour].serviceDuration + params->timeCost[noeud->pred->cour][noeud->cour] ;
		noeud->chargeAvant = charge ;
		noeud->est = earlT ;
	}
	file <<"depot"<<endl;
	noeud->route->temps = earlT ;
	noeud->route->charge = charge ;

}

/*
用于确保循环至少执行一次的布尔标志。
place：表示经过的节点数。
charge：累计的需求或载荷。
earlT：从起点到当前节点的总时间。
遍历路线：这个循环遍历整个路线，从起点到终点。

noeud 是当前正在处理的节点。
place 递增，表示经过了一个新的节点。
charge 累加，表示累积的需求。
earlT 累加，表示从起点到当前节点的总时间。
对于每个节点，它还更新了在该节点之前的累计需求 chargeAvant 和累计时间 est。
更新路线数据：在循环结束后，它更新路线的总时间 temps 和总需求 charge。

可行性检查：函数接下来检查这条路线是否可行，即它的总载荷是否低于车辆的容量 vehicleCapacity，以及总时间是否低于最大路线时间 maxRouteTime。如果都满足，路线被标记为可行，否则标记为不可行。

初始化插入：最后，函数调用了 initiateInsertions 方法（该方法的实现细节在提供的代码段中不可见，但可以推测它的目的是重置或准备路线的某些插入数据）。

总的来说，这个函数是用来更新路线的主要属性，并检查路线的可行性。如果路线的任何属性发生变化，如添加或删除客户，此函数可能会被调用以确保所有数据都是最新的。
*/
void Route::updateRouteData ()
{
	bool firstIt = true ;
	int place = 0 ;
	double charge = 0 ;
	double earlT = 0 ;

	// on parcourt du debut � la fin
	Noeud * noeud = depot ;
	noeud->place = place ;
	depot->chargeAvant = 0 ;
	depot->est = 0 ;

	while ( !noeud->estUnDepot || firstIt )
	{
		firstIt = false ;
		noeud = noeud->suiv ;
		place ++ ;
		noeud->place = place ;
		charge += myLS->demandPerDay[day][noeud->cour];
		earlT += params->cli[noeud->pred->cour].serviceDuration + params->timeCost[noeud->pred->cour][noeud->cour] ;
		noeud->chargeAvant = charge ;
		noeud->est = earlT ;
	}

	noeud->route->temps = earlT ;
	noeud->route->charge = charge ;

	if (charge < vehicleCapacity + 0.0001 && earlT < maxRouteTime + 0.0001) isFeasible = true ;
	else isFeasible = false;

	initiateInsertions();
}
void Route::evalInsertClientp (Noeud * U) 
{
	Noeud * courNoeud ;
	double cout;
	bestInsertion[U->cour].detour = 1.e30 ;
	bestInsertion[U->cour].place = NULL ;
	bestInsertion[U->cour].load = -1.e30 ;
	
	bool firstIt = true ;
	if ( U->route != this || !U->estPresent )
	{
		bestInsertion[U->cour].load = std::max<double>(0.,vehicleCapacity - charge);
		courNoeud = depot ;
		while (!courNoeud->estUnDepot || firstIt == true )
		{
			firstIt = false ;
			cout = params->timeCost[courNoeud->cour][U->cour] 
			+ params->timeCost[U->cour][courNoeud->suiv->cour] 
			- params->timeCost[courNoeud->cour][courNoeud->suiv->cour] ;

			if ( cout < bestInsertion[U->cour].detour - 0.0001 )
			{ 
				bestInsertion[U->cour].detour = cout ;
				bestInsertion[U->cour].place = courNoeud ;
			}
			courNoeud = courNoeud->suiv ;
		}
	}
	else // U is already  in our route
	{
		bestInsertion[U->cour].load = std::max<double>(0.,vehicleCapacity + myLS->demandPerDay[day][U->cour] - charge);
		bestInsertion[U->cour].detour = params->timeCost[U->pred->cour][U->cour] - params->timeCost[U->pred->cour][U->suiv->cour]   
										+ params->timeCost[U->cour][U->suiv->cour] ;
		bestInsertion[U->cour].place = U->pred ;
		std::cout << "else detour "<<bestInsertion[U->cour].detour<< " pre "<<U->pred->cour <<" next "<<U->suiv->cour<<endl;

		// however, we'll see if there's a better insertion possible
		// temporarily we'll remove the node from the chain�e list (in O(1))
		U->pred->suiv = U->suiv ;
		U->suiv->pred = U->pred ;
		courNoeud = depot ;

		// et parcourir la route � nouveau
		while (!courNoeud->estUnDepot || firstIt == true )
		{
			firstIt = false ;
			cout = params->timeCost[courNoeud->cour][U->cour] 
			+ params->timeCost[U->cour][courNoeud->suiv->cour]
			- params->timeCost[courNoeud->cour][courNoeud->suiv->cour] ;

			// au final on peut placer d'une meilleure mani�re
			if (cout < bestInsertion[U->cour].detour - 0.0001)
			{ 
				bestInsertion[U->cour].detour = cout ;
				bestInsertion[U->cour].place = courNoeud ;
				std::cout << "better "<<bestInsertion[U->cour].detour;
			}
			courNoeud = courNoeud->suiv ;
		}
		
		// on replace le noeud
		U->pred->suiv = U ;
		U->suiv->pred = U ;
	}
}


// pour un client donn�, trouve la meilleure position d'insertion et sa quantit�
/*
给定的客户（表示为节点 Noeud * U）找到在当前路线中的最佳插入位置以及对应的货物数量。该函数并不真的进行插入操作，只是评估并找出最佳的插入位置和相关的信息。
考虑客户 U 不在当前路线的情况：
在这种情况下，该函数尝试找出将客户 U 插入当前路线的最佳位置。为此，它会遍历整个路线，计算每一个可能的插入点的成本，并将其与当前已知的最佳插入成本进行比较。如果找到了一个更好的插入点，它就更新最佳插入信息。

考虑客户 U 已经在当前路线的情况：
首先，函数计算客户 U 当前位置的成本，并将其设为最佳插入成本。
接着，它暂时从链表中移除客户 U，并再次遍历整个路线，寻找一个更好的插入位置。
在遍历结束后，客户 U 被重新插入到链表中。
*/
void Route::evalInsertClient (Noeud * U) 
{
	
	Noeud * courNoeud ;
	double cout;
	bestInsertion[U->cour].detour = 1.e30 ;
	bestInsertion[U->cour].place = NULL ;
	bestInsertion[U->cour].load = -1.e30 ;
	
	bool firstIt = true ;
	if ( U->route != this || !U->estPresent )
	{
		bestInsertion[U->cour].load = std::max<double>(0.,vehicleCapacity - charge);
		courNoeud = depot ;
		while (!courNoeud->estUnDepot || firstIt == true )
		{
			firstIt = false ;
			cout = params->timeCost[courNoeud->cour][U->cour] 
			+ params->timeCost[U->cour][courNoeud->suiv->cour] 
			- params->timeCost[courNoeud->cour][courNoeud->suiv->cour] ;
			
			
			if ( cout < bestInsertion[U->cour].detour - 0.0001 )
			{ 
				bestInsertion[U->cour].detour = cout ;
				bestInsertion[U->cour].place = courNoeud ;
			}
			courNoeud = courNoeud->suiv ;
		}
	}
	else // U is already  in our route
	{
		bestInsertion[U->cour].load = std::max<double>(0.,vehicleCapacity + myLS->demandPerDay[day][U->cour] - charge);
		bestInsertion[U->cour].detour = params->timeCost[U->pred->cour][U->cour] - params->timeCost[U->pred->cour][U->suiv->cour]   
										+ params->timeCost[U->cour][U->suiv->cour] ;
		bestInsertion[U->cour].place = U->pred ;

		// however, we'll see if there's a better insertion possible
		// temporarily we'll remove the node from the chain�e list (in O(1))
		U->pred->suiv = U->suiv ;
		U->suiv->pred = U->pred ;
		courNoeud = depot ;

		// et parcourir la route � nouveau
		while (!courNoeud->estUnDepot || firstIt == true )
		{
			firstIt = false ;
			cout = params->timeCost[courNoeud->cour][U->cour] 
			+ params->timeCost[U->cour][courNoeud->suiv->cour]
			- params->timeCost[courNoeud->cour][courNoeud->suiv->cour] ;

			// au final on peut placer d'une meilleure mani�re
			if (cout < bestInsertion[U->cour].detour - 0.0001)
			{ 
				bestInsertion[U->cour].detour = cout ;
				bestInsertion[U->cour].place = courNoeud ;
			}
			courNoeud = courNoeud->suiv ;
		}
		
		// on replace le noeud
		U->pred->suiv = U ;
		U->suiv->pred = U ;
	}
}

// no insertion is calculated
void Route::initiateInsertions()
{
	for (int i=0 ; i < params->nbClients + params->nbDepots ; i ++ )
	{
		bestInsertion[i].detour = 1.e30 ;
		bestInsertion[i].load = -1.e30 ;
		bestInsertion[i].place = NULL ;
	}
}

// moves having nodes in this route need to be examined again
void Route::reinitSingleDayMoves()
{
	for (int i=0 ; i < params->nbClients + params->nbDepots ; i ++ )
		nodeAndRouteTested[i] = false ;
}


void Route::updateCentroidCoord ()
{
	double Xvalue = 0 ;
	double Yvalue = 0 ;
	double nbNodes = 0 ;
	Noeud * courNoeud  = depot->suiv ;

	while (!courNoeud->estUnDepot)
	{
		Xvalue += params->cli[courNoeud->cour].coord.x ;
		Yvalue += params->cli[courNoeud->cour].coord.y ;
		nbNodes ++ ;
		courNoeud = courNoeud->suiv ;
	}

	centroidX = Xvalue/nbNodes ;
	centroidY = Yvalue/nbNodes ;

	centroidAngle = atan2( centroidY - params->cli[depot->cour].coord.y, centroidX - params->cli[depot->cour].coord.x );
}
