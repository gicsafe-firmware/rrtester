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
 *  \file   test_StoreTest.c
 *  \brief  Unit test for this module.
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
#include "unity.h"
#include "StoreTest.h"
#include "Mock_anSampler.h"
#include "Mock_IOSampler.h"
#include "Mock_rkhassert.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void 
setUp(void)
{
    Mock_anSampler_Init();
    Mock_IOSampler_Init();
    Mock_rkhassert_Init();
}

void 
tearDown(void)
{
    Mock_anSampler_Verify();
    Mock_IOSampler_Verify();
    Mock_rkhassert_Verify();
    Mock_anSampler_Destroy();
    Mock_IOSampler_Destroy();
    Mock_rkhassert_Destroy();
}

void
test_Init(void)
{
    anSampler_init_ExpectAndReturn(0, 0);
    anSampler_init_IgnoreArg_sampler();
    IOSampler_init_ExpectAndReturn(0);

    StoreTest_init();
}

void
test_StoreRelayParameters(void)
{
    anSampler_put_ExpectAndReturn(0);
    StoreTest_setRelayParam(0xdead, 0xbeaf);
}

void
test_StoreDigitalInputStatus(void)
{
    DigIn digIn;
    unsigned char signal;

    digIn.clk = 1;
    digIn.clkX3 = 0;
    digIn.clkX6 = 1;
    digIn.failure = 0;
	signal = ((digIn.failure << 3) | 
              (digIn.clk << 2) | 
              (digIn.clkX3 << 1) | 
              (digIn.clkX6)); 
    IOSampler_put_ExpectAndReturn(signal, 0);

    StoreTest_digIn(digIn);
}

/* ------------------------------ End of file ------------------------------ */
