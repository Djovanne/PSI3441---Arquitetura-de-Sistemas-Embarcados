#include "acelerometro.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdlib.h> 

static const struct device *const accel_dev = DEVICE_DT_GET(ACCEL_NODE);

bool accel_init(void)
{
    if (!device_is_ready(accel_dev)) {
        return false;
    }
    return true;
}

bool accel_read(accel_data_t *dados)
{
    struct sensor_value raw_x, raw_y, raw_z;

    if (sensor_sample_fetch(accel_dev) != 0) {
        return false; 
    }

    sensor_channel_get(accel_dev, SENSOR_CHAN_ACCEL_X, &raw_x);
    sensor_channel_get(accel_dev, SENSOR_CHAN_ACCEL_Y, &raw_y);
    sensor_channel_get(accel_dev, SENSOR_CHAN_ACCEL_Z, &raw_z);

    dados->x = (float)sensor_value_to_double(&raw_x);
    dados->y = (float)sensor_value_to_double(&raw_y);
    dados->z = (float)sensor_value_to_double(&raw_z);

    return true;
}