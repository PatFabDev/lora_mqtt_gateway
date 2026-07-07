/*
 * spi.c
 *
 * Linux SPI interface implementation.
 */

#include "spi.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>


/* SPI configuration */
#define SPI_DEVICE              "/dev/spidev0.0"
#define SPI_MODE                SPI_MODE_0
#define SPI_BITS_PER_WORD       8
#define SPI_CLOCK_HZ            1000000U

/* Linux SPI file descriptor */
static int spi_fd = -1;

int spi_init(void)
{
    /* Open the SPI device */
    spi_fd = open(SPI_DEVICE, O_RDWR);

    if (spi_fd < 0)
    {
        perror("open");
        return -1;
    }

    /* SPI configuration */
    uint8_t mode = SPI_MODE;
    uint8_t bits = SPI_BITS_PER_WORD;
    uint32_t speed = SPI_CLOCK_HZ;

    /* Configure SPI mode */
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        perror("SPI_IOC_WR_MODE");
        close(spi_fd);
        spi_fd = -1;

        return -1;
    }

    /* Configure bits per word */
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
    {
        perror("SPI_IOC_WR_BITS_PER_WORD");
        close(spi_fd);
        spi_fd = -1;

        return -1;
    }

    /* Configure SPI clock frequency */
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        perror("SPI_IOC_WR_MAX_SPEED_HZ");
        close(spi_fd);
        spi_fd = -1;

        return -1;
    }

    return 0;
}

int spi_transfer(const uint8_t *tx, uint8_t *rx, uint16_t len)
{
    if (spi_fd < 0)
    {
        return -1;
    }

    if (len == 0)
    {
        return -1;
    }

    struct spi_ioc_transfer transfer = {0};

    /*
     * The Linux SPI driver expects userspace buffer addresses
     * to be passed as unsigned long values.
     */
    transfer.tx_buf = (unsigned long)(const void *)tx;
    transfer.rx_buf = (unsigned long)rx;
    transfer.len    = len;

    /* Execute a single SPI transfer */
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) < 0)
    {
        perror("SPI_IOC_MESSAGE");
        return -1;
    }

    return 0;
}

void spi_cleanup(void)
{
    if (spi_fd >= 0)
    {
        close(spi_fd);
        spi_fd = -1;
    }
}