/*                       Algorithme - HGSADC                         */
/*                    Propri�t� de Thibaut VIDAL                     */
/*                    thibaut.vidal@cirrelt.ca                       */
/*  Utilisation non autoris�e sans permission explicite des auteurs  */

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Genetic.h"
#include "LotSizingSolver.h"
#include "commandline.h"

using namespace std;

#define TRACES

int mainIRP(int argc, char *argv[])
{
  // try
  //{

  commandline c(argc, argv);

  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed; 
    nb_ticks_allowed =  CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed(),c.get_rou(), c.get_stockout());
        
    // initial population 
    Population *population = new Population(mesParametres);
  
    // on cree le solver we create the solver,we create the solver we create the solver
    Genetic solver(mesParametres, population, nb_ticks_allowed, true, true);

    // on lance l'evolution   launch evolution
    
    int max_iter = 100000;
    int maxIterNonProd = 10000;
    solver.evolve(max_iter, maxIterNonProd, 1);

    population->ExportPop(c.get_path_to_solution(),true);
    
    population->ExportBKS(c.get_path_to_BKS());
  
    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest1(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    /*
     solution fs[1]:
     for q = 0 => fs[1] = 0
     for q in (0, 75] => fs[1] = detour (298) + inventory cost
     for q in (75, ?] => fs[1] = detour(298) + inventory cost + exceed capacity
     for q in (?, 144] => fs[1] = detour(698) + inventory cost
     for q > 144 => fs[1] = detour(698) + inventory cost + exceed capacity
     (q-144)*penaltyCap
     */

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(3);
    insertions[0].push_back(Insertion(698, 144, NULL));

    insertions[1].push_back(Insertion(298, 75, NULL));
    insertions[1].push_back(Insertion(698, 144, NULL));

    insertions[2].push_back(Insertion(175, 35, NULL));
    insertions[2].push_back(Insertion(698, 144, NULL));

    vector<double> quantities = vector<double>(3);
    vector<Insertion *> breakpoints = vector<Insertion *>(3);
    double objective;

    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 2);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest2(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(3);
    insertions[0].push_back(Insertion(406, 144, NULL));

    insertions[1].push_back(Insertion(222, 87, NULL));
    insertions[1].push_back(Insertion(406, 144, NULL));

    insertions[2].push_back(Insertion(406, 144, NULL));

    vector<double> quantities = vector<double>(3);
    vector<Insertion *> breakpoints = vector<Insertion *>(3);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 2);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest3(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(3);
    insertions[0].push_back(Insertion(34, 144, NULL));

    insertions[1].push_back(Insertion(21, 39, NULL));
    insertions[1].push_back(Insertion(34, 144, NULL));

    insertions[2].push_back(Insertion(34, 144, NULL));

    vector<double> quantities = vector<double>(3);
    vector<Insertion *> breakpoints = vector<Insertion *>(3);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 3);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}


int mainTest5(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(860, 435, NULL));

    insertions[1].push_back(Insertion(135, 120, NULL));
    insertions[1].push_back(Insertion(860, 435, NULL));

    insertions[2].push_back(Insertion(82, 98, NULL));
    insertions[2].push_back(Insertion(385, 234, NULL));
    insertions[2].push_back(Insertion(860, 435, NULL));

    insertions[3].push_back(Insertion(144, 86, NULL));
    insertions[3].push_back(Insertion(327, 246, NULL));
    insertions[3].push_back(Insertion(860, 435, NULL));

    insertions[4].push_back(Insertion(82, 88, NULL));
    insertions[4].push_back(Insertion(385, 244, NULL));
    insertions[4].push_back(Insertion(860, 435, NULL));

    insertions[5].push_back(Insertion(82, 92, NULL));
    insertions[5].push_back(Insertion(542, 240, NULL));
    insertions[5].push_back(Insertion(860, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;

    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 8);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest6(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(366, 435, NULL));

    insertions[1].push_back(Insertion(41, 197, NULL));
    insertions[1].push_back(Insertion(366, 435, NULL));

    insertions[2].push_back(Insertion(33, 345, NULL));
    insertions[2].push_back(Insertion(366, 435, NULL));

    insertions[3].push_back(Insertion(33, 0, NULL));
    insertions[3].push_back(Insertion(201, 317, NULL));
    insertions[3].push_back(Insertion(366, 435, NULL));

    insertions[4].push_back(Insertion(33, 345, NULL));
    insertions[4].push_back(Insertion(366, 435, NULL));

    insertions[5].push_back(Insertion(33, 128, NULL));
    insertions[5].push_back(Insertion(201, 317, NULL));
    insertions[5].push_back(Insertion(366, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 9);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest7(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(94, 435, NULL));

    insertions[1].push_back(Insertion(24, 6, NULL));
    insertions[1].push_back(Insertion(73, 75, NULL));
    insertions[1].push_back(Insertion(94, 435, NULL));

    insertions[2].push_back(Insertion(60, 0, NULL));
    insertions[2].push_back(Insertion(94, 435, NULL));

    insertions[3].push_back(Insertion(24, 37, NULL));
    insertions[3].push_back(Insertion(64, 81, NULL));
    insertions[3].push_back(Insertion(94, 435, NULL));

    insertions[4].push_back(Insertion(60, 101, NULL));
    insertions[4].push_back(Insertion(94, 435, NULL));

    insertions[5].push_back(Insertion(94, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 4);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest8(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(270, 435, NULL));

    insertions[1].push_back(Insertion(0, 63, NULL));
    insertions[1].push_back(Insertion(236, 325, NULL));
    insertions[1].push_back(Insertion(270, 435, NULL));

    insertions[2].push_back(Insertion(19, 0, NULL));
    insertions[2].push_back(Insertion(49, 227, NULL));
    insertions[2].push_back(Insertion(270, 435, NULL));

    insertions[3].push_back(Insertion(19, 0, NULL));
    insertions[3].push_back(Insertion(270, 435, NULL));

    insertions[4].push_back(Insertion(49, 0, NULL));
    insertions[4].push_back(Insertion(76, 171, NULL));
    insertions[4].push_back(Insertion(270, 435, NULL));

    insertions[5].push_back(Insertion(49, 307, NULL));
    insertions[5].push_back(Insertion(270, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 10);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest9(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(470, 435, NULL));

    insertions[1].push_back(Insertion(0, 0, NULL));
    insertions[1].push_back(Insertion(457, 325, NULL));
    insertions[1].push_back(Insertion(470, 435, NULL));

    insertions[2].push_back(Insertion(24, 0, NULL));
    insertions[2].push_back(Insertion(81, 92, NULL));
    insertions[2].push_back(Insertion(470, 435, NULL));

    insertions[3].push_back(Insertion(24, 0, NULL));
    insertions[3].push_back(Insertion(137, 309, NULL));
    insertions[3].push_back(Insertion(470, 435, NULL));

    insertions[4].push_back(Insertion(24, 27, NULL));
    insertions[4].push_back(Insertion(457, 325, NULL));
    insertions[4].push_back(Insertion(470, 435, NULL));

    insertions[5].push_back(Insertion(54, 279, NULL));
    insertions[5].push_back(Insertion(270, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 2);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest10(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(270, 435, NULL));

    insertions[1].push_back(Insertion(0, 55, NULL));
    insertions[1].push_back(Insertion(236, 325, NULL));
    insertions[1].push_back(Insertion(270, 435, NULL));

    insertions[2].push_back(Insertion(19, 0, NULL));
    insertions[2].push_back(Insertion(49, 227, NULL));
    insertions[2].push_back(Insertion(270, 435, NULL));

    insertions[3].push_back(Insertion(49, 0, NULL));
    insertions[3].push_back(Insertion(270, 435, NULL));

    insertions[4].push_back(Insertion(19, 0, NULL));
    insertions[4].push_back(Insertion(270, 435, NULL));

    insertions[5].push_back(Insertion(85, 51, NULL));
    insertions[5].push_back(Insertion(270, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 10);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest11(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(470, 435, NULL));

    insertions[1].push_back(Insertion(0, 0, NULL));
    insertions[1].push_back(Insertion(457, 325, NULL));
    insertions[1].push_back(Insertion(470, 435, NULL));

    insertions[2].push_back(Insertion(24, 0, NULL));
    insertions[2].push_back(Insertion(81, 92, NULL));
    insertions[2].push_back(Insertion(470, 435, NULL));

    insertions[3].push_back(Insertion(24, 0, NULL));
    insertions[3].push_back(Insertion(470, 435, NULL));

    insertions[4].push_back(Insertion(24, 0, NULL));
    insertions[4].push_back(Insertion(457, 325, NULL));
    insertions[4].push_back(Insertion(470, 435, NULL));

    insertions[5].push_back(Insertion(0, 180, NULL));
    insertions[5].push_back(Insertion(470, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 2);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest12(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(560, 435, NULL));

    insertions[1].push_back(Insertion(104, 0, NULL));
    insertions[1].push_back(Insertion(441, 281, NULL));
    insertions[1].push_back(Insertion(560, 435, NULL));

    insertions[2].push_back(Insertion(117, 0, NULL));
    insertions[2].push_back(Insertion(441, 281, NULL));
    insertions[2].push_back(Insertion(560, 435, NULL));

    insertions[3].push_back(Insertion(183, 0, NULL));
    insertions[3].push_back(Insertion(560, 435, NULL));

    insertions[4].push_back(Insertion(51, 102, NULL));
    insertions[4].push_back(Insertion(560, 435, NULL));

    insertions[5].push_back(Insertion(117, 136, NULL));
    insertions[5].push_back(Insertion(560, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 5);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest13(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(366, 435, NULL));

    insertions[1].push_back(Insertion(33, 77, NULL));
    insertions[1].push_back(Insertion(361, 325, NULL));
    insertions[1].push_back(Insertion(366, 435, NULL));

    insertions[2].push_back(Insertion(33, 0, NULL));
    insertions[2].push_back(Insertion(366, 435, NULL));

    insertions[3].push_back(Insertion(57, 0, NULL));
    insertions[3].push_back(Insertion(201, 246, NULL));
    insertions[3].push_back(Insertion(366, 435, NULL));

    insertions[4].push_back(Insertion(57, 0, NULL));
    insertions[4].push_back(Insertion(366, 435, NULL));

    insertions[5].push_back(Insertion(33, 143, NULL));
    insertions[5].push_back(Insertion(366, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 9);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest14(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(752, 435, NULL));

    insertions[1].push_back(Insertion(222, 179, NULL));
    insertions[1].push_back(Insertion(752, 435, NULL));

    insertions[2].push_back(Insertion(24, 69, NULL));
    insertions[2].push_back(Insertion(331, 128, NULL));
    insertions[2].push_back(Insertion(752, 435, NULL));

    insertions[3].push_back(Insertion(24, 185, NULL));
    insertions[3].push_back(Insertion(712, 380, NULL));
    insertions[3].push_back(Insertion(752, 435, NULL));

    insertions[4].push_back(Insertion(24, 124, NULL));
    insertions[4].push_back(Insertion(752, 435, NULL));

    insertions[5].push_back(Insertion(24, 223, NULL));
    insertions[5].push_back(Insertion(326, 309, NULL));
    insertions[5].push_back(Insertion(752, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 6);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest15(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(72, 383, NULL));

    insertions[1].push_back(Insertion(0, 0, NULL));
    insertions[1].push_back(Insertion(72, 383, NULL));

    insertions[2].push_back(Insertion(0, 176, NULL));
    insertions[2].push_back(Insertion(72, 383, NULL));

    insertions[3].push_back(Insertion(19, 0, NULL));
    insertions[3].push_back(Insertion(72, 383, NULL));

    insertions[4].push_back(Insertion(0, 36, NULL));
    insertions[4].push_back(Insertion(72, 383, NULL));

    insertions[5].push_back(Insertion(72, 383, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 9);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest16(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(366, 435, NULL));

    insertions[1].push_back(Insertion(33, 134, NULL));
    insertions[1].push_back(Insertion(366, 435, NULL));

    insertions[2].push_back(Insertion(33, 39, NULL));
    insertions[2].push_back(Insertion(361, 380, NULL));
    insertions[2].push_back(Insertion(366, 435, NULL));

    insertions[3].push_back(Insertion(33, 219, NULL));
    insertions[3].push_back(Insertion(366, 435, NULL));

    insertions[4].push_back(Insertion(33, 408, NULL));

    insertions[5].push_back(Insertion(33, 408, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 9);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest17(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(3);
    insertions[0].push_back(Insertion(470, 476, NULL));

    insertions[1].push_back(Insertion(24, 14, NULL));
    insertions[1].push_back(Insertion(81, 115, NULL));
    insertions[1].push_back(Insertion(470, 476, NULL));

    insertions[2].push_back(Insertion(24, 0, NULL));
    insertions[2].push_back(Insertion(81, 330, NULL));
    insertions[2].push_back(Insertion(470, 476, NULL));

    vector<double> quantities = vector<double>(3);
    vector<Insertion *> breakpoints = vector<Insertion *>(3);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 2);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int mainTest50(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(386, 2022, NULL));

    insertions[1].push_back(Insertion(2, 0.33333, NULL));
    insertions[1].push_back(Insertion(386, 2022, NULL));

    insertions[2].push_back(Insertion(5, 195, NULL));
    insertions[2].push_back(Insertion(386, 2022, NULL));

    insertions[3].push_back(Insertion(3, 98, NULL));
    insertions[3].push_back(Insertion(386, 2022, NULL));

    insertions[4].push_back(Insertion(65, 856, NULL));
    insertions[4].push_back(Insertion(386, 2022, NULL));

    insertions[5].push_back(Insertion(8, 1122, NULL));
    insertions[5].push_back(Insertion(386, 2022, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 32);
    lotSizingSolver->solve();

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}


int mainTest4(int argc, char *argv[])
{
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // Nombre de ticks horloge que le programme est autorise a durer
    clock_t nb_ticks_allowed;
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution/]
    bool trace = true;
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed(),c.get_rou(), c.get_stockout());

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(470, 435, NULL));

    insertions[1].push_back(Insertion(100, 27, NULL));
    insertions[1].push_back(Insertion(470, 435, NULL));

    insertions[2].push_back(Insertion(240, 33, NULL));
    insertions[2].push_back(Insertion(470, 435, NULL));

    insertions[3].push_back(Insertion(27, 21, NULL));
    insertions[3].push_back(Insertion(81, 92, NULL));
    insertions[3].push_back(Insertion(470, 435, NULL));

    insertions[4].push_back(Insertion(24, 71, NULL));
    insertions[4].push_back(Insertion(129, 248, NULL));
    insertions[4].push_back(Insertion(470, 435, NULL));

    insertions[5].push_back(Insertion(470, 435, NULL));

    vector<double> quantities = vector<double>(6);
    LotSizingSolver *lotSizingSolver =  new LotSizingSolver(mesParametres, insertions, 2);
    cout<<"solve:"<<endl;
    if(mesParametres->isstockout)lotSizingSolver->solve_stockout();
    else
      lotSizingSolver->solve();
    // on desalloue la memoire
    delete mesParametres;
    delete lotSizingSolver;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
}

int main(int argc, char *argv[])
{
  mainIRP(argc, argv);
}
