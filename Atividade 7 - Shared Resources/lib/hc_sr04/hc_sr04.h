////////////////////////////////////////
////////////HC_SR04.h
////////////////////////////////////////

#ifndef HC_SR04_H_
#define HC_SR04_H_

#include <stdbool.h>

void hc_sr04_init(void);

bool hc_sr04_read(float *distancia);

#endif /* HC_SR04_H_ */