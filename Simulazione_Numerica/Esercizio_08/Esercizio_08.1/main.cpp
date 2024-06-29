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

   // Inizializzo il generatore random
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

   // Inizializzo le variabili necessarie e le leggo dal file di input
   int N; // Numero di blocchi
   int L; // Lunghezza dei blocchi
   double rho; // Parametri per il campionamento
   double sigma_1;
   double mu_1;
   double sigma_2;
   double mu_2;

   ifstream inputFile("INPUT/input.dat");

   if (!inputFile) {
      cerr << "Errore nell'aprire il file." << endl;
      return 1;
   }
   
   inputFile >> N;
   inputFile >> L;
   inputFile >> rho;
   inputFile >> sigma_1;
   inputFile >> mu_1;
   inputFile >> sigma_2;
   inputFile >> mu_2;

   inputFile.close();

   // Scelgo una posizione di partenza ed effetuo l'equilibrazione
   double x = 0.0;
   Equilibrate(100, 1000, x, rnd, rho, sigma_1, mu_1);

   vector<double> mean;
   vector<double> mean2;
   vector<double> sum_prog;
   vector<double> sum2_prog;
   vector<double> err_prog;
   vector<double> acceptance;
   vector<double> position;

   position.push_back(x);

   // Faccio evolvere il mio sistema e valuto il valore dell'Hamiltoniana col metodo della Blocking Average
   for (int i=0;i<N;i++) {
      double sum = 0.0;
      double attempted = 0;
      double accepted = 0;
      for (int j=0;j<L;j++){
         Step(rnd, x, rho, attempted, accepted, sigma_1, mu_1);
         sum += ((-0.5*Psi_Second_Derivate(x, sigma_1, mu_1))/Psi(x, sigma_1, mu_1)) + Potential(x);
         position.push_back(x);
      }
      mean.push_back(sum/L);
      mean2.push_back(pow(sum/L,2));
      acceptance.push_back(accepted/attempted);
   }

   for (int i=0;i<N;i++) {
      sum_prog.push_back(0);
      sum2_prog.push_back(0);
      for (int j=0;j<i+1;j++) {
         sum_prog[i] += mean[j];
         sum2_prog[i] += mean2[j];
      }
      sum_prog[i]/=(i+1);
      sum2_prog[i]/=(i+1);
      if (i==0) {
         err_prog.push_back(0);
      }
      else {err_prog.push_back(pow(((sum2_prog[i] - pow(sum_prog[i],2))/i),0.5));
      }
   }

   // Stampo i valori dell'Hamiltoniana con la rispettiva incertezza in funzione dei blocchi
   ofstream output("OUTPUT/output.dat");

   for (int i=0;i<N;i++) {
      output << i << " " << sum_prog[i] << " " << err_prog[i] << " " << acceptance[i] << endl;
   }

   output.close();

   // Stampo la posizione lungo il corso della simulazione
   ofstream posizioni("OUTPUT/position.dat");

   for (int i=0;i<N*L;i++) {
      posizioni << position[i] << endl;
   }

   posizioni.close();

   // Ripeto tutto il processo con un diverso set di parametri sigma e mu
   x = 0.0;
   Equilibrate(100, 1000, x, rnd, rho, sigma_2, mu_2);

   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();
   acceptance.clear();
   position.clear();

   position.push_back(x);

   for (int i=0;i<N;i++) {
      double sum = 0.0;
      double attempted = 0;
      double accepted = 0;
      for (int j=0;j<L;j++){
         Step(rnd, x, rho, attempted, accepted, sigma_2, mu_2);
         sum += ((-0.5*Psi_Second_Derivate(x, sigma_2, mu_2))/Psi(x, sigma_2, mu_2)) + Potential(x);
         position.push_back(x);
      }
      mean.push_back(sum/L);
      mean2.push_back(pow(sum/L,2));
      acceptance.push_back(accepted/attempted);
   }

   for (int i=0;i<N;i++) {
      sum_prog.push_back(0);
      sum2_prog.push_back(0);
      for (int j=0;j<i+1;j++) {
         sum_prog[i] += mean[j];
         sum2_prog[i] += mean2[j];
      }
      sum_prog[i]/=(i+1);
      sum2_prog[i]/=(i+1);
      if (i==0) {
         err_prog.push_back(0);
      }
      else {err_prog.push_back(pow(((sum2_prog[i] - pow(sum_prog[i],2))/i),0.5));
      }
   }

   ofstream output_2("OUTPUT/output_2.dat");

   for (int i=0;i<N;i++) {
      output_2 << i << " " << sum_prog[i] << " " << err_prog[i] << " " << acceptance[i] << endl;
   }

   output_2.close();

   ofstream posizioni_2("OUTPUT/position_2.dat");

   for (int i=0;i<N*L;i++) {
      posizioni_2  << position[i] << endl;
   }

   posizioni_2.close();
   
   rnd.SaveSeed();
   return 0;
}