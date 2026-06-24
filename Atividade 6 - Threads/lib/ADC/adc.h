#ifndef ADC_H
#define ADC_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

#include <stdbool.h>

//ADC
#define ADC_RESOLUTION      12
#define ADC_GAIN            ADC_GAIN_1
#define ADC_REFERENCE       ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT
#define ADC_CHANNEL_ID      0  //Canal do ADC, veja a pinagem
#define ADC_VREF_MV         3300

bool adc_init();
uint16_t adc_read_raw();
uint32_t adc_read_mv();

#endif