
#include "Client.h"

void Client::computeVisitsDyn () 
{
	visitsDyn.clear();

	// initialisation de la structure visitsDyn
	for (int i=0 ; i <= (int)pow((float)2,(int)nbJours) ; i++)
		visitsDyn.push_back(-1);

	// remplissage des champs valides
	for (int i=0 ; i < (int)visits.size() ; i++)
		visitsDyn[visits[i].pat] = i ;
}

void Client::computeJourSuiv () 
{
	int day,day2,j ;
	vector < int > temp ;
	vector < int > temp2 ;

	jourSuiv.clear();
	jourAccept.clear();

	// initialisation de la structure computeJourSuiv
	temp.push_back(-1);
	for (int i=0 ; i < nbJours ; i++)
	{temp.push_back(0);}

	for (int i=0 ; i <= (int)pow((float)2,(int)nbJours) ; i++)
	{
	jourSuiv.push_back(temp);
	jourAccept.push_back(temp2);
	}

	// remplissage de la structure avec des 1 et des 0 
	// donnant computeJourSuiv[codeT][jourT] : peut on rajouter la tache assimilée 
	// au code codeT sur le jour jourT
	for (int i=0 ; i < (int)visits.size() ; i++)
	{
		codeTravail = visits[i].pat ;
		frec ((int)codeTravail,(int)0,(int)pow((float)2,(int)nbJours)) ;
	}

	// on remplace les 0 et les 1 par la donnée du prochain jour où l'on peut
	// ajouter la tache

	for (int k=0 ; k <= (int)pow((float)2,(int)nbJours) ; k++)
	{
		// une petite marque dans le champ 0 qui était vide a été laissée
		// si cette valeur vaut -1 il n'y a rien à voir.
		if (jourSuiv[k][0] != -1)
		{

			// on parcourt de droite à gauche pour trouver le premier element non nul
			day = 0 ;
			j = (int)nbJours ;
			while ( day == 0 )
			{
				if (jourSuiv[k][j] == 1) { 
				day = j ;
				jourAccept[k].push_back(day);
				jourSuiv[k][j] = day ;
				}
				j-- ;
			}

			day2 = day ;
			// day2 pointe sur le dernier jour acceptable
			while ( j>= 1 )
			{
				if (jourSuiv[k][j] == 1) 
				{ 
				day = j ;
				jourAccept[k].push_back(day);
				}
				jourSuiv[k][j] = day ;
				j-- ;
			}

			// on repasse sur les dernieres cases en mettant le premier jour acceptable
			for ( j = day2 + 1 ; j <= (int)nbJours ; j++ )
			{
				jourSuiv[k][j] = day ;
			}
		}
	}
}

void Client::frec (int y, int z,int n) 
{
	if (n > y) 
	{ frec (y,z,n/2) ;}
	else 
	{
		if (y!=0)
		{
			frec(y-n,z,n/2);
			frec(y-n,z+n,n/2);
		}
		else
		{
			ajoute(z,(int)codeTravail-z);
		}
	}
}

void Client::ajoute (int y, int z)
{
	int temp = z ;
	if (temp !=0)
	{
		jourSuiv[y][0] = 0 ;

		for (int i=0 ; i < nbJours ; i++)
		{
			if (jourSuiv[y][nbJours-i] == 0) { jourSuiv[y][nbJours-i] = temp%2 ; }
			temp = temp/2 ;
		}
	}
}
Client::Client(void){}

Client::~Client(void){}
