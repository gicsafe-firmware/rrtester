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
#include "Mock_DigIn.h"
#include "Mock_rkhassert.h"
#include "Mock_rkhsma.h"
#include "Mock_Relay.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
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
}

void
tearDown(void)
{
}

void
test_StartSynchro(void)
{
    rkh_tmr_init__Expect(&me->evSyncObj.tmr, (RKH_EVT_T *)&me->evSyncObj);
    rkh_tmr_start_Expect(&me->evSyncObj.tmr, RKH_UPCAST(RKH_SMA_T, me), 
                         SIGMON_SYNC_TIME);
    SigMon_enSMActive(me);
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
    inSeqExpect = ((status.clk << 2) | (status.clkX3 << 1) | status.clkX6);
    DigIn_get_ExpectAndReturn(status);
    rkh_sma_post_fifo_Expect(RKH_UPCAST(RKH_SMA_T, me), &me->evInObj, me);

    SigMon_SMActiveToSMActiveLoc2(me, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj));
    TEST_ASSERT_EQUAL(inSeqExpect, me->digIn);
    TEST_ASSERT_EQUAL(mapDigIn[inSeqExpect], me->evInObj.e);
}

void
test_StartAdqCycle(void)
{
    Relay_getCurrent_ExpectAndReturn(0xdead);
    Relay_getVoltage_ExpectAndReturn(0xbeaf);

    SigMon_enSeq0(me);
    TEST_ASSERT_EQUAL(1, me->nAnSmp);
    TEST_ASSERT_EQUAL(0xdead, me->currVal);
    TEST_ASSERT_EQUAL(0xbeaf, me->voltVal);
}

void
test_AdquireSeq0(void)
{
    TEST_IGNORE();
}

void
test_AdquireSeq1(void)
{
    TEST_IGNORE();
}

void
test_AdquireSeq2(void)
{
    TEST_IGNORE();
}

void
test_StopAdqCycle(void)
{
    TEST_IGNORE();
}

/* ------------------------------ End of file ------------------------------ */
