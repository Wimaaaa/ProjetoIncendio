#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define TAMANHO 30

void *sensor_thread(void *arg);
void *gerador_incendio_thread(void *arg);
void *central_controle_thread(void *arg);
void inicializar_mutexes_e_cond();
void destruir_mutexes_e_cond();
void combater_fogo(int x, int y);

extern char floresta[TAMANHO][TAMANHO];
extern pthread_mutex_t mutex[TAMANHO][TAMANHO];
extern pthread_cond_t cond[TAMANHO][TAMANHO];

#endif