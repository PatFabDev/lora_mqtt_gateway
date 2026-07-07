/*
 * main.c
 *
 * LoRa MQTT gateway application entry point.
 */

#include "../hal/sx126x_hal_linux.h"

#include "radio.h"
#include "parser.h"
#include "json.h"
#include "mqtt.h"

#include <stdio.h>
#include <unistd.h>

int main(void)
{
    radio_packet_t packet;
    sensor_data_t sensor_data;
    char json[JSON_BUFFER_SIZE];

    if (sx126x_init() < 0)
    {
        return 1;
    }

    if (radio_init() < 0)
    {
        sx126x_deinit();
        return 1;
    }

    if (mqtt_init() < 0)
    {
        sx126x_deinit();
        return 1;
    }

    while (1)
    {
        int status;

        status = radio_process(&packet);

        if (status == -1)
        {
            printf("radio_process(): ERROR\n");
            break;
        }
        else if (status == 1)
        {
            /* No paket received */
        }
        else if (status == 0)
        {

            if (parse_payload((char *)packet.payload, &sensor_data) != 0)
            {
                printf("Parser ERROR!\n");
                continue;
            }

            /* Create JSON Packet */
            if (json_create(
                    json,
                    sizeof(json),
                    &sensor_data,
                    packet.rssi,
                    packet.snr,
                    packet.signal_rssi) != 0)
            {
                printf("JSON ERROR!\n");
                continue;
            }

            printf("\n%s\n", json);

            /* Publish MQTT message */
            if (mqtt_publish(json) < 0)
            {
                printf("MQTT publish failed!\n");
            }
        }

        usleep(1000);
    }

    sx126x_deinit();

    return 0;
}