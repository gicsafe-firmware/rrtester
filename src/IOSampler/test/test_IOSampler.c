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
 *  \file       test_IOSampler.c
 *  \brief      Unit test for IO sampler.
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 *  BrPa  Bruno Palacios     BrunoPalacios@Embels.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "unity.h"
#include "IOSampler.h"
#include "Mock_CirBuffer.h"
#include "Mock_epoch.h"

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
    Mock_CirBuffer_Init();
    Mock_epoch_Init();
}

void
tearDown(void)
{
    Mock_CirBuffer_Verify();
    Mock_epoch_Verify();
    Mock_CirBuffer_Destroy();
    Mock_epoch_Destroy();
}

void
test_ClearAfterInit(void)
{
    int result;

    cirBuffer_init_ExpectAndReturn( 0, 
                                    0, 
                                    sizeof(IOSample), 
                                    MAX_NUM_IO_SAMPLES,
                                    0);
    cirBuffer_init_IgnoreArg_me();
    cirBuffer_init_IgnoreArg_sto();

    result = IOSampler_init();
    TEST_ASSERT_EQUAL(0, result);
}

void
test_StoreOneSample(void)
{
    int result;
    IOSample signalValue;

    cirBuffer_init_ExpectAndReturn( 0, 
                                    0, 
                                    sizeof(IOSample), 
                                    MAX_NUM_IO_SAMPLES, 
                                    0);
    cirBuffer_init_IgnoreArg_me();
    cirBuffer_init_IgnoreArg_sto();
    epoch_get_ExpectAndReturn(123455);
    cirBuffer_put_ExpectAndReturn(0, 0, 0);
    cirBuffer_put_IgnoreArg_me();
    cirBuffer_put_IgnoreArg_elem();

    IOSampler_init();
    signalValue = 0xdead;

    result = IOSampler_put(signalValue);
    TEST_ASSERT_EQUAL(0, result);
}

void
test_GetOneSample(void)
{
    int result;
    IOSample signalValue;
    IOChg ioChanges;

    cirBuffer_init_ExpectAndReturn( 0, 
                                    0, 
                                    sizeof(IOSample), 
                                    MAX_NUM_IO_SAMPLES, 
                                    0);
    cirBuffer_init_IgnoreArg_me();
    cirBuffer_init_IgnoreArg_sto();
    epoch_get_ExpectAndReturn(0x123456);
    cirBuffer_put_ExpectAndReturn(0, 0, 0);
    cirBuffer_put_IgnoreArg_me();
    cirBuffer_put_IgnoreArg_elem();
    cirBuffer_getNumElem_ExpectAndReturn(0, 1);
    cirBuffer_getNumElem_IgnoreArg_me();
    cirBuffer_getBlock_ExpectAndReturn( 0, 
                                (unsigned char *)&ioChanges.signalValue, 
                                        1, 
                                        1);
    cirBuffer_getBlock_IgnoreArg_me();

    IOSampler_init();
    signalValue = 0xdead;
    IOSampler_put(signalValue);

    result = IOSampler_get(&ioChanges, 1);
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL_HEX(0x123456, ioChanges.timeStamp);
}

void
test_GetMultipleSamples(void)
{
    int result;
    IOSample signalValue;
    IOChg ioChanges;
    int nRequiredSamples, nStoredSamples;
    long timeStamp, ts;

    /* Get an even number of samples */
    nRequiredSamples = 8;
    nStoredSamples = 4;
    timeStamp = 0x00123456;
    epoch_get_ExpectAndReturn(timeStamp);
    cirBuffer_put_ExpectAndReturn(0, 0, 0);
    cirBuffer_put_IgnoreArg_me();
    cirBuffer_put_IgnoreArg_elem();
    cirBuffer_getNumElem_ExpectAndReturn(0, nStoredSamples);
    cirBuffer_getNumElem_IgnoreArg_me();
    cirBuffer_getBlock_ExpectAndReturn( 0, 
                                (unsigned char *)&ioChanges.signalValue, 
                                        nRequiredSamples, 
                                        nStoredSamples);
    cirBuffer_getBlock_IgnoreArg_me();

    signalValue = 0xdead;
    IOSampler_put(signalValue);

    result = IOSampler_get(&ioChanges, nRequiredSamples);
    TEST_ASSERT_EQUAL(nStoredSamples, result);
    ts = timeStamp - (nStoredSamples / 2);
    TEST_ASSERT_EQUAL_HEX(ts, ioChanges.timeStamp);

    /* Get an odd number of samples */
    nRequiredSamples = 8;
    nStoredSamples = 3;
    cirBuffer_getNumElem_ExpectAndReturn(0, nStoredSamples);
    cirBuffer_getNumElem_IgnoreArg_me();
    cirBuffer_getBlock_ExpectAndReturn( 0, 
                                (unsigned char *)&ioChanges.signalValue, 
                                        nRequiredSamples, 
                                        nStoredSamples);
    cirBuffer_getBlock_IgnoreArg_me();

    result = IOSampler_get(&ioChanges, nRequiredSamples);
    TEST_ASSERT_EQUAL(nStoredSamples, result);
    ts = timeStamp - (nStoredSamples / 2);
    TEST_ASSERT_EQUAL_HEX(ts, ioChanges.timeStamp);
}

/* ------------------------------ End of file ------------------------------ */
