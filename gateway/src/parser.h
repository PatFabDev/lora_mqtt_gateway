/*
 * parser.h
 *
 * LoRa packet parser interface.
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

/* Maximum node name length */
#define PARSER_NODE_NAME_SIZE    16U

/**
 * @brief Parsed sensor data.
 */
typedef struct
{
    char node[PARSER_NODE_NAME_SIZE];
    float temperature;
    float humidity;
} sensor_data_t;

/**
 * @brief Parse an ASCII LoRa payload.
 *
 * Payload format:
 * node01;<float1>;<float2>
 *   │    │    │
 *   │    │    └── Relative Humidity [%RH]
 *   │    └─────── Temperature [°C]
 *   └──────────── Node identifier
 *
 * @param[in]  payload      ASCII payload.
 * @param[out] sensor_data  Parsed sensor data.
 *
 * @return
 *         0 : Success
 *        -1 : Parse error
 */
int parse_payload(const char *payload, sensor_data_t *sensor_data);

#endif