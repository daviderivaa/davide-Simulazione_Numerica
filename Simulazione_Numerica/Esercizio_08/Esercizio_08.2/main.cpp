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
   double sigma;
   double mu;
   double T; // Temperatura di partenza

   ifstream inputFile("INPUT/input.dat");

   if (!inputFile) {
      cerr << "Errore nell'aprire il file." << endl;
      return 1;
   }
   
   inputFile >> N;
   inputFile >> L;
   inputFile >> rho;
   inputFile >> sigma;
   inputFile >> mu;
   inputFile >> T;

   // Definisco i paramteri necessari all'ottimizzazione dei parametri
   double beta = 1/T;
   double energy = 1.;
   double currentEnergy = energy;
   double currentError = 0.;
   double bestEnergy = energy;
   double bestParameters[2] = {sigma, mu};

   double x = 0.0;

   ofstream output_opt("OUTPUT/optimization.dat");
   ofstream output_par("OUTPUT/parameters.dat");

   // Ciclo sulla temperatura
   while (T>=0.01) {

      double old_sigma = sigma;
      double old_mu = mu;

      // Faccio evolvere i parametri
      sigma = abs(old_sigma + rnd.Rannyu(-1, 1) * .25 * T);
      mu = abs(old_mu + rnd.Rannyu(-1, 1) * .5 * T);

      // Eseguo l'equilibrazione e calcolo il valore dell'Hamiltoniana
      Equilibrate(N, L, x, rnd, rho, sigma, mu);

      vector<double> mean;
      vector<double> mean2;
      vector<double> sum_prog;
      vector<double> sum2_prog;
      vector<double> err_prog;

      for (int i=0;i<N;i++) {
         double sum = 0.0;
         double attempted = 0.0;
         double accepted = 0.0;
         for (int j=0;j<L;j++){
            Step(rnd, x, rho, attempted, accepted, sigma, mu);
            sum += ((-0.5*Psi_Second_Derivate(x, sigma, mu))/Psi(x, sigma, mu)) + Potential(x);
         }
         mean.push_back(sum/L);
         mean2.push_back(pow(sum/L,2));
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

      currentEnergy = sum_prog[N-1];
      currentError = err_prog[N-1];

      double A = std::min(1., (exp(- beta * (currentEnergy - energy))));
      double p = rnd.Rannyu();

      // Aggiorno l'energia e i paramteri con una certa probabilità
      if (A > p)
         energy = currentEnergy;
      else{
         mu = old_mu;
         sigma = old_sigma;
      }

      output_opt << T << " " << energy << " " << currentError << endl;
      output_par << T << " " << sigma << " " << mu << endl;

      T = T * 0.997;
      beta = 1/T;

      // Salvo i parametri migliori
      if (bestEnergy > energy) {
         bestEnergy = energy;
         bestParameters[0] = sigma;
         bestParameters[1] = mu;
      }
   }

   output_opt.close();
   output_par.close();

   // Stampo i paramteri best
   sigma = bestParameters[0];
   mu = bestParameters[1];

   ofstream output_best_par("OUTPUT/best_parameters.dat");

   output_best_par << sigma << " " << mu << endl;
   
   output_best_par.close();

   // Rieseguo la simulazione da zero coi parametri ottimali
   x = 0.0;

   Equilibrate(N, L, x, rnd, rho, sigma, mu);

   vector<double> mean;
   vector<double> mean2;
   vector<double> sum_prog;
   vector<double> sum2_prog;
   vector<double> err_prog;
   vector<double> acceptance;
   vector<double> position;

   position.push_back(x);

   for (int i=0;i<N;i++) {
      double sum = 0.0;
      double attempted = 0.0;
      double accepted = 0.0;
      for (int j=0;j<L;j++){
         Step(rnd, x, rho, attempted, accepted, sigma, mu);
         sum += ((-0.5*Psi_Second_Derivate(x, sigma, mu))/Psi(x, sigma, mu)) + Potential(x);
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

   ofstream output("OUTPUT/output.dat");

   for (int i=0;i<N;i++) {
      output << sum_prog[i] << " " << err_prog[i] << " " << acceptance[i] << endl;
   }

   output.close();

   ofstream posizioni("OUTPUT/position.dat");

   for (int i=0;i<N*L;i++) {
      posizioni << position[i] << endl;
   }

   posizioni.close();
   
   rnd.SaveSeed();
   return 0;
}