////////////////////////////////////////
////////////Main.c
////////////////////////////////////////

#include <zephyr/kernel.h>     

#define MUTEX // habilita o mutex
#define SEMAFORO // habilita o semaforo

// Tamanho de memória para cada thread
#define STACK_SIZE 1024

// prioridade da thread
#define PADEIRO_THREAD_PRIORITY 7
#define CLIENTE_THREAD_PRIORITY 5

// para alternar os modos do botao
static volatile int saldo_vitrine = 0;

#ifdef SEMAFORO
// DECLARAÇÃO DOS SEMÁFOROS
#define CAPACIDADE_VITRINE 10
//(nome, valor_inicial, valor_maximo)
K_SEM_DEFINE(sem_paes, 0, CAPACIDADE_VITRINE);
K_SEM_DEFINE(sem_vagas, CAPACIDADE_VITRINE, CAPACIDADE_VITRINE);
#endif

#ifdef MUTEX
// declaracao mutex
K_MUTEX_DEFINE(vitrine_mutex);
#endif

/////////////////////////////////// thread padeiro////////////////////////////////////////
void thread_padeiro_fn(void *arg1, void *arg2, void *arg3)
{
    for(;;) 
    {
        #ifdef SEMAFORO
        // se estiver cheio, task dorme para sempre
        k_sem_take(&sem_vagas, K_FOREVER);
        #endif

        #ifdef MUTEX
        // mutex trava o uso da variavel
        k_mutex_lock(&vitrine_mutex, K_FOREVER);
        #endif

        saldo_vitrine++;
        
        printk("[PADEIRO] Produziu 1 pao. Saldo atual da vitrine: %d\n", saldo_vitrine);
        
        #ifdef MUTEX
        k_mutex_unlock(&vitrine_mutex);
        #endif

        #ifdef SEMAFORO
        // adiciona um no semaforo
        k_sem_give(&sem_paes);
        #endif

        k_msleep(1000); // Aguarda 1 segundo
    }
}

////////////////////////////// thread cliente///////////////////////////////////
void thread_cliente_fn(void *arg1, void *arg2, void *arg3)
{
    for(;;) 
    {
        #ifdef SEMAFORO
        // semaforo de se vitrine vazia, task dorme
        k_sem_take(&sem_paes, K_FOREVER);
        #endif

        #ifdef MUTEX
        // mutex para uso da variavel
        k_mutex_lock(&vitrine_mutex, K_FOREVER);
        #endif

        saldo_vitrine--;
        
        printk("[CLIENTE] Retirou 1 pao. Saldo atual da vitrine: %d\n", saldo_vitrine);

        #ifdef MUTEX
        k_mutex_unlock(&vitrine_mutex);
        #endif

        #ifdef SEMAFORO
        // libera 1 vaga para a vitrine
        k_sem_give(&sem_vagas);
        #endif
        
        k_msleep(1500); // Aguarda 1.5 segundos
    }
}

//K_THREAD_DEFINE(
//    nome_da_thread,      // Um nome interno para a thread
//    tamanho_da_pilha,    // Memória reservada (geralmente 1024 bytes)
//    funcao_da_thread,    // O nome da função que tem o loop da thread
//    arg1, arg2, arg3,    // Argumentos (geralmente NULL)
//    prioridade,          // Número da prioridade (menor = mais importante)
//    opcoes,              // 0 por padrão
//    delay_inicial        // 0 para iniciar na hora
//);

// declaratacao thread adc
K_THREAD_DEFINE(
    padeiro_tid, 
    STACK_SIZE, 
    thread_padeiro_fn, 
    NULL, 
    NULL, 
    NULL, 
    PADEIRO_THREAD_PRIORITY, 
    0, 
    0
);

// declaratacao thread accel
K_THREAD_DEFINE(
    cliente_tid, 
    STACK_SIZE, 
    thread_cliente_fn,
    NULL, 
    NULL, 
    NULL, 
    CLIENTE_THREAD_PRIORITY, 
    0, 
    0
);

int main(void)
{
printk("==================================================\n");
    printk(" PSI3441 - Atividade 7 - Parte 3 (Com Semaforos)\n");
    printk("==================================================\n\n");

    // O main entra em repouso e deixa as threads rodando
    while (1) {
        k_sleep(K_FOREVER);
    }
    return 0;
}