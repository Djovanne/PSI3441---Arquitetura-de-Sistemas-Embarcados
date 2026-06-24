#include "adc.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

static int16_t sample_buffer;
static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc0));

static struct adc_sequence sequence = {
    .channels    = BIT(ADC_CHANNEL_ID),
    .buffer      = &sample_buffer,
    .buffer_size = sizeof(sample_buffer),
    .resolution  = ADC_RESOLUTION,
};

bool adc_init()
{
    if (!device_is_ready(adc_dev)) {
        return false;
    }

    struct adc_channel_cfg channel_cfg = {
        .gain = ADC_GAIN,
        .reference = ADC_REFERENCE,
        .acquisition_time = ADC_ACQUISITION_TIME,
        .channel_id = ADC_CHANNEL_ID,
        .differential = 0,
    };

        if (adc_channel_setup(adc_dev, &channel_cfg) != 0) {
        printk("Erro ao configurar canal ADC\n");
        return false;
    }

    return true;
}

uint16_t adc_read_raw()
{
    int err = adc_read(adc_dev, &sequence);

    if (err != 0)
    {
        printk("Falha na leitura do ADC: %d\n", err);
        return -1;
    }

    return sample_buffer;
}

uint32_t adc_read_mv()
{
    int32_t mv = (int32_t)adc_read_raw();

    if (mv == -1) return -1;

    adc_raw_to_millivolts(ADC_VREF_MV, ADC_GAIN, ADC_RESOLUTION, &mv);
    return mv;
}