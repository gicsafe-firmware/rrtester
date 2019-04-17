/**
 *  \file       config.h
 *  \brief      Specification for configure.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* ----------------------------- Include files ----------------------------- */
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

#define CONNECTION_PROT         "TCP"
#define CONNECTION_DOMAIN       MOSQUITTO_SERVER
#define CONNECTION_PORT         MOSQUITTO_PORT

#define MQTT_CLIENT_ID          "3002334"

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
extern MQTTProtCfg mqttProtCfg;

/* -------------------------- Function prototypes -------------------------- */
void config_clientId(char *pid);
void config_topic(char *topic);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
