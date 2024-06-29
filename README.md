# Laboratorio di Simulazione Numerica

### Davide Riva

Questa repository contiene le esercitazioni riguardanti il corso di Simulazione Numerica tenuto dal prof. Davide Emilio Galli (A.A. 2023-2024).

### Struttura della repository

Ogni directory contiene un notebook con la consegna dell'esercitazione, i risultati e i plot ottenuti e brevi spiegazioni o commenti a riguardo del codice utilizzato o dei risultati ottenuti.
Ci sono inoltre delle sotto-directory, una per ogni punto dell'esercizio, contenenti le seguenti directory e file:

1. main.cpp: contiene il codice in c++ che svolge l'esercizio
2. makefile: contiene le istruzione per compilare
3. INPUT: directory contenente il file input.dat laddove è necessario passare delle variabili di input al codice. Nel main.cpp è specificato cosa sono le varie variabili lette da input.dat
4. OUTPUT: directory che raccoglie tutti i file di output utilizzati nel notebook
5. file oggetto, eseguibili e seed.out creati durante la compilazione

Ogni esercizio è compilabile con il comando `make` ed eseguibile con il comando `./main.exe`. Fanno eccezione le esercitazioni 4, 6 e 7 nelle quali è stata utilizzato il codice fornito dal professore nella directory `NSL_SIMULATOR`: in questi tre casi la compilazione va eseguita all'interno della sotto-directory `SOURCE` e l'eseguibile si chiama `simulator.exe`.
Anche l'esercizio 10.2 fa eccezione, in questo caso trattandosi di un esercizio riguardanta la parallelizzazione la compilazione e l'esecuzione sono fatte tramite i seguenti due comandi:

1. `mpicxx -I/home/davide/Simulazione_Numerica/Utilities main.cpp /home/davide/Simulazione_Numerica/Utilities/random.cpp`

2. `mpiexec -np 16 ./a.out`

dove 16 è il numero di core utilizzati dal codice.