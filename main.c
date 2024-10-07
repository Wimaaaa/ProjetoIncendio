#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "functions.h"

// Função principal
int main() {
    pthread_t sensores[TAMANHO][TAMANHO];
    pthread_t gerador_incendio;
    pthread_t central_controle;
    int posicoes[TAMANHO][TAMANHO];

    inicializar_mutexes_e_cond();  // Inicializa os mutexes e variáveis de condição

    // Inicializa a floresta
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j] = '-';
            posicoes[i][j] = i * TAMANHO + j;
        }
    }

    // Cria as threads dos sensores
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_create(&sensores[i][j], NULL, sensor_thread, &posicoes[i][j]);
        }
    }

    // Cria a thread geradora de incêndios
    pthread_create(&gerador_incendio, NULL, gerador_incendio_thread, NULL);

    // Cria a thread central de controle
    pthread_create(&central_controle, NULL, central_controle_thread, NULL);

    // Aguarda as threads dos sensores
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_join(sensores[i][j], NULL);
        }
    }

    // Aguarda a thread geradora de incêndios e a central de controle
    pthread_join(gerador_incendio, NULL);
    pthread_join(central_controle, NULL);

    destruir_mutexes_e_cond();  // Destroi os mutexes e variáveis de condição

    return 0;
}