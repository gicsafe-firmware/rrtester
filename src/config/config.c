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
MQTTProtCfg mqttProtCfg;
MQTTBrokerCfg mqttBrokerCfg =
{
    .protocol = CONNECTION_PROT,
    .address = CONNECTION_DOMAIN,
    .port = CONNECTION_PORT
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
    snprintf(mqttProtCfg.topic, sizeof(mqttProtCfg.topic), "/rrtester/%s", t);
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

/* ------------------------------ End of file ------------------------------ */
