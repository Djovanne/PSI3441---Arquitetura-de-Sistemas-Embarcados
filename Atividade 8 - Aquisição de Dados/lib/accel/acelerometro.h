#ifndef ACELEROMETRO_H
#define ACELEROMETRO_H

#include <stdbool.h>

#define ACCEL_NODE DT_NODELABEL(mma8451q)

#define MMA8451Q_I2C_ADDR     0x1D 
#define MMA8451Q_CTRL_REG1    0x2A
#define MMA8451Q_ODR          (0x0 << 3)
#define MMA8451Q_ACTIVE_BIT   0x01

typedef struct {
    float x;
    float y;
    float z;
} accel_data_t;

// Para nao dar conflito com o compilador C++
#ifdef __cplusplus
extern "C" {
#endif

bool accel_init(void);
bool accel_read(accel_data_t *dados);
bool accel_configurar_odr(void);

#ifdef __cplusplus
}
#endif

#endif // ACELEROMETRO_H