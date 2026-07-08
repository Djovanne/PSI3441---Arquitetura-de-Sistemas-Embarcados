#include "acelerometro.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <stdlib.h> 

static const struct device *const accel_dev = DEVICE_DT_GET(ACCEL_NODE);
static const struct device *const i2c_dev = DEVICE_DT_GET(DT_BUS(ACCEL_NODE));

bool accel_init(void)
{
    if (!device_is_ready(accel_dev)) {
        return false;
    }

    if (!accel_configurar_odr())
    {
        printk("Aviso: Falha ao configurar ODR padrão.\n");
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

bool accel_configurar_odr(void)
{
    uint8_t buf[2];
    int ret;

    // 1️⃣ Colocar o sensor em standby (necessário antes de mudar ODR)
    buf[0] = MMA8451Q_CTRL_REG1;
    buf[1] = 0x00;
    ret = i2c_write(i2c_dev, buf, 2, MMA8451Q_I2C_ADDR);
    if (ret) {
        printk("ERRO ao colocar MMA8451Q em standby (%d)\n", ret);
        return false;
    }

    // 2️⃣ Configurar ODR = 100 Hz (bits DR[5:3] = 100)
    buf[0] = MMA8451Q_CTRL_REG1;
    buf[1] = MMA8451Q_ODR;
    ret = i2c_write(i2c_dev, buf, 2, MMA8451Q_I2C_ADDR);
    if (ret) {
        printk("ERRO ao configurar ODR (%d)\n", ret);
        return false;
    }

    // 3️⃣ Ativar o sensor novamente
    buf[0] = MMA8451Q_CTRL_REG1;
    buf[1] = MMA8451Q_ODR | MMA8451Q_ACTIVE_BIT;
    ret = i2c_write(i2c_dev, buf, 2, MMA8451Q_I2C_ADDR);
    if (ret) {
        printk("ERRO ao ativar MMA8451Q (%d)\n", ret);
        return false;
    }

    printk("MMA8451Q configurado para 100 Hz via I2C.\n");
    return true;
}
