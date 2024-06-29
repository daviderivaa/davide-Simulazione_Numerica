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

   vector<double> sum_prog_def;
   vector<double> err_prog_def;

   ofstream position_dis("OUTPUT/position_dis.dat");

   // Ciclo sulle lunghezze dei cammini e per caiscuna lunghezza fissata stimo il valore della distanza dall'origine con la rispettiva incertezza tramite Blocking Average
   for (int i=0;i<100;i++) {

      vector<double> mean;
      vector<double> mean2;
      vector<double> sum_prog;
      vector<double> sum2_prog;
      vector<double> err_prog;

      for (int j=0;j<N;j++) {
         double sum = 0;
         for (int k=0;k<L;k++){
            double pos_dis[3] = {0,0,0};
            for (int l=0;l<i+1;l++) {
               if (i==99 and j==N-1 and k==L-11) {
                  position_dis << l << " " << pos_dis[0] << " " << pos_dis[1] << " " << pos_dis[2] << endl;
               }
               int dir = floor(rnd.Rannyu(0,3));
               double ver = rnd.Rannyu(0,2);
               if (ver<1) {
                  pos_dis[dir]+=1;
               } else {
                  pos_dis[dir]-=1;
               }
            }
            sum += (pow(pos_dis[0],2) + pow(pos_dis[1],2) + pow(pos_dis[2],2));
         }
         mean.push_back(sqrt(sum/L));
         mean2.push_back(sum/L);
      }

      for (int j=0;j<N;j++) {
         sum_prog.push_back(0);
         sum2_prog.push_back(0);
         for (int k=0;k<j+1;k++) {
            sum_prog[j] += mean[k];
            sum2_prog[j] += mean2[k];
         }
         sum_prog[j]/=(j+1);
         sum2_prog[j]/=(j+1);
         if (j==0) {
            err_prog.push_back(0);
         }
         else {err_prog.push_back(pow(((sum2_prog[j] - pow(sum_prog[j],2))/j),0.5));
         }
      }

      sum_prog_def.push_back(sum_prog[N-1]);
      err_prog_def.push_back(err_prog[N-1]);
   }

   position_dis.close();

   ofstream output("OUTPUT/output.dat");
   
   for (int i=0;i<100;i++) {
      output << sum_prog_def[i] << " " << err_prog_def[i] << endl;
   }

   output.close();

   cout << "La radice del valor medio del modulo quadro della posizione del RW discreto dopo 100 passi è: " << sum_prog_def[99] << endl; 

   // Ripeto il tutto per un random walk continuo
   sum_prog_def.clear();
   err_prog_def.clear();

   ofstream position_con("OUTPUT/position_con.dat");

   for (int i=0;i<100;i++) {

      vector<double> mean;
      vector<double> mean2;
      vector<double> sum_prog;
      vector<double> sum2_prog;
      vector<double> err_prog;

      for (int j=0;j<N;j++) {
         double sum = 0;
         for (int k=0;k<L;k++){
            double pos_con[3] = {0,0,0};
            for (int l=0;l<i+1;l++) {
               if (i==99 and j==N-1 and k==L-11) {
                  position_con << l << " " << pos_con[0] << " " << pos_con[1] << " " << pos_con[2] << endl;
               }
               double teta = acos(1-2*rnd.Rannyu());
               double phi = rnd.Rannyu(0,2*M_PI);
               pos_con[0] += sin(teta)*cos(phi);
               pos_con[1] += sin(teta)*sin(phi);
               pos_con[2] += cos(teta);
            }
            sum += (pow(pos_con[0],2) + pow(pos_con[1],2) + pow(pos_con[2],2));
         }
         mean.push_back(sqrt(sum/L));
         mean2.push_back(sum/L);
      }

      for (int j=0;j<N;j++) {
         sum_prog.push_back(0);
         sum2_prog.push_back(0);
         for (int k=0;k<j+1;k++) {
            sum_prog[j] += mean[k];
            sum2_prog[j] += mean2[k];
         }
         sum_prog[j]/=(j+1);
         sum2_prog[j]/=(j+1);
         if (j==0) {
            err_prog.push_back(0);
         }
         else {err_prog.push_back(pow(((sum2_prog[j] - pow(sum_prog[j],2))/j),0.5));
         }
      }

      sum_prog_def.push_back(sum_prog[N-1]);
      err_prog_def.push_back(err_prog[N-1]);
   }

   position_con.close();

   ofstream output_2("OUTPUT/output_2.dat");
   
   for (int i=0;i<100;i++) {
      output_2 << sum_prog_def[i] << " " << err_prog_def[i] << endl;
   }

   output_2.close();

   cout << "La radice del valor medio del modulo quadro della posizione del RW continuo dopo 100 passi è: " << sum_prog_def[99] << endl; 

   rnd.SaveSeed();
   return 0;
}