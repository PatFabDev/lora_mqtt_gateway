/*
 * mqtt.c
 *
 * MQTT client implementation.
 */

#include "mqtt.h"

#include <stdio.h>
#include <string.h>
#include <mosquitto.h>

/* MQTT client instance */
static struct mosquitto *mqtt_client = NULL;

int mqtt_init(void)
{
    int rc;

    /* Initialize Mosquitto library */
    rc = mosquitto_lib_init();

    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,
                "mosquitto_lib_init() failed: %s\n",
                mosquitto_strerror(rc));

        return -1;
    }

    /* Create MQTT client with a clean session */
    mqtt_client = mosquitto_new(MQTT_CLIENT_ID, true, NULL);

    if (mqtt_client == NULL)
    {
        mosquitto_lib_cleanup();
        return -1;
    }

    /* Configure MQTT authentication (optional) */
    if (MQTT_USERNAME[0] != '\0')
    {
        rc = mosquitto_username_pw_set(mqtt_client, MQTT_USERNAME, MQTT_PASSWORD);

        if (rc != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr,
                    "mosquitto_username_pw_set() failed: %s\n",
                    mosquitto_strerror(rc));

            mosquitto_destroy(mqtt_client);
            mosquitto_lib_cleanup();

            return -1;
        }
    }

    /* Configure MQTT Last Will */
    rc = mosquitto_will_set(
            mqtt_client,
            MQTT_TOPIC_STATUS,
            strlen(MQTT_WILL_MESSAGE),
            MQTT_WILL_MESSAGE,
            MQTT_WILL_QOS,
            MQTT_WILL_RETAIN
        );

    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,
                "mosquitto_will_set() failed: %s\n",
                mosquitto_strerror(rc));

        mosquitto_destroy(mqtt_client);
        mosquitto_lib_cleanup();

        return -1;
    }

    /* Connect to MQTT broker */
    rc = mosquitto_connect(
            mqtt_client,
            MQTT_HOST,
            MQTT_PORT,
            MQTT_KEEP_ALIVE);

    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,
                "mosquitto_connect() failed: %s\n",
                mosquitto_strerror(rc));

        mosquitto_destroy(mqtt_client);
        mqtt_client = NULL;

        mosquitto_lib_cleanup();

        return -1;
    }

    /* Start MQTT network loop */
    rc = mosquitto_loop_start(mqtt_client);

    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,
                "mosquitto_loop_start() failed: %s\n",
                mosquitto_strerror(rc));

        mosquitto_disconnect(mqtt_client);
        mosquitto_destroy(mqtt_client);
        mqtt_client = NULL;

        mosquitto_lib_cleanup();

        return -1;
    }

    /* Publish gateway online status */
    rc = mosquitto_publish(
            mqtt_client,
            NULL,
            MQTT_TOPIC_STATUS,
            strlen(MQTT_ONLINE_MESSAGE),
            MQTT_ONLINE_MESSAGE,
            MQTT_WILL_QOS,
            MQTT_WILL_RETAIN
        );

    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,
                "mosquitto_publish() failed: %s\n",
                mosquitto_strerror(rc));

        mosquitto_loop_stop(mqtt_client, true);
        mosquitto_disconnect(mqtt_client);
        mosquitto_destroy(mqtt_client);
        mqtt_client = NULL;

        mosquitto_lib_cleanup();

        return -1;
    }

    return 0;
}

int mqtt_publish(const char *json)
{
    int rc;

    if ((mqtt_client == NULL) || (json == NULL))
    {
        return -1;
    }

    rc = mosquitto_publish(
            mqtt_client,
            NULL,
            MQTT_TOPIC_DATA,
            (int)strlen(json),
            json,
            MQTT_QOS,
            MQTT_RETAIN);

    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr,
                "mosquitto_publish() failed: %s\n",
                mosquitto_strerror(rc));

        return -1;
    }

    return 0;
}

void mqtt_deinit(void)
{
    if (mqtt_client != NULL)
    {
        /* Publish gateway offline status */
        mosquitto_publish(
            mqtt_client,
            NULL,
            MQTT_TOPIC_STATUS,
            strlen(MQTT_WILL_MESSAGE),
            MQTT_WILL_MESSAGE,
            MQTT_WILL_QOS,
            MQTT_WILL_RETAIN
        );

        mosquitto_loop_stop(mqtt_client, true);
        mosquitto_disconnect(mqtt_client);
        mosquitto_destroy(mqtt_client);

        mqtt_client = NULL;
    }

    mosquitto_lib_cleanup();
}