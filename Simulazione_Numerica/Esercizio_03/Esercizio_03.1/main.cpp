#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include "/home/davide/Simulazione_Numerica/Utilities/random.h"

using namespace std;

// Fa evolvere il sistema di moto Browniano in maniera diretta
double GBM(double t, double mu, double sigma, double S0, Random& rnd) {
   return S0*exp((mu-0.5*pow(sigma,2))*t+sigma*rnd.Gauss(0,t));
}

// Fa evolvere il sistema di moto Browniano in maniera discreta
double GBM_dis(double ti, double tii, double mu, double sigma, double Sti, Random& rnd) {
   return Sti*exp((mu-0.5*pow(sigma,2))*(tii-ti)+sigma*rnd.Gauss(0,1)*sqrt(tii-ti));
}

// Calcola la Call Option
double Call_opt(double r, double t, double S, double K) {
   return exp(-r*t)*max(0.0,S-K);
}

// Calcola la Put Option
double Put_opt(double r, double t, double S, double K) {
   return exp(-r*t)*max(0.0,K-S);
}

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

   ifstream inputFile("INPUT/input.dat");

   if (!inputFile) {
      cerr << "Errore nell'aprire il file." << endl;
      return 1;
   }
    
   inputFile >> N;
   inputFile >> L;

   inputFile.close();

   // Effettuo le mie simulazioni facendo evolvere il sistema e calcolando le stime delle Call e Put Option tramite Blocking Average nei 4 casi
   vector<double> mean;
   vector<double> mean2;
   vector<double> sum_prog;
   vector<double> sum2_prog;
   vector<double> err_prog;

   for (int i=0;i<N;i++) {
      double sum = 0;
      for (int j=0;j<L;j++){
         double St=GBM(1, 0.1, 0.25, 100, rnd);
         sum += Call_opt(0.1, 1, St, 100);
      }
      mean.push_back(sum/L);
      mean2.push_back(pow((sum/L),2));
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
      output << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output.close();

   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();

   for (int i=0;i<N;i++) {
      double sum = 0;
      for (int j=0;j<L;j++){
         double St_dis=100;
         for (int i=0;i<100;i++) {
            double app = GBM_dis(0.01*i, 0.01*(i+1), 0.1, 0.25, St_dis, rnd);
            St_dis = app;
         }
         sum += Call_opt(0.1, 1, St_dis, 100);
      }
      mean.push_back(sum/L);
      mean2.push_back(pow((sum/L),2));
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
      output_2 << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output_2.close();

   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();

   for (int i=0;i<N;i++) {
      double sum = 0;
      for (int j=0;j<L;j++){
         double St=GBM(1, 0.1, 0.25, 100, rnd);
         sum += Put_opt(0.1, 1, St, 100);
      }
      mean.push_back(sum/L);
      mean2.push_back(pow((sum/L),2));
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

   ofstream output_3("OUTPUT/output_3.dat");
   
   for (int i=0;i<N;i++) {
      output_3 << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output_3.close();

   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();

   for (int i=0;i<N;i++) {
      double sum = 0;
      for (int j=0;j<L;j++){
         double St_dis=100;
         for (int i=0;i<100;i++) {
            double app = GBM_dis(0.01*i, 0.01*(i+1), 0.1, 0.25, St_dis, rnd);
            St_dis = app;
         }
         sum += Put_opt(0.1, 1, St_dis, 100);
      }
      mean.push_back(sum/L);
      mean2.push_back(pow((sum/L),2));
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

   ofstream output_4("OUTPUT/output_4.dat");
   
   for (int i=0;i<N;i++) {
      output_4 << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output_4.close();

   rnd.SaveSeed();
   return 0;
}