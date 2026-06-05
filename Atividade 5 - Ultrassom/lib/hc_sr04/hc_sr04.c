////////////////////////////////////////
////////////HC_SR04.c
////////////////////////////////////////

#include <zephyr.h>
#include <drivers/gpio.h>
#include "pwm_z402.h"
#include "hc_sr04.h"

#define TPM_IRQ_LINE TPM1_IRQn  
#define TPM_IRQ_PRIORITY 1      
#define PULSE_WIDTH_TICKS 4

// variáveis privadas 
static volatile uint16_t capture1 = 0; 
static volatile uint16_t capture2 = 0; 
static volatile uint16_t pulse_width = 0; 
static volatile uint8_t new_data = 0;
static volatile uint8_t esperando_descida = 0;

// A ISR privada
static void tpm1_isr(const void *arg)
{
    TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
    TPM1->STATUS = TPM_STATUS_CH0F_MASK; // zerra a flag que gerou a interrupção

    uint16_t current_capture = TPM1->CONTROLS[0].CnV;
    // Lê fisicamente o bit 20 da porta E (PTE20)
    uint8_t pino_alto = (GPIOE->PDIR & (1 << 20)) ? 1 : 0;

    // borda de Subida (Pino está em 3.3V)
    if (pino_alto) 
    {
        capture1 = current_capture;
        esperando_descida = 1;
    }
    // borda Descida (Pino está em 0V)
    else 
    {
        if (esperando_descida == 1) 
        {
            capture2 = current_capture;
            pulse_width = (capture2 >= capture1) ? (capture2 - capture1) : ((65535 - capture1) + capture2 + 1);
            new_data = 1;
            esperando_descida = 0;
        }
    }
}

// FUNÇÕES PÚBLICAS

void hc_sr04_init(void)
{
    // Conecta a interrupção via Zephyr
    IRQ_CONNECT(TPM_IRQ_LINE, TPM_IRQ_PRIORITY, tpm1_isr, NULL, 0);
    irq_enable(TPM_IRQ_LINE);
 
    // Inicializa TPM1 com módulo e prescaler
    pwm_tpm_Init(TPM1, TPM_PLLFLL, 65535, TPM_CLK, PS_128, EDGE_PWM);

    // Configura TPM1_CH1 como PWM_H
    pwm_tpm_Ch_Init(TPM1, 1, TPM_PWM_H, GPIOE, 21);
    TPM1->CONTROLS[1].CnV = PULSE_WIDTH_TICKS;

    esperando_descida = 0;
    // Configura TPM1_CH0 como input capture em AMBAS as bordas
    pwm_tpm_Ch_Init(TPM1, 0, TPM_INPUT_CAPTURE_BOTH | TPM_CHANNEL_INTERRUPT, GPIOE, 20);
}

bool hc_sr04_read(float *distancia)
{
    if (new_data) 
    {
        new_data = 0;
        *distancia = pulse_width * 0.0458f;
        return true;
    }
    return false;
}