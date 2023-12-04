#include "commandline.h"

void commandline::set_instance_name(string to_parse)
{
  instance_name = to_parse;
}

void commandline::set_sortie_name(string to_parse) { sortie_name = to_parse; }

void commandline::set_BKS_name(string to_parse) { BKS_name = to_parse; }

void commandline::set_default_sorties_name(string to_parse)
{
  char caractere1 = '/';
  char caractere2 = '\\';
  int position = (int)to_parse.find_last_of(caractere1);
  int position2 = (int)to_parse.find_last_of(caractere2);
  if (position2 > position)
    position = position2;

  if (position != -1)
  {
    sortie_name = to_parse.substr(0, position + 1) + "sol-" +
                  to_parse.substr(position + 1, to_parse.length() - 1);
    BKS_name = to_parse.substr(0, position + 1) + "bks-" +
               to_parse.substr(position + 1, to_parse.length() - 1);
  }
  else
  {
    sortie_name = "sol-" + to_parse;
    BKS_name = "bks-" + to_parse;
  }
}

// constructeur
commandline::commandline(int argc, char *argv[])
{
  bool isTime = false;
  bool isOutput = false;
  bool isBKS = false;

  cout << argc << endl;
  cout << string(argv[0]) << endl;

  if (argc % 2 != 0 || argc > 24 || argc < 2)
  {
    cout << "incorrect command line" << endl;
    command_ok = false;
  }
  else
  {
    // default values
    set_instance_name(string(argv[1]));
    set_default_sorties_name(string(argv[1]));
    cpu_time = 1200;
    seed = 0;
    type = 0;   // unknown
    nbCli = -1; // unknown
    nbVeh = -1; // unknown
    relax = -1; // unknown

    // afficher le lancement du programme :
    // for ( int i = 0 ; i < argc ; i ++ )
    // cout << string(argv[i]) << " " ;
    // cout << endl ;

    // parameters
    for (int i = 2; i < argc; i += 2)
    {
      if (string(argv[i]) == "-t")
        cpu_time = atoi(argv[i + 1]);
      else if (string(argv[i]) == "-sol")
        set_sortie_name(string(argv[i + 1]));
      else if (string(argv[i]) == "-bks")
        set_BKS_name(string(argv[i + 1]));
      else if (string(argv[i]) == "-seed")
        seed = atoi(argv[i + 1]);
      else if (string(argv[i]) == "-type")
        type = atoi(argv[i + 1]);
      else if (string(argv[i]) == "-relax")
        relax = atoi(argv[i + 1]);
      else if (string(argv[i]) == "-cli")
        nbCli = atoi(argv[i + 1]);
      else if (string(argv[i]) == "-veh")
        nbVeh = atoi(argv[i + 1]);
      else
      {
        cout << "Commande non reconnue : " << string(argv[i]) << endl;
        command_ok = false;
      }
    }
    command_ok = true;
  }
}

void commandline::set_debug_prams(string instance)
{
  bool isTime = false;
  bool isOutput = false;
  bool isBKS = false;

  set_instance_name(instance);
  set_default_sorties_name(instance);

  cpu_time = 1200;
  seed = 1000;
  type = 38;  // unknown
  nbCli = -1; // unknown
  nbVeh = 2;  // unknown
  relax = -1; // unknown

  command_ok = true;
}

// destructeur
commandline::~commandline() {}

// renvoie le chemin de l'instance

string commandline::get_path_to_instance() { return instance_name; }

string commandline::get_path_to_solution() { return sortie_name + "." + to_string(seed); }

string commandline::get_path_to_BKS() { return BKS_name; }

int commandline::get_type() { return type; }

// renvoie le nombre de clients
int commandline::get_nbCli() { return nbCli; }

// renvoie le nombre de v�hicules optimal connu
int commandline::get_nbVeh() { return nbVeh; }

// renvoie le temps cpu allou
int commandline::get_cpu_time() { return cpu_time; }

// renvoie la seed
int commandline::get_seed() { return seed; }

// dit si la ligne de commande est valide
bool commandline::is_valid() { return command_ok; }