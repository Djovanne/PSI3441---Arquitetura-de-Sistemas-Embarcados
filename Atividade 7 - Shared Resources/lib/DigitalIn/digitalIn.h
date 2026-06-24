#ifndef DIGITALIN_H
#define DIGITALIN_H

#include <zephyr/drivers/gpio.h>
#include <stdbool.h>

#define BUTTON_NODE DT_NODELABEL(user_button_0)

typedef void (*botao_callback_t)(void);

bool meu_botao_init(botao_callback_t func);

#endif //DIGITALIN_H