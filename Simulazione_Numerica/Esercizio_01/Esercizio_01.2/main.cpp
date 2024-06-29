#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
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
   int M; // Lanci nella realizzazione
   int N[4]; // Numero di realizzazioni

   ifstream inputFile("INPUT/input.dat");

   if (!inputFile) {
      cerr << "Errore nell'aprire il file." << endl;
      return 1;
   }
    
   inputFile >> M;
   inputFile >> N[0];
   inputFile >> N[1];
   inputFile >> N[2];
   inputFile >> N[3];

   inputFile.close();

   vector<double> uni;
   vector<double> exp;
   vector<double> cau;

   for (int i=0;i<4;i++) {
      for (int j=0;j<M;j++) {
         double sum_uni=0;
         double sum_exp=0;
         double sum_cau=0;
         for (int k=0;k<N[i];k++) {
            sum_uni += rnd.Rannyu();
            sum_exp += rnd.Exp(1);
            sum_cau += rnd.CauchyL(0,1);
         }
         uni.push_back(sum_uni/N[i]);
         exp.push_back(sum_exp/N[i]);
         cau.push_back(sum_cau/N[i]);
      }
   }

   ofstream output("OUTPUT/output.dat");
   
   for (int i=0;i<4*M;i++) {
      output << uni[i] << " " << exp[i] << " " << cau[i] << endl;
   }

   output.close();

   rnd.SaveSeed();
   return 0;
}