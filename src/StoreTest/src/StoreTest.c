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
 *  \file   StoreTest.c
 *  \brief  Store test module implementation.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkhassert.h"
#include "StoreTest.h"
#include "anSampler.h"
#include "IOSampler.h"

RKH_MODULE_NAME(StoreTest)

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
typedef struct Params Params;
struct Params
{
    SampleValue current;
    SampleValue voltage;
    DigIn digIn;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static Params params;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static SampleValue
sampler(int channel)
{
    SampleValue value;

    RKH_REQUIRE((unsigned int)channel < NUM_AN_SIGNALS);
    if (channel == 0)
    {
        value = params.current;
    }
    else
    {
        value = params.voltage;
    }
    return value;
}

/* ---------------------------- Global functions --------------------------- */
void 
StoreTest_init(void)
{
    anSampler_init(sampler);
    IOSampler_init();
}

void 
StoreTest_saveRelayStatus(rui16_t currVal, rui16_t voltVal)
{
    params.current = currVal;
    params.voltage = voltVal;
    anSampler_put();
}

void 
StoreTest_saveDigInStatus(DigIn digIn)
{
    unsigned char signal;

	signal = ((digIn.failure << 3) | 
              (digIn.clk << 2) | 
              (digIn.clkX3 << 1) | 
              (digIn.clkX6)); 
    IOSampler_put(signal);
}

/* ------------------------------ End of file ------------------------------ */
