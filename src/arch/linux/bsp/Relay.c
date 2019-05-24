/*
 *  --------------------------------------------------------------------------
 *
 *                               GICSAFe-Firmware
 *                               ----------------
 *
 *                      Copyright (C) 2019 CONICET-GICSAFe
 *          All rights reserved. Protected by international copyright laws.
 *
 *  Contact information:
 *  site: https://github.com/gicsafe-firmware
 *  e-mail: <someone>@<somewhere>
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file   Relay.c
 *  \brief  Relay's inductor module emulation implementation.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  CaMa  Carlos Mancón     manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "Relay.h"
#include <stdlib.h>
#include <time.h>

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define CURRENT_UPPER_VALUE 1024
#define CURRENT_LOWER_VALUE 4
#define VOLTAGE_UPPER_VALUE 1024
#define VOLTAGE_LOWER_VALUE 4
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
int generateRandom();
/* ---------------------------- Local functions ---------------------------- */
int
generateRandom(int rangeLowerVal, int rangeUpperVal)
{
    srand(time(NULL));
    return (rand() % (CURRENT_UPPER_VALUE - CURRENT_LOWER_VALUE + 1)) +
           CURRENT_LOWER_VALUE;
}
/* ---------------------------- Global functions --------------------------- */

rui16_t
Relay_getCurrent(void)
{
    return (rui16_t) generateRandom(CURRENT_LOWER_VALUE, CURRENT_UPPER_VALUE);
}

rui16_t
Relay_getVoltage(void)
{
    return (rui16_t) generateRandom(VOLTAGE_LOWER_VALUE, VOLTAGE_UPPER_VALUE);
}
/* ------------------------------ End of file ------------------------------ */
