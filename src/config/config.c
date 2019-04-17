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
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "config.h"
#include "mqttProt.h"

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
MQTTProtCfg mqttProtCfg;

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

/* ------------------------------ End of file ------------------------------ */
