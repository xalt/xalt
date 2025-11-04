#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xalt_header.h"

#ifdef HAVE_LIBRDKAFKA_RDKAFKA_H
#include <librdkafka/rdkafka.h>
#endif

#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )

#ifndef KAFKA_TOPIC
#define KAFKA_TOPIC "default_topic"
#endif
#ifndef KAFKA_SERVERS
#define KAFKA_SERVERS "localhost:9092"
#endif
#ifndef KAFKA_USERNAME
#define KAFKA_USERNAME "default_user"
#endif
#ifndef KAFKA_PASSWORD
#define KAFKA_PASSWORD "default_password"
#endif
#ifndef KAFKA_CA_LOCATION
#define KAFKA_CA_LOCATION "/path/to/ca.pem"
#endif
#ifndef KAFKA_CERTIFICATE_LOCATION
#define KAFKA_CERTIFICATE_LOCATION "/path/to/cert.pem"
#endif

#ifdef HAVE_LIBRDKAFKA_RDKAFKA_H
/* Wrapper to set config values and error out if needed.
 */
static void set_config(rd_kafka_conf_t *conf, char *key, char *value) {
    char errstr[512];
    rd_kafka_conf_res_t res;

    res = rd_kafka_conf_set(conf, key, value, errstr, sizeof(errstr));
    if (res != RD_KAFKA_CONF_OK) {
        fprintf(stderr, "Unable to set config: %s\n", errstr);
        exit(1);
    }
}


/* Per-message delivery callback (triggered by poll() or flush())
 * when a message has been successfully delivered or permanently
 * failed delivery (after retries).
 */
static void dr_msg_cb (rd_kafka_t *kafka_handle,
                       const rd_kafka_message_t *rkmessage,
                       void *opaque) {
    if (rkmessage->err) {
        fprintf(stderr, "Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
    }
}
#endif

#ifndef HAVE_LIBRDKAFKA_RDKAFKA_H
int main (int argc, char **argv) {
    return 0;
}
#else
int main (int argc, char **argv) {
    // CLI args
    char *json_payload = argv[2];

    // ENVs
    const char *kafka_topic = KAFKA_TOPIC;
    const char *kafka_servers = KAFKA_SERVERS;
    const char *kafka_username = KAFKA_USERNAME;
    const char *kafka_password = KAFKA_PASSWORD;
    const char *kafka_ca_location = KAFKA_CA_LOCATION;
    const char *kafka_certificate_location = KAFKA_CERTIFICATE_LOCATION;

    rd_kafka_t *producer;
    rd_kafka_conf_t *conf;
    char errstr[512];

    // Create client configuration
    conf = rd_kafka_conf_new();

    /* Kafka configuration keys:
     * - bootstrap.servers
     * - security.protocol
     * - sasl.mechanism
     * - sasl.username
     * - sasl.password
     * - ssl.ca.location
     * - ssl.certificate.location
     */
    // User-specific properties that you must set
    set_config(conf, "bootstrap.servers", kafka_servers);
    set_config(conf, "security.protocol", "SASL_SSL");
    set_config(conf, "sasl.mechanism", "PLAIN");
    set_config(conf, "sasl.username", kafka_username);
    set_config(conf, "sasl.password", kafka_password);
    set_config(conf, "ssl.ca.location", kafka_ca_location);
    set_config(conf, "ssl.certificate.location", kafka_certificate_location);

    // Fixed properties
    set_config(conf, "acks", "all");

    // Install a delivery-error callback.
    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

    // Create the Producer instance.
    producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!producer) {
        fprintf(stderr, "Failed to create new producer: %s\n", errstr);
        return 1;
    }

    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    conf = NULL;

    // Produce data.
    size_t value_len = strlen(json_payload);

    rd_kafka_resp_err_t err;

    err = rd_kafka_producev(producer,
                            RD_KAFKA_V_TOPIC(kafka_topic),
                            RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                            RD_KAFKA_V_VALUE(json_payload, value_len),
                            RD_KAFKA_V_OPAQUE(NULL),
                            RD_KAFKA_V_END);

    if (err) {
        fprintf(stderr, "Failed to produce to topic %s: %s\n", kafka_topic, rd_kafka_err2str(err));
        return 1;
    }

    // Block until the messages are all sent.
    rd_kafka_flush(producer, 10 * 1000);

    if (rd_kafka_outq_len(producer) > 0) {
        fprintf(stderr, "Message was not delivered.\n");
        return 1;
    }

    rd_kafka_destroy(producer);

    return 0;
}
#endif
