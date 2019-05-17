/**
 *  \file       config.h
 *  \brief      Specification for configure.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include <string.h>
#include "mqttProt.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define MOSQUITTO_SERVER        "37.187.106.16" //"test.mosquitto.org"
#define MOSQUITTO_PORT          "1883"

#define AZURE_SERVER            "191.239.243.244"
#define AZURE_PORT              "1883"

#define DOCKLIGHT_SERVER        "181.230.72.86"
#define DOCKLIGHT_PORT          "33498"

#define CONNECTION_PROT_DFT      "TCP"
#define CONNECTION_DOMAIN_DFT    MOSQUITTO_SERVER
#define CONNECTION_PORT_DFT      MOSQUITTO_PORT

#define MQTT_CLIENT_ID_DFT       "1234567"
#define MQTT_TOPIC_ROOT          "/rrtester/"

/* ------------------------------- Data types ------------------------------ */
typedef struct MQTTBrokerCfg
{
    char protocol[6];
    char address[16];
    char port[6];
} MQTTBrokerCfg;

/* -------------------------- External variables --------------------------- */
extern MQTTProtCfg mqttProtCfg;
extern MQTTBrokerCfg mqttBrokerCfg;

/* -------------------------- Function prototypes -------------------------- */
void config_clientId(char *pid);
void config_topic(char *topic);
void config_brokerProtocol(char *prot);
void config_brokerAddress(char *address);
void config_brokerPort(char *port);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
