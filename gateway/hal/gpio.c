/*
 * gpio.c
 *
 * Linux GPIO interface implementation using libgpiod.
 */

#include "gpio.h"

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>


/* GPIO configuration */
#define GPIO_DEVICE              "/dev/gpiochip0"
#define SX126X_RESET_DELAY_US    10000

/* GPIO offsets using Broadcom (BCM) GPIO numbering */
#define GPIO_BUSY   23
#define GPIO_DIO1   24
#define GPIO_RESET  25

/* libgpiod objects */
static struct gpiod_chip *chip;
static struct gpiod_line_request *gpio_req;


int gpio_init(void)
{
    /* Open the GPIO controller */
    chip = gpiod_chip_open(GPIO_DEVICE);

    if (!chip)
    {
        perror("open");
        return -1;
    }

    /* Create line settings objects */
    struct gpiod_line_settings *input_settings = NULL;
    struct gpiod_line_settings *output_settings = NULL;

    /* Allocate line settings with default configuration */
    input_settings = gpiod_line_settings_new();

    if (!input_settings)
    {
        perror("gpiod_line_settings_new");
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    output_settings = gpiod_line_settings_new();

    if (!output_settings)
    {
        perror("gpiod_line_settings_new");
        gpiod_line_settings_free(input_settings);
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    /* Configure GPIO directions */
    if (gpiod_line_settings_set_direction(
            input_settings,
            GPIOD_LINE_DIRECTION_INPUT) < 0)
    {
        perror("gpiod_line_settings_set_direction");
        gpiod_line_settings_free(input_settings);
        gpiod_line_settings_free(output_settings);
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    if (gpiod_line_settings_set_direction(
            output_settings,
            GPIOD_LINE_DIRECTION_OUTPUT) < 0)
    {
        perror("gpiod_line_settings_set_direction");
        gpiod_line_settings_free(input_settings);
        gpiod_line_settings_free(output_settings);
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    /* Create the GPIO line configuration */
    struct gpiod_line_config *line_config = NULL;

    line_config = gpiod_line_config_new();

    if (!line_config)
    {
        perror("gpiod_line_config_new");
        gpiod_line_settings_free(input_settings);
        gpiod_line_settings_free(output_settings);
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    /* Group GPIO offsets by direction */
    const unsigned int input_offsets[] =
    {
        GPIO_BUSY,
        GPIO_DIO1
    };

    const unsigned int output_offsets[] =
    {
        GPIO_RESET
    };

    /* Assign input settings to BUSY and DIO1 */
    if (gpiod_line_config_add_line_settings(
            line_config,
            input_offsets,
            2,
            input_settings) < 0)
    {
        perror("gpiod_line_config_add_line_settings");
        gpiod_line_settings_free(input_settings);
        gpiod_line_settings_free(output_settings);
        gpiod_line_config_free(line_config);
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    /* Assign output settings to RESET */
    if (gpiod_line_config_add_line_settings(
            line_config,
            output_offsets,
            1,
            output_settings) < 0)
    {
        perror("gpiod_line_config_add_line_settings");
        gpiod_line_settings_free(input_settings);
        gpiod_line_settings_free(output_settings);
        gpiod_line_config_free(line_config);
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    /* Create the GPIO request */
    struct gpiod_request_config *request_config;

    request_config = gpiod_request_config_new();

    if (request_config == NULL)
    {
        return -1;
    }

    gpiod_request_config_set_consumer(request_config, "sx1262");

    gpio_req = gpiod_chip_request_lines(
        chip,
        request_config,
        line_config);

    if (!gpio_req)
    {
        perror("gpiod_chip_request_lines");
        gpiod_line_settings_free(input_settings);
        gpiod_line_settings_free(output_settings);
        gpiod_line_config_free(line_config);
        gpiod_chip_close(chip);
        chip = NULL;
        return -1;
    }

    /* The request now owns the GPIO configuration.
       The temporary configuration objects are no longer needed. */
    gpiod_line_settings_free(input_settings);
    gpiod_line_settings_free(output_settings);
    gpiod_line_config_free(line_config);
    gpiod_request_config_free(request_config);
    return 0;
}

void sx126x_hw_reset(void)
{
    int status;

    /* Assert reset (LOW) */
    status = gpiod_line_request_set_value(
        gpio_req,
        GPIO_RESET,
        GPIOD_LINE_VALUE_INACTIVE
    );

    if (status < 0)
    {
        perror("gpiod_line_request_set_value");
        return;
    }

    usleep(SX126X_RESET_DELAY_US);

    /* Release reset (HIGH) */
    status = gpiod_line_request_set_value(
        gpio_req,
        GPIO_RESET,
        GPIOD_LINE_VALUE_ACTIVE
    );

    if (status < 0)
    {
        perror("gpiod_line_request_set_value");
        return;
    }

    usleep(SX126X_RESET_DELAY_US);
}

int sx126x_busy(void)
{
    int value;

    value = gpiod_line_request_get_value(
                gpio_req,
                GPIO_BUSY
    );

    if (value < 0)
    {
        perror("gpiod_line_request_get_value");
        return -1;
    }

    return value;
}

int sx126x_dio1(void)
{
    int value;

    value = gpiod_line_request_get_value(
                gpio_req,
                GPIO_DIO1
    );

    if (value < 0)
    {
        perror("gpiod_line_request_get_value");
        return -1;
    }

    return value;
}

void gpio_cleanup(void)
{
    if (gpio_req)
    {
        gpiod_line_request_release(gpio_req);
        gpio_req = NULL;
    }

    if (chip)
    {
        gpiod_chip_close(chip);
        chip = NULL;
    }
}