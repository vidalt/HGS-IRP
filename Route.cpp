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
