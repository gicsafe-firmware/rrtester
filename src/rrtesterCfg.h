/**
 *  \file       rrtesterCfg.h
 *  \brief      Specification for configure rrtester.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __RRTESTERCFG_H__
#define __RRTESTERCFG_H__

/* ----------------------------- Include files ----------------------------- */
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define MOSQUITTO_SERVER        "test.mosquitto.org"
#define MOSQUITTO_PORT          "1883"

#define LINSSE_SERVER           "linsse.com.ar"
#define LINSSE_PORT             "2000"

#define AZURE_SERVER            "191.239.243.244"
#define AZURE_PORT              "1883"

#define DOCKLIGHT_SERVER        "181.230.72.86"
#define DOCKLIGHT_PORT          "33498"

#define CONNECTION_PROT         "TCP"
#define CONNECTION_DOMAIN       AZURE_SERVER
#define CONNECTION_PORT         AZURE_PORT

#define MQTT_CLIENT_ID          "3002334"

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void rrtesterCfg_clientId(char *pid);
void rrtesterCfg_topic(char *topic);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
