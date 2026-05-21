#include <zephyr.h>             // Funções básicas do Zephyr (ex: k_msleep)
#include <device.h>             // API  para obter e usar dispositivos
#include <drivers/gpio.h>       // API para controle de GPIO

#define PORTB_NODE DT_NODELABEL(gpiob)
#define PORTD_NODE DT_NODELABEL(gpiod)

#define LEDR_PIN       18       // Pino PTB18 onde está o LED vermelho
#define LEDG_PIN       19       // Pino PTB18 onde está o LED verde
#define LEDB_PIN       1       // Pino PTB18 onde está o LED azul

#define SLEEP_TIME_MS  500    // Intervalo de piscar (milissegundos)

typedef enum
{
	LED_RED = 0,
	LED_GREEN,
	LED_BLUE,
	PAUSE
} LED_State;

LED_State ledstate = LED_RED;

void main(void)
{
    const struct device *portR = DEVICE_DT_GET(PORTB_NODE);
	const struct device *portG = DEVICE_DT_GET(PORTB_NODE);
	const struct device *portB = DEVICE_DT_GET(PORTD_NODE);

    gpio_pin_configure(portR, LEDR_PIN, GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_LOW);
	gpio_pin_configure(portG, LEDG_PIN, GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_LOW);
	gpio_pin_configure(portB, LEDB_PIN, GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_LOW);
    
    while (1) 
	{
		switch (ledstate)
		{
		case LED_RED:
		{
			gpio_pin_set(portR, LEDR_PIN, 1);
			gpio_pin_set(portG, LEDG_PIN, 0);
			gpio_pin_set(portB, LEDB_PIN, 0);

			k_msleep(SLEEP_TIME_MS);

			ledstate = LED_GREEN;

			break;
		}
		case LED_GREEN:
		{
			gpio_pin_set(portR, LEDR_PIN, 0);
			gpio_pin_set(portG, LEDG_PIN, 1);
			gpio_pin_set(portB, LEDB_PIN, 0);

			k_msleep(SLEEP_TIME_MS/2);

			ledstate = LED_BLUE;

			break;
		}

		case LED_BLUE:
		{
			gpio_pin_set(portR, LEDR_PIN, 0);
			gpio_pin_set(portG, LEDG_PIN, 0);
			gpio_pin_set(portB, LEDB_PIN, 1);

			k_msleep(SLEEP_TIME_MS/4);

			ledstate = PAUSE;

			break;
		}

		case PAUSE:
		{
			gpio_pin_set(portR, LEDR_PIN, 0);
			gpio_pin_set(portG, LEDG_PIN, 0);
			gpio_pin_set(portB, LEDB_PIN, 0);

			k_msleep(SLEEP_TIME_MS*2);

			ledstate = LED_RED;

			break;
		}
		
		default:
			break;
		}
    }
}