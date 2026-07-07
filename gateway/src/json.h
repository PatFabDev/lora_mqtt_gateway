/*
 * json.h
 *
 * JSON message interface.
 */

#ifndef JSON_H
#define JSON_H

#include <stddef.h>

#include "parser.h"

/* Maximum JSON string size */
#define JSON_BUFFER_SIZE    512U

/**
 * @brief Create a JSON object from parsed sensor data.
 *
 * @param[out] json         JSON output buffer.
 * @param[in]  json_size    Size of output buffer.
 * @param[in]  sensor_data  Parsed sensor data.
 * @param[in]  rssi         Packet RSSI [dBm].
 * @param[in]  snr          Packet SNR [dB].
 * @param[in]  signal_rssi  Signal RSSI [dBm].
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int json_create(
    char *json,
    size_t json_size,
    const sensor_data_t *sensor_data,
    int16_t rssi,
    int8_t snr,
    int16_t signal_rssi
);

#endif