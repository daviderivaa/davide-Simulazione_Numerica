#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <unordered_set>

using namespace std;

// Genera le città casualmente su una circonferenza unitaria
vector<vector<double>> Generating_Cities_Circumference(Random &rnd) {
   vector<vector<double>> positions;
   for (int i=0;i<34;i++) {
      vector<double> coordinates; // Genero le coordinate su una circonferenza unitaria
      double pos = rnd.Rannyu(0,2*M_PI);
      double x = cos(pos);
      double y = sin(pos);
      coordinates.push_back(x);
      coordinates.push_back(y);
      positions.push_back(coordinates);
   }
   return positions;
};

// Genera le città casualmente in un quadrato unitario
vector<vector<double>> Generating_Cities_Square(Random &rnd) {
   vector<vector<double>> positions;
   for (int i=0;i<34;i++) {
      vector<double> coordinates; // Genero le coordinate in un quadrato unitario
      double x = rnd.Rannyu();
      double y = rnd.Rannyu();
      coordinates.push_back(x);
      coordinates.push_back(y);
      positions.push_back(coordinates);
   }
   return positions;
};

// Genera un cammino casuale
vector<int> Generating_Path(Random &rnd) {
   vector<int> path;
   path.push_back(0); // Metto sempre la prima città con indice 0
   vector<int> vec(33);
   iota(vec.begin(), vec.end(), 1);
   for (int i=0;i<33;i++) {
      int pos = floor(rnd.Rannyu(0,vec.size()));
      path.push_back(vec[pos]); // Inserisco gli altri indici da 1 a 33 in ordine casuale
      vec.erase(vec.begin() + pos);
   }
   return path;
};

// Controlla che un cammino rispetti tutti i vincoli
void Check_Operator(vector<int> path, int &n) {
   for (int i=0;i<34;i++) {
      if (path[i]<0 or path[i]>33) {
         cerr << "Bond not fulfilled on generation no. " << n << ": unknown city!" << endl; // Controllo che non ci siano indici minori di 0 o maggiori di 33
      }
   }
   for (int i=0;i<33;i++) {
      for (int j=i+1;j<34;j++) {
         if (path[i] == path[j]) {
            cerr << "Bond not fulfilled on generation no. " << n << ": same city two times!" << endl; // Controllo che non passi due volte per la stessa città
         }
      }
   }
   if (path.size() != 34) {
      cerr << "Bond not fulfilled on generation no. " << n << ": city missed!" << endl; // Controllo che passi per tutte le città
   } else if (path[0] != 0) {
      cerr << "Bond not fulfilled on generation no. " << n << ": wrong first city!" << endl; // Controllo che parta dalla città corretta
   }
};

// Misura la metrica di un cammino
double Metric(vector<int> path, vector<vector<double>> positions) {
   double L = 0.;
   for (int i=0;i<33;i++) {
      L += pow(abs(positions[path[i]][0]-positions[path[i+1]][0]),2) + pow(abs(positions[path[i]][1]-positions[path[i+1]][1]),2);
   }
   L += pow(abs(positions[path[33]][0]-positions[path[0]][0]),2) + pow(abs(positions[path[33]][1]-positions[path[0]][1]),2);
   return L;
};

// Ordina i cammini di una generazione in ordine crescente di metrica
void Sorting_Generation(vector<vector<int>> &paths, vector<vector<double>> positions) {
   sort(paths.begin(), paths.end(), [positions](const vector<int>& a, const vector<int>& b) {
      return Metric(a, positions) < Metric(b, positions); // Ordino una generazione in ordine crescente di metrica
   });
};

// Seleziona un cammino da una generazione scegliendo con maggiore probabilità i migliori
int Selection_Operator(Random &rnd) {
   double r = rnd.Rannyu();
   return floor(34*pow(r,2)); // Seleziono un cammino di una generazione con maggiore probabilità per le migliori
};

// Rispetta le condizioni periodiche al contorno
int pbc(int i){
  if(i >= 34) i = i - 33; // Impongo delle condizioni al contorno per i vari operatori che non tocchino la prima città e non sforino le dimensioni dei cammini
  else if(i < 1)  i = i + 33;
  return i;
};

// Effettua una permutazione di una coppia di città casuali
void Pair_Permutation(vector<int> &path, Random &rnd) {
   int n = floor(rnd.Rannyu(1,34)); // Primo indice
   int m = floor(rnd.Rannyu(1,34)); // Secondo indice
   while (n == m) {
      m = floor(rnd.Rannyu(1,34)); // Rigenero il secondo se è uguale al primo
   }
   swap(path[n], path[m]); // Scambio le due città nel cammino
};

// Effettua uno shifting casuale di un blocco di città
void Shifting(vector<int> &path, Random &rnd) {
   int n = floor(rnd.Rannyu(1,34)); // Città di partenza
   int m = floor(rnd.Rannyu(1,33)); // Numero di città da shiftare
   int l = floor(rnd.Rannyu(1,34-m)); // Lunghezza dello shifting

   for (int i=0;i<l;i++) { // Ciclo sui passi dello shifting
      vector<int> copy = path;
      for (int j=0;j<m;j++) { // Ciclo sulle città da shiftare
         path[pbc(n+j+1)] = copy[pbc(n+j)];
      }
      path[pbc(n)] = copy[pbc(n+m)]; // Rimetto in "prima" posizione la città che si sovrappone allo shifting
   }
};

// Effettua una permutazione casuale all'interno di un blocco di città
void Permutation(vector<int> &path, Random &rnd) {
   int n = floor(rnd.Rannyu(1,34)); // Città di partenza
   int m = floor(rnd.Rannyu(1,34)); // Seconda città di partenza
   while (n == m) {
      m = floor(rnd.Rannyu(1,34)); // Rigenero la seconda città se è uguale alla prima
   }
   int l = floor(rnd.Rannyu(1,std::min(abs(m-n),abs(n-m))+1)); // Lunghezza del blocco da permutare
   for (int i=0;i<l;i++) {
      swap(path[pbc(n+i)], path[pbc(m+i)]); // Scambio le città nei due blocchi
   }
};

// Inverte l'ordine delle città all'interno di un blocco
void Order_Inversion(vector<int> &path, Random &rnd) {
   int n = floor(rnd.Rannyu(1,34)); // Città di partenza
   int m = floor(rnd.Rannyu(1,34)); // Città finale
   while (n == m) {
      m = floor(rnd.Rannyu(1,34));
   }
   int l; // Lunghezza del blocco da invertire
   if (m>n) {
      l = m-n+1;
   } else {
      l = 34-n+m;
   }
   vector<int> copy = path;
   for (int i=0;i<l;i++) {
      path[pbc(n+i)] = copy[pbc(n+l-1-i)]; // Inverto le città nel blocco
   }
};

// Effettua il crossover tra due cammini genitori
void Crossover(vector<int> &path1, vector<int> &path2, Random &rnd) {
   int n = static_cast<int>(floor(rnd.Rannyu(2, 33)));
   vector<int> copy1 = path1;
   vector<int> copy2 = path2;

   // Utilizzo set per tracciare gli elementi già usati
   unordered_set<int> usedInPath1;
   unordered_set<int> usedInPath2;

   // Popolo i set con gli elementi presenti prima dell'indice n
   for (int i = 0; i < n; ++i) {
      usedInPath1.insert(path1[i]);
      usedInPath2.insert(path2[i]);
   }

   // Eseguo il crossover per path1
   int index1 = n;
   for (int i = n; i < 34; ++i) {
      for (int j = 1; j < 34; ++j) {
         if (usedInPath1.find(copy2[j]) == usedInPath1.end()) {
               path1[index1] = copy2[j];
               usedInPath1.insert(copy2[j]);
               ++index1;
               break;
         }
      }
   }

   // Eseguo il crossover per path2
   int index2 = n;
   for (int i = n; i < 34; ++i) {
      for (int j = 1; j < 34; ++j) {
         if (usedInPath2.find(copy1[j]) == usedInPath2.end()) {
               path2[index2] = copy1[j];
               usedInPath2.insert(copy1[j]);
               ++index2;
               break;
         }
      }
   }
};

// Fa avvenire le varie mutazioni con una data probabilità
bool Calling_probability(Random &rnd, double p) {
   bool nullahosta = 0; // Variabile che dice se eseguire la mutazione o meno
   double r = rnd.Rannyu(0,100);
   if (r<p) {
      nullahosta = 1;
   }
   return nullahosta;
};

// Fa evolvere il sistema generando una generazione dispari
void Odd_Generation_Generator(Random &rnd, vector<vector<int>> &paths_even, vector<vector<int>> &paths_odd, vector<vector<double>> positions, int generation) {

   Sorting_Generation(paths_even, positions); // Ordino la generazione

   for (int i=0;i<50;i++) {
      int father_idx = Selection_Operator(rnd); // Seleziono un cammino padre
      int mother_idx = Selection_Operator(rnd); // Seleziono un cammino madre
      while (father_idx == mother_idx) {
         mother_idx = Selection_Operator(rnd); // Rigenero la madre se è uguale al padre
      }
      vector<int> father = paths_even[father_idx]; // Creo una copia del padre
      vector<int> mother = paths_even[mother_idx]; // Creo una copia della madre

      // Eseguo il crossover e le varie mutazioni con delle probabilità che avvengano
      if (Calling_probability(rnd, 75)) {
         Crossover(father, mother, rnd);
      }

      if (Calling_probability(rnd, 10)) {
         Pair_Permutation(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Shifting(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Permutation(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Order_Inversion(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Pair_Permutation(mother, rnd);
      } if (Calling_probability(rnd, 10)) {
         Shifting(mother, rnd);
      } if (Calling_probability(rnd, 10)) {
         Permutation(mother, rnd);
      } if (Calling_probability(rnd, 10)) {
         Order_Inversion(mother, rnd);
      }

      // Controllo che i cammini figli rispettino i vincoli
      Check_Operator(father, generation);
      Check_Operator(mother, generation);

      // Aggiungo i cammini figli alla generazione futura
      paths_odd[2*i] = father;
      paths_odd[2*i + 1] = mother;
   }
};

// Fa evolvere il sistema generando una generazione pari
void Even_Generation_Generator(Random &rnd, vector<vector<int>> &paths_even, vector<vector<int>> &paths_odd, vector<vector<double>> positions, int generation) {
   
   Sorting_Generation(paths_odd, positions); // Ordino la generazione

   for (int i=0;i<50;i++) {
      int father_idx = Selection_Operator(rnd); // Seleziono un cammino padre
      int mother_idx = Selection_Operator(rnd); // Seleziono un cammino madre
      while (father_idx == mother_idx) {
         mother_idx = Selection_Operator(rnd); // Rigenero la madre se è uguale al padre
      }
      vector<int> father = paths_odd[father_idx]; // Creo una copia del padre
      vector<int> mother = paths_odd[mother_idx]; // Creo una copia della madre

      // Eseguo il crossover e le varie mutazioni con delle probabilità che avvengano
      if (Calling_probability(rnd, 75)) {
        Crossover(father, mother, rnd);
      }

      if (Calling_probability(rnd, 10)) {
         Pair_Permutation(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Shifting(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Permutation(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Order_Inversion(father, rnd);
      } if (Calling_probability(rnd, 10)) {
         Pair_Permutation(mother, rnd);
      } if (Calling_probability(rnd, 10)) {
         Shifting(mother, rnd);
      } if (Calling_probability(rnd, 10)) {
         Permutation(mother, rnd);
      } if (Calling_probability(rnd, 10)) {
         Order_Inversion(mother, rnd);
      }

      // Controllo che i cammini figli rispettino i vincoli
      Check_Operator(father, generation);
      Check_Operator(mother, generation);

      // Aggiungo i cammini figli alla generazione futura
      paths_even[2*i] = father;
      paths_even[2*i + 1] = mother;
   }
};