/*
 * gpio-mutex.h
 *
 *  Created on: Nov. 9, 2019
 *      Author: daniel
 */

#ifndef GPIO_MUTEX_H_
#define GPIO_MUTEX_H_

#include "shared_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

void i2c0Lock();
void i2c0Unlock();
void spi0Lock();
void spi0Unlock();

#ifdef __cplusplus
}
#endif

#endif /* GPIO_MUTEX_H_ */
