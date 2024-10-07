#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

// Definições das variáveis globais
char floresta[TAMANHO][TAMANHO];
pthread_mutex_t mutex[TAMANHO][TAMANHO];
pthread_cond_t cond[TAMANHO][TAMANHO];

// Inicializa os mutexes e as variáveis de condição
void inicializar_mutexes_e_cond() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_mutex_init(&mutex[i][j], NULL);
            pthread_cond_init(&cond[i][j], NULL);
        }
    }
}

// Destroi os mutexes e as variáveis de condição
void destruir_mutexes_e_cond() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_mutex_destroy(&mutex[i][j]);
            pthread_cond_destroy(&cond[i][j]);
        }
    }
}

// Função para combater o fogo, bloqueando o mutex temporariamente
void combater_fogo(int x, int y) {
    printf("[combater_fogo] Tentando combater o fogo no no (%d, %d)\n", x, y);
    pthread_mutex_lock(&mutex[x][y]);  // Bloqueia o mutex
    printf("[combater_fogo] Mutex bloqueado com sucesso no no (%d, %d)\n", x, y);
    printf("[combater_fogo] Estado da celula (%d, %d) antes do combate: %c\n", x, y, floresta[x][y]);

    // Verifica se há fogo na célula
    if (floresta[x][y] == '@') {
        floresta[x][y] = '/';  // Combate o fogo
        printf("[combater_fogo] Fogo combatido no no (%d, %d). Estado agora: %c\n", x, y, floresta[x][y]);

        pthread_cond_signal(&cond[x][y]);  // Sinaliza que o fogo foi combatido
        printf("[combater_fogo] Condicao sinalizada com sucesso no no (%d, %d)\n", x, y);
    } else {
        printf("[combater_fogo] Nenhum incendio ativo encontrado no no (%d, %d). Estado atual: %c\n", x, y, floresta[x][y]);
    }

    pthread_mutex_unlock(&mutex[x][y]);  // Desbloqueia o mutex
    printf("[combater_fogo] Mutex desbloqueado com sucesso no no (%d, %d)\n", x, y);
}

// Função da thread sensor
void *sensor_thread(void *arg) {
    int pos = *((int *)arg);
    int x = pos / TAMANHO;
    int y = pos % TAMANHO;

    while (1) {
        printf("[sensor_thread] Tentando bloquear o mutex para o no sensor (%d, %d)\n", x, y);
        pthread_mutex_lock(&mutex[x][y]);  // Bloqueia o mutex
        printf("[sensor_thread] Mutex bloqueado com sucesso para o no sensor (%d, %d)\n", x, y);

        printf("[sensor_thread] No sensor (%d, %d) verificando estado: %c\n", x, y, floresta[x][y]);

        // Verifica se há incêndio
        if (floresta[x][y] == '@') {
            printf("[sensor_thread] Incendio detectado no no (%d, %d). Desbloqueando mutex e chamando combater_fogo.\n", x, y);
            pthread_mutex_unlock(&mutex[x][y]);  // Desbloqueia o mutex antes de chamar combater_fogo
            combater_fogo(x, y);
            pthread_mutex_lock(&mutex[x][y]);  // Bloqueia novamente após chamar combater_fogo
        } else {
            printf("[sensor_thread] Nenhum incendio detectado no no (%d, %d)\n", x, y);
        }

        printf("[sensor_thread] Sensor esperando a condicao no no (%d, %d)\n", x, y);
        pthread_cond_wait(&cond[x][y], &mutex[x][y]);  // Espera a condição de combate ao fogo
        printf("[sensor_thread] Condicao de espera retornada com sucesso no no sensor (%d, %d)\n", x, y);

        pthread_mutex_unlock(&mutex[x][y]);  // Desbloqueia o mutex
        printf("[sensor_thread] Mutex desbloqueado com sucesso para o no sensor (%d, %d)\n", x, y);

        usleep(100000);  // Dorme por 100 milissegundos
    }

    return NULL;
}

// Função da thread de geração de incêndios
void *gerador_incendio_thread(void *arg) {
    while (1) {
        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        pthread_mutex_lock(&mutex[x][y]);  // Bloqueia o mutex
        if (floresta[x][y] == '-') {
            floresta[x][y] = '@';  // Inicia o incêndio
            printf("[gerador_incendio_thread] Incendio iniciado na posicao (%d, %d)\n", x, y);
            pthread_cond_signal(&cond[x][y]);  // Sinaliza que um incêndio foi iniciado
        }
        pthread_mutex_unlock(&mutex[x][y]);  // Desbloqueia o mutex
        sleep(3);  // Espera 3 segundos antes de iniciar outro incêndio
    }

    return NULL;
}

// Função da thread de controle central
void *central_controle_thread(void *arg) {
    while (1) {
        printf("[central_controle_thread] Estado atual da floresta:\n");
        for (int i = 0; i < TAMANHO; i++) {
            for (int j = 0; j < TAMANHO; j++) {
                pthread_mutex_lock(&mutex[i][j]);  // Bloqueia o mutex para leitura do estado
                printf("%c ", floresta[i][j]);  // Imprime o estado atual da célula
                pthread_mutex_unlock(&mutex[i][j]);  // Desbloqueia o mutex
            }
            printf("\n");
        }
        printf("\n");
        sleep(5);  // Espera 5 segundos antes de imprimir o estado novamente
    }
    return NULL;
}