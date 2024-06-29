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

   const int n_paths = 100; // Numero di cammini per ogni generazione
   const int path_size = 34; // Numero di città per ogni cammino
   const int best_to_change = 5; // Numero di cammini scambiati tra i vari continenti
   const int exchange_time = 12; // Generazioni tra uno scambio e l'altro
   const int changes = 12; // Scambi totali

   Random rnd_base;
   Random_Initializer(rnd_base);

   // Genero le città sulla circonferenza unitaria e ne stampo le coordinate in un file di output
   vector<vector<double>> positions_circ = Generating_Cities_Circumference(rnd_base);
   Cities_Printer("cities_positions_circ.out", positions_circ);

   // Genero le città in un quadrato unitario e ne stampo le coordinate in un file di output
   vector<vector<double>> positions_square = Generating_Cities_Square(rnd_base);
   Cities_Printer("cities_positions_square.out", positions_square);

   // Inizializzo i vari core coi loro nomi
   int size, rank;
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   Random rnd;
   Random_Initializer(rnd, rank);

   // Inizializzo un contatore per le generazioni e i vettori di cammini per le generazioni pari e dispari
   int generation_circ = 0;
   vector<vector<int>> paths_even_circ(n_paths, vector<int>(path_size, 0));
   vector<vector<int>> paths_odd_circ(n_paths, vector<int>(path_size, 0));
   int generation_square = 0;
   vector<vector<int>> paths_even_square(n_paths, vector<int>(path_size, 0));
   vector<vector<int>> paths_odd_square(n_paths, vector<int>(path_size, 0));

   // Riempio il vettore per la generazione 0 e controllo tutti i 100 cammini
   Generation_Creator(rnd, paths_even_circ, generation_circ);
   Generation_Creator(rnd, paths_even_square, generation_square);

   for (int i=0;i<changes;i++) {
      // Faccio evolvere il mio sistema per n generazioni
      Generation_Evolver(rnd, paths_even_circ, paths_odd_circ, positions_circ, generation_circ, exchange_time);
      Generation_Evolver(rnd, paths_even_square, paths_odd_square, positions_square, generation_square, exchange_time);
      // Scambio migliori path tra i continenti
      Best_Paths_Exchange(rnd, paths_even_circ, rank, size);
      Best_Paths_Exchange(rnd, paths_even_square, rank, size);
      for (int j=0;j<best_to_change;j++) {
         Check_Operator(paths_even_circ[j], generation_circ);
         Check_Operator(paths_even_square[j], generation_square);
      }
   }

   // Ordino l'ultima generazione
   Sorting_Generation(paths_odd_circ, positions_circ);
   Sorting_Generation(paths_odd_square, positions_square);

   if (rank == 0) {
      // Stampo il cammino in un file di output
      Best_Path_Printer("best_path_circ.out", paths_odd_circ);
      Best_Path_Printer("best_path_square.out", paths_odd_square);
   }

   rnd.SaveSeed();

   MPI_Finalize();

   return 0;
}