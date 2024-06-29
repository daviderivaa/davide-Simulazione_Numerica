#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include "/home/davide/Simulazione_Numerica/Utilities/random.h"

using namespace std;

// Valuta la funzione d'onda del Ground State
double psi_1(vector<double> &pos) {
   double r = sqrt(pow(pos[0],2) + pow(pos[1],2) + pow(pos[2],2));
   return pow((1/sqrt(M_PI))*exp(-r),2);
}

// Effettua uno step per il Ground State con probabilità di transizione uniforme
void step_1(Random &rnd, vector<double> &pos, double rho, double &counter) {
   vector<double> pos_1 = pos;
   pos_1[0] += rnd.Rannyu(-rho,rho);
   pos_1[1] += rnd.Rannyu(-rho,rho);
   pos_1[2] += rnd.Rannyu(-rho,rho);
   double A = min(1.0,(psi_1(pos_1)/psi_1(pos)));
   double r = rnd.Rannyu();
   if (r <= A) {
      pos[0] = pos_1[0];
      pos[1] = pos_1[1];
      pos[2] = pos_1[2];
      counter++;
   }
}

// Valuta la funzione d'onda dello Stato Eccitato
double psi_2(vector<double> &pos) {
   double r = sqrt(pow(pos[0],2) + pow(pos[1],2) + pow(pos[2],2));
   return pow((sqrt(2)/(8*sqrt(M_PI)))*r*exp(-r/2)*(pos[2]/r),2);
}

// Effettua uno step per lo Stato Eccitato con probabilità di transizione uniforme
void step_2(Random &rnd, vector<double> &pos, double rho, double &counter) {
   vector<double> pos_1 = pos;
   pos_1[0] += rnd.Rannyu(-rho,rho);
   pos_1[1] += rnd.Rannyu(-rho,rho);
   pos_1[2] += rnd.Rannyu(-rho,rho);
   double A = min(1.0,(psi_2(pos_1)/psi_2(pos)));
   double r = rnd.Rannyu();
   if (r <= A) {
      pos[0] = pos_1[0];
      pos[1] = pos_1[1];
      pos[2] = pos_1[2];
      counter++;
   }
}

// Effettua uno step per il Ground State con probabilità di transizione Gaussiana
void step_1_Gauss(Random &rnd, vector<double> &pos, double rho, double &counter) {
   vector<double> pos_1 = pos;
   pos_1[0] += rnd.Gauss(0,rho);
   pos_1[1] += rnd.Gauss(0,rho);
   pos_1[2] += rnd.Gauss(0,rho);
   double A = min(1.0,(psi_1(pos_1)/psi_1(pos)));
   double r = rnd.Rannyu();
   if (r <= A) {
      pos[0] = pos_1[0];
      pos[1] = pos_1[1];
      pos[2] = pos_1[2];
      counter++;
   }
}

// Effettua uno step per lo Stato Eccitato con probabilità di transizione Gaussiana
void step_2_Gauss(Random &rnd, vector<double> &pos, double rho, double &counter) {
   vector<double> pos_1 = pos;
   pos_1[0] += rnd.Gauss(0,rho);
   pos_1[1] += rnd.Gauss(0,rho);
   pos_1[2] += rnd.Gauss(0,rho);
   double A = min(1.0,(psi_2(pos_1)/psi_2(pos)));
   double r = rnd.Rannyu();
   if (r <= A) {
      pos[0] = pos_1[0];
      pos[1] = pos_1[1];
      pos[2] = pos_1[2];
      counter++;
   }
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
   double rho1; // Delta Ground State con probabilità di transizione unifrome
   double rho2; // Delta Stato Eccitato con probabilità di transizione unifrome
   double rho1_Gauss; // Delta Ground State con probabilità di transizione Gaussiana
   double rho2_Gauss; // Delta Stato Eccitato con probabilità di transizione Gaussiana

   ifstream inputFile("INPUT/input.dat");
    
   inputFile >> N;
   inputFile >> L;
   inputFile >> rho1;
   inputFile >> rho2;
   inputFile >> rho1_Gauss;
   inputFile >> rho2_Gauss;

   inputFile.close();

   vector<double> mean;
   vector<double> mean2;
   vector<double> sum_prog;
   vector<double> sum2_prog;
   vector<double> err_prog;
   vector<double> acceptance;
   vector<vector<double>> position;

   // Decido una posizione di partenza
   vector<double> pos = {1.0,1.0,1.0};
   position.push_back(pos);
   double counter = 0.0;
   
   // Eseguo la stima di r tramite Blocking Average
   for (int i=0;i<N;i++) {
      double sum = 0.0;
      for (int j=0;j<L;j++){
         step_1(rnd, pos, rho1, counter);
         position.push_back(pos);
         sum += sqrt(pow(pos[0],2) + pow(pos[1],2) + pow(pos[2],2));
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

   // Stampo i valori di r con la rispettiva incertezza
   ofstream output("OUTPUT/output.dat");
   
   for (int i=0;i<N;i++) {
      output << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output.close();

   // Stampo le posizioni lungo il corso della simulazione
   ofstream posizioni("OUTPUT/position.dat");
   
   for (int i=0;i<N*L;i++) {
      posizioni << position[i][0] << " " << position[i][1] << " " << position[i][2] << endl;
   }

   posizioni.close();

   // Controllo di aver tenuto un'accettanza intorno a 0.5
   cout << "Accettanza per il ground state (Unif.): " << counter/(N*L) << endl;

   // Ripeto il procedimento con un altra rho e poi facendo una probabilità di transizione Gaussiana invece che uniforme
   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();
   acceptance.clear();
   position.clear();

   pos = {1.0,1.0,1.0};
   position.push_back(pos);
   counter = 0.0;
   
   for (int i=0;i<N;i++) {
      double sum = 0.0;
      for (int j=0;j<L;j++){
         step_2(rnd, pos, rho2, counter);
         position.push_back(pos);
         sum += sqrt(pow(pos[0],2) + pow(pos[1],2) + pow(pos[2],2));
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

   ofstream output_2("OUTPUT/output_2.dat");
   
   for (int i=0;i<N;i++) {
      output_2 << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output_2.close();

   ofstream posizioni_2("OUTPUT/position_2.dat");
   
   for (int i=0;i<N*L;i++) {
      posizioni_2 << position[i][0] << " " << position[i][1] << " " << position[i][2] << endl;
   }

   posizioni_2.close();

   cout << "Accettanza per lo stato eccittato (Unif.): " << counter/(N*L) << endl;

   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();
   acceptance.clear();
   position.clear();

   pos = {1.0,1.0,1.0};
   position.push_back(pos);
   counter = 0.0;

   for (int i=0;i<N;i++) {
      double sum = 0.0;
      for (int j=0;j<L;j++){
         step_1_Gauss(rnd, pos, rho1_Gauss, counter);
         position.push_back(pos);
         sum += sqrt(pow(pos[0],2) + pow(pos[1],2) + pow(pos[2],2));
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

   ofstream output_Gauss("OUTPUT/output_Gauss.dat");
   
   for (int i=0;i<N;i++) {
      output_Gauss << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output_Gauss.close();

   ofstream posizioni_Gauss("OUTPUT/position_Gauss.dat");
   
   for (int i=0;i<N*L;i++) {
      posizioni_Gauss << position[i][0] << " " << position[i][1] << " " << position[i][2] << endl;
   }

   posizioni_Gauss.close();

   cout << "Accettanza per il ground state (Gauss.): " << counter/(N*L) << endl;

   mean.clear();
   mean2.clear();
   sum_prog.clear();
   sum2_prog.clear();
   err_prog.clear();
   acceptance.clear();
   position.clear();

   pos = {1.0,1.0,1.0};
   position.push_back(pos);
   counter = 0.0;

   for (int i=0;i<N;i++) {
      double sum = 0.0;
      for (int j=0;j<L;j++){
         step_2_Gauss(rnd, pos, rho2_Gauss, counter);
         position.push_back(pos);
         sum += sqrt(pow(pos[0],2) + pow(pos[1],2) + pow(pos[2],2));
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

   ofstream output_2_Gauss("OUTPUT/output_2_Gauss.dat");
   
   for (int i=0;i<N;i++) {
      output_2_Gauss << sum_prog[i] << " " << err_prog[i] << endl;
   }

   output_2_Gauss.close();

   ofstream posizioni_2_Gauss("OUTPUT/position_2_Gauss.dat");
   
   for (int i=0;i<N*L;i++) {
      posizioni_2_Gauss << position[i][0] << " " << position[i][1] << " " << position[i][2] << endl;
   }

   posizioni_2_Gauss.close();

   cout << "Accettanza per lo stato eccittato (Gauss.): " << counter/(N*L) << endl;

   rnd.SaveSeed();
   return 0;
}