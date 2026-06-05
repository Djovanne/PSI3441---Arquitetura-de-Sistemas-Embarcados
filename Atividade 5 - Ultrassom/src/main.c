////////////////////////////////////////
////////////Main.c
////////////////////////////////////////

#include <zephyr.h>             
#include <stdio.h>
#include "hc_sr04.h"

void main(void)
{
    // Inicializa o hardware do sensor em 1 linha
    hc_sr04_init();

    printf("Sensor distancia conectado.\n");

    while (1)
    {
        float distancia_atual;
        
        // Passa o endereço da variável para a biblioteca preencher
        if (hc_sr04_read(&distancia_atual)) 
        {
            int dist_inteira = (int)distancia_atual;
            int dist_decimal = (int)(distancia_atual * 10) % 10;

            printf("Distancia: %d.%d cm\n", dist_inteira, dist_decimal);
        }
        
        k_msleep(100); 
    }
}