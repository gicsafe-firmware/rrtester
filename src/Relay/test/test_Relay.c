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
 *  \file   test_Relay.c
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
#include "Relay.h"
#include "Mock_adconv.h"

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
}

void 
tearDown(void)
{
}

void
test_getCurrent(void)
{
    SampleValue sample;
    
    ADConv_getSample_ExpectAndReturn(AnIn0, 0xdead);
    sample = Relay_getCurrent();
    TEST_ASSERT_EQUAL(0xdead, sample);
}

void
test_getVoltage(void)
{
    SampleValue sample;
    
    ADConv_getSample_ExpectAndReturn(AnIn1, 0xdead);
    sample = Relay_getVoltage();
    TEST_ASSERT_EQUAL(0xdead, sample);
}

/* ------------------------------ End of file ------------------------------ */
