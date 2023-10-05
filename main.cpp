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
    nb_ticks_allowed = c.get_cpu_time() * CLOCKS_PER_SEC;

    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    cout << "init population" << endl;
    // on cree la population
    Population *population = new Population(mesParametres);

    cout << "init solver" << endl;
    // on cree le solver
    Genetic solver(mesParametres, population, nb_ticks_allowed, true, true);

    // on lance l'evolution
    cout << "######### Phase de descente principale : "
         << mesParametres->nbVehiculesPerDep << endl;
    cout << "  " << endl;
    int max_iter = 100000000;

    cout << "solver running..." << endl;
    solver.evolve(max_iter, 1000, 1);
    cout << "  " << endl;

    population->ExportPop(c.get_path_to_solution());
    population->ExportBKS(c.get_path_to_BKS());

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest1(int argc, char *argv[])
{
  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    // //        plf->testBasicFuncs(50);
    // plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /*
     * insertion 0: detour:698 load: 144
        insertion 1: detour:298 load: 75, detour:698 load: 144
        insertion 2: detour:175 load: 35, detour:698 load: 144
        CPXPARAM_Threads                                 1
        CPXPARAM_MIP_Display                             0
        day: 3 quantity: 35 route: 8 in route: 5
        Cost: 175.7

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 2,
    //        quantities,
    //                                           breakpoints, objective, true);

    // on desalloue la memoire
    delete mesParametres;
    return 0;
  }
  else
    throw string(
        "ligne de commande non parsable, Usage : genvrp instance [-t cpu-time] "
        "[-sol solution]");
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest2(int argc, char *argv[])
{
  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    // //        plf->testBasicFuncs(50);
    // plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        insertion 0: detour:406 load: 144
        insertion 1: detour:222 load: 87
        detour:406 load: 144
        insertion 2: detour:406 load: 144
        CPXPARAM_Threads                                 1
        CPXPARAM_MIP_Display                             0
        myCost 1: -6.72myCost 2: 215.28myCost 3: 215.28client: 4 re-obj: 215.28
        day: 2 quantity: 48 route: 9 in route: 2
        Best cost: 215.28

     */

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(3);
    insertions[0].push_back(Insertion(406, 144, NULL));

    insertions[1].push_back(Insertion(222, 87, NULL));
    insertions[1].push_back(Insertion(406, 144, NULL));

    insertions[2].push_back(Insertion(406, 144, NULL));

    vector<double> quantities = vector<double>(3);
    vector<Insertion *> breakpoints = vector<Insertion *>(3);
    double objective;
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 4,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest3(int argc, char *argv[])
{
  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    // //        plf->testBasicFuncs(50);
    // plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        insertion 0: detour:34 load: 144
        insertion 1: detour:21 load: 39
        detour:34 load: 144
        insertion 2: detour:34 load: 144
        CPXPARAM_Threads                                 1
        CPXPARAM_MIP_Display                             0
        myCost 1: 6.96myCost 2: 40.96myCost 3: 40.96client: 3 re-obj: 40.96
        day: 2 quantity: 116 route: 0 in route: 0
        Best cost: 40.96

     */

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(3);
    insertions[0].push_back(Insertion(34, 144, NULL));

    insertions[1].push_back(Insertion(21, 39, NULL));
    insertions[1].push_back(Insertion(34, 144, NULL));

    insertions[2].push_back(Insertion(34, 144, NULL));

    vector<double> quantities = vector<double>(3);
    vector<Insertion *> breakpoints = vector<Insertion *>(3);
    double objective;
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 3,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest4(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    //        PLFunction *plf = new PLFunction(mesParametres);
    //        plf->testBasicFuncs(50);
    //        plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        insertion 0: detour:470 load: 435

        insertion 1: detour:0 load: 27
        detour:470 load: 435

        insertion 2: detour:24 load: 33
        detour:470 load: 435

        insertion 3: detour:24 load: 21
        detour:81 load: 92
        detour:470 load: 435

        insertion 4: detour:24 load: 71
        detour:129 load: 248
        detour:470 load: 435

        insertion 5: detour:470 load: 435

        CPXPARAM_Threads                                 1
        CPXPARAM_MIP_Display                             0
        myCost 1: 2.7myCost 2: 2.7myCost 3: 2.7client: 2 re-obj: 2.7
        day: 2 quantity: 27 route: 9 in route: 1

        myCost 1: 2.7myCost 2: 2.7myCost 3: 2.7myCost 1: 5.34myCost 2:
       29.34myCost 3: 29.34client: 2 re-obj: 29.34
        day: 3 quantity: 33 route: 8 in route: 1

        myCost 1: 2.7myCost 2: 2.7myCost 3: 2.7myCost 1: 5.34myCost 2:
       29.34myCost 3: 29.34myCost 1: 30.6myCost 2: 54.6myCost 3: 54.6client: 2
       re-obj: 54.6
        day: 4 quantity: 21 route: 8 in route: 0

        myCost 1: 2.7myCost 2: 2.7myCost 3: 2.7myCost 1: 5.34myCost 2:
       29.34myCost 3: 29.34myCost 1: 30.6myCost 2: 54.6myCost 3: 54.6myCost 1:
       56.76myCost 2: 80.76myCost 3: 80.76client: 2 re-obj: 80.76
        day: 5 quantity: 54 route: 8 in route: 1

        Best cost: 80.76

     */

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(470, 435, NULL));

    insertions[1].push_back(Insertion(0, 27, NULL));
    insertions[1].push_back(Insertion(470, 435, NULL));

    insertions[2].push_back(Insertion(24, 33, NULL));
    insertions[2].push_back(Insertion(470, 435, NULL));

    insertions[3].push_back(Insertion(24, 21, NULL));
    insertions[3].push_back(Insertion(81, 92, NULL));
    insertions[3].push_back(Insertion(470, 435, NULL));

    insertions[4].push_back(Insertion(24, 71, NULL));
    insertions[4].push_back(Insertion(129, 248, NULL));
    insertions[4].push_back(Insertion(470, 435, NULL));

    insertions[5].push_back(Insertion(470, 435, NULL));

    vector<double> quantities = vector<double>(6);
    //        vector<Insertion *> breakpoints = vector<Insertion *>(6);
    //        double objective;
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 2,
    //        quantities,
    //                                           breakpoints, objective, true);

    LotSizingSolver *lotSizingSolver =
        new LotSizingSolver(mesParametres, insertions, 2);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest5(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    // //        plf->testBasicFuncs(50);
    // plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        insertion 0: detour:860 load: 435

        insertion 1: detour:135 load: 120
        detour:860 load: 435

        insertion 2: detour:82 load: 98
        detour:385 load: 234
        detour:860 load: 435

        insertion 3: detour:144 load: 86
        detour:327 load: 246
        detour:860 load: 435

        insertion 4: detour:82 load: 88
        detour:385 load: 244
        detour:860 load: 435

        insertion 5: detour:82 load: 92
        detour:542 load: 240
        detour:860 load: 435

        CPXPARAM_Threads                                 1
        CPXPARAM_MIP_Display                             0
        myCost 1: 41.28myCost 2: 123.28myCost 3: 123.28client: 8 re-obj: 123.28
        day: 3 quantity: 86 route: 9 in route: 6

        myCost 1: 41.28myCost 2: 123.28myCost 3: 123.28myCost 1: 143.92myCost 2:
       225.92myCost 3: 225.92client: 8 re-obj: 225.92
        day: 5 quantity: 86 route: 8 in route: 5

        Best cost: 225.92

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 8,
    //        quantities,
    //                                           breakpoints, objective, true);

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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest6(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    // //        plf->testBasicFuncs(50);
    // plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
                insertion 0: [detour: 366 load: 435]
                insertion 1: [detour: 41 load: 197] [detour: 366 load: 435]
                insertion 2: [detour: 33 load: 345] [detour: 366 load: 435]
                insertion 3: [detour: 33 load: 0] [detour: 201 load: 317]
       [detour: 366 load: 435]
                insertion 4: [detour: 33 load: 345] [detour: 366 load: 435]
                insertion 5: [detour: 33 load: 128] [detour: 201 load: 317]
       [detour: 366 load: 435]

                objective: 23.84

     */

    vector<vector<Insertion>> insertions = vector<vector<Insertion>>(6);
    insertions[0].push_back(Insertion(366, 435, NULL));

    insertions[1].push_back(Insertion(41, 197, NULL));
    insertions[1].push_back(Insertion(366, 435, NULL));

    insertions[2].push_back(Insertion(33, 345, NULL));
    insertions[2].push_back(Insertion(366, 435, NULL));
    // insertions[2].push_back(Insertion(860, 435, NULL));

    insertions[3].push_back(Insertion(33, 0, NULL));
    insertions[3].push_back(Insertion(201, 317, NULL));
    insertions[3].push_back(Insertion(366, 435, NULL));

    insertions[4].push_back(Insertion(33, 345, NULL));
    insertions[4].push_back(Insertion(366, 435, NULL));
    // insertions[4].push_back(Insertion(860, 435, NULL));

    insertions[5].push_back(Insertion(33, 128, NULL));
    insertions[5].push_back(Insertion(201, 317, NULL));
    insertions[5].push_back(Insertion(366, 435, NULL));

    vector<double> quantities = vector<double>(6);
    vector<Insertion *> breakpoints = vector<Insertion *>(6);
    double objective;
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 9,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest7(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    // //        plf->testBasicFuncs(50);
    // plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        //insertion 0: [detour: 94 load: 435]
        //insertion 1: [detour: 24 load: 6] [detour: 73 load: 75] [detour: 94
       load: 435]
        //insertion 2: [detour: 60 load: 0] [detour: 94 load: 435]
        //insertion 3: [detour: 24 load: 37] [detour: 64 load: 81] [detour: 94
       load: 435]
        //insertion 4: [detour: 60 load: 101] [detour: 94 load: 435]
        //insertion 5: [detour: 94 load: 435]

        day: 2 quantity: 110
        day: 4 quantity: 81
        day: 5 quantity: 84

        Best cost: 150.73
     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 4,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest8(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    // mesParametres->endTime = 0;
    // for (int i = 0; i < mesParametres->cli.size(); i++)
    //     for (int j = 0; j < mesParametres->cli[i].dailyDemand.size(); j++)
    //         mesParametres->endTime += mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    // //        plf->testBasicFuncs(50);
    // plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        //insertion 0: [detour: 270 load: 435]
        //insertion 1: [detour: 0 load: 63] [detour: 236 load: 325] [detour: 270
       load: 435]
        //insertion 2: [detour: 19 load: 0] [detour: 49 load: 227] [detour: 270
       load: 435]
        //insertion 3: [detour: 19 load: 0] [detour: 270 load: 435]
        //insertion 4: [detour: 49 load: 0] [detour: 76 load: 171] [detour: 270
       load: 435]
        //insertion 5: [detour: 49 load: 307] [detour: 270 load: 435]

        //day: 3 quantity: 189
        //day: 6 quantity: 63

        //Best cost: 204.47
     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 10,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest9(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    //        mesParametres->endTime = 0;
    //        for (int i = 0; i < mesParametres->cli.size(); i++)
    //            for (int j = 0; j < mesParametres->cli[i].dailyDemand.size();
    //            j++)
    //                mesParametres->endTime +=
    //                mesParametres->cli[i].dailyDemand[j];
    //        PLFunction *plf = new PLFunction(mesParametres);
    //        plf->testBasicFuncs(50);
    //        plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
           insertion 0: [detour: 470 load: 435]
          insertion 1: [detour: 0 load: 0] [detour: 457 load: 325] [detour: 470
         load: 435]
          insertion 2: [detour: 24 load: 0] [detour: 81 load: 92] [detour: 470
       load:
         435]
          insertion 3: [detour: 24 load: 0] [detour: 137 load: 309] [detour: 470
         load: 435]
          insertion 4: [detour: 24 load: 27] [detour: 457 load: 325] [detour:
       470
         load: 435]
          insertion 5: [detour: 54 load: 279] [detour: 470 load: 435]


     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 2,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest10(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    //        mesParametres->endTime = 0;
    //        for (int i = 0; i < mesParametres->cli.size(); i++)
    //            for (int j = 0; j < mesParametres->cli[i].dailyDemand.size();
    //            j++)
    //                mesParametres->endTime +=
    //                mesParametres->cli[i].dailyDemand[j];
    // PLFunction *plf = new PLFunction(mesParametres);
    //        plf->testBasicFuncs(50);
    //        plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        insertion 0: [detour: 270 load: 435]
        insertion 1: [detour: 0 load: 55] [detour: 236 load: 325] [detour: 270
       load: 435]
        insertion 2: [detour: 19 load: 0] [detour: 49 load: 227] [detour: 270
       load: 435]
        insertion 3: [detour: 49 load: 0] [detour: 270 load: 435]
        insertion 4: [detour: 19 load: 0] [detour: 270 load: 435]
        insertion 5: [detour: 85 load: 51] [detour: 270 load: 435]

         day: 3 quantity: 189
        day: 6 quantity: 63
        Best cost: 360.47

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 10,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest11(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    //        mesParametres->endTime = 0;
    //        for (int i = 0; i < mesParametres->cli.size(); i++)
    //            for (int j = 0; j < mesParametres->cli[i].dailyDemand.size();
    //            j++)
    //                mesParametres->endTime +=
    //                mesParametres->cli[i].dailyDemand[j];
    //        PLFunction *plf = new PLFunction(mesParametres);
    //        plf->testBasicFuncs(50);
    //        plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        insertion 0: [detour: 470 load: 435]
        insertion 1: [detour: 0 load: 0] [detour: 457 load: 325] [detour: 470
       load: 435]
        insertion 2: [detour: 24 load: 0] [detour: 81 load: 92] [detour: 470
       load: 435]
        insertion 3: [detour: 24 load: 0] [detour: 470 load: 435]
        insertion 4: [detour: 24 load: 0] [detour: 457 load: 325] [detour: 470
       load: 435]
        insertion 5: [detour: 0 load: 180] [detour: 470 load: 435]

        day: 2 quantity: 27
        day: 3 quantity: 54
        day: 5 quantity: 27
        day: 6 quantity: 27
        Best cost: 653.64

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 2,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest12(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    //        mesParametres->endTime = 0;
    //        for (int i = 0; i < mesParametres->cli.size(); i++)
    //            for (int j = 0; j < mesParametres->cli[i].dailyDemand.size();
    //            j++)
    //                mesParametres->endTime +=
    //                mesParametres->cli[i].dailyDemand[j];
    //        PLFunction *plf = new PLFunction(mesParametres);
    //        plf->testBasicFuncs(50);
    //        plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 5,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest13(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38

  // try
  //{
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
    //        mesParametres->endTime = 0;
    //        for (int i = 0; i < mesParametres->cli.size(); i++)
    //            for (int j = 0; j < mesParametres->cli[i].dailyDemand.size();
    //            j++)
    //                mesParametres->endTime +=
    //                mesParametres->cli[i].dailyDemand[j];
    //        PLFunction *plf = new PLFunction(mesParametres);
    //        plf->testBasicFuncs(50);
    //        plf->testSuperposition();
    //        ModelLotSizingPI::testDPLotSizing(mesParametres);

    /* Data
        objective: 512.5
        day 2: quantity: 154 cost: 314.8
        day 4: quantity: 154 cost: 173.28
        day 6: quantity: 143 cost: 24.42
     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 9,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //}
  // catch(const string& e)  // Catching and printing any thrown string
  //{
  // cout << e << endl ;
  // return 0 ;
  //}
}

int mainTest14(int argc, char *argv[])
{
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38 -seed 1000

  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    /* Data

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 6,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38 -seed 1000

  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    /* Data
        day: 2 quantity: 154
        day: 3 quantity: 77
        day: 5 quantity: 154
        Best cost: 60.84

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 9,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs1n10_1.dat -type
  // 38 -seed 1000

  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    /* Data
        day: 2 quantity: 134
        day: 3 quantity: 39
        day: 4 quantity: 135
        day: 5 quantity: 77
        Best cost: 48.9

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 9,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  //    argv[1] = "/home/pta/vrp/IRP/Data/Small/Istanze0105h3/abs1n10_1.dat";
  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    /* Data

     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 2,
    //        quantities,
    //                                           breakpoints, objective, true);
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
  // ./irpcplex /home/pta/vrp/IRP/Data/Small/Istanze0105h6/abs4n50_1.dat -type
  // 38 -seed 1000

  commandline c(argc, argv);
  if (c.is_valid())
  {
    // initialisation des Parametres � partir du fichier d'instance et du chemin
    // de la solution
    Params *mesParametres = new Params(
        c.get_path_to_instance(), c.get_path_to_solution(), c.get_type(),
        c.get_nbVeh(), c.get_path_to_BKS(), c.get_seed());

    /* Data
        insertion 0: [detour: 386 load: 2022]
        insertion 1: [detour: 2 load: 0.333333] [detour: 386 load: 2022]
        insertion 2: [detour: 5 load: 195] [detour: 386 load: 2022]
        insertion 3: [detour: 3 load: 98] [detour: 386 load: 2022]
        insertion 4: [detour: 65 load: 856] [detour: 386 load: 2022]
        insertion 5: [detour: 8 load: 1122] [detour: 386 load: 2022]

        client: 32
     */

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
    //        ModelLotSizingPI::solveDPLotSizing(mesParametres, insertions, 32,
    //        quantities,
    //                                           breakpoints, objective, true);
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

int main(int argc, char *argv[])
{
  //   mainTest4(argc, argv);
  //  mainTest3(argc, argv);
  mainIRP(argc, argv);
}
