/**
 *  \file       config.c
 *  \brief      Implements configuration parameters.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "config.h"

/* ----------------------------- Local macros ------------------------------ */
#define CONFIG_FIELD(f,v) snprintf(f, sizeof(f), "%s", v)

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
MQTTProtCfg mqttProtCfg =
{
    .clientId = MQTT_CLIENT_ID_DFT,
    .topic = MQTT_TOPIC_ROOT MQTT_CLIENT_ID_DFT
};

MQTTBrokerCfg mqttBrokerCfg =
{
    .protocol = CONNECTION_PROT_DFT,
    .address = CONNECTION_DOMAIN_DFT,
    .port = CONNECTION_PORT_DFT
};

/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
config_clientId(char *pid)
{
    strncpy(mqttProtCfg.clientId, pid, sizeof(mqttProtCfg.clientId));
}

void
config_topic(char *t)
{
    snprintf(mqttProtCfg.topic, sizeof(mqttProtCfg.topic), "%s%s",
                    MQTT_TOPIC_ROOT, t);
}

void
config_brokerProtocol(char *prot)
{
    CONFIG_FIELD(mqttBrokerCfg.protocol, prot);
}

void
config_brokerAddress(char *address)
{
    CONFIG_FIELD(mqttBrokerCfg.address, address);
}

void
config_brokerPort(char *port)
{
    CONFIG_FIELD(mqttBrokerCfg.port, port);
}

