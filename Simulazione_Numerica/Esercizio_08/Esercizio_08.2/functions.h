#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

// Valuta la funzione d'onda
double Psi(double x, double sigma, double mu) {
   return exp(-pow(x-mu,2)/(2*pow(sigma,2))) + exp(-pow(x+mu,2)/(2*pow(sigma,2)));
};

// Valuta il modulo quadro della funzione d'onda
double Psi_Squared(double x, double sigma, double mu) {
   return pow(std::abs(exp(-pow(x-mu,2)/(2*pow(sigma,2))) + exp(-pow(x+mu,2)/(2*pow(sigma,2)))),2);
};

// Valuta la derivata seconda della funzione d'onda rispetto alla posizione x
double Psi_Second_Derivate(double x, double sigma, double mu) {
   double minusExp = exp(-0.5*(pow(x-mu,2)/(pow(sigma,2))));
   double plusExp = exp(-0.5*(pow(x+mu,2)/(pow(sigma,2))));
   return ((-1/pow(sigma,2))*minusExp)+((-1/pow(sigma,2))*plusExp)+((pow(x-mu,2)/pow(sigma,4))*minusExp)+((pow(x+mu,2)/pow(sigma,4))*plusExp);
};

// Effettua un passo dell'algoritmo di campionamento di Metropolis
void Step(Random &rnd, double &x, double rho, double &attempted, double &accepted, double sigma, double mu) {
   double x_new = x + rnd.Rannyu(-1,1)*rho;
   double A = std::min(1.0,(Psi_Squared(x_new, sigma, mu)/Psi_Squared(x, sigma, mu)));
   double r = rnd.Rannyu();
   if (r <= A) {
      x = x_new;
      accepted++;
   }
   attempted++;
};

// Valuta il potenziale
double Potential(double x) {
   return pow(x,4) - (5/2)*pow(x,2);
};

// Effettua un'equilibrazione con un numero dato di step "a vuoto"
void Equilibrate(int N, int L, double &x, Random &rnd, double rho, double sigma, double mu){
   double accepted = 0;
   double attempted = 0;
   for (int j = 0; j < N; j++){
      for (int i = 0; i < L; i++){
         Step(rnd, x, rho, attempted, accepted, sigma, mu);
      }
   }
};