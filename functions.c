#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

// Definicoes das variaveis globais
char floresta[TAMANHO][TAMANHO];
pthread_mutex_t mutex[TAMANHO][TAMANHO];
pthread_cond_t cond[TAMANHO][TAMANHO];

// Inicializa os mutexes e as variaveis de condicao
void inicializar_mutexes_e_cond() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_mutex_init(&mutex[i][j], NULL);
            pthread_cond_init(&cond[i][j], NULL);
        }
    }
}

// Destroi os mutexes e as variaveis de condicao
void destruir_mutexes_e_cond() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_mutex_destroy(&mutex[i][j]);
            pthread_cond_destroy(&cond[i][j]);
        }
    }
}

// Funcao para combater o fogo
void combater_fogo(int x, int y) {
    pthread_mutex_lock(&mutex[x][y]);

    // Verifica se ha fogo na celula
    if (floresta[x][y] == '@') {
        floresta[x][y] = '/';
        printf("[Combate ao Fogo] Fogo combatido na celula (%d, %d).\n", x, y);
        pthread_cond_signal(&cond[x][y]);
    }

    pthread_mutex_unlock(&mutex[x][y]);
}

// Funcao da thread sensor
void *sensor_thread(void *arg) {
    int pos = *((int *)arg);
    int x = pos / TAMANHO;
    int y = pos % TAMANHO;

    while (1) {
        pthread_mutex_lock(&mutex[x][y]);

        // Marca a celula como monitorada por um no sensor
        if (floresta[x][y] == '-') {
            floresta[x][y] = 'T';
            printf("[Sensor] Sensor ativado na celula (%d, %d).\n", x, y);
        }

        // Verifica se ha incendio
        if (floresta[x][y] == '@') {
            pthread_mutex_unlock(&mutex[x][y]);
            combater_fogo(x,y);
            pthread_mutex_lock(&mutex[x][y]);
        }

        pthread_cond_wait(&cond[x][y], &mutex[x][y]);

        pthread_mutex_unlock(&mutex[x][y]);

        usleep(100000);  // Dorme por 100 milissegundos
    }

    return NULL;
}

// Funcao da thread de geracao de incendios
void *gerador_incendio_thread(void *arg) {
    while (1) {
        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        pthread_mutex_lock(&mutex[x][y]);

        if (floresta[x][y] == '-' || floresta[x][y] == 'T') {
            floresta[x][y] = '@';
            printf("[Incendio] Incendio iniciado na celula (%d, %d).\n", x, y);
            pthread_cond_signal(&cond[x][y]);
        }

        pthread_mutex_unlock(&mutex[x][y]);
        sleep(3);  // Espera 3 segundos antes de iniciar outro incendio
    }

    return NULL;
}

// Funcao da thread de controle central
void *central_controle_thread(void *arg) {
    while (1) {
        printf("[Central de Controle] Estado atual da floresta:\n");
        for (int i = 0; i < TAMANHO; i++) {
            for (int j = 0; j < TAMANHO; j++) {
                pthread_mutex_lock(&mutex[i][j]);
                printf("%c ", floresta[i][j]);
                pthread_mutex_unlock(&mutex[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        sleep(5);  // Espera 5 segundos antes de imprimir o estado novamente
    }
    return NULL;
}