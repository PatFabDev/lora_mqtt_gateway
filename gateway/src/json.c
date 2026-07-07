/*
 * json.c
 *
 * JSON message generation.
 */

#include "json.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define GATEWAY_NAME "pi4"

int json_create(
    char *json,
    size_t json_size,
    const sensor_data_t *sensor_data,
    int16_t rssi,
    int8_t snr,
    int16_t signal_rssi
)
{
    time_t now;
    struct tm tm_now;
    char timestamp[32];

    /* Validate input parameters */
    if ((json == NULL) || (sensor_data == NULL))
    {
        return -1;
    }

    /* Get current local time */
    now = time(NULL);

    if (gmtime_r(&now, &tm_now) == NULL)
    {
        return -1;
    }

    /* Format timestamp */
    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &tm_now) == 0)
    {
        return -1;
    }

    /* Create JSON object */
    if (snprintf(
            json,
            json_size,
            "{"
            "\"gateway\":\"%s\","
            "\"timestamp\":\"%s\","
            "\"payload\":{"
                "\"node\":\"%s\","
                "\"temperature\":%.1f,"
                "\"humidity\":%.1f"
            "},"
            "\"rssi\":%d,"
            "\"snr\":%d,"
            "\"signal_rssi\":%d"
            "}",
            GATEWAY_NAME,
            timestamp,
            sensor_data->node,
            sensor_data->temperature,
            sensor_data->humidity,
            rssi,
            snr,
            signal_rssi) >= (int)json_size)
    {
        return -1;
    }

    return 0;
}