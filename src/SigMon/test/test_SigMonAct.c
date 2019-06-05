/*
 *  --------------------------------------------------------------------------
 *
 *                                Framework RKH
 *                                -------------
 *
 *            State-machine framework for reactive embedded systems
 *
 *                      Copyright (C) 2010 Leandro Francucci.
 *          All rights reserved. Protected by international copyright laws.
 *
 *
 *  RKH is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any
 *  later version.
 *
 *  RKH is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with RKH, see copying.txt file.
 *
 *  Contact information:
 *  RKH site: http://vortexmakes.com/que-es/
 *  RKH GitHub: https://github.com/vortexmakes/RKH
 *  RKH Sourceforge: https://sourceforge.net/projects/rkh-reactivesys/
 *  e-mail: lf@vortexmakes.com
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file       test_smTransition.c
 *  \brief      Unit test for state machine module.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2016.12.15  LeFr  v2.4.05  ---
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "unity.h"
#include "SigMonAct.h"
#include "SigMon.h"
#include "signal.h"
#include "rkhfwk_cast.h"
#include "Mock_rkhtmr.h"
#include "Mock_rkhtrc_record.h"
#include "Mock_DigIn.h"
#include "Mock_rkhassert.h"
#include "Mock_rkhsma.h"
#include "Mock_Relay.h"
#include "Mock_StoreTest.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
RKHROM RKH_SBSC_T SMInactive, WaitSyncSeq, Seq0, Seq2, Seq3, Seq4, Seq5, 
                  Seq11, OutOfSeq, Seq1, Seq10, Seq8, Seq7, Seq6, Seq9;
RKHROM RKH_SCMP_T SMActive;
RKHROM RKH_FINAL_T SigMon_Final;

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
RKH_SMA_CREATE(SigMon, sigMon, 0, HCAL, NULL, NULL, NULL);
RKH_SMA_DEF_PTR(sigMon);
static SigMon *me;
static const RKH_SIG_T mapDigIn[7] = 
{
    evIn0, evIn1, evIn2, 0, evIn4, evIn5, evIn6
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
setUp(void)
{
    me = RKH_DOWNCAST(SigMon, sigMon);
    Mock_DigIn_Init();
    Mock_Relay_Init();
}

void
tearDown(void)
{
    Mock_DigIn_Verify();
    Mock_DigIn_Destroy();
    Mock_Relay_Verify();
    Mock_Relay_Destroy();
}

void
test_Initialize(void)
{
    RKH_EVT_T *evt;

    rkh_trc_ao_Ignore();
    rkh_trc_obj_Ignore();
    rkh_trc_state_Ignore();
    rkh_trc_sig_Ignore();
    StoreTest_init_Expect();

    SigMon_ToSMInactiveExt0(me, evt);
}

void
test_StartSynchro(void)
{
    rkh_tmr_init__Expect(&me->evSyncObj.tmr, (RKH_EVT_T *)&me->evSyncObj);
    rkh_tmr_start_Expect(&me->evSyncObj.tmr, RKH_UPCAST(RKH_SMA_T, me), 
                         SIGMON_SYNC_TIME);
    SigMon_enSMActive(me);
    TEST_ASSERT_EQUAL(SIGMON_DIGIN_TICKS - 1, me->nDigIn);
}

void
test_StopSynchro(void)
{
    rkh_tmr_stop_Expect(&me->evSyncObj.tmr);
    SigMon_exSMActive(me);
}

void
test_Synchro(void)
{
    DigIn status;
    rInt inSeqExpect;

    inSeqExpect = 0;
    status.clk = 1;
    status.clkX3 = 0;
    status.clkX6 = 1;
    status.failure = 0;
    inSeqExpect = ((status.clk << 2) | (status.clkX3 << 1) | status.clkX6);
    DigIn_get_ExpectAndReturn(status);
    rkh_sma_post_lifo_Expect(RKH_UPCAST(RKH_SMA_T, me), &me->evInObj, me);

    SigMon_SMActiveToSMActiveLoc2(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    TEST_ASSERT_EQUAL(status.clk, me->digIn.clk);
    TEST_ASSERT_EQUAL(status.clkX3, me->digIn.clkX3);
    TEST_ASSERT_EQUAL(status.clkX6, me->digIn.clkX6);
    TEST_ASSERT_EQUAL(status.failure, me->digIn.failure);
    TEST_ASSERT_EQUAL(mapDigIn[inSeqExpect], me->evInObj.e);
}

void
test_StoreDigInput(void)
{
    DigIn status;
    int i, j, nTest;

    status.clk = 1;
    status.clkX3 = 0;
    status.clkX6 = 1;
    status.failure = 0;
    DigIn_get_IgnoreAndReturn(status);
    rkh_sma_post_lifo_Ignore();

    /* middle */
    for (me->nDigIn = SIGMON_DIGIN_TICKS - 1, i = 0, nTest = 3; i < nTest; ++i)
    {
        SigMon_SMActiveToSMActiveLoc2(me, 
                                      RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    }
    TEST_ASSERT_EQUAL(6, me->nDigIn);

    /* upper bound */
    StoreTest_digIn_Expect(status);
    for (me->nDigIn = SIGMON_DIGIN_TICKS - 1, i = 0, 
         nTest = SIGMON_DIGIN_TICKS; 
         i < nTest; ++i)
    {
        SigMon_SMActiveToSMActiveLoc2(me, 
                                      RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    }
    TEST_ASSERT_EQUAL(9, me->nDigIn);

    /* beyond upper bound */
    StoreTest_digIn_Expect(status);
    for (me->nDigIn = SIGMON_DIGIN_TICKS - 1, i = 0, 
         nTest = SIGMON_DIGIN_TICKS + 1; 
         i < nTest; ++i)
    {
        SigMon_SMActiveToSMActiveLoc2(me, 
                                      RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    }
    TEST_ASSERT_EQUAL(8, me->nDigIn);
}

void
test_StartAdqCycle(void)
{
    Relay_getCurrent_ExpectAndReturn(0xdead);
    Relay_getVoltage_ExpectAndReturn(0xbeaf);

    SigMon_enSeq0(me);
    TEST_ASSERT_EQUAL(0xdead, me->currVal);
    TEST_ASSERT_EQUAL(0xbeaf, me->voltVal);
}

void
test_AdquireSeq0(void)
{
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);

    SigMon_enSeq0(me);
    SigMon_Seq0ToSeq0Loc4(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    SigMon_Seq0ToSeq0Loc4(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));

    TEST_ASSERT_EQUAL(2, me->currVal);
    TEST_ASSERT_EQUAL(4, me->voltVal);
}

void
test_AdquireSeq1(void)
{
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);

    SigMon_enSeq0(me);
    SigMon_Seq1ToSeq1Loc6(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    SigMon_Seq1ToSeq1Loc6(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));

    TEST_ASSERT_EQUAL(2, me->currVal);
    TEST_ASSERT_EQUAL(4, me->voltVal);
}

void
test_AdquireSeq2(void)
{
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);
    Relay_getCurrent_ExpectAndReturn(2);
    Relay_getVoltage_ExpectAndReturn(4);

    SigMon_enSeq0(me);
    SigMon_Seq2ToSeq2Loc5(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    SigMon_Seq2ToSeq2Loc5(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));

    TEST_ASSERT_EQUAL(2, me->currVal);
    TEST_ASSERT_EQUAL(4, me->voltVal);
}

void
test_StopAdqCycle(void)
{
    StoreTest_setRelayParam_Expect(me->currVal, me->voltVal);
    SigMon_Seq2ToSeq3Ext8(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
}

void
test_Failure(void)
{
    DigIn status;

    StoreTest_digIn_Expect(status);
    StoreTest_digIn_IgnoreArg_digIn();
    SigMon_SMActiveToSigMon_FinalExt3(me, 
                                      RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
}

/* ------------------------------ End of file ------------------------------ */
