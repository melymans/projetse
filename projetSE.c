# #include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define N 100 

// Variables globales
int** B;
int** C;
int** A;
int* T;
int n1, m1, n2, m2;
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// Producteur
void producer(void* arg)
{
  int row = (int)arg;
  for (int j = 0; j < m2; j++)
  {
    int sum = 0;
    for (int k = 0; k < n2; k++)
    {
      sum += B[row][k] * C[k][j];
    }
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    T[row * m2 + j] = sum;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
}

// Consommateur
void consumer(void* arg)
{
  int col = (int)arg;
  for (int i = 0; i < n1; i++)
  {
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    A[i][col] = T[i * m2 + col];
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
}

// Fonction pour initialiser les valeurs de la matrice avec des nombres aléatoires entre 0 et 10
void initMatrix(int** matrix, int rows, int cols)
{
  srand(time(NULL));
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      matrix[i][j] = rand() % 11;
    }
  }
}

int main()
{
  // Initialisation des dimensions des matrices
  n1 = 3;
  m1 = 2;
  n2 = 2;
  m2 = 4;

  // Allocation de mémoire pour les matrices et le tampon
  B = (int**)malloc(n1 * sizeof(int*));
  for (int i = 0; i < n1; i++)
  {
    B[i] = (int*)malloc(m1 * sizeof(int));
  }
  C = (int**)malloc(n2 * sizeof(int*));
  for (int i = 0; i < n2; i++)
  {
    C[i] = (int*)malloc(m2 * sizeof(int));
  }
  A = (int**)malloc(n1 * sizeof(int*));
  for (int i = 0; i < n1; i++)
  {
    A[i] = (int*)malloc(m2 * sizeof(int));
  }
  T = (int*)malloc(n1 * m2 * sizeof(int)); // Correction de la taille du tampon

  // Initialisation des matrices B et C avec des valeurs aléatoires entre 0 et 10
  initMatrix(B, n1, m1);
  initMatrix(C, n2, m2);

  // Initialisation des sémaphores et du mutex
  sem_init(&empty, 0, N);
  sem_init(&full, 0, 0);
  pthread_mutex_init(&mutex, NULL);

  // Création des threads producteurs
  pthread_t producers[n1];
  for (int i = 0; i < n1; i++)
  {
    pthread_create(&producers[i], NULL, producer, (void*)i);
  }

  // Création des threads consommateurs
  pthread_t consumers[m2];
  for (int i = 0; i < m2; i++)
  {
    pthread_create(&consumers[i], NULL, consumer, (void*)i);
  }

  // Attente des threads producteurs
  for (int i = 0; i < n1; i++)
  {
    pthread_join(producers[i], NULL);
  }

  // Attente des threads consommateurs
  for (int i = 0; i < m2; i++)
  {
    pthread_join(consumers[i], NULL);
  }

  // Affichage de la matrice résultante A
  for (int i = 0; i < n1; i++)
  {
    for (int j = 0; j < m2; j++)
    {
      printf("%d ", A[i][j]);
    }
    printf("\n");
  }

  // Libération de la mémoire allouée
  for (int i = 0; i < n1; i++)
  {
    free(B[i]);
  }
  free(B);
  for (int i = 0; i < n2; i++)
  {
    free(C[i]);
  }
  free(C);
  for (int i = 0; i < n1; i++)
  {
    free(A[i]);
  }
  free(A);
  free(T);

  // Destruction des sémaphores et du mutex
  sem_destroy(&empty);
  sem_destroy(&full);
  pthread_mutex_destroy(&mutex);

  return 0;
}
