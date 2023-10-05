#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

// Cette classe parse la ligne de commande entre par le user
class commandline
{
private:
    // dit si la ligne de commande est valide
    bool command_ok;

    // temps alloue en calcul
    int cpu_time;

    // seed
    int seed;

    // instance type, if given
    int type;

    // relaxation type, if given
    int relax;

    // nbClients, if given
    int nbCli;

    // nbVehicles, if given
    int nbVeh;

    // nom de l'instance
    string instance_name;

    // nom de la sortie
    string sortie_name;

    // nom de la BKS
    string BKS_name;

    // remplit l'attribut instance_name
    void set_instance_name(string to_parse);

    // remplit l'attribut sortie_name
    void set_sortie_name(string to_parse);

    // remplit l'attribut BKS_name
    void set_BKS_name(string to_parse);

    // donne une valeur par default a la solution
    // en fonction du nom de l'instance
    void set_default_sorties_name(string to_parse);

public:
    // constructeur
    commandline(int argc, char *argv[]);

    // destructeur
    ~commandline();

    void set_debug_prams(string instance);

    // renvoie le chemin de l'instance
    string get_path_to_instance();

    // renvoie le chemin vers la solution
    string get_path_to_solution();

    // renvoie le chemin vers la meilleure solution connue
    string get_path_to_BKS();

    // renvoie le temps cpu allou
    int get_cpu_time();

    // renvoie le type de l'instance
    int get_type();

    // renvoie le nombre de clients
    int get_nbCli();

    // renvoie le nombre de v�hicules optimal connu
    int get_nbVeh();

    // renvoie la seed
    int get_seed();

    // dit si la ligne de commande est valide
    bool is_valid();
};
#endif
