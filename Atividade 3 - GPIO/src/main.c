#include <zephyr.h>

/* Define o endereço do registrador SIM_SCGC5 */
#define SIM_SCGC5 (*((volatile unsigned int*)0x40048038))

//PTB19 - Pino verde
/* Multiplexador do pino*/
#define PORTB_PCR19  (*((volatile unsigned int*)0x4004A04C))
// Define se é Input ou Output (direção)
#define GPIOB_PDDR   (*((volatile unsigned int*)0x400FF054))
// Registrador Toggle
#define GPIOB_PTOR   (*((volatile unsigned int*)0x400FF04C))
// Nivel lógico Alto
#define GPIOB_PSOR   (*((volatile unsigned int*)0x400FF044))
// Nivel lógico Baixo
#define GPIOB_PCOR   (*((volatile unsigned int*)0x400FF048))


// Função Delay
void delayMs(int n){
    for (int i = 0; i < n; i++) for (int j = 0; j < 7000; j++) __asm__ volatile ("nop");
}

void main(void)
{
    // Ativa Clock do Barramento
    SIM_SCGC5 |= (uint32_t)(0b1 << 10); 
    // Limpa e Configura Tipo de saída do pino (GPIO)
    PORTB_PCR19 &= ~(uint32_t)(0b111 << 8); 
    PORTB_PCR19 |= (uint32_t)(0b001 << 8); 
    // Direção de Saída do Pino
    GPIOB_PDDR |= (uint32_t)(0b1 << 19);
    // Nivel Inicial Alto (por ser anodo comum)
    GPIOB_PSOR |= (uint32_t)(0b1 << 19);
    
    while(1)
    {
        delayMs(2000);
        GPIOB_PTOR = (uint32_t)(0b1 << 19);
    }
    return;
}
