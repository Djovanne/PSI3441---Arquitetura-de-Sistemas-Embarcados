#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include "acelerometro.h"
#include "nlms_filter.h"

/* * Regista o módulo de log para este ficheiro.
 * Parâmetro 1: Nome do módulo (aparecerá no monitor serial).
 * Parâmetro 2: Nível de log a ser compilado (LOG_LEVEL_DBG permite todos os níveis).
 */
LOG_MODULE_REGISTER(accelerometer_log, LOG_LEVEL_DBG);

// Tamanho da Stack (pilha) das threads
#define STACK_SIZE 0x800 // 4096

// Prioridade das mesmas
#define SERIAL_THREAD_PRIORITY 4
#define ACCEL_THREAD_PRIORITY 5

// Parametros do filtro adaptativo
#define BUFFER_SIZE  20
#define DATA_TYPTE    float
#define LEARNING_RATE 0.3f
#define REGULATION    1e-4f

#define ALE_DELAY 5

// Alterna modo do filtro
#define ENABLE_FILTER false

// Frequencia de amostragem

#define AMOS_FREQ 1000

#define PERIOD_AMOS (uint32_t)1000/AMOS_FREQ

// Estrutura dos dados estraidos do acelerometro
// Primeiro dado do arrey 


//K_THREAD_DEFINE(
//    nome_da_thread,      // Um nome interno para a thread
//    tamanho_da_pilha,    // Memória reservada (geralmente 1024 bytes)
//    funcao_da_thread,    // O nome da função que tem o loop da thread
//    arg1, arg2, arg3,    // Argumentos (geralmente NULL)
//    prioridade,          // Número da prioridade (menor = mais importante)
//    opcoes,              // 0 por padrão
//    delay_inicial        // 0 para iniciar na hora
//);

////////////////////////////////// codigo acelerometro ///////////////////////////////////



// declaracao struct de dados
typedef struct 
{
    uint32_t timestamp;       // tempo da amostra
    float raw_sensor[3];      // dado bruto (x,y,z)  
    float filt_sensor[3];     // dado filtrado (x,y,z)
}packet_t;

// Fila para envio de mensagens (Maximo de 10 pacotes)
K_MSGQ_DEFINE(data_msgq, sizeof(packet_t), 50, 4);

//static NLMSFilter<BUFFER_SIZE, DATA_TYPTE> xFilter(LEARNING_RATE, REGULATION);
//static NLMSFilter<BUFFER_SIZE, DATA_TYPTE> yFilter(LEARNING_RATE, REGULATION);
//static NLMSFilter<BUFFER_SIZE, DATA_TYPTE> zFilter(LEARNING_RATE, REGULATION);

static nlms_type_t x_weights[BUFFER_SIZE];
static nlms_type_t x_delay_line[BUFFER_SIZE];
static NLMSFilter xFilter;

static nlms_type_t y_weights[BUFFER_SIZE];
static nlms_type_t y_delay_line[BUFFER_SIZE];
static NLMSFilter  yFilter;

static nlms_type_t z_weights[BUFFER_SIZE];
static nlms_type_t z_delay_line[BUFFER_SIZE];
static NLMSFilter  zFilter;

void thread_accel_fn(void *arg1, void *arg2, void *arg3)
{
    packet_t packet;       // pacote a ser transmitido
    accel_data_t raw_data; // pacote do acelerometro
    uint32_t perdas = 0;

    float delay_linex[ALE_DELAY] = {0,0,0,0,0};
    float delay_liney[ALE_DELAY] = {0,0,0,0,0};
    float delay_linez[ALE_DELAY] = {0,0,0,0,0};

    size_t ale_index = 0;

    // Instancia os filtros

    NLMSFilter_Init(&xFilter, BUFFER_SIZE, x_weights, x_delay_line, LEARNING_RATE, REGULATION);
    NLMSFilter_Init(&yFilter, BUFFER_SIZE, y_weights, y_delay_line, LEARNING_RATE, REGULATION);
    NLMSFilter_Init(&zFilter, BUFFER_SIZE, z_weights, z_delay_line, LEARNING_RATE, REGULATION);

    if (!accel_init()) 
    {
        LOG_ERR("Falha ao inicializar o acelerômetro!");
        return;
    }

    LOG_INF("Thread de aquisição iniciada.");

    for(;;)
    {
        if (accel_read(&raw_data)) 
        {
            packet.timestamp = k_uptime_get_32(); // Timestamp em ms
            packet.raw_sensor[0] = raw_data.x;
            packet.raw_sensor[1] = raw_data.y;
            packet.raw_sensor[2] = raw_data.z;
            //printk("[ACCEL] X: %.2f | Y: %.2f | Z: %.2f\n", (double)raw_data.x, (double)raw_data.y, (double)raw_data.z);

            // Aplica o filtro (ou não)
            if (ENABLE_FILTER)
            {
                float delay_datax = delay_linex[ale_index];
                float delay_datay = delay_liney[ale_index];
                float delay_dataz = delay_linez[ale_index];

                delay_linex[ale_index] = raw_data.x;
                delay_liney[ale_index] = raw_data.y;
                delay_linez[ale_index] = raw_data.z;

                ale_index = (ale_index + 1) % ALE_DELAY;

                packet.filt_sensor[0] = NLMSFilter_Update(&xFilter, delay_datax, raw_data.x);
                packet.filt_sensor[1] = NLMSFilter_Update(&yFilter, delay_datay, raw_data.y);
                packet.filt_sensor[2] = NLMSFilter_Update(&zFilter, delay_dataz, raw_data.z);
            }
            else
            {
                packet.filt_sensor[0] = 0.0f;
                packet.filt_sensor[1] = 0.0f;
                packet.filt_sensor[2] = 0.0f;
            }

            // Envia mensagem para a fila, se cheia, descarta
            bool ret = (bool)k_msgq_put(&data_msgq, &packet, K_NO_WAIT);
            if (ret)
            {
                perdas++;
                LOG_WRN("Fila de mensagens cheia numero: %d", perdas);
            }
        }
        else LOG_ERR("[ACCEL] Falha na leitura!");

        k_msleep(PERIOD_AMOS);
    }
    


    return;
}

////////////////////////////////// codigo Comunicação ///////////////////////////////////

void thread_serial_fn(void *arg1, void *arg2, void *arg3)
{
    packet_t received_packet;
    LOG_INF("Thread de comunicação iniciada.");

    for(;;)
    {
        // Aguarda chegar algum dado na fila
        if (k_msgq_get(&data_msgq, &received_packet, K_FOREVER) == 0)
        {
            // Envia em formato CSV
            printk("%u;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f\n", 
                   received_packet.timestamp, 
                   (double)received_packet.raw_sensor[0], 
                   (double)received_packet.raw_sensor[1],
                   (double)received_packet.raw_sensor[2],
                   (double)received_packet.filt_sensor[0],
                   (double)received_packet.filt_sensor[1], 
                   (double)received_packet.filt_sensor[2]
                );
                //k_msleep(1);
        }

    }
    return;
}

// declaratacao thread serial
K_THREAD_DEFINE(
    serial_tid, 
    STACK_SIZE, 
    thread_serial_fn,
    NULL, 
    NULL, 
    NULL, 
    SERIAL_THREAD_PRIORITY, 
    0, 
    0
);

// declaratacao thread sensor)
K_THREAD_DEFINE(
    sensor__tid, 
    STACK_SIZE, 
    thread_accel_fn, 
    NULL, 
    NULL, 
    NULL, 
    ACCEL_THREAD_PRIORITY, 
    0, 
    0
);

int main(void)
{
    for(;;) k_sleep(K_FOREVER);
}