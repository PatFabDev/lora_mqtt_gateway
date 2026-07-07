/*
 * sx126x_hal_linux.h
 *
 * Linux HAL interface for the SX126x driver.
 */

#ifndef SX126X_HAL_LINUX_H
#define SX126X_HAL_LINUX_H

#include "../sx126x_driver/src/sx126x_hal.h"
#include "gpio.h"
#include "spi.h"

/**
 * @brief Initialize the Linux hardware abstraction layer.
 *
 * Initializes the GPIO and SPI interfaces required by the SX126x driver.
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int sx126x_init(void);

/**
 * @brief Release all hardware resources used by the Linux HAL.
 */
void sx126x_deinit(void);

#endif /* SX126X_HAL_LINUX_H */