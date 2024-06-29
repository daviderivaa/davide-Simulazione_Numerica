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

using namespace std;

int main ( int argc, char** argv ) {

	if ( argc < 1 ) {
    cout << "Uso del programma : " << argv[0] << endl;
    return -1 ;
   }

   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("/home/davide/Simulazione_Numerica/Utilities/Primes");
   if (Primes.is_open()){
      Primes >> p1 >> p2 ;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   ifstream input("/home/davide/Simulazione_Numerica/Utilities/seed.in");
   string property;
   if (input.is_open()){
      while ( !input.eof() ){
         input >> property;
         if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed,p1,p2);
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;

   // Genero le 34 città sulla circonferenza unitaria e ne stampo le coordinate in un file di output
   vector<vector<double>> positions_circ = Generating_Cities_Circumference(rnd);

   ofstream output_cities_circ("cities_positions_circ.out");

   for (int i=0;i<34;i++) {
      output_cities_circ << positions_circ[i][0] << " " << positions_circ[i][1] << endl;
   }

   output_cities_circ.close();

   // Inizializzo un contatore per le generazioni e i vettori di cammini per le generazioni pari e dispari
   int generation_circ = 0;
   vector<vector<int>> paths_even_circ(100, vector<int>(34, 0));
   vector<vector<int>> paths_odd_circ(100, vector<int>(34, 0));

   // Riempio il vettore per la generazione 0 e controllo tutti i 100 cammini
   for (int i=0;i<100;i++) {
      paths_even_circ[i] = Generating_Path(rnd);
      Check_Operator(paths_even_circ[i], generation_circ);
   }

   ofstream output_metric_circ("metric_circ.out");
   Sorting_Generation(paths_even_circ, positions_circ);
   double sum = 0.;
   for (int j=0;j<50;j++) {
      sum += Metric(paths_even_circ[j], positions_circ)/50;
   }
   output_metric_circ << generation_circ << " " << Metric(paths_even_circ[0], positions_circ) << " " << sum << endl;

   // Faccio evolvere il mio sistema per 200 generazioni
   for (int i=0;i<200;i++) {
      if (generation_circ%2 == 0) {
         Odd_Generation_Generator(rnd, paths_even_circ, paths_odd_circ, positions_circ, generation_circ);
         Sorting_Generation(paths_odd_circ, positions_circ);
         sum = 0.;
         for (int j=0;j<50;j++) {
            sum += Metric(paths_odd_circ[j], positions_circ)/50;
         }
         output_metric_circ << generation_circ << " " << Metric(paths_odd_circ[0], positions_circ) << " " << sum << endl;
      } else {
         Even_Generation_Generator(rnd, paths_even_circ, paths_odd_circ, positions_circ, generation_circ);
         Sorting_Generation(paths_even_circ, positions_circ);
         double sum = 0.;
         for (int j=0;j<50;j++) {
            sum += Metric(paths_even_circ[j], positions_circ)/50;
         }
         output_metric_circ << generation_circ << " " << Metric(paths_even_circ[0], positions_circ) << " " << sum << endl;
      }
      generation_circ++;
   }

   output_metric_circ.close();

   // Ordino l'ultima generazione e ne stampo il cammino in un file di output
   Sorting_Generation(paths_odd_circ, positions_circ);

   ofstream output_best_path_circ("best_path_circ.out");

   for (int i=0;i<34;i++) {
      output_best_path_circ << paths_odd_circ[0][i] << endl;
   }
   output_best_path_circ << 0 << endl;

   output_best_path_circ.close();

   // Rieseguo il tutto analogamente per delle città in un quadrato unitario
   vector<vector<double>> positions_square = Generating_Cities_Square(rnd);

   ofstream output_cities_square("cities_positions_square.out");

   for (int i=0;i<34;i++) {
      output_cities_square << positions_square[i][0] << " " << positions_square[i][1] << endl;
   }

   output_cities_square.close();

   int generation_square = 0;
   vector<vector<int>> paths_even_square(100, vector<int>(34, 0));
   vector<vector<int>> paths_odd_square(100, vector<int>(34, 0));

   for (int i=0;i<100;i++) {
      paths_even_square[i] = Generating_Path(rnd);
      Check_Operator(paths_even_square[i], generation_square);
   }

   ofstream output_metric_square("metric_square.out");
   Sorting_Generation(paths_even_square, positions_square);
   sum = 0.;
   for (int j=0;j<50;j++) {
      sum += Metric(paths_even_square[j], positions_square)/50;
   }
   output_metric_square << generation_square << " " << Metric(paths_even_square[0], positions_square) << " " << sum << endl;

   for (int i=0;i<200;i++) {
      if (generation_square%2 == 0) {
         Odd_Generation_Generator(rnd, paths_even_square, paths_odd_square, positions_square, generation_square);
         Sorting_Generation(paths_odd_square, positions_square);
         double sum = 0.;
         for (int j=0;j<50;j++) {
            sum += Metric(paths_odd_square[j], positions_square)/50;
         }
         output_metric_square << generation_square << " " << Metric(paths_odd_square[0], positions_square) << " " << sum << endl;
      } else {
         Even_Generation_Generator(rnd, paths_even_square, paths_odd_square, positions_square, generation_square);
         Sorting_Generation(paths_odd_square, positions_square);
         sum = 0.;
         for (int j=0;j<50;j++) {
            sum += Metric(paths_odd_square[j], positions_square)/50;
         }
         output_metric_square << generation_square << " " << Metric(paths_odd_square[0], positions_square) << " " << sum << endl;
      }
      generation_square++;
   }

   output_metric_square.close();

   Sorting_Generation(paths_odd_square, positions_square);

   ofstream output_best_path_square("best_path_square.out");

   for (int i=0;i<34;i++) {
      output_best_path_square << paths_odd_square[0][i] << endl;
   }
   output_best_path_square << 0 << endl;

   output_best_path_square.close();

   rnd.SaveSeed();
   return 0;
}