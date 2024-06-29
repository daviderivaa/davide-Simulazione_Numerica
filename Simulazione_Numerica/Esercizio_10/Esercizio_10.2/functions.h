#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <unordered_set>
#include "mpi.h"

using namespace std;

const int n_paths = 100; // Numero di cammini per ogni generazione
const int path_size = 110; // Numero di città per ogni cammino
const int best_to_change = 10; // Numero di cammini scambiati tra i vari continenti
const int exchange_time = 100; // Generazioni tra uno scambio e l'altro
const int changes = 100; // Scambi totali

// Legge le posizioni delle città da file
void Cities_Reader(const string &path, vector<vector<double>> &positions) {
   ifstream file(path);
   if (!file) {
      cerr << "Errore nell'apertura del file." << endl;
   }

   string line;
   while (getline(file, line)) {
      istringstream iss(line);
      double x, y;
      if (iss >> x >> y) {
         positions.push_back({x,y});
      }
   }

   file.close();
};

// Inizializza il generatore di numeri casuali
void Random_Initializer(Random &rnd) {
   int seed[4];
   int cestino;
   int p1, p2;
   ifstream Primes("/home/davide/Simulazione_Numerica/Utilities/Primes");
   if (Primes.is_open()){
      for (int i=0;i<10;i++) {
         Primes >> cestino;
         Primes >> cestino;
      }
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
};

// Inizializza il generatore di numeri casuali per ogni continente
void Random_Initializer(Random &rnd, int &rank) {
   int seed[4];
   int cestino;
   int p1, p2;
   ifstream Primes("/home/davide/Simulazione_Numerica/Utilities/Primes");
   if (Primes.is_open()){
      for (int i=0;i<rank;i++) {
         Primes >> cestino;
         Primes >> cestino;
      }
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
};

// Mischia in maniera casuale i cammini da scambiare tra i continenti
void Shuffler(Random &rnd, vector<vector<int>> &vec) {
   vector<vector<int>> copy = vec;

   for (int i=0;i<copy.size();i++) {
      int n = rnd.Rannyu(0,copy.size());
      vec[i] = copy[n];
      copy.erase(copy.begin() + n);
   }
};

// Genera le città casualmente su una circonferenza unitaria
vector<vector<double>> Generating_Cities_Circumference(Random &rnd) {
   vector<vector<double>> positions;
   for (int i=0;i<path_size;i++) {
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
   for (int i=0;i<path_size;i++) {
      vector<double> coordinates; // Genero le coordinate in un quadrato unitario
      double x = rnd.Rannyu();
      double y = rnd.Rannyu();
      coordinates.push_back(x);
      coordinates.push_back(y);
      positions.push_back(coordinates);
   }
   return positions;
};

// Stampa le posizioni delle città
void Cities_Printer(const string &path, vector<vector<double>> positions) {
   ofstream output(path);

   for (int i=0;i<path_size;i++) {
      output << positions[i][0] << " " << positions[i][1] << endl;
   }

   output.close();
};

// Stampa il cammino migliore
void Best_Path_Printer(const string &path, vector<vector<int>> &paths_odd) {
   ofstream output_best_path(path);

   for (int i=0;i<path_size;i++) {
      output_best_path << paths_odd[0][i] << endl;
   }
   output_best_path << 0 << endl;

   output_best_path.close();
};

// Genera un cammino casuale
vector<int> Generating_Path(Random &rnd) {
   vector<int> path;
   path.push_back(0); // Metto sempre la prima città con indice 0
   vector<int> vec((path_size-1));
   iota(vec.begin(), vec.end(), 1);
   for (int i=0;i<(path_size-1);i++) {
      int pos = floor(rnd.Rannyu(0,vec.size()));
      path.push_back(vec[pos]); // Inserisco gli altri indici da 1 a 33 in ordine casuale
      vec.erase(vec.begin() + pos);
   }
   return path;
};

// Controlla che un cammino rispetti tutti i vincoli
void Check_Operator(vector<int> path, int &n) {
   for (int i=0;i<path_size;i++) {
      if (path[i]<0 or path[i]>(path_size-1)) {
         cerr << "Bond not fulfilled on generation no. " << n << ": unknown city!" << endl; // Controllo che non ci siano indici minori di 0 o maggiori di 33
      }
   }
   for (int i=0;i<(path_size-1);i++) {
      for (int j=i+1;j<path_size;j++) {
         if (path[i] == path[j]) {
            cerr << "Bond not fulfilled on generation no. " << n << ": same city two times!" << endl; // Controllo che non passi due volte per la stessa città
         }
      }
   }
   if (path.size() != path_size) {
      cerr << "Bond not fulfilled on generation no. " << n << ": city missed!" << endl; // Controllo che passi per tutte le città
   } else if (path[0] != 0) {
      cerr << "Bond not fulfilled on generation no. " << n << ": wrong first city!" << endl; // Controllo che parta dalla città corretta
   }
};

// Inizializza una generazione di cammini casuali
void Generation_Creator(Random &rnd, vector<vector<int>> &paths_even, int &generation) {
   for (int i=0;i<n_paths;i++) {
      paths_even[i] = Generating_Path(rnd);
      Check_Operator(paths_even[i], generation);
   }
};

// Misura la metrica di un cammino
double Metric(vector<int> path, vector<vector<double>> positions) {
   double L = 0.;
   for (int i=0;i<(path_size-1);i++) {
      L += pow(abs(positions[path[i]][0]-positions[path[i+1]][0]),2) + pow(abs(positions[path[i]][1]-positions[path[i+1]][1]),2);
   }
   L += pow(abs(positions[path[(path_size-1)]][0]-positions[path[0]][0]),2) + pow(abs(positions[path[(path_size-1)]][1]-positions[path[0]][1]),2);
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
   return floor(n_paths*pow(r,2)); // Seleziono un cammino di una generazione con maggiore probabilità per le migliori
};

// Rispetta le condizioni periodiche al contorno
int pbc(int i) {
  if(i >= path_size) i = i - (path_size-1); // Impongo delle condizioni al contorno per i vari operatori che non tocchino la prima città e non sforino le dimensioni dei cammini
  else if(i < 1)  i = i + (path_size-1);
  return i;
};

// Effettua una permutazione di una coppia di città casuali
void Pair_Permutation(vector<int> &path, Random &rnd) {
   int n = floor(rnd.Rannyu(1,path_size)); // Primo indice
   int m = floor(rnd.Rannyu(1,path_size)); // Secondo indice
   while (n == m) {
      m = floor(rnd.Rannyu(1,path_size)); // Rigenero il secondo se è uguale al primo
   }
   swap(path[n], path[m]); // Scambio le due città nel cammino
};

// Effettua uno shifting casuale di un blocco di città
void Shifting(vector<int> &path, Random &rnd) {
   int n = floor(rnd.Rannyu(1,path_size)); // Città di partenza
   int m = floor(rnd.Rannyu(1,(path_size-1))); // Numero di città da shiftare
   int l = floor(rnd.Rannyu(1,path_size-m)); // Lunghezza dello shifting

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
   int n = floor(rnd.Rannyu(1,path_size)); // Città di partenza
   int m = floor(rnd.Rannyu(1,path_size)); // Seconda città di partenza
   while (n == m) {
      m = floor(rnd.Rannyu(1,path_size)); // Rigenero la seconda città se è uguale alla prima
   }
   int l = floor(rnd.Rannyu(1,std::min(abs(m-n),abs(n-m))+1)); // Lunghezza del blocco da permutare
   for (int i=0;i<l;i++) {
      swap(path[pbc(n+i)], path[pbc(m+i)]); // Scambio le città nei due blocchi
   }
};

// Inverte l'ordine delle città all'interno di un blocco
void Order_Inversion(vector<int> &path, Random &rnd) {
   int n = floor(rnd.Rannyu(1,path_size)); // Città di partenza
   int m = floor(rnd.Rannyu(1,path_size)); // Città finale
   while (n == m) {
      m = floor(rnd.Rannyu(1,path_size));
   }
   int l; // Lunghezza del blocco da invertire
   if (m>n) {
      l = m-n+1;
   } else {
      l = path_size-n+m;
   }
   vector<int> copy = path;
   for (int i=0;i<l;i++) {
      path[pbc(n+i)] = copy[pbc(n+l-1-i)]; // Inverto le città nel blocco
   }
};

// Effettua il crossover tra due cammini genitori
void Crossover(vector<int> &path1, vector<int> &path2, Random &rnd) {
   int n = static_cast<int>(floor(rnd.Rannyu(2, (path_size-1))));
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
   for (int i = n; i < path_size; ++i) {
      for (int j = 1; j < path_size; ++j) {
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
   for (int i = n; i < path_size; ++i) {
      for (int j = 1; j < path_size; ++j) {
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
   for (int i=0;i<(n_paths/2);i++) {
      int father_idx = Selection_Operator(rnd); // Seleziono un cammino padre
      int mother_idx = Selection_Operator(rnd); // Seleziono un cammino madre
      while (father_idx == mother_idx) {
         mother_idx = Selection_Operator(rnd); // Rigenero la madre se è uguale al padre
      }
      vector<int>father = paths_even[father_idx]; // Creo una copia del padre
      vector<int>mother = paths_even[mother_idx]; // Creo una copia della madre

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

   for (int i=0;i<(n_paths/2);i++) {
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

// Fa evolvere il sistema per un numero dato di generazioni
void Generation_Evolver(Random &rnd, vector<vector<int>> &paths_even, vector<vector<int>> &paths_odd, vector<vector<double>> positions, int generation, int n_generations) {
   for (int i=0;i<n_generations;i++) { // Ciclo sulle generazioni
      if (generation%2 == 0) {
         Odd_Generation_Generator(rnd, paths_even, paths_odd, positions, generation); // Genero le generazioni dispari
      } else {
         Even_Generation_Generator(rnd, paths_even, paths_odd, positions, generation); // Genero le generazioni pari
      }
      generation++;
   }
   Sorting_Generation(paths_even, positions); // Riordino le generazioni dopo averle aggiornate
   Sorting_Generation(paths_odd, positions);
};

// Riassegna in maniera casuale i primi 10 cammini di ogni continente
void Best_Paths_Exchange(Random &rnd, vector<vector<int>> &paths, int rank, int size) {
   // Inizializzo i migliori cammini
   vector<vector<int>> best_paths(best_to_change);

   // Estraggo i migliori cammini
   for (int i=0;i<best_to_change;i++) {
      best_paths[i] = paths[i];
   }

   // Flatten il vettore dei migliori cammini
   vector<int> best_paths_flat;
   for (const auto& path : best_paths) {
      best_paths_flat.insert(best_paths_flat.end(), path.begin(), path.end());
   }

   // Raccolgo i migliori cammini da tutti i processi
   vector<int> olimpic_games_flat(size*best_to_change*path_size);
   MPI_Gather(best_paths_flat.data(), best_to_change*path_size, MPI_INT,
            olimpic_games_flat.data(), best_to_change*path_size, MPI_INT,
            0, MPI_COMM_WORLD);

   // Solo il processo root (rank 0) mischia i cammini
   if (rank == 0) {
      // Converti il flat vector in un vettore di vettori
      vector<vector<int>> olimpic_games(size*best_to_change);
      for (int i=0;i<size*best_to_change;i++) {
         olimpic_games[i] = vector<int>(olimpic_games_flat.begin() + i*path_size, olimpic_games_flat.begin() + (i+1)*path_size);
      }

      // Mischio casualmente i migliori
      Shuffler(rnd, olimpic_games);

      // Converti di nuovo in un vettore flat
      olimpic_games_flat.clear();
      for (const auto& game : olimpic_games) {
         olimpic_games_flat.insert(olimpic_games_flat.end(), game.begin(), game.end());
      }
   }

   // Distribuisci i cammini mischiati a tutti i processi
   MPI_Bcast(olimpic_games_flat.data(), size*best_to_change*path_size, MPI_INT, 0, MPI_COMM_WORLD);

   // Aggiorna i cammini migliori per ogni processo
   for (int i=0;i<best_to_change;i++) {
      paths[i] = vector<int>(olimpic_games_flat.begin() + i*path_size, olimpic_games_flat.begin() + (i+1)*path_size);
   }
};