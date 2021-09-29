/***************************************************************************
 *            test_mqtt.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Opera.
 *
 *  Opera is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Opera is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Opera.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <list>
#include <thread>

/* Callback called when the client receives a CONNACK message from the broker. */
void subscriber_on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    int rc;
    /* Print out the connection result. mosquitto_connack_string() produces an
     * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
     * clients is mosquitto_reason_string().
     */
    printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
    if(reason_code != 0){
        /* If the connection fails for any reason, we don't want to keep on
         * retrying in this example, so disconnect. Without this, the client
         * will attempt to reconnect. */
        mosquitto_disconnect(mosq);
    }

    /* Making subscriptions in the on_connect() callback means that if the
     * connection drops and is automatically resumed by the client, then the
     * subscriptions will be recreated when the client reconnects. */
    rc = mosquitto_subscribe(mosq, NULL, "example/temperature", 1);
    if(rc != MOSQ_ERR_SUCCESS){
        fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
        /* We might as well disconnect if we were unable to subscribe */
        mosquitto_disconnect(mosq);
    }
}


/* Callback called when the broker sends a SUBACK in response to a SUBSCRIBE. */
void subscriber_on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
    int i;
    bool have_subscription = false;

    /* In this example we only subscribe to a single topic at once, but a
     * SUBSCRIBE can contain many topics at once, so this is one way to check
     * them all. */
    for(i=0; i<qos_count; i++){
        printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
        if(granted_qos[i] <= 2){
            have_subscription = true;
        }
    }
    if(have_subscription == false){
        /* The broker rejected all of our subscriptions, we know we only sent
         * the one SUBSCRIBE, so there is no point remaining connected. */
        fprintf(stderr, "Error: All subscriptions rejected.\n");
        mosquitto_disconnect(mosq);
    }
}


/* Callback called when the client receives a message. */
void subscriber_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    /* This blindly prints the payload, but the payload can be anything so take care. */
    printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

/* Callback called when the client receives a CONNACK message from the broker. */
void publisher_on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    /* Print out the connection result. mosquitto_connack_string() produces an
     * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
     * clients is mosquitto_reason_string().
     */
    printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
    if(reason_code != 0){
        /* If the connection fails for any reason, we don't want to keep on
         * retrying in this example, so disconnect. Without this, the client
         * will attempt to reconnect. */
        mosquitto_disconnect(mosq);
    }

    /* You may wish to set a flag here to indicate to your application that the
     * client is now connected. */
}


/* Callback called when the client knows to the best of its abilities that a
 * PUBLISH has been successfully sent. For QoS 0 this means the message has
 * been completely written to the operating system. For QoS 1 this means we
 * have received a PUBACK from the broker. For QoS 2 this means we have
 * received a PUBCOMP from the broker. */
void publisher_on_publish(struct mosquitto *mosq, void *obj, int mid)
{
    printf("Message with mid %d has been published.\n", mid);
}


int get_temperature(void)
{
    sleep(1); /* Prevent a storm of messages - this pretend sensor works at 1Hz */
    return random()%100;
}

/* This function pretends to read some data from a sensor and publish it.*/
void publish_sensor_data(struct mosquitto *mosq)
{
    char payload[20];
    int temp;
    int rc;

    /* Get our pretend data */
    temp = get_temperature();
    /* Print it to a string for easy human reading - payload format is highly
     * application dependent. */
    snprintf(payload, sizeof(payload), "%d", temp);

    /* Publish the message
     * mosq - our client instance
     * *mid = NULL - we don't want to know what the message id for this message is
     * topic = "example/temperature" - the topic on which this message will be published
     * payloadlen = strlen(payload) - the length of our payload in bytes
     * payload - the actual payload
     * qos = 2 - publish with QoS 2 for this example
     * retain = false - do not use the retained message feature for this message
     */
    rc = mosquitto_publish(mosq, NULL, "example/temperature", strlen(payload), payload, 2, false);
    if(rc != MOSQ_ERR_SUCCESS){
        fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
    }
}

int main(int argc, char *argv[])
{
    struct mosquitto *mosq_subscriber;
    struct mosquitto *mosq_publisher;

    /* Required before calling other mosquitto functions */
    mosquitto_lib_init();

    std::thread subscriber_thr([&] {

        int rc;

        /* Create a new client instance.
         * id = NULL -> ask the broker to generate a client id for us
         * clean session = true -> the broker should remove old sessions when we connect
         * obj = NULL -> we aren't passing any of our private data for callbacks
         */
        mosq_subscriber = mosquitto_new(NULL, true, NULL);
        if (mosq_subscriber == NULL) {
            fprintf(stderr, "Error: Out of memory.\n");
            return 1;
        }

        /* Configure callbacks. This should be done before connecting ideally. */
        mosquitto_connect_callback_set(mosq_subscriber, subscriber_on_connect);
        mosquitto_subscribe_callback_set(mosq_subscriber, subscriber_on_subscribe);
        mosquitto_message_callback_set(mosq_subscriber, subscriber_on_message);

        /* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
         * This call makes the socket connection only, it does not complete the MQTT
         * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
         * mosquitto_loop_forever() for processing net traffic. */
        rc = mosquitto_connect(mosq_subscriber, "localhost", 1883, 60);
        if (rc != MOSQ_ERR_SUCCESS) {
            mosquitto_destroy(mosq_subscriber);
            fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
            return 1;
        }

        /* Run the network loop in a blocking call. The only thing we do in this
         * example is to print incoming messages, so a blocking call here is fine.
         *
         * This call will continue forever, carrying automatic reconnections if
         * necessary, until the user calls mosquitto_disconnect().
         */
        mosquitto_loop_forever(mosq_subscriber, -1, 1);
        return 0;
    });

    std::thread publisher_thr([&]{

        int rc;

        /* Required before calling other mosquitto functions */
        mosquitto_lib_init();

        /* Create a new client instance.
         * id = NULL -> ask the broker to generate a client id for us
         * clean session = true -> the broker should remove old sessions when we connect
         * obj = NULL -> we aren't passing any of our private data for callbacks
         */
        mosq_publisher = mosquitto_new(NULL, true, NULL);
        if(mosq_publisher == NULL){
            fprintf(stderr, "Error: Out of memory.\n");
            return 1;
        }

        /* Configure callbacks. This should be done before connecting ideally. */
        mosquitto_connect_callback_set(mosq_publisher, publisher_on_connect);
        mosquitto_publish_callback_set(mosq_publisher, publisher_on_publish);

        /* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
         * This call makes the socket connection only, it does not complete the MQTT
         * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
         * mosquitto_loop_forever() for processing net traffic. */
        rc = mosquitto_connect(mosq_publisher, "localhost", 1883, 60);
        if(rc != MOSQ_ERR_SUCCESS){
            mosquitto_destroy(mosq_publisher);
            fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
            return 1;
        }

        /* Run the network loop in a background thread, this call returns quickly. */
        rc = mosquitto_loop_start(mosq_publisher);
        if(rc != MOSQ_ERR_SUCCESS){
            mosquitto_destroy(mosq_publisher);
            fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
            return 1;
        }

        /* At this point the client is connected to the network socket, but may not
         * have completed CONNECT/CONNACK.
         * It is fairly safe to start queuing messages at this point, but if you
         * want to be really sure you should wait until after a successful call to
         * the connect callback.
         * In this case we know it is 1 second before we start publishing.
         */
        unsigned int num_data = 5;
        while(num_data--){
            publish_sensor_data(mosq_publisher);
        }
        return 0;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    mosquitto_disconnect(mosq_subscriber);
    subscriber_thr.join();
    publisher_thr.join();

    mosquitto_lib_cleanup();

    return 0;
}