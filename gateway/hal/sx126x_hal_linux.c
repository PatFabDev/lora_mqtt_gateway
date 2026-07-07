/*
 * sx126x_hal_linux.c
 *
 * Linux HAL implementation for the SX126x driver.
 */

#include "sx126x_hal_linux.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>

/* Delay between BUSY pin polls */
#define SX126X_BUSY_POLL_DELAY_US   100U

/* Maximum SPI transfer size */
#define SX126X_HAL_BUFFER_SIZE      256U

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS -------------------------------------------------------
 */

static int wait_busy(void)
{
    int busy;

    do
    {
        busy = sx126x_busy();

        if (busy < 0)
        {
            return -1;
        }

        if (busy == 1)
        {
            usleep(SX126X_BUSY_POLL_DELAY_US);
        }

    } while (busy);

    return 0;
}

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS --------------------------------------------------------
 */

int sx126x_init(void)
{
    if (gpio_init() < 0)
    {
        return -1;
    }

    if (spi_init() < 0)
    {
        gpio_cleanup();
        return -1;
    }

    sx126x_hw_reset();

    return 0;
}

void sx126x_deinit(void)
{
    spi_cleanup();
    gpio_cleanup();
}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )
{
    (void)context;

    int status;
    uint16_t len;
    uint8_t tx[SX126X_HAL_BUFFER_SIZE] = {0};
    uint8_t rx[SX126X_HAL_BUFFER_SIZE] = {0};

    /* Validate input parameters */
    if ((command == NULL) || ((data == NULL) && (data_length > 0)))
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Check transfer length */
    len = command_length + data_length;

    if (len > sizeof(tx))
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Wait until the radio is ready */
    status = wait_busy();

    if (status < 0)
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Copy command bytes */
    memcpy(tx, command, command_length);

    /* Append payload data */
    if (data_length > 0)
    {
        memcpy(&tx[command_length], data, data_length);
    }

    /* Execute SPI transfer */
    status = spi_transfer(tx, rx, len);

    if (status < 0)
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length )
{
    (void) context;

    int status;
    uint16_t len;
    uint8_t tx[SX126X_HAL_BUFFER_SIZE] = { 0 };
    uint8_t rx[SX126X_HAL_BUFFER_SIZE] = { 0 };

    /* Validate input parameters */
    if((command == NULL) || ((data == NULL) && (data_length > 0)))
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Check transfer length */
    len = command_length + data_length;

    if (len > sizeof(tx))
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Wait until the radio is ready */
    status = wait_busy();

    if (status < 0)
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Copy command bytes */
    memcpy(tx, command, command_length);

    /* Fill remaining bytes with NOP to clock out the response */
    if (data_length > 0)
    {
        memset(&tx[command_length], SX126X_NOP, data_length);
    }

    /* Execute SPI transfer */
    status = spi_transfer(tx, rx, len);

    if (status < 0)
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Wait until the radio has completed the command */
    status = wait_busy();

    if (status < 0)
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    /* Copy received data */
    if (data_length > 0)
    {
        memcpy(data, &rx[command_length], data_length);
    }

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset( const void* context )
{
    (void)context;

    sx126x_hw_reset();

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context )
{
    (void) context;

    /* Not required for the current application.
       The radio remains in continuous receive mode and is never put
       into sleep mode. */
    return SX126X_HAL_STATUS_OK;
}