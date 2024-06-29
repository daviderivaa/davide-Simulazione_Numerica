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
   int M; // Numero di passi per la stima iniziale di pi greco
   int N; // Numero di blocchi
   int L; // Lunghezza dei blocchi
   double l; // Lunghezza dei bastoncini
   double d; // Distanza tra le linee

   ifstream inputFile("INPUT/input.dat");

   if (!inputFile) {
      cerr << "Errore nell'aprire il file." << endl;
      return 1;
   }
    
   inputFile >> M;
   inputFile >> N;
   inputFile >> L;
   inputFile >> l;
   inputFile >> d;

   inputFile.close();

   // Calcolo pi greco necessario alla generazione di teta successivamente tramite un algoritmo convergente
   int hits=0;
   double pi_temp=1.0;
   double pi=0.;
   int k=0;

   ofstream optimization("OUTPUT/optimization.dat");
   optimization << k << " " << pi << endl;

   while (abs(pi-pi_temp)>0.001) {
      pi_temp=pi;
      hits=0;
      for (int i=0;i<M;i++) {
         double x = rnd.Rannyu(0,d);
         double teta = rnd.Rannyu(0,2*pi_temp);
         if ((x+l*cos(teta))>=d) {
            hits += 1;
         }
         else if ((x+l*cos(teta))<=0) {
            hits += 1;
         }
      }
      pi=(2*l*M)/(hits*d);
      k++;
      optimization << k << " " << pi << endl;
   }

   optimization.close();

   cout << "La stima di \u03C0 per convergenza è: " << pi << " dopo " << k << " interazioni." << endl;

   // Eseguo la stima di pi greco tramite Blocking Average utilizzando la stima precednte di pi greco per la generazione di un angolo theta
   vector<double> mean;
   vector<double> mean2;
   vector<double> sum_prog;
   vector<double> sum2_prog;
   vector<double> err_prog;

   for (int i=0;i<N;i++) {
      hits = 0;
      for (int j=0;j<L;j++){
         double x = rnd.Rannyu(0,d);
         double teta = rnd.Rannyu(0,2*pi);
         if ((x+l*cos(teta))>=d) {
            hits += 1;
         }
         else if ((x+l*cos(teta))<=0) {
            hits += 1;
         }
      }
      double sum = (2*l*L)/(hits*d);
      mean.push_back(sum);
      mean2.push_back(pow((sum),2));
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

   rnd.SaveSeed();
   return 0;
}