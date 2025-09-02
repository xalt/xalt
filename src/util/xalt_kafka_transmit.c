#include <glib.h>
#include <librdkafka/rdkafka.h>

#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )


/* Wrapper to set config values and error out if needed.
 */
static void set_config(rd_kafka_conf_t *conf, char *key, char *value) {
    char errstr[512];
    rd_kafka_conf_res_t res;

    res = rd_kafka_conf_set(conf, key, value, errstr, sizeof(errstr));
    if (res != RD_KAFKA_CONF_OK) {
        g_error("Unable to set config: %s", errstr);
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
        g_error("Message delivery failed: %s", rd_kafka_err2str(rkmessage->err));
    }
}

int main (int argc, char **argv) {
    // CLI args
    char *syshost      = argv[1];
    char *json_payload = argv[2];

    // ENVs
    const char *kafka_topic = getenv("XALT_KAFKA_TOPIC");
    char *kafka_servers = getenv("XALT_KAFKA_SERVERS");
    char *kafka_username = getenv("XALT_KAFKA_USERNAME");
    char *kafka_password = getenv("XALT_KAFKA_PASSWORD");
    char *kafka_ca_location = getenv("XALT_KAFKA_CA_LOCATION");
    char *kafka_certificate_location = getenv("XALT_KAFKA_CERTIFICATE_LOCATION");

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
        g_error("Failed to create new producer: %s", errstr);
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
        g_error("Failed to produce to topic %s: %s", kafka_topic, rd_kafka_err2str(err));
        return 1;
    } else {
        g_message("Produced event to topic %s: value = %50s", kafka_topic, json_payload);
    }

    // Block until the messages are all sent.
    rd_kafka_flush(producer, 10 * 1000);

    if (rd_kafka_outq_len(producer) > 0) {
        g_error("message was not delivered");
    }

    g_message("event was produced to topic %s.", kafka_topic);

    rd_kafka_destroy(producer);

    return 0;
}
