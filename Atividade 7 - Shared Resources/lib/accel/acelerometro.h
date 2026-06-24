#ifndef ACELEROMETRO_H
#define ACELEROMETRO_H

#include <stdbool.h>

#define ACCEL_NODE DT_NODELABEL(mma8451q)

typedef struct {
    float x;
    float y;
    float z;
} accel_data_t;

bool accel_init(void);
bool accel_read(accel_data_t *dados);

#endif // ACELEROMETRO_H