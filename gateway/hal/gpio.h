/*
 * gpio.h
 *
 * GPIO interface.
 */

#ifndef GPIO_H
#define GPIO_H


/**
 * @brief Initialize the GPIO controller and configure all SX126x GPIOs.
 *
 * GPIO23 -> BUSY  (Input)
 * GPIO24 -> DIO1  (Input)
 * GPIO25 -> RESET (Output)
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int gpio_init(void);

/**
 * @brief Reset the SX126x.
 */
void sx126x_hw_reset(void);

/**
 * @brief Read the BUSY pin.
 *
 * @return
 *         1 : BUSY is high
 *         0 : BUSY is low
 *        -1 : Read error
 */
int sx126x_busy(void);

/**
 * @brief Read the DIO1 interrupt (IRQ) pin.
 *
 * @return
 *         1 : DIO1 is high 
 *         0 : DIO1 is low
 *        -1 : Read error
 */
int sx126x_dio1(void);

/**
 * @brief Release all allocated GPIO resources.
 */
void gpio_cleanup(void);

#endif /* GPIO_H */