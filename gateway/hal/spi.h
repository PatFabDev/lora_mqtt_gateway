/*
 * spi.h
 *
 * SPI interface.
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>


/**
 * @brief Initialize the SPI interface.
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int spi_init(void);

/**
 * @brief Transfer data over the SPI bus.
 *
 * @param tx
 *        Pointer to the transmit buffer.
 *
 * @param rx
 *        Pointer to the receive buffer.
 *
 * @param len
 *        Number of bytes to transfer.
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int spi_transfer(const uint8_t *tx, uint8_t *rx, uint16_t len);

/**
 * @brief Release all allocated SPI resources.
 */
void spi_cleanup(void);

#endif /* SPI_H */