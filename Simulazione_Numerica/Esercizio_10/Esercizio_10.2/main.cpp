#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include "/home/davide/Simulazione_Numerica/Utilities/random.h"
#include "functions.h"
#include "mpi.h"

using namespace std;

int main ( int argc, char** argv ) {

	if ( argc < 1 ) {
    cout << "Uso del programma : " << argv[0] << endl;
    return -1;
   }

   // Leggo le posizioni dei capoluoghi di provincia italiani
   vector<vector<double>> positions;
   Cities_Reader("cap_prov_ita.dat", positions);

   const int n_paths = 100; // Numero di cammini per ogni generazione
   const int path_size = 110; // Numero di città per ogni cammino
   const int best_to_change = 10; // Numero di cammini scambiati tra i vari continenti
   const int exchange_time = 100; // Generazioni tra uno scambio e l'altro
   const int changes = 100; // Scambi totali

   Random rnd_base;
   Random_Initializer(rnd_base);

   // Inizializzo i vari core coi loro nomi
   int size, rank;
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   Random rnd;
   Random_Initializer(rnd, rank);

   // Inizializzo un contatore per le generazioni e i vettori di cammini per le generazioni pari e dispari
   int generation = 0;
   vector<vector<int>> paths_even(n_paths, vector<int>(path_size, 0));
   vector<vector<int>> paths_odd(n_paths, vector<int>(path_size, 0));

   // Riempio il vettore per la generazione 0 e controllo tutti i 100 cammini
   Generation_Creator(rnd, paths_even, generation);
   for (int i=0;i<n_paths;i++) {
      Check_Operator(paths_even[i], generation);
   }

   for (int i=0;i<changes;i++) {
      // Faccio evolvere il mio sistema per n generazioni
      Generation_Evolver(rnd, paths_even, paths_odd, positions, generation, exchange_time);
      // Scambio migliori path tra i continenti
      Best_Paths_Exchange(rnd, paths_even, rank, size);
      for (int j=0;j<best_to_change;j++) {
         Check_Operator(paths_even[j], generation);
      }
      cout << i << endl;
   }

   // Ordino l'ultima generazione
   Sorting_Generation(paths_odd, positions);

   if (rank == 0) {
      // Stampo il cammino in un file di output
      Best_Path_Printer("best_path_100.out", paths_odd);
   }

   rnd.SaveSeed();

   MPI_Finalize();

   return 0;
}