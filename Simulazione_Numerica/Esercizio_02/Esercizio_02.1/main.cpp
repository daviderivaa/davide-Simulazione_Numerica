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

   // Eseguo la stima dell'integrale con sampling uniforme e con importance sampling su una distribuzione lineare e stimo il valore con Blocking Average
   vector<double> mean;
   vector<double> mean2;
   vector<double> sum_prog;
   vector<double> sum2_prog;
   vector<double> err_prog;

   for (int i=0;i<N;i++) {
      double sum = 0;
      for (int j=0;j<L;j++){
         sum += (M_PI/2)*cos((M_PI*rnd.Rannyu())/2);
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

   double I = sum_prog[N-1];

   cout << "La stima dell'integrale tramite MC con distribuzione uniforme è: " << I << endl;

   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();

   for (int i=0;i<N;i++) {
      double sum = 0;
      for (int i=0;i<L;i++){
      double x=rnd.Lin(-2,2,0,1);
      sum += ((M_PI/2)*cos((M_PI*x)/2))/(2-2*x);
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

   output.close();

   double I_2 = sum_prog[N-1];

   cout << "La stima dell'integrale tramite MC con distribuzione lineare è: " << I_2 << endl;

   rnd.SaveSeed();
   return 0;
}