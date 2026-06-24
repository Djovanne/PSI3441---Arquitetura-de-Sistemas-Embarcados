#include "digitalIn.h"

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static struct gpio_callback button_cb_data;

static botao_callback_t callback_usuario = NULL;

static void meu_botao_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (callback_usuario != NULL) {
        callback_usuario();
    }
}

bool meu_botao_init(botao_callback_t funcao_usuario)
{
    if (!gpio_is_ready_dt(&button)) {
        return false;
    }

    callback_usuario = funcao_usuario;

    int ret = gpio_pin_configure_dt(&button, GPIO_INPUT | GPIO_PULL_UP);
    if (ret != 0) return false;

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);
    if (ret != 0) return false;

    gpio_init_callback(&button_cb_data, meu_botao_isr, BIT(button.pin));
    ret = gpio_add_callback(button.port, &button_cb_data);
    if (ret != 0) return false;

    return true;
}