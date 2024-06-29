#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <typeinfo>
#include "/home/davide/Simulazione_Numerica/Utilities/random.h"

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
   int M; // Numero di passi totali
   int N; // Numero di blocchi

   ifstream inputFile("INPUT/input.dat");

   if (!inputFile) {
      cerr << "Errore nell'aprire il file." << endl;
      return 1;
   }
    
   inputFile >> M;
   inputFile >> N;

   inputFile.close();

   int L = int(M/N); // Lunghezza dei blocchi

   vector<double> mean;
   vector<double> mean2;
   vector<double> sum_prog;
   vector<double> sum2_prog;
   vector<double> err_prog;

   // Eseguo le mie simulazioni tramite metodo della Blocking Average
   for (int i=0;i<N;i++) {
      double sum = 0;
      for (int i=0;i<L;i++){
      sum += rnd.Rannyu();
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

   // Stampo i risultati
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
      for (int j=0;j<L;j++) {
         sum += pow((rnd.Rannyu()-0.5),2);
      }
      mean.push_back(sum/L);
      mean2.push_back(pow((mean[i]),2));
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

   vector<double> chi;

   for (int i=0;i<100;i++) {
      double chi_temp = 0;
      int counter[100] = {};
      for (int j=0;j<10000;j++) {
         double n = rnd.Rannyu();
         int N = int(floor(n*100));
         counter[N]++;
      }
      for (int j=0;j<100;j++) {
         chi_temp += pow((counter[j]-100),2)/100;
      }
      chi.push_back(chi_temp);
   }

   ofstream output_3("OUTPUT/output_3.dat");
   
   for (int i=0;i<100;i++) {
      output_3 << i << " " << chi[i] << endl;
   }

   output_2.close();
   
   rnd.SaveSeed();
   return 0;
}