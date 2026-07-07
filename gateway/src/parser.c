/*
 * parser.c
 *
 * LoRa packet parser implementation.
 */

#include "parser.h"

#include <stdio.h>

int parse_payload(const char *payload, sensor_data_t *sensor_data)
{
    int fields;

    /* Validate input parameters */
    if ((payload == NULL) || (sensor_data == NULL))
    {
        return -1;
    }

    /* Parse payload */
    fields = sscanf(
        payload,
        "%15[^;];%f;%f",
        sensor_data->node,
        &sensor_data->temperature,
        &sensor_data->humidity
    );

    if (fields != 3)
    {
        return -1;
    }

    return 0;
}